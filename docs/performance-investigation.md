# CPU small-base specialization investigation

## Durable state
- Repository: `cherubrock-seb/genefer26`.
- Branch: `perf/cpu-fft-static-recursion` (retained after rejecting that initial hypothesis).
- Draft PR: https://github.com/cherubrock-seb/genefer26/pull/1
- Fork/upstream baseline: `0e962f8f09f0e4dbdb2acaf76b30bd7aa30a199e` (identical).
- Previous checkpoint: `f7cc2be1fe7036f5c07e32e938e2d232f4ec488b`. Resolve the current containing head
  with `git rev-parse HEAD`.
- Connected identity/commit attribution: `cherubrock-seb`; admin permission verified.
- No other fork branches or PRs existed at reconnaissance. Cherub GPU work is
  outside this CPU branch.

## Decision and hypothesis
REJECT the initial static-recursion optimization: alternating GCC measurements
did not establish a repeatable gain. Its prototype remains in Git history.
The baseline n=15/batch=8 gprof sample placed 57.36% in square_e/o and 24.63% in carry.

The replacement specializes batches whose largest base is <= 65536. Normalized
digits are about b/2, well below half of the 2^20 split. The high component is
therefore zero. Its FFT, products and carry arithmetic can be removed by the
compiler while preserving the low-component expressions. Larger or mixed batches
keep the original split path. This affects most arithmetic, unlike recursion
overhead. Storage deliberately retains both components for checkpoint compatibility.

Invariants to verify: every active high component remains zero; carry correction
does not reach the split threshold; masks, signed digits, twiddle order, roundoff
collection, proof/Gerbicz operations, and saved registers remain compatible.
Direct construction of the unsplit path rejects larger bases before allocation.

## Files and evidence
- `src/transformCPU.h`: compile-time high-component accessor, bounded factory dispatch.
- `test/cpu_square.cpp`: GMP reconstruction of full residues, deterministic dense,
  sparse and alternating digits, masks, prepared multiplication, timings.
- `test/run_cpu_square.py`: paired builds, exactness runs, alternating A/B timings,
  command/compiler/source/binary provenance and incremental JSON output.
- GCC 13.3, n=15/batch=8/small bases: 72 exact residue checks PASS, plus 72
  adversarial checks PASS. Baseline/candidate timed hashes and error agree.
- Preliminary unpaired whole-square timing: baseline 1463.46 us vs candidate
  1036.31 us (29.2% less time). This is a screening result, NOT a KEEP.
- Clang 18.1.3, n=18/batch=8/small bases: 72 normal + 72 adversarial checks PASS,
  maximum observed roundoff 0.0195312.
- Full n=15..18 / batch=8,16,32 matrix is running. Full application, checkpoint
  compatibility and alternative ISA checks remain PENDING. No GPU evidence.

## Inherited limitations found
The new oracle also tests large bases outside the optimized domain:
- GCC baseline and static prototype n=15: all -1 digits, base 1048574, fail
  at the third square with roundoff 0.5.
- GCC static prototype n=18: dense input near base 2e9 fails at the first square
  with roundoff 0.5; a direct unchanged-baseline comparison remains required.
These are not evidence against the small-base specialization, but prevent any
blanket all-domain exactness claim. The failing cases remain reproducible with
the default (large-base) harness and `verify-adversarial`.

## Reproduction and next action
Host reports AMD EPYC 9V74 with AVX-512, Linux x86-64 virtual machine, 8 CPU quota.
GMP 6.3 headers and Clang 18.1.3 were extracted from matching Ubuntu packages.
No BOINC/GPU experiment has been run.

```sh
# Use an installed clang++-18 and libgmp development headers, or add their paths.
python3 test/run_cpu_square.py \
  --baseline ../genefer26-baseline --candidate . --output ../matrix-clang \
  --cxx clang++-18 \
  --flags '-O3 -ffinite-math-only -ffp-contract=fast -fno-trapping-math -fno-signed-zeros -mavx512f -mtune=znver4' \
  --repeats 3 --iterations 512 --affinity 0-3
```

Next: finish and inspect the matrix, then validate checkpoint interchange and
complete application proof/certificate flows. Run a real n=15 PRP A/B before
claiming full-application throughput. Leave this PR draft until gates pass.
