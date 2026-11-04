# Experimental GPU register-tiled square

Repository: `cherubrock-seb/genefer26`
Branch: `perf/gpu-register-tiled-square-20261104`
Base / inspected upstream: `0e962f8f09f0e4dbdb2acaf76b30bd7aa30a199e`

## Checkpoint 1: source hypothesis

The production square128/256/512 kernels perform a global-to-local radix-8
transform, another local radix-8 transform, a local terminal square, then
reverse the two transforms. They move eight tile-sized arrays through local
memory (four writes and four reads), with four local-memory barriers.

Candidate: factor each tile as F x 16, F = 8, 16, 32. Compute the F-point
column transforms in private registers, exchange once through local memory,
square each 16-coefficient row entirely in private registers, exchange once,
then compute the inverse column transforms and write globally. Each tile
requires four local-memory array transfers and two barriers. Global traffic
and the algebraic butterfly schedule are unchanged.

A work-item owns 16 coefficients for 128/256 tiles and 32 for 512 tiles.
Independent short transforms are serialized within a work-item where needed,
so no work-items are intentionally inactive. The intended workgroup geometry
is 32 work-items, with 8 KiB / 8 KiB / 16 KiB local storage respectively at
OCL_VSIZE=4. Extra register pressure and private-array spilling can reverse the
benefit; this is an experimental, default-off candidate.

All modular operations retain the existing canonical [0,p) arithmetic,
including the separate IS32 path. No lazy reduction or widened residue ranges
are introduced. Forward and inverse twiddle indices must be verified against
the original staged schedule before claiming source-level equivalence.

## Evidence and next action

- Identity verified through connected GitHub: cherubrock-seb.
- Fork and upstream heads match the base above.
- Existing carry-workgroup and CPU branches are independent and untouched.
- STATIC_VALIDATION: source reconnaissance only at this checkpoint.
- THEORETICAL_GAIN: 50% less local-memory traffic and 50% fewer barriers in the
  affected square kernel; no measured speedup. If this removed work is fraction
  f of total runtime, the optimistic ceiling is 1/(1-f/2), before occupancy costs.
- GPU_VALIDATION=DEFERRED_TO_CHERUB
- GPU_EXACTNESS=PENDING_CHERUB
- GPU_PERFORMANCE=PENDING_CHERUB
- PRODUCTION_READINESS=NOT_ESTABLISHED

Next executable action: implement the OpenCL register helpers and tile kernels,
wire an explicit host compile-time opt-in, regenerate the embedded source, and
publish the implementation checkpoint. Then add scalar exact-arithmetic and
mapping oracles plus the Cherub execution recipe.

The authoritative current SHA is the branch ref / PR head. This journal cannot
contain its own commit hash; later checkpoints will name their parent explicitly.
