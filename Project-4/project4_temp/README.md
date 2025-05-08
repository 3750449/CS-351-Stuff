
# Project 4 - Julia Set GPU Acceleration

## Files Included

- `julia.cu` — CUDA implementation of Julia set with fixed `c = -0.8 + 0.156i`
- `julia.cpp` — CPU version of Julia set with dynamic image sizing
- `iota.cu` — CUDA `iota` kernel implementation, validated
- `Makefile` — Builds `.gpu` and `.cpu` binaries
- `runTrials.sh` — Runs benchmarks at various sizes
- `julia.png` — Output image generated from `julia.gpu 1024`

---

## Performance Results (GPU)

| Vector Length | Wall Clock Time (s) | User Time (s) | System Time (s) |
|:-------------:|--------------------:|--------------:|----------------:|
| 10            | 0.30                | 0.01          | 0.28            |
| 100           | 0.23                | 0.01          | 0.21            |
| 1,000         | 0.23                | 0.01          | 0.21            |
| 10,000        | 0.21                | 0.01          | 0.20            |
| 100,000       | 0.23                | 0.01          | 0.21            |
| 1,000,000     | 0.22                | 0.01          | 0.21            |
| 5,000,000     | 0.22                | 0.00          | 0.21            |
| 100,000,000   | 0.23                | 0.01          | 0.21            |
| 500,000,000   | 0.23                | 0.00          | 0.22            |
| 1,000,000,000 | 0.22                | 0.01          | 0.21            |

---

## Fractal Output

Generated with:

```bash
./julia.gpu 1024



