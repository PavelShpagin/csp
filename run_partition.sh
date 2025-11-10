#!/usr/bin/env bash
set -euo pipefail
SRC=partition_csp.cpp
BIN=./partition_csp
LOG="partition_log_$(date +%Y%m%d_%H%M%S).txt"
g++ -O3 -march=native -std=c++20 "$SRC" -o "$BIN"
sets=("none" "mrv" "lcv" "fc" "prop" "mrv,lcv" "mrv,fc" "mrv,prop" "lcv,fc" "lcv,prop" "fc,prop" "mrv,lcv,fc,prop" "all")
Ns=(24 28 32 36)
MAX=1000
TR=5
SEED=42
echo "start $(date)" | tee -a "$LOG"
for H in "${sets[@]}"; do
  for N in "${Ns[@]}"; do
    echo "----- heuristics=$H n=$N -----" | tee -a "$LOG"
    "$BIN" --heuristics="$H" --n="$N" --max="$MAX" --trials="$TR" --seed="$SEED" | tee -a "$LOG"
  done
done
echo "end $(date)" | tee -a "$LOG"
echo "log $LOG"
