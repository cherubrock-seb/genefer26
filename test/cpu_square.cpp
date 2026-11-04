// Whole-operation timing and independent GMP checks for the CPU transform.
// Build with -DTEST_N=15..18 -DTEST_BATCH=8/16/32 and the desired ISA flags.
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#define arch_namespace cpu_square_test
#include "transformCPU.h"

#ifndef TEST_N
#define TEST_N 15
#endif
#ifndef TEST_BATCH
#define TEST_BATCH 8
#endif
static_assert(TEST_N >= 10 && TEST_N <= 18, "unsupported test exponent");
static_assert(TEST_BATCH == 8 || TEST_BATCH == 16 || TEST_BATCH == 32, "unsupported batch");

constexpr size_t lanes = TEST_BATCH, digits = size_t(1) << TEST_N;
using Digit = SVint<Int32_8, lanes / 8>;
using Base = b_vec<lanes / 8>;
class TestTransform : public cpu_square_test::transformCPU<lanes, digits / 2>
{
    using Parent = cpu_square_test::transformCPU<lanes, digits / 2>;
public:
    explicit TestTransform(const Base & base) : Parent(base, TEST_N, 3) {}
    using Parent::getZi;
    using Parent::setZi;
};

struct Integer
{
    mpz_t value;
    Integer() { mpz_init(value); }
    ~Integer() { mpz_clear(value); }
    Integer(const Integer &) = delete;
    Integer & operator=(const Integer &) = delete;
};

// Evaluate the entire balanced base-b polynomial, using a product tree so
// conversion remains practical for millions of bits. This shares no FFT code.
static void evaluate(mpz_t out, const std::vector<Digit> & input, size_t lane,
                     size_t offset, unsigned level, const Integer * powers)
{
    const size_t count = size_t(1) << level;
    if (level <= 5)
    {
        mpz_set_ui(out, 0);
        const unsigned long base = mpz_get_ui(powers[0].value);
        for (size_t i = count; i != 0; --i)
        {
            mpz_mul_ui(out, out, base);
            const int32_t d = input[offset + i - 1][lane / 8][lane % 8];
            if (d >= 0) mpz_add_ui(out, out, static_cast<unsigned long>(d));
            else mpz_sub_ui(out, out, static_cast<unsigned long>(-int64_t(d)));
        }
        return;
    }
    Integer low;
    evaluate(low.value, input, lane, offset, level - 1, powers);
    evaluate(out, input, lane, offset + count / 2, level - 1, powers);
    mpz_mul(out, out, powers[level - 1].value);
    mpz_add(out, out, low.value);
}

static Base make_bases()
{
    // Small bases, the split boundary, large bases, and the accepted upper bound.
    const uint32_t values[8] = {10000, 10002, 65534, 1048574, 1048578,
                               999999998, 1999999998, 2000000000};
    Base base;
    for (size_t group = 0; group < lanes / 8; ++group) base[group] = UInt32_8(values);
    return base;
}

static uint64_t next_random(uint64_t & state)
{
    state ^= state << 13; state ^= state >> 7; state ^= state << 17;
    return state;
}

