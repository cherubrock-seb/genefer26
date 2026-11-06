# NVIDIA-only square128 pad16 experiment

This draft experiment changes the GFN16 `square128` OpenCL local-memory layout
only when explicitly compiled with:

```
-DGENEFER_GPU_SQ128_PAD16_NVIDIA=1
```

The selector is default-off.

## Vendor scope

A generic pad16 layout was exact on both tested GPUs but had opposite
performance behavior:

- NVIDIA GeForce RTX 3080: +4.0102% median full-workload throughput over five
  counterbalanced A/B pairs; all five pairs were positive and at least +3%.
- AMD Radeon VII / gfx906: -4.8707% median full-workload throughput over four
  position-balanced A/B pairs.

The generic cross-vendor layout is rejected.

With the experimental selector enabled, the existing OpenCL vendor
classification gates the layout. NVIDIA uses pad16 with 136 VTYPE local slots
per square128 block. Non-NVIDIA devices keep the canonical 128-slot layout.

## Validation

Runtime predicate tracing:

- Radeon VII / gfx906: `is_nvidia=0 use_pad16=0 local_slots=128`
- RTX 3080: `is_nvidia=1 use_pad16=1 local_slots=136`

RTX 3080 gated implementation:

- event timing: +4.3630% complete measured iteration and +14.5086% square128
- production: +3.9808% median throughput over three counterbalanced pairs
- pair speedups: +3.9706%, +3.9975%, +3.9808%
- pair spread: 0.0268 percentage points
- default-off median: 179.970 s versus prior canonical 180.000 s

AMD fallback:

- six balanced event pairs classified the enabled binary as the neutral
  canonical path
- enabled full workload: 542.560 s versus prior canonical median 541.010 s,
  a -0.2857% sanity delta
- rejected generic pad16 median: 568.710 s

Correctness:

- RTX gated production: 48/48 lane results matched
- AMD enabled fallback: 8/8 lane results matched
- final gated matrix: 56/56 lane results matched

Arithmetic, twiddles, and workgroup geometry are unchanged. Performance claims
apply only to the tested hardware and workload; this remains a default-off
experimental path.

## NVIDIA GFN16 carry-shift5 composition

A second default-off experiment can be composed with the square128 pad16 path:

```
-DGENEFER_GPU_CARRY_SHIFT5_NVIDIA_GFN16=1
```

When enabled, carry shift 5 is selected only when the runtime device is NVIDIA
and `ln == 16`. Radeon and every non-GFN16 length retain the canonical carry
shift calculation. The square128 pad16 selector remains independent and
default-off.

### Composition validation

RTX 3080 event-timing census, six fully position-balanced three-way triplets:

- existing PR3 pad16 versus canonical: **+4.5328%** total
- pad16 plus carry-shift5 versus canonical: **+5.7856%** total
- carry-shift5 incremental versus PR3: **+1.1984%** total
- carry1 incremental versus PR3: **+6.7052%**
- all six matched total and carry1 comparisons were positive

Final runtime-gated confirmation on RTX 3080:

- total event timing: **+1.4740%** incremental versus PR3
- carry1 event timing: **+7.5506%**
- all three matched event pairs were positive

Radeon VII canonical-fallback confirmation:

- total event delta: **+0.8523%**
- carry1 event delta: **-0.6420%**
- classified as neutral fallback under the validation gate
- GFN16 production results matched PR3 exactly for all eight tested bases

Production RTX GFN16 wall-clock confirmation versus PR3 over three
counterbalanced eight-base pairs:

- median incremental speedup: **+0.5808%**
- pair speedups: **+0.2689%, +0.5808%, +0.7476%**
- all three pairs were positive
- candidate wall-time spread: **0.1573%**
- all result rows matched pairwise

Change-surface correctness also reused exact RTX PR3-versus-candidate matches at
GFN15 and GFN16. GFN17/GFN18 were not rerun because the carry selector is
source-gated to `ln == 16`; those lengths retain the canonical carry path.

Both experimental selectors remain default-off. Performance claims apply only
to the measured hardware and workloads.

