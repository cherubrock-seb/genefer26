/*
Copyright 2026, Yves Gallot

genefer is free source code, under the MIT license (see LICENSE). You can redistribute, use and/or modify it.
Please give feedback to the authors if improvement is realized. It is distributed in the hope that it will be useful.
*/

#pragma once

#include <cstdint>

static const char * const src_ocl_kernel = \
"/*\n" \
"Copyright 2026, Yves Gallot\n" \
"\n" \
"genefer is free source code, under the MIT license (see LICENSE). You can redistribute, use and/or modify it.\n" \
"Please give feedback to the authors if improvement is realized. It is distributed in the hope that it will be useful.\n" \
"*/\n" \
"\n" \
"#if __OPENCL_VERSION__ >= 120\n" \
"\t#define INLINE\tstatic inline\n" \
"#else\n" \
"\t#define INLINE\n" \
"#endif\n" \
"\n" \
"#if defined(__NV_CL_C_VERSION)\n" \
"\t#define PTX_ASM\t1\n" \
"#endif\n" \
"\n" \
"#if !defined(N_SZ)\n" \
"#define N_SZ\t\t65536u\n" \
"#define LN_SZ\t\t16\n" \
"#define VSIZE\t\t8\n" \
"// #define IS32\t\t1\n" \
"#define P1\t\t\t2130706433u\n" \
"#define Q1\t\t\t2164260865u\n" \
"#define RSQ1\t\t402124772u\n" \
"#define IM1\t\t\t2063729671u\n" \
"#define MFIM1\t\t1930170389u\n" \
"#define SQRTI1\t\t1626730317u\n" \
"#define ISQRTI1\t\t856006302u\n" \
"#define P2\t\t\t2113929217u\n" \
"#define Q2\t\t\t2181038081u\n" \
"#define RSQ2\t\t2111798781u\n" \
"#define IM2\t\t\t530075385u\n" \
"#define MFIM2\t\t1036950657u\n" \
"#define SQRTI2\t\t338852760u\n" \
"#define ISQRTI2\t\t1090446030u\n" \
"#define P3\t\t\t2013265921u\n" \
"#define Q3\t\t\t2281701377u\n" \
"#define RSQ3\t\t1172168163u\n" \
"#define IM3\t\t\t473486609u\n" \
"#define MFIM3\t\t734725699u\n" \
"#define SQRTI3\t\t1032137103u\n" \
"#define ISQRTI3\t\t1964242958u\n" \
"#define INVP2_P1\t2130706177u\n" \
"#define INVP3_P1\t608773230u\n" \
"#define INVP3_P2\t1409286102u\n" \
"#define P1P2P3L\t\t1962934273u\n" \
"#define P1P2P3H\t\t2111326211158966273ul\n" \
"#define P1P2P3_2L\t3128950784u\n" \
"#define P1P2P3_2H\t1055663105579483136ul\n" \
"#define NORM1\t\t2130641409u\n" \
"#define NORM2\t\t2113864705u\n" \
"#define NORM3\t\t2013204481u\n" \
"#define W_SZ\t\t32768u\n" \
"#define OCL_VSIZE\t\t4\n" \
"#define OCL_CARRY_VSIZE\t1\n" \
"#define CARRY_LENGTH\t4\n" \
"#define CARRY_WG_SZ\t\t128u\n" \
"#define BLK16\t\t\t32\n" \
"#define BLK32\t\t\t16\n" \
"#define BLK64\t\t\t8\n" \
"#define BLK128\t\t\t4\n" \
"#define BLK256\t\t\t2\n" \
"#define BLK512\t\t\t1\n" \
"#define CHUNK64\t\t\t8\n" \
"#define CHUNK512\t\t1\n" \
"// #define QVALID\t\t\t1\n" \
"#endif\n" \
"\n" \
"#define N_VSIZE\t\t(N_SZ * VSIZE)\n" \
"#define N_VLEN\t\t(N_SZ * VSIZE / OCL_VSIZE)\n" \
"\n" \
"#define CARRY_VSIZE\t\t(OCL_VSIZE / OCL_CARRY_VSIZE)\n" \
"#define N_CARRY_VSIZE\t(N_VSIZE / OCL_CARRY_VSIZE)\n" \
"\n" \
"typedef uint\tsz_t;\n" \
"typedef uint\tuint_32;\n" \
"typedef int\t\tint_32;\n" \
"typedef ulong\tuint_64;\n" \
"typedef long\tint_64;\n" \
"typedef uint2\tuint2_32;\n" \
"typedef int2\tint2_32;\n" \
"typedef long2\tint2_64;\n" \
"typedef uint4\tuint4_32;\n" \
"typedef int4\tint4_32;\n" \
"typedef long4\tint4_64;\n" \
"typedef uint8\tuint8_32;\n" \
"\n" \
"// --- modular arithmetic\n" \
"\n" \
"#define\tPQ1\t\t(uint2_32)(P1, Q1)\n" \
"#define\tPQ2\t\t(uint2_32)(P2, Q2)\n" \
"#define\tPQ3\t\t(uint2_32)(P3, Q3)\n" \
"\n" \
"__constant uint2_32 g_pq[3] = { PQ1, PQ2, PQ3 };\n" \
"__constant uint4_32 g_f0[3] = { (uint4_32)(RSQ1, MFIM1, SQRTI1, ISQRTI1), (uint4_32)(RSQ2, MFIM2, SQRTI2, ISQRTI2), (uint4_32)(RSQ3, MFIM3, SQRTI3, ISQRTI3) };\n" \
"__constant uint4_32 g_f0i[3] = { (uint4_32)(NORM1, IM1, SQRTI1, ISQRTI1), (uint4_32)(NORM2, IM2, SQRTI2, ISQRTI2), (uint4_32)(NORM3, IM3, SQRTI3, ISQRTI3) };\n" \
"\n" \
"INLINE uint_32 addmod(const uint_32 lhs, const uint_32 rhs, const uint_32 p)\n" \
"{\n" \
"#if defined(IS32)\n" \
"\treturn lhs + rhs - ((lhs >= p - rhs) ? p : 0);\n" \
"#else\n" \
"\tconst uint_32 t = lhs + rhs;\n" \
"\treturn t - ((t >= p) ? p : 0);\n" \
"#endif\n" \
"}\n" \
"\n" \
"INLINE uint_32 submod(const uint_32 lhs, const uint_32 rhs, const uint_32 p)\n" \
"{\n" \
"#if defined(IS32)\n" \
"\treturn lhs - rhs + ((lhs < rhs) ? p : 0);\n" \
"#else\n" \
"\tconst uint_32 t = lhs - rhs;\n" \
"\treturn t + (((int_32)(t) < 0) ? p : 0);\n" \
"#endif\n" \
"}\n" \
"\n" \
"// 2 mul + 2 mul_hi\n" \
"INLINE uint_32 mulmod(const uint_32 lhs, const uint_32 rhs, const uint2_32 pq)\n" \
"{\n" \
"\tconst uint_64 t = lhs * (uint_64)(rhs);\n" \
"\tconst uint_32 lo = (uint_32)(t), hi = (uint_32)(t >> 32);\n" \
"\tconst uint_32 mp = mul_hi(lo * pq.s1, pq.s0);\n" \
"\treturn submod(hi, mp, pq.s0);\n" \
"}\n" \
"\n" \
"INLINE int_32 get_int(const uint_32 n, const uint_32 p) { return (int_32)(n - ((n >= p / 2) ? p : 0)); }\n" \
"INLINE uint_32 set_int(const int_32 i, const uint_32 p) { return (uint_32)(i + ((i < 0) ? p : 0)); }\n" \
"\n" \
"// --- v2\n" \
"\n" \
"INLINE uint2_32 addmod2(const uint2_32 lhs, const uint2_32 rhs, const uint_32 p)\n" \
"{\n" \
"\treturn (uint2_32)(addmod(lhs.s0, rhs.s0, p), addmod(lhs.s1, rhs.s1, p));\n" \
"}\n" \
"\n" \
"INLINE uint2_32 submod2(const uint2_32 lhs, const uint2_32 rhs, const uint_32 p)\n" \
"{\n" \
"\treturn (uint2_32)(submod(lhs.s0, rhs.s0, p), submod(lhs.s1, rhs.s1, p));\n" \
"}\n" \
"\n" \
"INLINE uint2_32 mulmod2(const uint2_32 lhs, const uint2_32 rhs, const uint2_32 pq)\n" \
"{\n" \
"\treturn (uint2_32)(mulmod(lhs.s0, rhs.s0, pq), mulmod(lhs.s1, rhs.s1, pq));\n" \
"}\n" \
"\n" \
"INLINE uint2_32 mulmods2(const uint2_32 lhs, const uint_32 rhs, const uint2_32 pq)\n" \
"{\n" \
"\treturn (uint2_32)(mulmod(lhs.s0, rhs, pq), mulmod(lhs.s1, rhs, pq));\n" \
"}\n" \
"\n" \
"INLINE int2_32 get_int2(const uint2_32 n, const uint_32 p) { return (int2_32)(get_int(n.s0, p), get_int(n.s1, p)); }\n" \
"INLINE uint2_32 set_int2(const int2_32 i, const uint_32 p) { return (uint2_32)(set_int(i.s0, p), set_int(i.s1, p)); }\n" \
"\n" \
"// --- v4\n" \
"\n" \
"INLINE uint4_32 addmod4(const uint4_32 lhs, const uint4_32 rhs, const uint_32 p)\n" \
"{\n" \
"\treturn (uint4_32)(addmod2(lhs.s01, rhs.s01, p), addmod2(lhs.s23, rhs.s23, p));\n" \
"}\n" \
"\n" \
"INLINE uint4_32 submod4(const uint4_32 lhs, const uint4_32 rhs, const uint_32 p)\n" \
"{\n" \
"\treturn (uint4_32)(submod2(lhs.s01, rhs.s01, p), submod2(lhs.s23, rhs.s23, p));\n" \
"}\n" \
"\n" \
"INLINE uint4_32 mulmod4(const uint4_32 lhs, const uint4_32 rhs, const uint2_32 pq)\n" \
"{\n" \
"\treturn (uint4_32)(mulmod2(lhs.s01, rhs.s01, pq), mulmod2(lhs.s23, rhs.s23, pq));\n" \
"}\n" \
"\n" \
"INLINE uint4_32 mulmods4(const uint4_32 lhs, const uint_32 rhs, const uint2_32 pq)\n" \
"{\n" \
"\treturn (uint4_32)(mulmods2(lhs.s01, rhs, pq), mulmods2(lhs.s23, rhs, pq));\n" \
"}\n" \
"\n" \
"INLINE int4_32 get_int4(const uint4_32 n, const uint_32 p) { return (int4_32)(get_int2(n.s01, p), get_int2(n.s23, p)); }\n" \
"INLINE uint4_32 set_int4(const int4_32 i, const uint_32 p) { return (uint4_32)(set_int2(i.s01, p), set_int2(i.s23, p)); }\n" \
"\n" \
"// --- uint96/int96 ---\n" \
"\n" \
"typedef struct { uint_32 s0; uint_64 s1; } uint96;\n" \
"typedef struct { uint_32 s0; int_64 s1; } int96;\n" \
"\n" \
"INLINE int96 uint96_i(const uint96 x) { int96 r; r.s0 = x.s0; r.s1 = (int_64)(x.s1); return r; }\n" \
"\n" \
"INLINE uint96 uint96_set(const uint_32 s0, const uint_64 s1) { uint96 r; r.s0 = s0; r.s1 = s1; return r; }\n" \
"\n" \
"INLINE int96 int96_set_si(const int_64 n) { int96 r; r.s0 = (uint_32)(n); r.s1 = n >> 32; return r; }\n" \
"\n" \
"INLINE bool int96_is_neg(const int96 x) { return (x.s1 < 0); }\n" \
"\n" \
"INLINE bool uint96_is_greater(const uint96 x, const uint96 y) { return (x.s1 > y.s1) || ((x.s1 == y.s1) && (x.s0 > y.s0)); }\n" \
"\n" \
"INLINE uint96 uint96_add_64(const uint96 x, const uint_64 y)\n" \
"{\n" \
"\tconst uint_32 yl = (uint_32)(y); const uint_64 yh = y >> 32;\n" \
"\tuint96 r;\n" \
"#if defined(PTX_ASM)\n" \
"\tasm volatile (\"add.cc.u32 %0, %1, %2;\" : \"=r\" (r.s0) : \"r\" (x.s0), \"r\" (yl));\n" \
"\tasm volatile (\"addc.u64 %0, %1, %2;\" : \"=l\" (r.s1) : \"l\" (x.s1), \"l\" (yh));\n" \
"#else\n" \
"\tconst uint_32 s0 = x.s0 + yl;\n" \
"\tr.s0 = s0; r.s1 = x.s1 + yh + ((s0 < x.s0) ? 1 : 0);\n" \
"#endif\n" \
"\treturn r;\n" \
"}\n" \
"\n" \
"INLINE int96 int96_add_64(const int96 x, const int_64 y)\n" \
"{\n" \
"\tconst uint_32 yl = (uint_32)(y); const int_64 yh = y >> 32;\n" \
"\tint96 r;\n" \
"#if defined(PTX_ASM)\n" \
"\tasm volatile (\"add.cc.u32 %0, %1, %2;\" : \"=r\" (r.s0) : \"r\" (x.s0), \"r\" (yl));\n" \
"\tasm volatile (\"addc.s64 %0, %1, %2;\" : \"=l\" (r.s1) : \"l\" (x.s1), \"l\" (yh));\n" \
"#else\n" \
"\tconst uint_32 s0 = x.s0 + yl;\n" \
"\tr.s0 = s0; r.s1 = x.s1 + yh + ((s0 < x.s0) ? 1 : 0);\n" \
"#endif\n" \
"\treturn r;\n" \
"}\n" \
"\n" \
"INLINE int96 int96_add(const int96 x, const int96 y)\n" \
"{\n" \
"\tint96 r;\n" \
"#if defined(PTX_ASM)\n" \
"\tasm volatile (\"add.cc.u32 %0, %1, %2;\" : \"=r\" (r.s0) : \"r\" (x.s0), \"r\" (y.s0));\n" \
"\tasm volatile (\"addc.s64 %0, %1, %2;\" : \"=l\" (r.s1) : \"l\" (x.s1), \"l\" (y.s1));\n" \
"#else\n" \
"\tconst uint_32 s0 = x.s0 + y.s0;\n" \
"\tr.s0 = s0; r.s1 = x.s1 + y.s1 + ((s0 < x.s0) ? 1 : 0);\n" \
"#endif\n" \
"\treturn r;\n" \
"}\n" \
"\n" \
"INLINE uint96 uint96_sub(const uint96 x, const uint96 y)\n" \
"{\n" \
"\tuint96 r;\n" \
"#if defined(PTX_ASM)\n" \
"\tasm volatile (\"sub.cc.u32 %0, %1, %2;\" : \"=r\" (r.s0) : \"r\" (x.s0), \"r\" (y.s0));\n" \
"\tasm volatile (\"subc.u64 %0, %1, %2;\" : \"=l\" (r.s1) : \"l\" (x.s1), \"l\" (y.s1));\n" \
"#else\n" \
"\tr.s0 = x.s0 - y.s0; r.s1 = (int_64)(x.s1 - y.s1 - ((x.s0 < y.s0) ? 1 : 0));\n" \
"#endif\n" \
"\treturn r;\n" \
"}\n" \
"\n" \
"INLINE uint96 int96_abs(const int96 x)\n" \
"{\n" \
"\tconst bool is_neg = int96_is_neg(x);\n" \
"\tconst uint96 mask = uint96_set(is_neg ? ~0u : 0u, is_neg ? ~0ul : 0ul);\n" \
"\tconst uint96 t = uint96_set(x.s0 ^ mask.s0, (uint_64)(x.s1) ^ mask.s1);\n" \
"\treturn uint96_sub(t, mask);\n" \
"}\n" \
"\n" \
"INLINE uint96 uint96_mul_64_32(const uint_64 x, const uint_32 y)\n" \
"{\n" \
"\tconst uint_64 l = (uint_32)(x) * (uint_64)(y);\n" \
"\tuint96 r; r.s0 = (uint_32)(l); r.s1 = (x >> 32) * y + (l >> 32);\n" \
"\treturn r;\n" \
"}\n" \
"\n" \
"// --- internal vector size (1, 2 or 4) ---\n" \
"\n" \
"#if OCL_VSIZE == 4\n" \
"#define VTYPE\t\tuint4_32\n" \
"#define addmodv\t\taddmod4\n" \
"#define submodv\t\tsubmod4\n" \
"#define mulmodv\t\tmulmod4\n" \
"#define mulmodsv\tmulmods4\n" \
"#elif OCL_VSIZE == 2\n" \
"#define VTYPE\t\tuint2_32\n" \
"#define addmodv\t\taddmod2\n" \
"#define submodv\t\tsubmod2\n" \
"#define mulmodv\t\tmulmod2\n" \
"#define mulmodsv\tmulmods2\n" \
"#else\n" \
"#define VTYPE\t\tuint_32\n" \
"#define addmodv\t\taddmod\n" \
"#define submodv\t\tsubmod\n" \
"#define mulmodv\t\tmulmod\n" \
"#define mulmodsv\tmulmod\n" \
"#endif\n" \
"\n" \
"// --- I/O ---\n" \
"\n" \
"INLINE void _loadg(const sz_t n, VTYPE * const zl, __global const VTYPE * restrict const z, const sz_t s) { for (sz_t l = 0; l < n; ++l) zl[l] = z[l * s]; }\n" \
"INLINE void _storeg(const sz_t n, __global VTYPE * restrict const z, const sz_t s, const VTYPE * const zl) { for (sz_t l = 0; l < n; ++l) z[l * s] = zl[l]; }\n" \
"\n" \
"INLINE void _loadg_1(const sz_t n, uint_32 * const zl, __global const uint_32 * restrict const z, const sz_t s) { for (sz_t l = 0; l < n; ++l) zl[l] = z[l * s]; }\n" \
"INLINE void _storeg_1(const sz_t n, __global uint_32 * restrict const z, const sz_t s, const uint_32 * const zl) { for (sz_t l = 0; l < n; ++l) z[l * s] = zl[l]; }\n" \
"\n" \
"INLINE uint2_32 _load2g(__global const uint_32 * restrict const w, const sz_t j) { return ((__global const uint2_32 *)w)[j]; }\n" \
"INLINE uint4_32 _load4g(__global const uint_32 * restrict const w, const sz_t j) { return ((__global const uint4_32 *)w)[j]; }\n" \
"\n" \
"INLINE void _loadl(const sz_t n, VTYPE * const zl, __local const VTYPE * restrict const Z, const sz_t s) { for (sz_t l = 0; l < n; ++l) zl[l] = Z[l * s]; }\n" \
"INLINE void _storel(const sz_t n, __local VTYPE * restrict const Z, const sz_t s, const VTYPE * const zl) { for (sz_t l = 0; l < n; ++l) Z[l * s] = zl[l]; }\n" \
"\n" \
"// --- transform/macro ---\n" \
"\n" \
"#define FWD2(z0, z1, w) \\\n" \
"{ \\\n" \
"\tconst VTYPE t = mulmodsv(z1, w, pq); \\\n" \
"\tz1 = submodv(z0, t, pq.s0); z0 = addmodv(z0, t, pq.s0); \\\n" \
"}\n" \
"\n" \
"#define BCK2(z0, z1, wi) \\\n" \
"{ \\\n" \
"\tconst VTYPE t = submodv(z1, z0, pq.s0); z0 = addmodv(z0, z1, pq.s0); \\\n" \
"\tz1 = mulmodsv(t, wi, pq); \\\n" \
"}\n" \
"\n" \
"#define SQR2(z0, z1, w) \\\n" \
"{ \\\n" \
"\tconst VTYPE t = mulmodsv(mulmodv(z1, z1, pq), w, pq); \\\n" \
"\tz1 = mulmodv(addmodv(z0, z0, pq.s0), z1, pq); \\\n" \
"\tz0 = addmodv(mulmodv(z0, z0, pq), t, pq.s0); \\\n" \
"}\n" \
"\n" \
"#define SQR2N(z0, z1, w) \\\n" \
"{ \\\n" \
"\tconst VTYPE t = mulmodsv(mulmodv(z1, z1, pq), w, pq); \\\n" \
"\tz1 = mulmodv(addmodv(z0, z0, pq.s0), z1, pq); \\\n" \
"\tz0 = submodv(mulmodv(z0, z0, pq), t, pq.s0); \\\n" \
"}\n" \
"\n" \
"#define MUL2(z0, z1, zp0, zp1, w) \\\n" \
"{ \\\n" \
"\tconst VTYPE t = mulmodsv(mulmodv(z1, zp1, pq), w, pq); \\\n" \
"\tz1 = addmodv(mulmodv(z0, zp1, pq), mulmodv(zp0, z1, pq), pq.s0); \\\n" \
"\tz0 = addmodv(mulmodv(z0, zp0, pq), t, pq.s0); \\\n" \
"}\n" \
"\n" \
"#define MUL2N(z0, z1, zp0, zp1, w) \\\n" \
"{ \\\n" \
"\tconst VTYPE t = mulmodsv(mulmodv(z1, zp1, pq), w, pq); \\\n" \
"\tz1 = addmodv(mulmodv(z0, zp1, pq), mulmodv(zp0, z1, pq), pq.s0); \\\n" \
"\tz0 = submodv(mulmodv(z0, zp0, pq), t, pq.s0); \\\n" \
"}\n" \
"\n" \
"#define FWD2_1(z0, z1, w) \\\n" \
"{ \\\n" \
"\tconst uint_32 t = mulmod(z1, w, pq); \\\n" \
"\tz1 = submod(z0, t, pq.s0); z0 = addmod(z0, t, pq.s0); \\\n" \
"}\n" \
"\n" \
"#define BCK2_1(z0, z1, wi) \\\n" \
"{ \\\n" \
"\tconst uint_32 t = submod(z1, z0, pq.s0); z0 = addmod(z0, z1, pq.s0); \\\n" \
"\tz1 = mulmod(t, wi, pq); \\\n" \
"}\n" \
"\n" \
"#define MUL2_1(z0, z1, zp0, zp1, w) \\\n" \
"{ \\\n" \
"\tconst uint_32 t = mulmod(mulmod(z1, zp1, pq), w, pq); \\\n" \
"\tz1 = addmod(mulmod(z0, zp1, pq), mulmod(zp0, z1, pq), pq.s0); \\\n" \
"\tz0 = addmod(mulmod(z0, zp0, pq), t, pq.s0); \\\n" \
"}\n" \
"\n" \
"#define MUL2N_1(z0, z1, zp0, zp1, w) \\\n" \
"{ \\\n" \
"\tconst uint_32 t = mulmod(mulmod(z1, zp1, pq), w, pq); \\\n" \
"\tz1 = addmod(mulmod(z0, zp1, pq), mulmod(zp0, z1, pq), pq.s0); \\\n" \
"\tz0 = submod(mulmod(z0, zp0, pq), t, pq.s0); \\\n" \
"}\n" \
"\n" \
"// --- transform/inline ---\n" \
"\n" \
"INLINE void _forward8(const uint2_32 pq, VTYPE z[8], const uint_32 w1, const uint2_32 w2, const uint4_32 w4)\n" \
"{\n" \
"\tFWD2(z[0], z[4], w1); FWD2(z[2], z[6], w1); FWD2(z[1], z[5], w1); FWD2(z[3], z[7], w1);\n" \
"\tFWD2(z[0], z[2], w2.s0); FWD2(z[1], z[3], w2.s0); FWD2(z[4], z[6], w2.s1); FWD2(z[5], z[7], w2.s1);\n" \
"\tFWD2(z[0], z[1], w4.s0); FWD2(z[2], z[3], w4.s1); FWD2(z[4], z[5], w4.s2); FWD2(z[6], z[7], w4.s3);\n" \
"}\n" \
"\n" \
"INLINE void _backward8r(const uint2_32 pq, VTYPE z[8], const uint_32 wi1, const uint2_32 wi2r, const uint4_32 wi4r)\n" \
"{\n" \
"\tBCK2(z[0], z[1], wi4r.s3); BCK2(z[2], z[3], wi4r.s2); BCK2(z[4], z[5], wi4r.s1); BCK2(z[6], z[7], wi4r.s0);\n" \
"\tBCK2(z[0], z[2], wi2r.s1); BCK2(z[1], z[3], wi2r.s1); BCK2(z[4], z[6], wi2r.s0); BCK2(z[5], z[7], wi2r.s0);\n" \
"\tBCK2(z[0], z[4], wi1); BCK2(z[2], z[6], wi1); BCK2(z[1], z[5], wi1); BCK2(z[3], z[7], wi1);\n" \
"}\n" \
"\n" \
"INLINE void _forward8_0(const uint2_32 pq, const uint4_32 f0, VTYPE z[8], const uint4_32 w4)\n" \
"{\n" \
"\tfor (sz_t i = 0; i < 4; ++i) z[i] = mulmodsv(z[i], f0.s0, pq);\n" \
"\t_forward8(pq, z, f0.s1, f0.s23, w4);\n" \
"}\n" \
"\n" \
"INLINE void _backward8r_0(const uint2_32 pq, const uint4_32 f0i, VTYPE z[8], const uint4_32 wi4r)\n" \
"{\n" \
"\t_backward8r(pq, z, f0i.s1, f0i.s23, wi4r);\n" \
"\tfor (sz_t i = 0; i < 8; ++i) z[i] = mulmodsv(z[i], f0i.s0, pq);\n" \
"}\n" \
"\n" \
"INLINE void _square2x4(const uint2_32 pq, VTYPE z[8], const uint2_32 w2)\n" \
"{\n" \
"\tSQR2(z[0], z[1], w2.s0); SQR2N(z[2], z[3], w2.s0); SQR2(z[4], z[5], w2.s1); SQR2N(z[6], z[7], w2.s1);\n" \
"}\n" \
"\n" \
"INLINE void _square4x2r(const uint2_32 pq, VTYPE z[8], const uint2_32 w2, const uint2_32 wi2r)\n" \
"{\n" \
"\tFWD2(z[0], z[2], w2.s0); FWD2(z[1], z[3], w2.s0); FWD2(z[4], z[6], w2.s1); FWD2(z[5], z[7], w2.s1);\n" \
"\t_square2x4(pq, z, w2);\n" \
"\tBCK2(z[0], z[2], wi2r.s1); BCK2(z[1], z[3], wi2r.s1); BCK2(z[4], z[6], wi2r.s0); BCK2(z[5], z[7], wi2r.s0);\n" \
"}\n" \
"\n" \
"INLINE void _square8r(const uint2_32 pq, VTYPE z[8], const uint_32 w1, const uint_32 wi1, const uint2_32 w2, const uint2_32 wi2r)\n" \
"{\n" \
"\tFWD2(z[0], z[4], w1); FWD2(z[2], z[6], w1); FWD2(z[1], z[5], w1); FWD2(z[3], z[7], w1);\n" \
"\t_square4x2r(pq, z, w2, wi2r);\n" \
"\tBCK2(z[0], z[4], wi1); BCK2(z[2], z[6], wi1); BCK2(z[1], z[5], wi1); BCK2(z[3], z[7], wi1);\n" \
"}\n" \
"\n" \
"INLINE void _fwd4x2(const uint2_32 pq, VTYPE z[8], const uint2_32 w2)\n" \
"{\n" \
"\tFWD2(z[0], z[2], w2.s0); FWD2(z[1], z[3], w2.s0); FWD2(z[4], z[6], w2.s1); FWD2(z[5], z[7], w2.s1);\n" \
"}\n" \
"\n" \
"INLINE void _fwd8(const uint2_32 pq, VTYPE z[8], const uint_32 w1, const uint2_32 w2)\n" \
"{\n" \
"\tFWD2(z[0], z[4], w1); FWD2(z[2], z[6], w1); FWD2(z[1], z[5], w1); FWD2(z[3], z[7], w1);\n" \
"\t_fwd4x2(pq, z, w2);\n" \
"}\n" \
"\n" \
"INLINE void _mul2x4(const uint2_32 pq, VTYPE z[8], const VTYPE zp[8], const uint2_32 w2)\n" \
"{\n" \
"\tMUL2(z[0], z[1], zp[0], zp[1], w2.s0); MUL2N(z[2], z[3], zp[2], zp[3], w2.s0);\n" \
"\tMUL2(z[4], z[5], zp[4], zp[5], w2.s1); MUL2N(z[6], z[7], zp[6], zp[7], w2.s1);\n" \
"}\n" \
"\n" \
"INLINE void _mul4x2r(const uint2_32 pq, VTYPE z[8], const VTYPE zp[8],\n" \
"\tconst uint2_32 w2, const uint2_32 wi2r)\n" \
"{\n" \
"\tFWD2(z[0], z[2], w2.s0); FWD2(z[1], z[3], w2.s0); FWD2(z[4], z[6], w2.s1); FWD2(z[5], z[7], w2.s1);\n" \
"\t_mul2x4(pq, z, zp, w2);\n" \
"\tBCK2(z[0], z[2], wi2r.s1); BCK2(z[1], z[3], wi2r.s1); BCK2(z[4], z[6], wi2r.s0); BCK2(z[5], z[7], wi2r.s0);\n" \
"}\n" \
"\n" \
"INLINE void _mul8r(const uint2_32 pq, VTYPE z[8], const VTYPE zp[8],\n" \
"\tconst uint_32 w1, const uint_32 wi1, const uint2_32 w2, const uint2_32 wi2r)\n" \
"{\n" \
"\tFWD2(z[0], z[4], w1); FWD2(z[2], z[6], w1); FWD2(z[1], z[5], w1); FWD2(z[3], z[7], w1);\n" \
"\t_mul4x2r(pq, z, zp, w2, wi2r);\n" \
"\tBCK2(z[0], z[4], wi1); BCK2(z[2], z[6], wi1); BCK2(z[1], z[5], wi1); BCK2(z[3], z[7], wi1);\n" \
"}\n" \
"\n" \
"INLINE void _mul2x4_1(const uint2_32 pq, uint_32 z[8], const uint_32 zp[8], const uint2_32 w2)\n" \
"{\n" \
"\tMUL2_1(z[0], z[1], zp[0], zp[1], w2.s0); MUL2N_1(z[2], z[3], zp[2], zp[3], w2.s0);\n" \
"\tMUL2_1(z[4], z[5], zp[4], zp[5], w2.s1); MUL2N_1(z[6], z[7], zp[6], zp[7], w2.s1);\n" \
"}\n" \
"\n" \
"INLINE void _mul4x2r_1(const uint2_32 pq, uint_32 z[8], const uint_32 zp[8],\n" \
"\tconst uint2_32 w2, const uint2_32 wi2r, const bool bmask)\n" \
"{\n" \
"\tFWD2_1(z[0], z[2], w2.s0); FWD2_1(z[1], z[3], w2.s0); FWD2_1(z[4], z[6], w2.s1); FWD2_1(z[5], z[7], w2.s1);\n" \
"\tif (bmask) _mul2x4_1(pq, z, zp, w2);\n" \
"\tBCK2_1(z[0], z[2], wi2r.s1); BCK2_1(z[1], z[3], wi2r.s1); BCK2_1(z[4], z[6], wi2r.s0); BCK2_1(z[5], z[7], wi2r.s0);\n" \
"}\n" \
"\n" \
"INLINE void _mul8r_1(const uint2_32 pq, uint_32 z[8], const uint_32 zp[8],\n" \
"\tconst uint_32 w1, const uint_32 wi1, const uint2_32 w2, const uint2_32 wi2r, const bool bmask)\n" \
"{\n" \
"\tFWD2_1(z[0], z[4], w1); FWD2_1(z[2], z[6], w1); FWD2_1(z[1], z[5], w1); FWD2_1(z[3], z[7], w1);\n" \
"\t_mul4x2r_1(pq, z, zp, w2, wi2r, bmask);\n" \
"\tBCK2_1(z[0], z[4], wi1); BCK2_1(z[2], z[6], wi1); BCK2_1(z[1], z[5], wi1); BCK2_1(z[3], z[7], wi1);\n" \
"}\n" \
"\n" \
"// ---\n" \
"\n" \
"#define LOADG(s)\tVTYPE zl[8]; _loadg(8, zl, z, s);\n" \
"#define LOADPG(s)\tVTYPE zpl[8]; _loadg(8, zpl, zp, s);\n" \
"#define STOREG(s)\t_storeg(8, z, s, zl);\n" \
"#define LOADL(s) \\\n" \
"\tbarrier(CLK_LOCAL_MEM_FENCE); \\\n" \
"\tVTYPE zl[8]; _loadl(8, zl, Z, s);\n" \
"#define STOREL(s)\t_storel(8, Z, s, zl);\n" \
"#define LOADG1(s) \\\n" \
"\tuint_32 zl[8];\t_loadg_1(8, zl, z, s); \\\n" \
"\tuint_32 zpl[8];\t_loadg_1(8, zpl, zp, s);\n" \
"#define STOREG1(s)\t_storeg_1(8, z, s, zl);\n" \
"\n" \
"#define DECLARE_W124() \\\n" \
"\tconst uint_32 w1 = w[sj]; \\\n" \
"\tconst uint2_32 w2 = _load2g(w, sj); \\\n" \
"\tconst uint4_32 w4 = _load4g(w, sj);\n" \
"\n" \
"#define DECLARE_WI124() \\\n" \
"\tconst uint_32 wi1 = w[sji]; \\\n" \
"\tconst uint2_32 wi2r = _load2g(w, sji); \\\n" \
"\tconst uint4_32 wi4r = _load4g(w, sji);\n" \
"\n" \
"#define DECLARE_W2I2()\tconst uint2_32 w2 = _load2g(w, sj), wi2r = _load2g(w, sji);\n" \
"\n" \
"#define DECLARE_W12I12() \\\n" \
"\tconst uint_32 w1 = w[sj], wi1 = w[sji]; \\\n" \
"\tDECLARE_W2I2();\n" \
"\n" \
"\n" \
"#if defined(SQ128_PAD_GROUP) && (SQ128_PAD_GROUP != 0)\n" \
"\n" \
"INLINE sz_t sq128_pad_index(const sz_t i)\n" \
"{\n" \
"\treturn i + i / SQ128_PAD_GROUP;\n" \
"}\n" \
"\n" \
"INLINE void sq128_loadl(\n" \
"\tconst sz_t n,\n" \
"\tVTYPE * const zl,\n" \
"\t__local const VTYPE * restrict const Z,\n" \
"\tconst sz_t logical_base,\n" \
"\tconst sz_t logical_stride)\n" \
"{\n" \
"\tbarrier(CLK_LOCAL_MEM_FENCE);\n" \
"\n" \
"\tfor (sz_t l = 0; l < n; ++l)\n" \
"\t\tzl[l] = Z[\n" \
"\t\t\tsq128_pad_index(\n" \
"\t\t\t\tlogical_base + l * logical_stride\n" \
"\t\t\t)\n" \
"\t\t];\n" \
"}\n" \
"\n" \
"INLINE void sq128_storel(\n" \
"\tconst sz_t n,\n" \
"\t__local VTYPE * restrict const Z,\n" \
"\tconst sz_t logical_base,\n" \
"\tconst sz_t logical_stride,\n" \
"\tconst VTYPE * const zl)\n" \
"{\n" \
"\tfor (sz_t l = 0; l < n; ++l)\n" \
"\t\tZ[\n" \
"\t\t\tsq128_pad_index(\n" \
"\t\t\t\tlogical_base + l * logical_stride\n" \
"\t\t\t)\n" \
"\t\t] = zl[l];\n" \
"}\n" \
"\n" \
"INLINE void forward8i_sq128_pad(\n" \
"\tconst uint2_32 pq,\n" \
"\tconst sz_t logical_base,\n" \
"\tconst sz_t local_stride,\n" \
"\t__local VTYPE * restrict const Z,\n" \
"\tconst sz_t global_stride,\n" \
"\t__global VTYPE * restrict const z,\n" \
"\t__global const uint_32 * restrict const w,\n" \
"\tconst sz_t sj)\n" \
"{\n" \
"\tDECLARE_W124();\n" \
"\tLOADG(global_stride);\n" \
"\n" \
"\t_forward8(\n" \
"\t\tpq,\n" \
"\t\tzl,\n" \
"\t\tw1,\n" \
"\t\tw2,\n" \
"\t\tw4\n" \
"\t);\n" \
"\n" \
"\tsq128_storel(\n" \
"\t\t8,\n" \
"\t\tZ,\n" \
"\t\tlogical_base,\n" \
"\t\tlocal_stride,\n" \
"\t\tzl\n" \
"\t);\n" \
"}\n" \
"\n" \
"INLINE void forward8l_sq128_pad(\n" \
"\tconst uint2_32 pq,\n" \
"\tconst sz_t logical_base,\n" \
"\tconst sz_t local_stride,\n" \
"\t__local VTYPE * restrict const Z,\n" \
"\t__global const uint_32 * restrict const w,\n" \
"\tconst sz_t sj)\n" \
"{\n" \
"\tDECLARE_W124();\n" \
"\tVTYPE zl[8];\n" \
"\n" \
"\tsq128_loadl(\n" \
"\t\t8,\n" \
"\t\tzl,\n" \
"\t\tZ,\n" \
"\t\tlogical_base,\n" \
"\t\tlocal_stride\n" \
"\t);\n" \
"\n" \
"\t_forward8(\n" \
"\t\tpq,\n" \
"\t\tzl,\n" \
"\t\tw1,\n" \
"\t\tw2,\n" \
"\t\tw4\n" \
"\t);\n" \
"\n" \
"\tsq128_storel(\n" \
"\t\t8,\n" \
"\t\tZ,\n" \
"\t\tlogical_base,\n" \
"\t\tlocal_stride,\n" \
"\t\tzl\n" \
"\t);\n" \
"}\n" \
"\n" \
"INLINE void square2x4l_sq128_pad(\n" \
"\tconst uint2_32 pq,\n" \
"\tconst sz_t logical_base,\n" \
"\t__local VTYPE * restrict const Z,\n" \
"\t__global const uint_32 * restrict const w,\n" \
"\tconst sz_t sj)\n" \
"{\n" \
"\tconst uint2_32 w2 = _load2g(w, sj);\n" \
"\tVTYPE zl[8];\n" \
"\n" \
"\tsq128_loadl(\n" \
"\t\t8,\n" \
"\t\tzl,\n" \
"\t\tZ,\n" \
"\t\tlogical_base,\n" \
"\t\t1\n" \
"\t);\n" \
"\n" \
"\t_square2x4(\n" \
"\t\tpq,\n" \
"\t\tzl,\n" \
"\t\tw2\n" \
"\t);\n" \
"\n" \
"\tsq128_storel(\n" \
"\t\t8,\n" \
"\t\tZ,\n" \
"\t\tlogical_base,\n" \
"\t\t1,\n" \
"\t\tzl\n" \
"\t);\n" \
"}\n" \
"\n" \
"INLINE void backward8l_sq128_pad(\n" \
"\tconst uint2_32 pq,\n" \
"\tconst sz_t logical_base,\n" \
"\tconst sz_t local_stride,\n" \
"\t__local VTYPE * restrict const Z,\n" \
"\t__global const uint_32 * restrict const w,\n" \
"\tconst sz_t sji)\n" \
"{\n" \
"\tDECLARE_WI124();\n" \
"\tVTYPE zl[8];\n" \
"\n" \
"\tsq128_loadl(\n" \
"\t\t8,\n" \
"\t\tzl,\n" \
"\t\tZ,\n" \
"\t\tlogical_base,\n" \
"\t\tlocal_stride\n" \
"\t);\n" \
"\n" \
"\t_backward8r(\n" \
"\t\tpq,\n" \
"\t\tzl,\n" \
"\t\twi1,\n" \
"\t\twi2r,\n" \
"\t\twi4r\n" \
"\t);\n" \
"\n" \
"\tsq128_storel(\n" \
"\t\t8,\n" \
"\t\tZ,\n" \
"\t\tlogical_base,\n" \
"\t\tlocal_stride,\n" \
"\t\tzl\n" \
"\t);\n" \
"}\n" \
"\n" \
"INLINE void backward8o_sq128_pad(\n" \
"\tconst uint2_32 pq,\n" \
"\tconst sz_t global_stride,\n" \
"\t__global VTYPE * restrict const z,\n" \
"\tconst sz_t logical_base,\n" \
"\tconst sz_t local_stride,\n" \
"\t__local VTYPE * restrict const Z,\n" \
"\t__global const uint_32 * restrict const w,\n" \
"\tconst sz_t sji)\n" \
"{\n" \
"\tDECLARE_WI124();\n" \
"\tVTYPE zl[8];\n" \
"\n" \
"\tsq128_loadl(\n" \
"\t\t8,\n" \
"\t\tzl,\n" \
"\t\tZ,\n" \
"\t\tlogical_base,\n" \
"\t\tlocal_stride\n" \
"\t);\n" \
"\n" \
"\t_backward8r(\n" \
"\t\tpq,\n" \
"\t\tzl,\n" \
"\t\twi1,\n" \
"\t\twi2r,\n" \
"\t\twi4r\n" \
"\t);\n" \
"\n" \
"\tSTOREG(global_stride);\n" \
"}\n" \
"\n" \
"#endif\n" \
"\n" \
"INLINE void forward8g(const uint2_32 pq, const sz_t m, __global VTYPE * restrict const z,\n" \
"\t__global const uint_32 * restrict const w, const sz_t sj)\n" \
"{\n" \
"\tDECLARE_W124();\n" \
"\tLOADG(m);\n" \
"\t_forward8(pq, zl, w1, w2, w4);\n" \
"\tSTOREG(m);\n" \
"}\n" \
"\n" \
"INLINE void forward8i(const uint2_32 pq, const sz_t ml, __local VTYPE * restrict const Z, const sz_t mg,\n" \
"\t__global VTYPE * restrict const z, __global const uint_32 * restrict const w, const sz_t sj)\n" \
"{\n" \
"\tDECLARE_W124();\n" \
"\tLOADG(mg);\n" \
"\t_forward8(pq, zl, w1, w2, w4);\n" \
"\tSTOREL(ml);\n" \
"}\n" \
"\n" \
"INLINE void forward8l(const uint2_32 pq, const sz_t m, __local VTYPE * restrict const Z,\n" \
"\t__global const uint_32 * restrict const w, const sz_t sj)\n" \
"{\n" \
"\tDECLARE_W124();\n" \
"\tLOADL(m);\n" \
"\t_forward8(pq, zl, w1, w2, w4);\n" \
"\tSTOREL(m);\n" \
"}\n" \
"\n" \
"INLINE void forward8o(const uint2_32 pq, const sz_t mg, __global VTYPE * restrict const z, const sz_t ml,\n" \
"\t__local VTYPE * restrict const Z, __global const uint_32 * restrict const w, const sz_t sj)\n" \
"{\n" \
"\tDECLARE_W124();\n" \
"\tLOADL(ml);\n" \
"\t_forward8(pq, zl, w1, w2, w4);\n" \
"\tSTOREG(mg);\n" \
"}\n" \
"\n" \
"INLINE void backward8g(const uint2_32 pq, const sz_t m, __global VTYPE * restrict const z,\n" \
"\t__global const uint_32 * restrict const w, const sz_t sji)\n" \
"{\n" \
"\tDECLARE_WI124();\n" \
"\tLOADG(m);\n" \
"\t_backward8r(pq, zl, wi1, wi2r, wi4r);\n" \
"\tSTOREG(m);\n" \
"}\n" \
"\n" \
"INLINE void backward8i(const uint2_32 pq, const sz_t ml, __local VTYPE * restrict const Z, const sz_t mg,\n" \
"\t__global VTYPE * restrict const z, __global const uint_32 * restrict const w, const sz_t sji)\n" \
"{\n" \
"\tDECLARE_WI124();\n" \
"\tLOADG(mg);\n" \
"\t_backward8r(pq, zl, wi1, wi2r, wi4r);\n" \
"\tSTOREL(ml);\n" \
"}\n" \
"\n" \
"INLINE void backward8l(const uint2_32 pq, const sz_t m, __local VTYPE * restrict const Z,\n" \
"\t__global const uint_32 * restrict const w, const sz_t sji)\n" \
"{\n" \
"\tDECLARE_WI124();\n" \
"\tLOADL(m);\n" \
"\t_backward8r(pq, zl, wi1, wi2r, wi4r);\n" \
"\tSTOREL(m);\n" \
"}\n" \
"\n" \
"INLINE void backward8o(const uint2_32 pq, const sz_t mg, __global VTYPE * restrict const z, const sz_t ml,\n" \
"\t__local VTYPE * restrict const Z, __global const uint_32 * restrict const w, const sz_t sji)\n" \
"{\n" \
"\tDECLARE_WI124();\n" \
"\tLOADL(ml);\n" \
"\t_backward8r(pq, zl, wi1, wi2r, wi4r);\n" \
"\tSTOREG(mg);\n" \
"}\n" \
"\n" \
"INLINE void forward8_0g(const uint2_32 pq, const uint4_32 f0, const sz_t m,\n" \
"\t__global VTYPE * restrict const z, __global const uint_32 * restrict const w)\n" \
"{\n" \
"\tconst uint4_32 w4 = _load4g(w, 1);\n" \
"\tLOADG(m);\n" \
"\t_forward8_0(pq, f0, zl, w4);\n" \
"\tSTOREG(m);\n" \
"}\n" \
"\n" \
"INLINE void forward8_0i(const uint2_32 pq, const uint4_32 f0, const sz_t ml, __local VTYPE * restrict const Z,\n" \
"\tconst sz_t mg, __global VTYPE * restrict const z, __global const uint_32 * restrict const w)\n" \
"{\n" \
"\tconst uint4_32 w4 = _load4g(w, 1);\n" \
"\tLOADG(mg);\n" \
"\t_forward8_0(pq, f0, zl, w4);\n" \
"\tSTOREL(ml);\n" \
"}\n" \
"\n" \
"INLINE void backward8_0g(const uint2_32 pq, const uint4_32 f0i, const sz_t m,\n" \
"\t__global VTYPE * restrict const z, __global const uint_32 * restrict const w)\n" \
"{\n" \
"\tconst uint4_32 wi4r = _load4g(w, 1);\n" \
"\tLOADG(m);\n" \
"\t_backward8r_0(pq, f0i, zl, wi4r);\n" \
"\tSTOREG(m);\n" \
"}\n" \
"\n" \
"INLINE void backward8_0o(const uint2_32 pq, const uint4_32 f0i, const sz_t mg, __global VTYPE * restrict const z,\n" \
"\tconst sz_t ml, __local VTYPE * restrict const Z, __global const uint_32 * restrict const w)\n" \
"{\n" \
"\tconst uint4_32 wi4r = _load4g(w, 1);\n" \
"\tLOADL(ml);\n" \
"\t_backward8r_0(pq, f0i, zl, wi4r);\n" \
"\tSTOREG(mg);\n" \
"}\n" \
"\n" \
"INLINE void square2x4g(const uint2_32 pq, __global VTYPE * restrict const z,\n" \
"\t__global const uint_32 * restrict const w, const sz_t sj)\n" \
"{\n" \
"\tconst uint2_32 w2 = _load2g(w, sj);\n" \
"\tLOADG(1);\n" \
"\t_square2x4(pq, zl, w2);\n" \
"\tSTOREG(1);\n" \
"}\n" \
"\n" \
"INLINE void square2x4l(const uint2_32 pq, __local VTYPE * restrict const Z,\n" \
"\t__global const uint_32 * restrict const w, const sz_t sj)\n" \
"{\n" \
"\tconst uint2_32 w2 = _load2g(w, sj);\n" \
"\tLOADL(1);\n" \
"\t_square2x4(pq, zl, w2);\n" \
"\tSTOREL(1);\n" \
"}\n" \
"\n" \
"INLINE void square4x2g(const uint2_32 pq, __global VTYPE * restrict const z,\n" \
"\t__global const uint_32 * restrict const w, const sz_t sj, const sz_t sji)\n" \
"{\n" \
"\tDECLARE_W2I2();\n" \
"\tLOADG(1);\n" \
"\t_square4x2r(pq, zl, w2, wi2r);\n" \
"\tSTOREG(1);\n" \
"}\n" \
"\n" \
"INLINE void square4x2l(const uint2_32 pq, __local VTYPE * restrict const Z,\n" \
"\t__global const uint_32 * restrict const w, const sz_t sj, const sz_t sji)\n" \
"{\n" \
"\tDECLARE_W2I2();\n" \
"\tLOADL(1);\n" \
"\t_square4x2r(pq, zl, w2, wi2r);\n" \
"\tSTOREL(1);\n" \
"}\n" \
"\n" \
"INLINE void square8g(const uint2_32 pq, __global VTYPE * restrict const z,\n" \
"\t__global const uint_32 * restrict const w, const sz_t sj, const sz_t sji)\n" \
"{\n" \
"\tDECLARE_W12I12();\n" \
"\tLOADG(1);\n" \
"\t_square8r(pq, zl, w1, wi1, w2, wi2r);\n" \
"\tSTOREG(1);\n" \
"}\n" \
"\n" \
"INLINE void square8l(const uint2_32 pq, __local VTYPE * restrict const Z,\n" \
"\t__global const uint_32 * restrict const w, const sz_t sj, const sz_t sji)\n" \
"{\n" \
"\tDECLARE_W12I12();\n" \
"\tLOADL(1);\n" \
"\t_square8r(pq, zl, w1, wi1, w2, wi2r);\n" \
"\tSTOREL(1);\n" \
"}\n" \
"\n" \
"INLINE void fwd4x2g(const uint2_32 pq, __global VTYPE * restrict const z,\n" \
"\t__global const uint_32 * restrict const w, const sz_t sj)\n" \
"{\n" \
"\tconst uint2_32 w2 = _load2g(w, sj);\n" \
"\tLOADG(1);\n" \
"\t_fwd4x2(pq, zl, w2);\n" \
"\tSTOREG(1);\n" \
"}\n" \
"\n" \
"INLINE void fwd4x2o(const uint2_32 pq, __global VTYPE * restrict const z, __local VTYPE * restrict const Z,\n" \
"\t__global const uint_32 * restrict const w, const sz_t sj)\n" \
"{\n" \
"\tconst uint2_32 w2 = _load2g(w, sj);\n" \
"\tLOADL(1);\n" \
"\t_fwd4x2(pq, zl, w2);\n" \
"\tSTOREG(1);\n" \
"}\n" \
"\n" \
"INLINE void fwd8g(const uint2_32 pq, __global VTYPE * restrict const z,\n" \
"\t__global const uint_32 * restrict const w, const sz_t sj)\n" \
"{\n" \
"\tconst uint_32 w1 = w[sj];\n" \
"\tconst uint2_32 w2 = _load2g(w, sj);\n" \
"\tLOADG(1);\n" \
"\t_fwd8(pq, zl, w1, w2);\n" \
"\tSTOREG(1);\n" \
"}\n" \
"\n" \
"INLINE void fwd8o(const uint2_32 pq, __global VTYPE * restrict const z, __local VTYPE * restrict const Z,\n" \
"\t__global const uint_32 * restrict const w, const sz_t sj)\n" \
"{\n" \
"\tconst uint_32 w1 = w[sj];\n" \
"\tconst uint2_32 w2 = _load2g(w, sj);\n" \
"\tLOADL(1);\n" \
"\t_fwd8(pq, zl, w1, w2);\n" \
"\tSTOREG(1);\n" \
"}\n" \
"\n" \
"INLINE void mul2x4g(const uint2_32 pq, __global VTYPE * restrict const z, __global const VTYPE * restrict const zp,\n" \
"\t__global const uint_32 * restrict const w, const sz_t sj)\n" \
"{\n" \
"\tconst uint2_32 w2 = _load2g(w, sj);\n" \
"\tLOADG(1);\n" \
"\tLOADPG(1);\n" \
"\t_mul2x4(pq, zl, zpl, w2);\n" \
"\tSTOREG(1);\n" \
"}\n" \
"\n" \
"INLINE void mul2x4l(const uint2_32 pq, __local VTYPE * restrict const Z, __global const VTYPE * restrict const zp,\n" \
"\t__global const uint_32 * restrict const w, const sz_t sj)\n" \
"{\n" \
"\tconst uint2_32 w2 = _load2g(w, sj);\n" \
"\tLOADL(1);\n" \
"\tLOADPG(1);\n" \
"\t_mul2x4(pq, zl, zpl, w2);\n" \
"\tSTOREL(1);\n" \
"}\n" \
"\n" \
"INLINE void mul4x2g(const uint2_32 pq, __global VTYPE * restrict const z, __global const VTYPE * restrict const zp,\n" \
"\t__global const uint_32 * restrict const w, const sz_t sj, const sz_t sji)\n" \
"{\n" \
"\tDECLARE_W2I2();\n" \
"\tLOADG(1);\n" \
"\tLOADPG(1);\n" \
"\t_mul4x2r(pq, zl, zpl, w2, wi2r);\n" \
"\tSTOREG(1);\n" \
"}\n" \
"\n" \
"INLINE void mul4x2l(const uint2_32 pq, __local VTYPE * restrict const Z, __global const VTYPE * restrict const zp,\n" \
"\t__global const uint_32 * restrict const w, const sz_t sj, const sz_t sji)\n" \
"{\n" \
"\tDECLARE_W2I2();\n" \
"\tLOADL(1);\n" \
"\tLOADPG(1);\n" \
"\t_mul4x2r(pq, zl, zpl, w2, wi2r);\n" \
"\tSTOREL(1);\n" \
"}\n" \
"\n" \
"INLINE void mul8g(const uint2_32 pq, __global VTYPE * restrict const z, __global const VTYPE * restrict const zp,\n" \
"\t__global const uint_32 * restrict const w, const sz_t sj, const sz_t sji)\n" \
"{\n" \
"\tDECLARE_W12I12();\n" \
"\tLOADG(1);\n" \
"\tLOADPG(1);\n" \
"\t_mul8r(pq, zl, zpl, w1, wi1, w2, wi2r);\n" \
"\tSTOREG(1);\n" \
"}\n" \
"\n" \
"INLINE void mul8l(const uint2_32 pq, __local VTYPE * restrict const Z, __global const VTYPE * restrict const zp,\n" \
"\t__global const uint_32 * restrict const w, const sz_t sj, const sz_t sji)\n" \
"{\n" \
"\tDECLARE_W12I12();\n" \
"\tLOADL(1);\n" \
"\tLOADPG(1);\n" \
"\t_mul8r(pq, zl, zpl, w1, wi1, w2, wi2r);\n" \
"\tSTOREL(1);\n" \
"}\n" \
"\n" \
"INLINE void mul2x4g_1(const uint2_32 pq, const sz_t m, __global uint_32 * restrict const z, __global const uint_32 * restrict const zp,\n" \
"\t__global const uint_32 * restrict const w, const sz_t sj)\n" \
"{\n" \
"\tconst uint2_32 w2 = _load2g(w, sj);\n" \
"\tLOADG1(m);\n" \
"\t_mul2x4_1(pq, zl, zpl, w2);\n" \
"\tSTOREG1(m);\n" \
"}\n" \
"\n" \
"INLINE void mul4x2g_1(const uint2_32 pq, const sz_t m, __global uint_32 * restrict const z, __global const uint_32 * restrict const zp,\n" \
"\t__global const uint_32 * restrict const w, const sz_t sj, const sz_t sji, const bool bmask)\n" \
"{\n" \
"\tDECLARE_W2I2();\n" \
"\tLOADG1(m);\n" \
"\t_mul4x2r_1(pq, zl, zpl, w2, wi2r, bmask);\n" \
"\tSTOREG1(m);\n" \
"}\n" \
"\n" \
"INLINE void mul8g_1(const uint2_32 pq, const sz_t m, __global uint_32 * restrict const z, __global const uint_32 * restrict const zp,\n" \
"\t__global const uint_32 * restrict const w, const sz_t sj, const sz_t sji, const bool bmask)\n" \
"{\n" \
"\tDECLARE_W12I12();\n" \
"\tLOADG1(m);\n" \
"\t_mul8r_1(pq, zl, zpl, w1, wi1, w2, wi2r, bmask);\n" \
"\tSTOREG1(m);\n" \
"}\n" \
"\n" \
"// --- transform/macro ---\n" \
"\n" \
"#define DECLARE_VAR_REG_N(LEN, TYPE) \\\n" \
"\tconst sz_t gid = (sz_t)get_global_id(0), lid = gid / (LEN / 8), vid = gid % (LEN / 8), id = gid % (N_SZ / 8); \\\n" \
"\tconst uint2_32 pq = g_pq[lid]; \\\n" \
"\t__global TYPE * restrict const z = &zg[lid * LEN]; \\\n" \
"\t__global const uint_32 * restrict const w = &wg[lid * W_SZ];\n" \
"\n" \
"#define DECLARE_VAR_REG() DECLARE_VAR_REG_N(N_VLEN, VTYPE)\n" \
"#define DECLARE_VARP_REG() __global const VTYPE * restrict const zp = &zpg[lid * N_VLEN];\n" \
"\n" \
"#define DECLARE_VAR_REG_1() DECLARE_VAR_REG_N(N_VSIZE, uint_32)\n" \
"#define DECLARE_VARP_REG_1() __global const uint_32 * restrict const zp = &zpg[N_VSIZE * lid];\n" \
"\n" \
"// --- transform without local mem ---\n" \
"\n" \
"__kernel\n" \
"void forward8(__global VTYPE * restrict const zg, __global const uint_32 * restrict const wg, const int_32 lm, const uint_32 s)\n" \
"{\n" \
"\tDECLARE_VAR_REG();\n" \
"\tconst sz_t m = 1u << lm, j = id >> lm, k = 7 * (vid & ~(m - 1)) + vid;\n" \
"\tforward8g(pq, m, &z[k], w, s + j);\n" \
"}\n" \
"\n" \
"__kernel\n" \
"void backward8(__global VTYPE * restrict const zg, __global const uint_32 * restrict const wg, const int_32 lm, const uint_32 s)\n" \
"{\n" \
"\tDECLARE_VAR_REG();\n" \
"\tconst sz_t m = 1u << lm, j = id >> lm, ji = s - j - 1, k = 7 * (vid & ~(m - 1)) + vid;\n" \
"\tbackward8g(pq, m, &z[k], w, s + ji);\n" \
"}\n" \
"\n" \
"/*__kernel\n" \
"void forward8_0(__global VTYPE * restrict const zg, __global const uint_32 * restrict const wg)\n" \
"{\n" \
"\tDECLARE_VAR_REG();\n" \
"\tconst sz_t m = N_SZ / 8, k = 7 * (vid & ~(m - 1)) + vid;\n" \
"\tforward8_0g(pq, g_f0[lid], m, &z[k], w);\n" \
"}\n" \
"\n" \
"__kernel\n" \
"void backward8_0(__global VTYPE * restrict const zg, __global const uint_32 * restrict const wg)\n" \
"{\n" \
"\tDECLARE_VAR_REG();\n" \
"\tconst sz_t m = N_SZ / 8, k = 7 * (vid & ~(m - 1)) + vid;\n" \
"\tbackward8_0g(pq, g_f0i[lid], m, &z[k], w);\n" \
"}*/\n" \
"\n" \
"#define DECLARE_VAR_FB(N, CHUNK_N) \\\n" \
"\tDECLARE_VAR_REG(); \\\n" \
"\tconst sz_t mid = vid & ~(N_SZ / 8 - 1); \\\n" \
"\t__global VTYPE * restrict const zv = &z[8 * mid]; \\\n" \
"\tconst size_t chunk_id = id % CHUNK_N, local_id = (id / CHUNK_N) % (N / 8), block_id = id & ~(N / 8 * CHUNK_N - 1); \\\n" \
"\t__global VTYPE * restrict const zt = &zv[block_id / (N / 8) + chunk_id]; \\\n" \
"\t__local VTYPE * const Zt = &Z[N * chunk_id];\n" \
"\n" \
"#if (LN_SZ <= 15)\n" \
"\n" \
"#define DECLARE_VAR_FB64() \\\n" \
"\tconst sz_t kl0 = local_id, k0 = kl0 * (N_SZ / 64), ml0 = 8, m0 = ml0 * (N_SZ / 64); \\\n" \
"\tconst sz_t kl8 = local_id * 8, k8 = kl8 * (N_SZ / 64), ml8 = 1, m8 = ml8 * (N_SZ / 64);\n" \
"\n" \
"__kernel __attribute__((reqd_work_group_size(64 / 8 * CHUNK64, 1, 1)))\n" \
"void forward64_0(__global VTYPE * restrict const zg, __global const uint_32 * restrict const wg, __local VTYPE * const Z)\n" \
"{\n" \
"\tDECLARE_VAR_FB(64, CHUNK64);\n" \
"\tDECLARE_VAR_FB64();\n" \
"\n" \
"\tforward8_0i(pq, g_f0[lid], ml0, &Zt[kl0], m0, &zt[k0], w);\n" \
"\n" \
"\tconst sz_t j8 = local_id;\n" \
"\tforward8o(pq, m8, &zt[k8], ml8, &Zt[kl8], w, 8 + j8);\n" \
"}\n" \
"\n" \
"__kernel __attribute__((reqd_work_group_size(64 / 8 * CHUNK64, 1, 1)))\n" \
"void backward64_0(__global VTYPE * restrict const zg, __global const uint_32 * restrict const wg, __local VTYPE * const Z)\n" \
"{\n" \
"\tDECLARE_VAR_FB(64, CHUNK64);\n" \
"\tDECLARE_VAR_FB64();\n" \
"\n" \
"\tconst sz_t j8 = local_id, j8i = 8 - j8 - 1;\n" \
"\tbackward8i(pq, ml8, &Zt[kl8], m8, &zt[k8], w, 8 + j8i);\n" \
"\n" \
"\tbackward8_0o(pq, g_f0i[lid], m0, &zt[k0], ml0, &Zt[kl0], w);\n" \
"}\n" \
"\n" \
"#endif\n" \
"\n" \
"#define DECLARE_VAR_FB512() \\\n" \
"\tconst sz_t kl0 = local_id, k0 = kl0 * (N_SZ / 512), ml0 = 64, m0 = ml0 * (N_SZ / 512); \\\n" \
"\tconst sz_t kl8 = (local_id % 8) + (local_id / 8) * 64, ml8 = 8; \\\n" \
"\tconst sz_t kl64 = local_id * 8, k64 = kl64 * (N_SZ / 512), ml64 = 1, m64 = ml64 * (N_SZ / 512);\n" \
"\n" \
"__kernel __attribute__((reqd_work_group_size(512 / 8 * CHUNK512, 1, 1)))\n" \
"void forward512_0(__global VTYPE * restrict const zg, __global const uint_32 * restrict const wg, __local VTYPE * const Z)\n" \
"{\n" \
"\tDECLARE_VAR_FB(512, CHUNK512);\n" \
"\tDECLARE_VAR_FB512();\n" \
"\n" \
"\tforward8_0i(pq, g_f0[lid], ml0, &Zt[kl0], m0, &zt[k0], w);\n" \
"\n" \
"\tconst sz_t j8 = (local_id / 8) % 8;\n" \
"\tforward8l(pq, ml8, &Zt[kl8], w, 8 + j8);\n" \
"\n" \
"\tconst sz_t j64 = local_id;\n" \
"\tforward8o(pq, m64, &zt[k64], ml64, &Zt[kl64], w, 64 + j64);\n" \
"}\n" \
"\n" \
"__kernel __attribute__((reqd_work_group_size(512 / 8 * CHUNK512, 1, 1)))\n" \
"void backward512_0(__global VTYPE * restrict const zg, __global const uint_32 * restrict const wg, __local VTYPE * const Z)\n" \
"{\n" \
"\tDECLARE_VAR_FB(512, CHUNK512);\n" \
"\tDECLARE_VAR_FB512();\n" \
"\n" \
"\tconst sz_t j64 = local_id, j64i = 64 - j64 - 1;\n" \
"\tbackward8i(pq, ml64, &Zt[kl64], m64, &zt[k64], w, 64 + j64i);\n" \
"\n" \
"\tconst sz_t j8 = (local_id / 8) % 8, j8i = 8 - j8 - 1;\n" \
"\tbackward8l(pq, ml8, &Zt[kl8], w, 8 + j8i);\n" \
"\n" \
"\tbackward8_0o(pq, g_f0i[lid], m0, &zt[k0], ml0, &Zt[kl0], w);\n" \
"}\n" \
"\n" \
"/*__kernel\n" \
"void square2x4(__global VTYPE * restrict const zg, __global const uint_32 * restrict const wg)\n" \
"{\n" \
"\tDECLARE_VAR_REG();\n" \
"\tconst sz_t s = N_SZ / 8, j = id, k = 8 * vid;\n" \
"\tsquare2x4g(pq, &z[k], w, s + j);\n" \
"}\n" \
"\n" \
"__kernel\n" \
"void square4x2(__global VTYPE * restrict const zg, __global const uint_32 * restrict const wg)\n" \
"{\n" \
"\tDECLARE_VAR_REG();\n" \
"\tconst sz_t s = N_SZ / 8, j = id, ji = s - j - 1, k = 8 * vid;\n" \
"\tsquare4x2g(pq, &z[k], w, s + j, s + ji);\n" \
"}\n" \
"\n" \
"__kernel\n" \
"void square8(__global VTYPE * restrict const zg, __global const uint_32 * restrict const wg)\n" \
"{\n" \
"\tDECLARE_VAR_REG();\n" \
"\tconst sz_t s = N_SZ / 8, j = id, ji = s - j - 1, k = 8 * vid;\n" \
"\tsquare8g(pq, &z[k], w, s + j, s + ji);\n" \
"}*/\n" \
"\n" \
"#define DECLARE_VAR_SQRMUL(N, BLK_N) \\\n" \
"\tDECLARE_VAR_REG(); \\\n" \
"\tconst sz_t block_id = (vid / (N / 8)) % BLK_N; \\\n" \
"\t__local VTYPE * const Zb = &Z[N * block_id]; \\\n" \
"\tconst sz_t s = N_SZ / 8, j = id, ji = s - j - 1, k = 8 * vid;\n" \
"\n" \
"#ifdef QVALID\n" \
"\n" \
"__kernel __attribute__((reqd_work_group_size(16 / 8 * BLK16, 1, 1)))\n" \
"void square16(__global VTYPE * restrict const zg, __global const uint_32 * restrict const wg, __local VTYPE * const Z)\n" \
"{\n" \
"\tDECLARE_VAR_SQRMUL(16, BLK16);\n" \
"\tconst sz_t k2 = 7 * (vid & ~(2 - 1)) + vid;\n" \
"\n" \
"\tforward8i(pq, 2, &Zb[k2 % 16], 2, &z[k2], w, (s + j) / 2);\n" \
"\tsquare2x4l(pq, &Zb[k % 16], w, s + j);\n" \
"\tbackward8o(pq, 2, &z[k2], 2, &Zb[k2 % 16], w, (s + ji) / 2);\n" \
"}\n" \
"\n" \
"__kernel __attribute__((reqd_work_group_size(32 / 8 * BLK32, 1, 1)))\n" \
"void square32(__global VTYPE * restrict const zg, __global const uint_32 * restrict const wg, __local VTYPE * const Z)\n" \
"{\n" \
"\tDECLARE_VAR_SQRMUL(32, BLK32);\n" \
"\tconst sz_t k4 = 7 * (vid & ~(4 - 1)) + vid;\n" \
"\n" \
"\tforward8i(pq, 4, &Zb[k4 % 32], 4, &z[k4], w, (s + j) / 4);\n" \
"\tsquare4x2l(pq, &Zb[k % 32], w, s + j, s + ji);\n" \
"\tbackward8o(pq, 4, &z[k4], 4, &Zb[k4 % 32], w, (s + ji) / 4);\n" \
"}\n" \
"\n" \
"__kernel __attribute__((reqd_work_group_size(64 / 8 * BLK64, 1, 1)))\n" \
"void square64(__global VTYPE * restrict const zg, __global const uint_32 * restrict const wg, __local VTYPE * const Z)\n" \
"{\n" \
"\tDECLARE_VAR_SQRMUL(64, BLK64);\n" \
"\tconst sz_t k8 = 7 * (vid & ~(8 - 1)) + vid;\n" \
"\n" \
"\tforward8i(pq, 8, &Zb[k8 % 64], 8, &z[k8], w, (s + j) / 8);\n" \
"\tsquare8l(pq, &Zb[k % 64], w, s + j, s + ji);\n" \
"\tbackward8o(pq, 8, &z[k8], 8, &Zb[k8 % 64], w, (s + ji) / 8);\n" \
"}\n" \
"\n" \
"#else\n" \
"\n" \
"#if (LN_SZ % 3 == 1)\n" \
"\n" \
"__kernel __attribute__((reqd_work_group_size(128 / 8 * BLK128, 1, 1)))\n" \
"void square128(__global VTYPE * restrict const zg, __global const uint_32 * restrict const wg, __local VTYPE * const Z)\n" \
"{\n" \
"#if defined(SQ128_PAD_GROUP) && (SQ128_PAD_GROUP != 0)\n" \
"\tDECLARE_VAR_REG();\n" \
"\n" \
"\tconst sz_t block_id = (vid / (128 / 8)) % BLK128;\n" \
"\t__local VTYPE * const Zb = &Z[SQ128_PAD_SLOTS * block_id];\n" \
"\n" \
"\tconst sz_t s = N_SZ / 8;\n" \
"\tconst sz_t j = id;\n" \
"\tconst sz_t ji = s - j - 1;\n" \
"\tconst sz_t k = 8 * vid;\n" \
"\n" \
"\tconst sz_t k2 =\n" \
"\t\t7 * (vid & ~(2 - 1)) + vid;\n" \
"\n" \
"\tconst sz_t k16 =\n" \
"\t\t7 * (vid & ~(16 - 1)) + vid;\n" \
"\n" \
"\tforward8i_sq128_pad(\n" \
"\t\tpq,\n" \
"\t\tk16 % 128,\n" \
"\t\t16,\n" \
"\t\tZb,\n" \
"\t\t16,\n" \
"\t\t&z[k16],\n" \
"\t\tw,\n" \
"\t\t(s + j) / 16\n" \
"\t);\n" \
"\n" \
"\tforward8l_sq128_pad(\n" \
"\t\tpq,\n" \
"\t\tk2 % 128,\n" \
"\t\t2,\n" \
"\t\tZb,\n" \
"\t\tw,\n" \
"\t\t(s + j) / 2\n" \
"\t);\n" \
"\n" \
"\tsquare2x4l_sq128_pad(\n" \
"\t\tpq,\n" \
"\t\tk % 128,\n" \
"\t\tZb,\n" \
"\t\tw,\n" \
"\t\ts + j\n" \
"\t);\n" \
"\n" \
"\tbackward8l_sq128_pad(\n" \
"\t\tpq,\n" \
"\t\tk2 % 128,\n" \
"\t\t2,\n" \
"\t\tZb,\n" \
"\t\tw,\n" \
"\t\t(s + ji) / 2\n" \
"\t);\n" \
"\n" \
"\tbackward8o_sq128_pad(\n" \
"\t\tpq,\n" \
"\t\t16,\n" \
"\t\t&z[k16],\n" \
"\t\tk16 % 128,\n" \
"\t\t16,\n" \
"\t\tZb,\n" \
"\t\tw,\n" \
"\t\t(s + ji) / 16\n" \
"\t);\n" \
"#else\n" \
"\tDECLARE_VAR_SQRMUL(128, BLK128);\n" \
"\n" \
"\tconst sz_t k2 =\n" \
"\t\t7 * (vid & ~(2 - 1)) + vid;\n" \
"\n" \
"\tconst sz_t k16 =\n" \
"\t\t7 * (vid & ~(16 - 1)) + vid;\n" \
"\n" \
"\tforward8i(\n" \
"\t\tpq,\n" \
"\t\t16,\n" \
"\t\t&Zb[k16 % 128],\n" \
"\t\t16,\n" \
"\t\t&z[k16],\n" \
"\t\tw,\n" \
"\t\t(s + j) / 16\n" \
"\t);\n" \
"\n" \
"\tforward8l(\n" \
"\t\tpq,\n" \
"\t\t2,\n" \
"\t\t&Zb[k2 % 128],\n" \
"\t\tw,\n" \
"\t\t(s + j) / 2\n" \
"\t);\n" \
"\n" \
"\tsquare2x4l(\n" \
"\t\tpq,\n" \
"\t\t&Zb[k % 128],\n" \
"\t\tw,\n" \
"\t\ts + j\n" \
"\t);\n" \
"\n" \
"\tbackward8l(\n" \
"\t\tpq,\n" \
"\t\t2,\n" \
"\t\t&Zb[k2 % 128],\n" \
"\t\tw,\n" \
"\t\t(s + ji) / 2\n" \
"\t);\n" \
"\n" \
"\tbackward8o(\n" \
"\t\tpq,\n" \
"\t\t16,\n" \
"\t\t&z[k16],\n" \
"\t\t16,\n" \
"\t\t&Zb[k16 % 128],\n" \
"\t\tw,\n" \
"\t\t(s + ji) / 16\n" \
"\t);\n" \
"#endif\n" \
"}\n" \
"\n" \
"#elif (LN_SZ % 3 == 2)\n" \
"\n" \
"__kernel __attribute__((reqd_work_group_size(256 / 8 * BLK256, 1, 1)))\n" \
"void square256(__global VTYPE * restrict const zg, __global const uint_32 * restrict const wg, __local VTYPE * const Z)\n" \
"{\n" \
"\tDECLARE_VAR_SQRMUL(256, BLK256);\n" \
"\tconst sz_t k4 = 7 * (vid & ~(4 - 1)) + vid, k32 = 7 * (vid & ~(32 - 1)) + vid;\n" \
"\n" \
"\tforward8i(pq, 32, &Zb[k32 % 256], 32, &z[k32], w, (s + j) / 32);\n" \
"\tforward8l(pq, 4, &Zb[k4 % 256], w, (s + j) / 4);\n" \
"\tsquare4x2l(pq, &Zb[k % 256], w, s + j, s + ji);\n" \
"\tbackward8l(pq, 4, &Zb[k4 % 256], w, (s + ji) / 4);\n" \
"\tbackward8o(pq, 32, &z[k32], 32, &Zb[k32 % 256], w, (s + ji) / 32);\n" \
"}\n" \
"\n" \
"#else\n" \
"\n" \
"__kernel __attribute__((reqd_work_group_size(512 / 8 * BLK512, 1, 1)))\n" \
"void square512(__global VTYPE * restrict const zg, __global const uint_32 * restrict const wg, __local VTYPE * const Z)\n" \
"{\n" \
"\tDECLARE_VAR_SQRMUL(512, BLK512);\n" \
"\tconst sz_t k8 = 7 * (vid & ~(8 - 1)) + vid, k64 = 7 * (vid & ~(64 - 1)) + vid;\n" \
"\n" \
"\tforward8i(pq, 64, &Zb[k64 % 512], 64, &z[k64], w, (s + j) / 64);\n" \
"\tforward8l(pq, 8, &Zb[k8 % 512], w, (s + j) / 8);\n" \
"\tsquare8l(pq, &Zb[k % 512], w, s + j, s + ji);\n" \
"\tbackward8l(pq, 8, &Zb[k8 % 512], w, (s + ji) / 8);\n" \
"\tbackward8o(pq, 64, &z[k64], 64, &Zb[k64 % 512], w, (s + ji) / 64);\n" \
"}\n" \
"\n" \
"#endif\n" \
"#endif\n" \
"\n" \
"/*__kernel\n" \
"void fwd4x2(__global VTYPE * restrict const zg, __global const uint_32 * restrict const wg)\n" \
"{\n" \
"\tDECLARE_VAR_REG();\n" \
"\tconst sz_t n_8 = N_SZ / 8, j = id, k = 8 * vid;\n" \
"\tfwd4x2g(pq, &z[k], w, n_8 + j);\n" \
"}\n" \
"\n" \
"__kernel\n" \
"void fwd8(__global VTYPE * restrict const zg, __global const uint_32 * restrict const wg)\n" \
"{\n" \
"\tDECLARE_VAR_REG();\n" \
"\tconst sz_t n_8 = N_SZ / 8, j = id, k = 8 * vid;\n" \
"\tfwd8g(pq, &z[k], w, n_8 + j);\n" \
"}*/\n" \
"\n" \
"#ifdef QVALID\n" \
"\n" \
"__kernel\n" \
"void fwd16(__global VTYPE * restrict const zg, __global const uint_32 * restrict const wg)\n" \
"{\n" \
"\tDECLARE_VAR_REG();\n" \
"\tconst sz_t n_8 = N_SZ / 8, j = id, k2 = 7 * (vid & ~(2 - 1)) + vid;\n" \
"\tforward8g(pq, 2, &z[k2], w, (n_8 + j) / 2);\n" \
"}\n" \
"\n" \
"__kernel __attribute__((reqd_work_group_size(32 / 8 * BLK32, 1, 1)))\n" \
"void fwd32(__global VTYPE * restrict const zg, __global const uint_32 * restrict const wg, __local VTYPE * const Z)\n" \
"{\n" \
"\tDECLARE_VAR_SQRMUL(32, BLK32);\n" \
"\tconst sz_t k4 = 7 * (vid & ~(4 - 1)) + vid;\n" \
"\n" \
"\tforward8i(pq, 4, &Zb[k4 % 32], 4, &z[k4], w, (s + j) / 4);\n" \
"\tfwd4x2o(pq, &z[k], &Zb[k % 32], w, s + j);\n" \
"}\n" \
"\n" \
"__kernel __attribute__((reqd_work_group_size(64 / 8 * BLK64, 1, 1)))\n" \
"void fwd64(__global VTYPE * restrict const zg, __global const uint_32 * restrict const wg, __local VTYPE * const Z)\n" \
"{\n" \
"\tDECLARE_VAR_SQRMUL(64, BLK64);\n" \
"\tconst sz_t k8 = 7 * (vid & ~(8 - 1)) + vid;\n" \
"\n" \
"\tforward8i(pq, 8, &Zb[k8 % 64], 8, &z[k8], w, (s + j) / 8);\n" \
"\tfwd8o(pq, &z[k], &Zb[k % 64], w, s + j);\n" \
"}\n" \
"\n" \
"#else\n" \
"\n" \
"#if (LN_SZ % 3 == 1)\n" \
"\n" \
"__kernel __attribute__((reqd_work_group_size(128 / 8 * BLK128, 1, 1)))\n" \
"void fwd128(__global VTYPE * restrict const zg, __global const uint_32 * restrict const wg, __local VTYPE * const Z)\n" \
"{\n" \
"\tDECLARE_VAR_SQRMUL(128, BLK128);\n" \
"\tconst sz_t k2 = 7 * (vid & ~(2 - 1)) + vid, k16 = 7 * (vid & ~(16 - 1)) + vid;\n" \
"\n" \
"\tforward8i(pq, 16, &Zb[k16 % 128], 16, &z[k16], w, (s + j) / 16);\n" \
"\tforward8o(pq, 2, &z[k2], 2, &Zb[k2 % 128], w, (s + j) / 2);\n" \
"}\n" \
"\n" \
"#elif (LN_SZ % 3 == 2)\n" \
"\n" \
"__kernel __attribute__((reqd_work_group_size(256 / 8 * BLK256, 1, 1)))\n" \
"void fwd256(__global VTYPE * restrict const zg, __global const uint_32 * restrict const wg, __local VTYPE * const Z)\n" \
"{\n" \
"\tDECLARE_VAR_SQRMUL(256, BLK256);\n" \
"\tconst sz_t k4 = 7 * (vid & ~(4 - 1)) + vid, k32 = 7 * (vid & ~(32 - 1)) + vid;\n" \
"\n" \
"\tforward8i(pq, 32, &Zb[k32 % 256], 32, &z[k32], w, (s + j) / 32);\n" \
"\tforward8l(pq, 4, &Zb[k4 % 256], w, (s + j) / 4);\n" \
"\tfwd4x2o(pq, &z[k], &Zb[k % 256], w, s + j);\n" \
"}\n" \
"\n" \
"#else\n" \
"\n" \
"__kernel __attribute__((reqd_work_group_size(512 / 8 * BLK512, 1, 1)))\n" \
"void fwd512(__global VTYPE * restrict const zg, __global const uint_32 * restrict const wg, __local VTYPE * const Z)\n" \
"{\n" \
"\tDECLARE_VAR_SQRMUL(512, BLK512);\n" \
"\tconst sz_t k8 = 7 * (vid & ~(8 - 1)) + vid, k64 = 7 * (vid & ~(64 - 1)) + vid;\n" \
"\n" \
"\tforward8i(pq, 64, &Zb[k64 % 512], 64, &z[k64], w, (s + j) / 64);\n" \
"\tforward8l(pq, 8, &Zb[k8 % 512], w, (s + j) / 8);\n" \
"\tfwd8o(pq, &z[k], &Zb[k % 512], w, s + j);\n" \
"}\n" \
"\n" \
"#endif\n" \
"#endif\n" \
"\n" \
"/*__kernel\n" \
"void mul2x4(__global VTYPE * restrict const zg, const __global VTYPE * restrict const zpg, __global const uint_32 * restrict const wg)\n" \
"{\n" \
"\tDECLARE_VAR_REG();\n" \
"\tDECLARE_VARP_REG();\n" \
"\tconst sz_t n_8 = N_SZ / 8, j = id, k = 8 * vid;\n" \
"\tmul2x4g(pq, &z[k], &zp[k], w, n_8 + j);\n" \
"}\n" \
"\n" \
"__kernel\n" \
"void mul4x2(__global VTYPE * restrict const zg, const __global VTYPE * restrict const zpg, __global const uint_32 * restrict const wg)\n" \
"{\n" \
"\tDECLARE_VAR_REG();\n" \
"\tDECLARE_VARP_REG();\n" \
"\tconst sz_t n_8 = N_SZ / 8, j = id, k = 8 * vid;\n" \
"\tconst sz_t ji = n_8 - j - 1;\n" \
"\tmul4x2g(pq, &z[k], &zp[k], w, n_8 + j, n_8 + ji);\n" \
"}\n" \
"\n" \
"__kernel\n" \
"void mul8(__global VTYPE * restrict const zg, const __global VTYPE * restrict const zpg, __global const uint_32 * restrict const wg)\n" \
"{\n" \
"\tDECLARE_VAR_REG();\n" \
"\tDECLARE_VARP_REG();\n" \
"\tconst sz_t n_8 = N_SZ / 8, j = id, k = 8 * vid;\n" \
"\tconst sz_t ji = n_8 - j - 1;\n" \
"\tmul8g(pq, &z[k], &zp[k], w, n_8 + j, n_8 + ji);\n" \
"}*/\n" \
"\n" \
"#ifdef QVALID\n" \
"\n" \
"__kernel __attribute__((reqd_work_group_size(16 / 8 * BLK16, 1, 1)))\n" \
"void mul16(__global VTYPE * restrict const zg, const __global VTYPE * restrict const zpg,\n" \
"\t__global const uint_32 * restrict const wg, __local VTYPE * const Z)\n" \
"{\n" \
"\tDECLARE_VAR_SQRMUL(16, BLK16);\n" \
"\tDECLARE_VARP_REG();\n" \
"\tconst sz_t k2 = 7 * (vid & ~(2 - 1)) + vid;\n" \
"\n" \
"\tforward8i(pq, 2, &Zb[k2 % 16], 2, &z[k2], w, (s + j) / 2);\n" \
"\tmul2x4l(pq, &Zb[k % 16], &zp[k], w, s + j);\n" \
"\tbackward8o(pq, 2, &z[k2], 2, &Zb[k2 % 16], w, (s + ji) / 2);\n" \
"}\n" \
"\n" \
"__kernel __attribute__((reqd_work_group_size(32 / 8 * BLK32, 1, 1)))\n" \
"void mul32(__global VTYPE * restrict const zg, const __global VTYPE * restrict const zpg,\n" \
"\t__global const uint_32 * restrict const wg, __local VTYPE * const Z)\n" \
"{\n" \
"\tDECLARE_VAR_SQRMUL(32, BLK32);\n" \
"\tDECLARE_VARP_REG();\n" \
"\tconst sz_t k4 = 7 * (vid & ~(4 - 1)) + vid;\n" \
"\n" \
"\tforward8i(pq, 4, &Zb[k4 % 32], 4, &z[k4], w, (s + j) / 4);\n" \
"\tmul4x2l(pq, &Zb[k % 32], &zp[k], w, s + j, s + ji);\n" \
"\tbackward8o(pq, 4, &z[k4], 4, &Zb[k4 % 32], w, (s + ji) / 4);\n" \
"}\n" \
"\n" \
"__kernel __attribute__((reqd_work_group_size(64 / 8 * BLK64, 1, 1)))\n" \
"void mul64(__global VTYPE * restrict const zg, const __global VTYPE * restrict const zpg,\n" \
"\t__global const uint_32 * restrict const wg, __local VTYPE * const Z)\n" \
"{\n" \
"\tDECLARE_VAR_SQRMUL(64, BLK64);\n" \
"\tDECLARE_VARP_REG();\n" \
"\tconst sz_t k8 = 7 * (vid & ~(8 - 1)) + vid;\n" \
"\n" \
"\tforward8i(pq, 8, &Zb[k8 % 64], 8, &z[k8], w, (s + j) / 8);\n" \
"\tmul8l(pq, &Zb[k % 64], &zp[k], w, s + j, s + ji);\n" \
"\tbackward8o(pq, 8, &z[k8], 8, &Zb[k8 % 64], w, (s + ji) / 8);\n" \
"}\n" \
"\n" \
"#else\n" \
"\n" \
"#if (LN_SZ % 3 == 1)\n" \
"\n" \
"__kernel __attribute__((reqd_work_group_size(128 / 8 * BLK128, 1, 1)))\n" \
"void mul128(__global VTYPE * restrict const zg, const __global VTYPE * restrict const zpg,\n" \
"\t__global const uint_32 * restrict const wg, __local VTYPE * const Z)\n" \
"{\n" \
"\tDECLARE_VAR_SQRMUL(128, BLK128);\n" \
"\tDECLARE_VARP_REG();\n" \
"\tconst sz_t k2 = 7 * (vid & ~(2 - 1)) + vid, k16 = 7 * (vid & ~(16 - 1)) + vid;\n" \
"\n" \
"\tforward8i(pq, 16, &Zb[k16 % 128], 16, &z[k16], w, (s + j) / 16);\n" \
"\tforward8l(pq, 2, &Zb[k2 % 128], w, (s + j) / 2);\n" \
"\tmul2x4l(pq, &Zb[k % 128], &zp[k], w, s + j);\n" \
"\tbackward8l(pq, 2, &Zb[k2 % 128], w, (s + ji) / 2);\n" \
"\tbackward8o(pq, 16, &z[k16], 16, &Zb[k16 % 128], w, (s + ji) / 16);\n" \
"}\n" \
"\n" \
"#elif (LN_SZ % 3 == 2)\n" \
"\n" \
"__kernel __attribute__((reqd_work_group_size(256 / 8 * BLK256, 1, 1)))\n" \
"void mul256(__global VTYPE * restrict const zg, const __global VTYPE * restrict const zpg,\n" \
"\t__global const uint_32 * restrict const wg, __local VTYPE * const Z)\n" \
"{\n" \
"\tDECLARE_VAR_SQRMUL(256, BLK256);\n" \
"\tDECLARE_VARP_REG();\n" \
"\tconst sz_t k4 = 7 * (vid & ~(4 - 1)) + vid, k32 = 7 * (vid & ~(32 - 1)) + vid;\n" \
"\n" \
"\tforward8i(pq, 32, &Zb[k32 % 256], 32, &z[k32], w, (s + j) / 32);\n" \
"\tforward8l(pq, 4, &Zb[k4 % 256], w, (s + j) / 4);\n" \
"\tmul4x2l(pq, &Zb[k % 256], &zp[k], w, s + j, s + ji);\n" \
"\tbackward8l(pq, 4, &Zb[k4 % 256], w, (s + ji) / 4);\n" \
"\tbackward8o(pq, 32, &z[k32], 32, &Zb[k32 % 256], w, (s + ji) / 32);\n" \
"}\n" \
"\n" \
"#else\n" \
"\n" \
"__kernel __attribute__((reqd_work_group_size(512 / 8 * BLK512, 1, 1)))\n" \
"void mul512(__global VTYPE * restrict const zg, const __global VTYPE * restrict const zpg,\n" \
"\t__global const uint_32 * restrict const wg, __local VTYPE * const Z)\n" \
"{\n" \
"\tDECLARE_VAR_SQRMUL(512, BLK512);\n" \
"\tDECLARE_VARP_REG();\n" \
"\tconst sz_t k8 = 7 * (vid & ~(8 - 1)) + vid, k64 = 7 * (vid & ~(64 - 1)) + vid;\n" \
"\n" \
"\tforward8i(pq, 64, &Zb[k64 % 512], 64, &z[k64], w, (s + j) / 64);\n" \
"\tforward8l(pq, 8, &Zb[k8 % 512], w, (s + j) / 8);\n" \
"\tmul8l(pq, &Zb[k % 512], &zp[k], w, s + j, s + ji);\n" \
"\tbackward8l(pq, 8, &Zb[k8 % 512], w, (s + ji) / 8);\n" \
"\tbackward8o(pq, 64, &z[k64], 64, &Zb[k64 % 512], w, (s + ji) / 64);\n" \
"}\n" \
"\n" \
"#endif\n" \
"#endif\n" \
"\n" \
"#define DECLARE_VAR_MUL_MASK() \\\n" \
"\tDECLARE_VAR_REG_1(); \\\n" \
"\tDECLARE_VARP_REG_1(); \\\n" \
"\tconst sz_t oid = vid / OCL_VSIZE, i = (vid % OCL_VSIZE) + OCL_VSIZE * (oid / (N_SZ / 8)); \\\n" \
"\tconst sz_t n_8 = N_SZ / 8, j = oid % (N_SZ / 8), ji = n_8 - j - 1, k = 7 * (vid & ~(OCL_VSIZE - 1)) + vid;\n" \
"\n" \
"__kernel\n" \
"void mul2x4_mask(__global uint_32 * restrict const zg, const __global uint_32 * restrict const zpg,\n" \
"\t__global const uint_32 * restrict const wg, const uint_32 mask)\n" \
"{\n" \
"\tDECLARE_VAR_MUL_MASK();\n" \
"\tif ((mask & (1u << i)) != 0) mul2x4g_1(pq, OCL_VSIZE, &z[k], &zp[k], w, n_8 + j);\n" \
"}\n" \
"\n" \
"__kernel\n" \
"void mul4x2_mask(__global uint_32 * restrict const zg, const __global uint_32 * restrict const zpg,\n" \
"\t__global const uint_32 * restrict const wg, const uint_32 mask)\n" \
"{\n" \
"\tDECLARE_VAR_MUL_MASK();\n" \
"\tmul4x2g_1(pq, OCL_VSIZE, &z[k], &zp[k], w, n_8 + j, n_8 + ji, (mask & (1u << i)) != 0);\n" \
"}\n" \
"\n" \
"__kernel\n" \
"void mul8_mask(__global uint_32 * restrict const zg, const __global uint_32 * restrict const zpg,\n" \
"\t__global const uint_32 * restrict const wg, const uint_32 mask)\n" \
"{\n" \
"\tDECLARE_VAR_MUL_MASK();\n" \
"\tmul8g_1(pq, OCL_VSIZE, &z[k], &zp[k], w, n_8 + j, n_8 + ji, (mask & (1u << i)) != 0);\n" \
"}\n" \
"\n" \
"// --- carry ---\n" \
"\n" \
"INLINE uint_32 barrett(const uint_64 a, const uint_32 b, const uint_32 b_inv, const int_32 b_s, uint_32 * a_p)\n" \
"{\n" \
"\t// Using notations of Modular SIMD arithmetic in Mathemagix, Joris van der Hoeven, Grégoire Lecerf, Guillaume Quintin, 2014, HAL.\n" \
"\t// n = 31, alpha = 2^{n-2} = 2^29, s = r - 2, t = n + 1 = 32 => h = 1.\n" \
"\t// b < 2^31, alpha = 2^29 => a < 2^29 b\n" \
"\t// 2^{r-1} < b <= 2^r then a < 2^{r + 29} = 2^{s + 31} and (a >> s) < 2^31\n" \
"\t// b_inv = [2^{s + 32} / b]\n" \
"\t// b_inv < 2^{s + 32} / b < 2^{s + 32} / 2^{r-1} = 2^{s + 32} / 2^{s + 1} < 2^31\n" \
"\t// Let h be the number of iterations in Barrett's reduction, we have h = [a / b] - [[a / 2^s] b_inv / 2^32].\n" \
"\t// h = ([a/b] - a/b) + a/2^{s + 32} (2^{s + 32}/b - b_inv) + b_inv/2^32 (a/2^s - [a/2^s]) + ([a/2^s] b_inv / 2^32 - [[a/2^s] b_inv / 2^32])\n" \
"\t// Then -1 + 0 + 0 + 0 < h < 0 + 1/2 (2^{s + 32}/b - b_inv) + b_inv/2^32 + 1,\n" \
"\t// 0 <= h < 1 + 1/2 + 1/2 => h = 1.\n" \
"\n" \
"\tconst uint_32 d = mul_hi((uint_32)(a >> b_s), b_inv), r = (uint_32)(a) - d * b;\n" \
"\tconst bool o = (r >= b);\n" \
"\t*a_p = d + (o ? 1 : 0);\n" \
"\treturn r - (o ? b : 0);\n" \
"}\n" \
"\n" \
"INLINE int_32 reduce64(int_64 * f, const uint_32 b, const uint_32 b_inv, const int_32 b_s)\n" \
"{\n" \
"\t// 1- t < 2^63 => t_h < 2^34. We must have t_h < 2^29 b => b > 32\n" \
"\t// 2- t < 2^23 b^2 => t_h < b^2 / 2^6. If 2 <= b < 32 then t_h < 32^2 / 2^6 = 16 < 2^29 b\n" \
"\tconst uint_64 t = abs(*f);\n" \
"\tconst uint_64 t_h = t >> 29;\n" \
"\tconst uint_32 t_l = (uint_32)(t) % (1u << 29);\n" \
"\n" \
"\tuint_32 d_h, r_h = barrett(t_h, b, b_inv, b_s, &d_h);\n" \
"\tuint_32 d_l, r_l = barrett(((uint_64)(r_h) << 29) | t_l, b, b_inv, b_s, &d_l);\n" \
"\tconst uint_64 d = ((uint_64)(d_h) << 29) | d_l;\n" \
"\n" \
"\tconst bool s = (*f < 0);\n" \
"\t*f = s ? -(int_64)(d) : (int_64)(d);\n" \
"\treturn s ? -(int_32)(r_l) : (int_32)(r_l);\n" \
"}\n" \
"\n" \
"INLINE int_32 reduce96(int_64 * f, const int96 l, const uint_32 b, const uint_32 b_inv, const int_32 b_s)\n" \
"{\n" \
"\tconst uint96 t = int96_abs(l);\n" \
"\tconst uint_64 t_h = (t.s1 << (32 - 29)) | (t.s0 >> 29);\n" \
"\tconst uint_32 t_l = t.s0 % (1u << 29);\n" \
"\n" \
"\tuint_32 d_h, r_h = barrett(t_h, b, b_inv, b_s, &d_h);\n" \
"\tuint_32 d_l, r_l = barrett(((uint_64)(r_h) << 29) | t_l, b, b_inv, b_s, &d_l);\n" \
"\tconst uint_64 d = ((uint_64)(d_h) << 29) | d_l;\n" \
"\n" \
"\tconst bool s = int96_is_neg(l);\n" \
"\t*f = s ? -(int_64)(d) : (int_64)(d);\n" \
"\treturn s ? -(int_32)(r_l) : (int_32)(r_l);\n" \
"}\n" \
"\n" \
"INLINE int96 garner3(const uint_32 r1, const uint_32 r2, const uint_32 r3)\n" \
"{\n" \
"\tconst uint_32 u13 = mulmod(submod(r1, r3, P1), INVP3_P1, PQ1);\n" \
"\tconst uint_32 u23 = mulmod(submod(r2, r3, P2), INVP3_P2, PQ2);\n" \
"\tconst uint_32 u123 = mulmod(submod(u13, u23, P1), INVP2_P1, PQ1);\n" \
"\tconst uint96 n = uint96_add_64(uint96_mul_64_32(P2 * (uint_64)(P3), u123), u23 * (uint_64)(P3) + r3);\n" \
"\tconst bool b = uint96_is_greater(n, uint96_set(P1P2P3_2L, P1P2P3_2H));\n" \
"\treturn uint96_i(b ? uint96_sub(n, uint96_set(P1P2P3L, P1P2P3H)) : n);\n" \
"}\n" \
"\n" \
"INLINE int_32 reduce(int_64 * f, const uint_32 u1, const uint_32 u2, const uint_32 u3,\n" \
"\tconst uint2_32 bb_inv, const int_32 bs, const bool dup)\n" \
"{\n" \
"\tint96 l = garner3(u1, u2, u3);\n" \
"\tif (dup) l = int96_add(l, l);\n" \
"\tl = int96_add_64(l, *f);\n" \
"\treturn reduce96(f, l, bb_inv.s0, bb_inv.s1, bs);\n" \
"}\n" \
"\n" \
"INLINE void write_rns(__global uint_32 * restrict const z, const int_32 r)\n" \
"{\n" \
"\tz[0 * N_VSIZE] = set_int(r, P1);\n" \
"\tz[1 * N_VSIZE] = set_int(r, P2);\n" \
"\tz[2 * N_VSIZE] = set_int(r, P3);\n" \
"}\n" \
"\n" \
"#if OCL_CARRY_VSIZE == 4\n" \
"\n" \
"INLINE void carry_1x4(const __global uint4_32 * restrict const zk, __global int4_64 * restrict const c, __local int4_64 * const cl,\n" \
"\tint4_32 r[CARRY_LENGTH], const sz_t id, const uint8_32 bb_inv_i, const int4_32 bs_i, const uint_32 dup)\n" \
"{\n" \
"\tint_64 f0 = 0, f1 = 0, f2 = 0, f3 = 0;\n" \
"\tfor (sz_t j = 0; j < CARRY_LENGTH; ++j)\n" \
"\t{\n" \
"\t\tconst uint4_32 u1 = zk[j * CARRY_VSIZE + 0 * N_CARRY_VSIZE];\n" \
"\t\tconst uint4_32 u2 = zk[j * CARRY_VSIZE + 1 * N_CARRY_VSIZE];\n" \
"\t\tconst uint4_32 u3 = zk[j * CARRY_VSIZE + 2 * N_CARRY_VSIZE];\n" \
"\t\tr[j].s0 = reduce(&f0, u1.s0, u2.s0, u3.s0, bb_inv_i.s01, bs_i.s0, (dup & 1u) != 0);\n" \
"\t\tr[j].s1 = reduce(&f1, u1.s1, u2.s1, u3.s1, bb_inv_i.s23, bs_i.s1, (dup & 2u) != 0);\n" \
"\t\tr[j].s2 = reduce(&f2, u1.s2, u2.s2, u3.s2, bb_inv_i.s45, bs_i.s2, (dup & 4u) != 0);\n" \
"\t\tr[j].s3 = reduce(&f3, u1.s3, u2.s3, u3.s3, bb_inv_i.s67, bs_i.s3, (dup & 8u) != 0);\n" \
"\t}\n" \
"\tconst int4_64 f = (int4_64)(f0, f1, f2, f3);\n" \
"\n" \
"\tconst sz_t lid = id % CARRY_WG_SZ;\n" \
"\tcl[lid] = f;\n" \
"\n" \
"\tif (lid >= CARRY_WG_SZ - CARRY_VSIZE)\n" \
"\t{\n" \
"\t\tconst sz_t svid = (id / CARRY_VSIZE) & ~(N_SZ / CARRY_LENGTH - 1);\n" \
"\t\tconst sz_t vid = (id / CARRY_VSIZE + 1) % (N_SZ / CARRY_LENGTH);\n" \
"\t\tconst sz_t cid = (id % CARRY_VSIZE) + CARRY_VSIZE * ((svid + vid) / (CARRY_WG_SZ / CARRY_VSIZE));\n" \
"\t\tc[cid] = (vid == 0) ? -f : f;\n" \
"\t}\n" \
"}\n" \
"\n" \
"INLINE void carry_2x4(__global uint4_32 * restrict const zk, const __local int4_64 * const cl,\n" \
"\tint4_32 r[CARRY_LENGTH], const sz_t id, const uint8_32 bb_inv_i, const int4_32 bs_i)\n" \
"{\n" \
"\tconst sz_t lid = id % CARRY_WG_SZ;\n" \
"\tif (lid >= CARRY_VSIZE)\n" \
"\t{\n" \
"\t\tconst int4_64 f = cl[lid - CARRY_VSIZE];\n" \
"\t\tint_64 f0 = f.s0, f1 = f.s1, f2 = f.s2, f3 = f.s3;\n" \
"\t\tfor (size_t j = 0; j < CARRY_LENGTH - 1; ++j)\n" \
"\t\t{\n" \
"\t\t\tif (f0 != 0)\n" \
"\t\t\t{\n" \
"\t\t\t\tf0 += r[j].s0;\n" \
"\t\t\t\tr[j].s0 = reduce64(&f0, bb_inv_i.s0, bb_inv_i.s1, bs_i.s0);\n" \
"\t\t\t}\n" \
"\t\t\tif (f1 != 0)\n" \
"\t\t\t{\n" \
"\t\t\t\tf1 += r[j].s1;\n" \
"\t\t\t\tr[j].s1 = reduce64(&f1, bb_inv_i.s2, bb_inv_i.s3, bs_i.s1);\n" \
"\t\t\t}\n" \
"\t\t\tif (f2 != 0)\n" \
"\t\t\t{\n" \
"\t\t\t\tf2 += r[j].s2;\n" \
"\t\t\t\tr[j].s2 = reduce64(&f2, bb_inv_i.s4, bb_inv_i.s5, bs_i.s2);\n" \
"\t\t\t}\n" \
"\t\t\tif (f3 != 0)\n" \
"\t\t\t{\n" \
"\t\t\t\tf3 += r[j].s3;\n" \
"\t\t\t\tr[j].s3 = reduce64(&f3, bb_inv_i.s6, bb_inv_i.s7, bs_i.s3);\n" \
"\t\t\t}\n" \
"\t\t}\n" \
"\t\tif (f0 != 0) r[CARRY_LENGTH - 1].s0 = (int_32)(f0 + r[CARRY_LENGTH - 1].s0);\n" \
"\t\tif (f1 != 0) r[CARRY_LENGTH - 1].s1 = (int_32)(f1 + r[CARRY_LENGTH - 1].s1);\n" \
"\t\tif (f2 != 0) r[CARRY_LENGTH - 1].s2 = (int_32)(f2 + r[CARRY_LENGTH - 1].s2);\n" \
"\t\tif (f3 != 0) r[CARRY_LENGTH - 1].s3 = (int_32)(f3 + r[CARRY_LENGTH - 1].s3);\n" \
"\t}\n" \
"\n" \
"\tfor (size_t j = 0; j < CARRY_LENGTH; ++j)\n" \
"\t{\n" \
"\t\tzk[j * CARRY_VSIZE + 0 * N_CARRY_VSIZE] = set_int4(r[j], P1);\n" \
"\t\tzk[j * CARRY_VSIZE + 1 * N_CARRY_VSIZE] = set_int4(r[j], P2);\n" \
"\t\tzk[j * CARRY_VSIZE + 2 * N_CARRY_VSIZE] = set_int4(r[j], P3);\n" \
"\t}\n" \
"}\n" \
"\n" \
"__kernel __attribute__((reqd_work_group_size(CARRY_WG_SZ, 1, 1)))\n" \
"void carry1(const __global uint8_32 * restrict const bb_inv, const __global int4_32 * restrict const bs,\n" \
"\t__global uint4_32 * restrict const z, __global int4_64 * restrict const c, __local int4_64 * cl, const uint_32 dup)\n" \
"{\n" \
"\tconst sz_t id = (sz_t)get_global_id(0), i = (id % CARRY_VSIZE) + ((id / (N_SZ / CARRY_LENGTH)) & ~(CARRY_VSIZE - 1));\n" \
"\tconst sz_t k = (CARRY_LENGTH - 1) * (id & ~(CARRY_VSIZE - 1)) + id;\n" \
"\tconst uint8_32 bb_inv_i = bb_inv[i]; const int4_32 bs_i = bs[i];\n" \
"\tint4_32 r[CARRY_LENGTH];\n" \
"\n" \
"\tcarry_1x4(&z[k], c, cl, r, id, bb_inv_i, bs_i, dup >> (4 * i));\n" \
"\n" \
"\tbarrier(CLK_LOCAL_MEM_FENCE);\n" \
"\n" \
"\tcarry_2x4(&z[k], cl, r, id, bb_inv_i, bs_i);\n" \
"}\n" \
"\n" \
"#elif OCL_CARRY_VSIZE == 2\n" \
"\n" \
"INLINE void carry_1x2(const __global uint2_32 * restrict const zk, __global int2_64 * restrict const c, __local int2_64 * const cl,\n" \
"\tint2_32 r[CARRY_LENGTH], const sz_t id, const uint4_32 bb_inv_i, const int2_32 bs_i, const uint_32 dup)\n" \
"{\n" \
"\tint_64 f0 = 0, f1 = 0;\n" \
"\tfor (sz_t j = 0; j < CARRY_LENGTH; ++j)\n" \
"\t{\n" \
"\t\tconst uint2_32 u1 = zk[j * CARRY_VSIZE + 0 * N_CARRY_VSIZE];\n" \
"\t\tconst uint2_32 u2 = zk[j * CARRY_VSIZE + 1 * N_CARRY_VSIZE];\n" \
"\t\tconst uint2_32 u3 = zk[j * CARRY_VSIZE + 2 * N_CARRY_VSIZE];\n" \
"\t\tr[j].s0 = reduce(&f0, u1.s0, u2.s0, u3.s0, bb_inv_i.s01, bs_i.s0, (dup & 1u) != 0);\n" \
"\t\tr[j].s1 = reduce(&f1, u1.s1, u2.s1, u3.s1, bb_inv_i.s23, bs_i.s1, (dup & 2u) != 0);\n" \
"\t}\n" \
"\tconst int2_64 f = (int2_64)(f0, f1);\n" \
"\n" \
"\tconst sz_t lid = id % CARRY_WG_SZ;\n" \
"\tcl[lid] = f;\n" \
"\n" \
"\tif (lid >= CARRY_WG_SZ - CARRY_VSIZE)\n" \
"\t{\n" \
"\t\tconst sz_t svid = (id / CARRY_VSIZE) & ~(N_SZ / CARRY_LENGTH - 1);\n" \
"\t\tconst sz_t vid = (id / CARRY_VSIZE + 1) % (N_SZ / CARRY_LENGTH);\n" \
"\t\tconst sz_t cid = (id % CARRY_VSIZE) + CARRY_VSIZE * ((svid + vid) / (CARRY_WG_SZ / CARRY_VSIZE));\n" \
"\t\tc[cid] = (vid == 0) ? -f : f;\n" \
"\t}\n" \
"}\n" \
"\n" \
"INLINE void carry_2x2(__global uint2_32 * restrict const zk, const __local int2_64 * const cl,\n" \
"\tint2_32 r[CARRY_LENGTH], const sz_t id, const uint4_32 bb_inv_i, const int2_32 bs_i)\n" \
"{\n" \
"\tconst sz_t lid = id % CARRY_WG_SZ;\n" \
"\tif (lid >= CARRY_VSIZE)\n" \
"\t{\n" \
"\t\tconst int2_64 f = cl[lid - CARRY_VSIZE];\n" \
"\t\tint_64 f0 = f.s0, f1 = f.s1;\n" \
"\t\tfor (size_t j = 0; j < CARRY_LENGTH - 1; ++j)\n" \
"\t\t{\n" \
"\t\t\tif (f0 != 0)\n" \
"\t\t\t{\n" \
"\t\t\t\tf0 += r[j].s0;\n" \
"\t\t\t\tr[j].s0 = reduce64(&f0, bb_inv_i.s0, bb_inv_i.s1, bs_i.s0);\n" \
"\t\t\t}\n" \
"\t\t\tif (f1 != 0)\n" \
"\t\t\t{\n" \
"\t\t\t\tf1 += r[j].s1;\n" \
"\t\t\t\tr[j].s1 = reduce64(&f1, bb_inv_i.s2, bb_inv_i.s3, bs_i.s1);\n" \
"\t\t\t}\n" \
"\t\t}\n" \
"\t\tif (f0 != 0) r[CARRY_LENGTH - 1].s0 = (int_32)(f0 + r[CARRY_LENGTH - 1].s0);\n" \
"\t\tif (f1 != 0) r[CARRY_LENGTH - 1].s1 = (int_32)(f1 + r[CARRY_LENGTH - 1].s1);\n" \
"\t}\n" \
"\n" \
"\tfor (size_t j = 0; j < CARRY_LENGTH; ++j)\n" \
"\t{\n" \
"\t\tzk[j * CARRY_VSIZE + 0 * N_CARRY_VSIZE] = set_int2(r[j], P1);\n" \
"\t\tzk[j * CARRY_VSIZE + 1 * N_CARRY_VSIZE] = set_int2(r[j], P2);\n" \
"\t\tzk[j * CARRY_VSIZE + 2 * N_CARRY_VSIZE] = set_int2(r[j], P3);\n" \
"\t}\n" \
"}\n" \
"\n" \
"__kernel __attribute__((reqd_work_group_size(CARRY_WG_SZ, 1, 1)))\n" \
"void carry1(const __global uint4_32 * restrict const bb_inv, const __global int2_32 * restrict const bs,\n" \
"\t__global uint2_32 * restrict const z, __global int2_64 * restrict const c, __local int2_64 * cl, const uint_32 dup)\n" \
"{\n" \
"\tconst sz_t id = (sz_t)get_global_id(0), i = (id % CARRY_VSIZE) + ((id / (N_SZ / CARRY_LENGTH)) & ~(CARRY_VSIZE - 1));\n" \
"\tconst sz_t k = (CARRY_LENGTH - 1) * (id & ~(CARRY_VSIZE - 1)) + id;\n" \
"\tconst uint4_32 bb_inv_i = bb_inv[i]; const int2_32 bs_i = bs[i];\n" \
"\tint2_32 r[CARRY_LENGTH];\n" \
"\n" \
"\tcarry_1x2(&z[k], c, cl, r, id, bb_inv_i, bs_i, dup >> (2 * i));\n" \
"\n" \
"\tbarrier(CLK_LOCAL_MEM_FENCE);\n" \
"\n" \
"\tcarry_2x2(&z[k], cl, r, id, bb_inv_i, bs_i);\n" \
"}\n" \
"\n" \
"#else\t// OCL_CARRY_VSIZE = 1\n" \
"\n" \
"INLINE void carry_1x1(const __global uint_32 * restrict const zk, __global int_64 * restrict const c, __local int_64 * const cl,\n" \
"\tint_32 r[CARRY_LENGTH], const sz_t id, const uint2_32 bb_inv_i, const int_32 bs_i, const uint_32 dup)\n" \
"{\n" \
"\tint_64 f = 0;\n" \
"\tfor (sz_t j = 0; j < CARRY_LENGTH; ++j)\n" \
"\t{\n" \
"\t\tconst uint_32 u1 = zk[j * CARRY_VSIZE + 0 * N_CARRY_VSIZE];\n" \
"\t\tconst uint_32 u2 = zk[j * CARRY_VSIZE + 1 * N_CARRY_VSIZE];\n" \
"\t\tconst uint_32 u3 = zk[j * CARRY_VSIZE + 2 * N_CARRY_VSIZE];\n" \
"\t\tr[j] = reduce(&f, u1, u2, u3, bb_inv_i, bs_i, (dup & 1u) != 0);\n" \
"\t}\n" \
"\n" \
"\tconst sz_t lid = id % CARRY_WG_SZ;\n" \
"\tcl[lid] = f;\n" \
"\n" \
"\tif (lid >= CARRY_WG_SZ - CARRY_VSIZE)\n" \
"\t{\n" \
"\t\tconst sz_t svid = (id / CARRY_VSIZE) & ~(N_SZ / CARRY_LENGTH - 1);\n" \
"\t\tconst sz_t vid = (id / CARRY_VSIZE + 1) % (N_SZ / CARRY_LENGTH);\n" \
"\t\tconst sz_t cid = (id % CARRY_VSIZE) + CARRY_VSIZE * ((svid + vid) / (CARRY_WG_SZ / CARRY_VSIZE));\n" \
"\t\tc[cid] = (vid == 0) ? -f : f;\n" \
"\t}\n" \
"}\n" \
"\n" \
"INLINE void carry_2x1(__global uint_32 * restrict const zk, const __local int_64 * const cl,\n" \
"\tint_32 r[CARRY_LENGTH], const sz_t id, const uint2_32 bb_inv_i, const int_32 bs_i)\n" \
"{\n" \
"\tconst sz_t lid = id % CARRY_WG_SZ;\n" \
"\tif (lid >= CARRY_VSIZE)\n" \
"\t{\n" \
"\t\tint_64 f = cl[lid - CARRY_VSIZE];\n" \
"\t\tfor (size_t j = 0; j < CARRY_LENGTH - 1; ++j)\n" \
"\t\t{\n" \
"\t\t\tif (f != 0)\n" \
"\t\t\t{\n" \
"\t\t\t\tf += r[j];\n" \
"\t\t\t\tr[j] = reduce64(&f, bb_inv_i.s0, bb_inv_i.s1, bs_i);\n" \
"\t\t\t}\n" \
"\t\t}\n" \
"\t\tif (f != 0) r[CARRY_LENGTH - 1] = (int_32)(f + r[CARRY_LENGTH - 1]);\n" \
"\t}\n" \
"\n" \
"\tfor (size_t j = 0; j < CARRY_LENGTH; ++j) write_rns(&zk[j * CARRY_VSIZE], r[j]);\n" \
"}\n" \
"\n" \
"__kernel __attribute__((reqd_work_group_size(CARRY_WG_SZ, 1, 1)))\n" \
"void carry1(const __global uint2_32 * restrict const bb_inv, const __global int_32 * restrict const bs,\n" \
"\t__global uint_32 * restrict const z, __global int_64 * restrict const c, __local int_64 * cl, const uint_32 dup)\n" \
"{\n" \
"\tconst sz_t id = (sz_t)get_global_id(0), i = (id % CARRY_VSIZE) + ((id / (N_SZ / CARRY_LENGTH)) & ~(CARRY_VSIZE - 1));\n" \
"\tconst sz_t k = (CARRY_LENGTH - 1) * (id & ~(CARRY_VSIZE - 1)) + id;\n" \
"\tconst uint2_32 bb_inv_i = bb_inv[i]; const int_32 bs_i = bs[i];\n" \
"\tint_32 r[CARRY_LENGTH];\n" \
"\n" \
"\tcarry_1x1(&z[k], c, cl, r, id, bb_inv_i, bs_i, dup >> i);\n" \
"\n" \
"\tbarrier(CLK_LOCAL_MEM_FENCE);\n" \
"\n" \
"\tcarry_2x1(&z[k], cl, r, id, bb_inv_i, bs_i);\n" \
"}\n" \
"#endif\n" \
"\n" \
"__kernel\n" \
"void carry2(const __global uint2_32 * restrict const bb_inv, const __global int_32 * restrict const bs,\n" \
"\t__global uint_32 * restrict const z, const __global int_64 * restrict const c)\n" \
"{\n" \
"\tconst sz_t gid = (sz_t)get_global_id(0), id = (gid / OCL_VSIZE) * CARRY_WG_SZ * OCL_CARRY_VSIZE + (gid % OCL_VSIZE);\n" \
"\tconst sz_t i = (id % OCL_VSIZE) + ((id / (N_SZ / CARRY_LENGTH)) & ~(OCL_VSIZE - 1));\n" \
"\tconst sz_t k = (CARRY_LENGTH - 1) * (id & ~(OCL_VSIZE - 1)) + id;\n" \
"\tconst uint2_32 bb_inv_i = bb_inv[i]; const int_32 bs_i = bs[i];\n" \
"\n" \
"\tint_64 f = c[gid];\n" \
"\tfor (size_t j = 0; j < 3; ++j)\n" \
"\t{\n" \
"\t\tif (f != 0)\n" \
"\t\t{\n" \
"\t\t\tf += get_int(z[k + j * OCL_VSIZE], P1);\n" \
"\t\t\tconst int_32 r = reduce64(&f, bb_inv_i.s0, bb_inv_i.s1, bs_i);\n" \
"\t\t\twrite_rns(&z[k + j * OCL_VSIZE], r);\n" \
"\t\t}\n" \
"\t}\n" \
"\n" \
"\tif (f != 0)\n" \
"\t{\n" \
"\t\tf += get_int(z[k + 3 * OCL_VSIZE], P1);\n" \
"\t\twrite_rns(&z[k + 3 * OCL_VSIZE], (int_32)(f));\n" \
"\t}\n" \
"}\n" \
"\n" \
"// --- misc ---\n" \
"\n" \
"__kernel\n" \
"void extend_z(__global VTYPE * restrict const z)\n" \
"{\n" \
"\tconst sz_t id = (sz_t)get_global_id(0), k = 2 * (id & ~(N_VLEN - 1)) + id;\n" \
"\n" \
"#if OCL_VSIZE == 4\n" \
"\tconst int4_32 r = get_int4(z[k], P1);\n" \
"\tz[k + 1 * N_VLEN] = set_int4(r, P2);\n" \
"\tz[k + 2 * N_VLEN] = set_int4(r, P3);\n" \
"#elif OCL_VSIZE == 2\n" \
"\tconst int2_32 r = get_int2(z[k], P1);\n" \
"\tz[k + 1 * N_VLEN] = set_int2(r, P2);\n" \
"\tz[k + 2 * N_VLEN] = set_int2(r, P3);\n" \
"#else\n" \
"\tconst int_32 r = get_int(z[k], P1);\n" \
"\tz[k + 1 * N_VLEN] = set_int(r, P2);\n" \
"\tz[k + 2 * N_VLEN] = set_int(r, P3);\n" \
"#endif\n" \
"}\n" \
"\n" \
"__kernel\n" \
"void set(__global VTYPE * restrict const z, const uint_32 a)\n" \
"{\n" \
"\tconst sz_t id = (sz_t)get_global_id(0);\n" \
"\tz[id] = (id % N_SZ == 0) ? (VTYPE)(a) : (VTYPE)(0);\n" \
"}\n" \
"\n" \
"__kernel\n" \
"void copy(__global VTYPE * restrict const z, const sz_t dst, const sz_t src)\n" \
"{\n" \
"\tconst sz_t id = (sz_t)get_global_id(0);\n" \
"\tz[3 * N_VLEN * dst + id] = z[3 * N_VLEN * src + id];\n" \
"}\n" \
"\n" \
"__kernel\n" \
"void copyp(__global VTYPE * restrict const zp, __global const VTYPE * restrict const z, const sz_t src)\n" \
"{\n" \
"\tconst sz_t id = (sz_t)get_global_id(0);\n" \
"\tzp[id] = z[3 * N_VLEN * src + id];\n" \
"}\n" \
"\n" \
"__kernel\n" \
"void copy_mask(__global uint_32 * restrict const z, const sz_t dst, const sz_t src, const uint_32 mask)\n" \
"{\n" \
"\tconst sz_t id = (sz_t)get_global_id(0), i = (id % OCL_VSIZE) + (((id / N_SZ) % VSIZE) & ~(OCL_VSIZE - 1));\n" \
"\tif ((mask & (1u << i)) != 0) z[3 * N_VSIZE * dst + id] = z[3 * N_VSIZE * src + id];\n" \
"}\n" \
"\n" \
"#ifdef QVALID\n" \
"\n" \
"__kernel\n" \
"void cosmic_ray(__global uint_32 * restrict const z)\n" \
"{\n" \
"\tconst sz_t id = (sz_t)get_global_id(0);\n" \
"\tif (id == (N_SZ * OCL_VSIZE) / 2) z[id] = addmod(z[id], 1, P1);\n" \
"}\n" \
"\n" \
"#endif\n";
