#!/usr/bin/env python3

import sys
import json

if (len(sys.argv) < 2):
    print("Usage: python3 mean_extract.py <path_to_log_file> <path_to_output_json>")
    sys.exit(1)

BASE_PATH = "/home/jayasri/fall-2025-sem1/ug-rd/pbbsbench-fall-2025/"

LOG_PATH = BASE_PATH + sys.argv[1]

OUTPUT_PATH = BASE_PATH + sys.argv[2]

benchmarks_timings = dict()

with open(LOG_PATH, "r") as f:
    read_data = f.readlines()
    lines = [line.strip().split() for line in read_data if line.strip()]
    for line in lines:
        if "geomeans" in line:
            benchmarks_timings[line[0]] = line[-1]

with open(OUTPUT_PATH, "w") as f:
    json.dump(benchmarks_timings, f, indent=4)