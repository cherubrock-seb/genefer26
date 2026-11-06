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
