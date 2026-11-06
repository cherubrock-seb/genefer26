#!/usr/bin/env python3
"""Source-level invariants for the NVIDIA-only square128 pad16 experiment."""

from pathlib import Path
import ast
import re

ROOT = Path(__file__).resolve().parents[1]

ocl = (ROOT / "src/ocl.h").read_text()
engine = (ROOT / "src/engine.h").read_text()
tgpu = (ROOT / "src/transformGPU.h").read_text()
kernel = (ROOT / "ocl/kernel.cl").read_text()
header = (ROOT / "src/ocl/kernel.h").read_text()

assert "#define GENEFER_GPU_SQ128_PAD16_NVIDIA 0u" in engine
assert "bool isNVIDIA() const" in ocl
assert "use_sq128_pad16_nvidia()" in engine
assert "get_square128_local_slots()" in engine
assert "use_sq128_pad16_nvidia ? 16u : 0u" in tgpu
assert "use_sq128_pad16_nvidia ? 136u : 128u" in tgpu
assert "get_square128_local_slots() * BLK128" in engine

assert "sq128_pad_index" in kernel
assert "forward8i_sq128_pad" in kernel
assert "forward8l_sq128_pad" in kernel
assert "square2x4l_sq128_pad" in kernel
assert "backward8l_sq128_pad" in kernel
assert "backward8o_sq128_pad" in kernel

body = header[header.index("static const char * const src_ocl_kernel"):]
literals = re.findall(r'"(?:\\.|[^"\\])*"', body)
decoded = "".join(ast.literal_eval(x) for x in literals)
assert decoded == kernel

print("GPU_SQ128_PAD16_NVIDIA_SOURCE_GATE=PASS")
