# CPU FFT specialization investigation

## Ownership and baseline
- Repository: `cherubrock-seb/genefer26`.
- Branch: `perf/cpu-fft-static-recursion`; owned by this engineering session.
- Fork and upstream baseline: `0e962f8f09f0e4dbdb2acaf76b30bd7aa30a199e` (identical).
- Starting head: same baseline. The current containing commit is resolved with
  `git rev-parse HEAD`; later checkpoints record their exact predecessor to avoid
  claiming a self-referential commit hash.
- Verified connected identity: `cherubrock-seb`, repository permission: admin.
- On 2026-11-04, neither fork nor upstream had pull requests; the fork had only
  `main` before this branch was created.
- Cherub's separately reported n=15 GPU inner64 experiment is outside this scope.

## Hypothesis and invariants
The hot CPU square calls `square_e/o(z, w, m, sj)` recursively despite all
transform sizes being compile-time constants. Specializing the recursion by
`m` may remove runtime branches and address arithmetic and expose smaller leaf
loops to the compiler. The affected fraction and speedup are NOT yet measured.
For affected runtime fraction f and local speedup s, the end-to-end upper
estimate is 1 / (1 - f + f/s). Do not infer a 10% gain without measurements.

Keep butterfly order, twiddle indices, split arithmetic, modular carry,
duplication masks, roundoff collection, and proof/Gerbicz paths unchanged.
Start with the square path only. Use the original source as a build-time A/B
control; revert the optimization if exactness or performance fails.
Risk: code size, inlining/register pressure, and compiler-specific regressions.

## Environment and status
- Linux x86-64 container; reported CPU AMD EPYC 9V74, AVX-512 available.
- GCC 13.3.0; 8 CPU quota, 9 exposed logical processors; shared virtual host.
- No exposed GPU device: no modern GPU performance claim is possible here.
- Source inspection PASS; compilation, exactness, profiling and timing PENDING.
- GMP development headers were absent; acquiring matching Ubuntu headers.
- Files changed at this checkpoint: this journal only.

## Experiments and next executable action
1. Build a bounded whole-square harness with deterministic inputs; save source
   and binary identity, all bases, n, batch, compiler flags, samples and residues.
2. Profile the baseline (GCC/gprof if hardware counters are unavailable).
3. Implement compile-time square recursion only if the profile supports it.
4. Compare every output limb against baseline and independently against GMP,
   including heterogeneous bases, n=15..18, batch 8/16/32 and duplicate masks.
5. Measure alternating A/B whole-square runs, then application quick/proof/
   certificate flows. Keep benchmark scope explicit if full PRP is too long.
6. Open a draft PR in this fork; leave integration and GPU claims pending.

Initial probe command (local, not yet executed successfully):
```sh
g++ -std=c++17 -O3 -ffinite-math-only -frename-registers \
  -fprefetch-loop-arrays -mavx512f -mtune=znver3 \
  -I src -pg probe_cpu.cpp -o probe_cpu_profile
./probe_cpu_profile 2048
gprof -b probe_cpu_profile gmon.out
```
The probe is temporary; the reproducible harness will be committed once built.
