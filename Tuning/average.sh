#!/bin/bash

# Check if input file is provided
if [ $# -ne 1 ]; then
    echo "Usage: $0 <input_file>"
    exit 1
fi

input_file="$1"

# Check if file exists
if [ ! -f "$input_file" ]; then
    echo "Error: File '$input_file' not found"
    exit 1
fi

# Use awk to process the file and calculate averages per setRxBandwidth
awk '
    /Finished/ {
        split($3, bw_arr, ",")
        setting=$2
        bandwidth = bw_arr[1]
        count = $NF
        sum[bandwidth] += count
        num[bandwidth]++
    }
    END {
        for (bw in sum) {
            avg = sum[bw] / num[bw]
            printf "%s, %.2f,%.2f\n", setting, bw, avg
        }
    }' "$input_file" | sort +1n