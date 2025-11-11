#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

SRC="partition_csp.cpp"
BIN="./partition_csp"
N="${N:-50}"
TRIALS="${TRIALS:-10}"
SEED="${SEED:-12345}"

echo "[compile] g++ -O3 -std=c++20 ${SRC} -o ${BIN}"
g++ -O3 -std=c++20 "${SRC}" -o "${BIN}"

echo
echo "=== BRUTEFORCE (no heuristics) ==="
time "${BIN}" --heuristics=none --n="${N}" --max=1000 --trials="${TRIALS}" --seed="${SEED}"

echo
echo "=== WITH HEURISTICS (mrv,lcv,fc,prop) ==="
time "${BIN}" --heuristics=all --n="${N}" --max=1000 --trials="${TRIALS}" --seed="${SEED}"

