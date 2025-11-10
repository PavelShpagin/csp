# Partition CSP Solver

Build one C++ program that decides PARTITION (Equal-Subset Sum) as a CSP over signs (x_i in {-1,+1}) with the global constraint (sum_i x_i a_i = 0). Implement a backtracking solver with togglable heuristics: MRV, LCV, forward checking (FC), and constraint propagation (PROP). AC-3 must not be included. Provide CLI flags: `--n`, `--max`, `--trials`, `--seed`, `--heuristics=all|none|comma,list` where the names are `mrv,lcv,fc,prop`. Instances are generated uniformly from `[1..max]`. For each trial print one line: `heuristics=..., n=..., max=..., trials=...` header once, then per trial `seed=..., SAT=0/1, time_ms=..., nodes=..., forced=..., pruned=...`. Keep code clean and without comments. Provide a bash script that compiles with `-O3 -march=native`, runs a small grid over heuristic sets and sizes, and appends all outputs to one timestamped log file.

## Usage

- Compile and run manually:
  ```
  g++ -O3 -march=native -std=c++20 partition_csp.cpp -o partition_csp
  ./partition_csp --n=30 --max=1000 --trials=1 --seed=12345 --heuristics=all
  ```
- Batch sweep and log results:
  ```
  bash run_partition.sh
  ```