static int verify()
{
    const Base base = make_bases();
    TestTransform transform(base);
    std::vector<Digit> input(digits), output(digits);
    uint64_t random = 0x83657a1b92d4e6cfull;
    for (size_t k = 0; k < digits; ++k)
        for (size_t group = 0; group < lanes / 8; ++group)
        {
            int32_t values[8];
            for (size_t j = 0; j < 8; ++j)
            {
                const int32_t b = static_cast<int32_t>(base[group][j]);
                values[j] = static_cast<int32_t>(next_random(random) % uint32_t(b)) - b / 2;
                if (j == 1) values[j] = (k & 1) ? b / 2 - 1 : -b / 2;
                if (j == 2) values[j] = (k == 0 || k == digits - 1) ? b / 2 - 1 : 0;
                if (j == 3) values[j] = -1;
            }
            input[k][group] = Int32_8(values);
        }

    Integer expected[lanes], saved[lanes], modulus[lanes], powers[lanes][TEST_N + 1];
    for (size_t lane = 0; lane < lanes; ++lane)
    {
        mpz_set_ui(powers[lane][0].value, base[lane / 8][lane % 8]);
        for (size_t j = 1; j <= TEST_N; ++j)
            mpz_mul(powers[lane][j].value, powers[lane][j - 1].value, powers[lane][j - 1].value);
        mpz_add_ui(modulus[lane].value, powers[lane][TEST_N].value, 1);
        evaluate(expected[lane].value, input, lane, 0, TEST_N, powers[lane]);
        mpz_mod(expected[lane].value, expected[lane].value, modulus[lane].value);
    }
    transform.setZi(input.data());
    size_t checks = 0;
    auto check = [&]()
    {
        transform.getZi(output.data());
        for (size_t lane = 0; lane < lanes; ++lane)
        {
            Integer actual;
            evaluate(actual.value, output, lane, 0, TEST_N, powers[lane]);
            mpz_mod(actual.value, actual.value, modulus[lane].value);
            if (mpz_cmp(actual.value, expected[lane].value) != 0)
                throw std::runtime_error("GMP residue mismatch at check " + std::to_string(checks) +
                                         ", lane " + std::to_string(lane));
        }
        if (!std::isfinite(transform.get_error()) || transform.get_error() >= 0.5)
            throw std::runtime_error("roundoff limit exceeded");
        ++checks;
    };
    check();
    for (uint32_t mask : {0u, 0xffffffffu, 0xaaaaaaaaU, 0x55555555U, 0x80018001U})
    {
        transform.square_dup(mask);
        for (size_t lane = 0; lane < lanes; ++lane)
        {
            mpz_mul(expected[lane].value, expected[lane].value, expected[lane].value);
            if ((mask >> lane) & 1) mpz_mul_2exp(expected[lane].value, expected[lane].value, 1);
            mpz_mod(expected[lane].value, expected[lane].value, modulus[lane].value);
        }
        check();
    }
    transform.copy(1, 0);
    transform.init_multiplicand(1);
    for (size_t lane = 0; lane < lanes; ++lane) mpz_set(saved[lane].value, expected[lane].value);
    for (uint32_t mask : {0u, 0x84218421U, 0xffffffffU})
    {
        transform.mul_mask(mask);
        for (size_t lane = 0; lane < lanes; ++lane)
            if ((mask >> lane) & 1)
            {
                mpz_mul(expected[lane].value, expected[lane].value, saved[lane].value);
                mpz_mod(expected[lane].value, expected[lane].value, modulus[lane].value);
            }
        check();
    }
    std::cout << "{\"mode\":\"verify\",\"n\":" << TEST_N << ",\"batch\":" << lanes
              << ",\"checks\":" << checks * lanes << ",\"error\":" << transform.get_error()
              << ",\"exactness\":\"PASS\"}\n";
    return 0;
}

static int bench(size_t iterations)
{
    Base base; base.init(1000000000, 2, 2);
    TestTransform transform(base);
    transform.set(3);
    for (size_t i = 0; i < 32; ++i) transform.square_dup((i & 1) ? 0x55555555U : 0xaaaaaaaaU);
    const auto start = std::chrono::steady_clock::now();
    for (size_t i = 0; i < iterations; ++i) transform.square_dup((i & 1) ? 0x55555555U : 0xaaaaaaaaU);
    const double seconds = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
    transform.to_int();
    const auto hash = transform.gethash64();
    std::cout << std::setprecision(12) << "{\"mode\":\"bench\",\"n\":" << TEST_N
              << ",\"batch\":" << lanes << ",\"iterations\":" << iterations
              << ",\"seconds\":" << seconds << ",\"us_per_square\":" << seconds * 1e6 / iterations
              << ",\"error\":" << transform.get_error() << ",\"bases\":[";
    for (size_t lane = 0; lane < lanes; ++lane)
        std::cout << (lane ? "," : "") << base[lane / 8][lane % 8];
    std::cout << "],\"hashes\":[";
    for (size_t lane = 0; lane < lanes; ++lane)
        std::cout << (lane ? "," : "") << '"' << std::hex << hash[lane / 8][lane % 8] << '"';
    std::cout << "]}\n";
    return 0;
}

int main(int argc, char **argv)
{
    try
    {
        if (argc == 2 && std::string(argv[1]) == "verify") return verify();
        if (argc == 3 && std::string(argv[1]) == "bench")
        {
            const std::string value = argv[2];
            if (value.empty() || value.find_first_not_of("0123456789") != std::string::npos)
                throw std::runtime_error("invalid iteration count");
            const size_t count = std::stoul(value);
            if (count == 0 || count > 1000000) throw std::runtime_error("iterations must be in [1, 1000000]");
            return bench(count);
        }
        std::cerr << "Usage: cpu_square verify | bench ITERATIONS\n";
    }
    catch (const std::exception & e) { std::cerr << e.what() << '\n'; }
    return 1;
}
