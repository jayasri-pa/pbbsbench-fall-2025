#!/bin/bash

./perf_metrics_logger -w workloads.txt -o 21Oct2025_log_hwoff_swoff.txt 
./perf_metrics_logger -w workloads.txt -o 21Oct2025_log_hwoff_swon.txt -son
./perf_metrics_logger -w workloads.txt -o 21Oct2025_log_hwon_swoff.txt -hon
./perf_metrics_logger -w workloads.txt -o 21Oct2025_log_hwon_swon.txt -hon -son
