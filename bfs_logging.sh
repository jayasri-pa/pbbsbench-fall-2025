#!/bin/bash

./setOptimization.sh "./runall -only breadthFirstSearch/simpleBFS" log/bfs_perf/simple_bfs.csv breadthFirstSearch simpleBFS
./setOptimization.sh "./runall -only breadthFirstSearch/backForwardBFS" log/bfs_perf/backForward_bfs.csv breadthFirstSearch backForwardBFS
./setOptimization.sh "./runall -only breadthFirstSearch/deterministicBFS" log/bfs_perf/deterministic_bfs.csv breadthFirstSearch deterministicBFS
./setOptimization.sh "./runall -only breadthFirstSearch/ndBFS" log/bfs_perf/nd_bfs.csv breadthFirstSearch ndBFS
./setOptimization.sh "./runall -only breadthFirstSearch/serialBFS" log/bfs_perf/serial_bfs.csv breadthFirstSearch serialBFS
