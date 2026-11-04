#!/usr/bin/env python3
"""Build both CPU variants, verify with GMP, and alternate whole-square timings."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import platform
import shlex
import statistics
import subprocess
import time


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--baseline", type=Path, required=True)
    p.add_argument("--candidate", type=Path, required=True)
    p.add_argument("--output", type=Path, required=True)
    p.add_argument("--cxx", default="g++")
    p.add_argument("--flags", default="-O3 -mavx512f")
    p.add_argument("--n", type=int, nargs="+", default=[15, 16, 17, 18])
    p.add_argument("--batch", type=int, nargs="+", default=[8, 16, 32])
    p.add_argument("--repeats", type=int, default=5)
    p.add_argument("--iterations", type=int, default=1024,
                   help="n=15 iterations, halved for each larger n")
    p.add_argument("--timeout", type=int, default=300)
    p.add_argument("--verify-only", action="store_true")
    p.add_argument("--affinity", help="optional taskset CPU list; use enough CPUs for the batch")
    a = p.parse_args()
    if any(n not in range(15, 19) for n in a.n) or any(b not in [8, 16, 32] for b in a.batch):
        p.error("require n=15..18 and batch=8/16/32")
    if a.repeats < 3 or a.iterations < 8 or a.timeout <= 0:
        p.error("require repeats >= 3, iterations >= 8, and timeout > 0")
    a.output.mkdir(parents=True, exist_ok=False)
    baseline, candidate = a.baseline.resolve(), a.candidate.resolve()
    harness = candidate / "test/cpu_square.cpp"
    flags = shlex.split(a.flags)
    report = {"scope": "whole CPU modular square+duplication, not full PRP throughput",
              "date_utc": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
              "host": platform.platform(), "compiler": subprocess.check_output(
                  [a.cxx, "--version"], text=True).splitlines()[0],
              "flags": flags, "affinity": a.affinity,
              "cpuinfo": Path("/proc/cpuinfo").read_text().split("\n\n")[0]
                  if Path("/proc/cpuinfo").exists() else "unavailable",
              "sources": {}, "builds": [], "runs": [], "summary": []}
    for name, root in [("baseline", baseline), ("candidate", candidate)]:
        report["sources"][name] = {
            "head": subprocess.check_output(["git", "-C", str(root), "rev-parse", "HEAD"], text=True).strip(),
            "status": subprocess.check_output(["git", "-C", str(root), "status", "--porcelain"], text=True),
            "transform_sha256": digest(root / "src/transformCPU.h"),
            "harness_sha256": digest(harness)}

    def save():
        temp = a.output / "results.tmp"
        temp.write_text(json.dumps(report, indent=2) + "\n")
        temp.replace(a.output / "results.json")

    def execute(command, log):
        r = subprocess.run(command, text=True, stdout=subprocess.PIPE,
                           stderr=subprocess.STDOUT, timeout=a.timeout)
        log.write_text(r.stdout)
        if r.returncode:
            raise RuntimeError(f"command failed ({r.returncode}); see {log}: {r.stdout[-2000:]}")
        return r.stdout

    save()
    try:
        for n in a.n:
            for batch in a.batch:
                binaries = {}
                for variant, root in [("baseline", baseline), ("candidate", candidate)]:
                    binary = (a.output / f"{variant}-n{n}-b{batch}").resolve()
                    cmd = [a.cxx, "-std=c++17", *flags, f"-DTEST_N={n}", f"-DTEST_BATCH={batch}",
                           "-DTEST_SMALL_BASE", "-I", str(root / "src")]
                    if variant == "candidate":
                        cmd.append("-DTEST_UNSPLIT")
                    cmd += [str(harness), "-l:libgmp.so.10", "-o", str(binary)]
                    execute(cmd, a.output / f"build-{variant}-n{n}-b{batch}.log")
                    report["builds"].append({"command": cmd, "binary_sha256": digest(binary)})
                    binaries[variant] = binary
                    save()
                    for mode in ["verify", "verify-adversarial"]:
                        cmd = [str(binary), mode]
                        data = json.loads(execute(cmd, a.output / f"{mode}-{variant}-n{n}-b{batch}.log"))
                        report["runs"].append({"variant": variant, "command": cmd, "result": data})
                        save()
                        print(f"{variant} n={n} batch={batch} {mode}: PASS", flush=True)
                if a.verify_only:
                    continue
                samples = {"baseline": [], "candidate": []}
                hashes = None
                count = max(8, a.iterations >> (n - 15))
                for pair in range(a.repeats):
                    order = ["baseline", "candidate"] if pair % 2 == 0 else ["candidate", "baseline"]
                    for variant in order:
                        cmd = [str(binaries[variant]), "bench", str(count)]
                        if a.affinity:
                            cmd = ["taskset", "-c", a.affinity, *cmd]
                        data = json.loads(execute(cmd, a.output / f"bench-{variant}-n{n}-b{batch}-r{pair}.log"))
                        if hashes is None:
                            hashes = data["hashes"]
                        if data["hashes"] != hashes:
                            raise RuntimeError("baseline/candidate final residue hashes differ")
                        if data["error"] >= 0.5:
                            raise RuntimeError("benchmark roundoff limit exceeded")
                        samples[variant].append(data["us_per_square"])
                        report["runs"].append({"variant": variant, "pair": pair, "command": cmd, "result": data})
                        save()
                med = {k: statistics.median(v) for k, v in samples.items()}
                row = {"n": n, "batch": batch, "medians_us": med,
                       "runtime_reduction_percent": 100 * (1 - med["candidate"] / med["baseline"]),
                       "samples_us": samples,
                       "relative_stdev": {k: statistics.stdev(v) / statistics.mean(v) for k, v in samples.items()}}
                report["summary"].append(row)
                save()
                print(json.dumps(row), flush=True)
    except Exception as error:
        report["failure"] = str(error)
        save()
        raise


if __name__ == "__main__":
    main()
