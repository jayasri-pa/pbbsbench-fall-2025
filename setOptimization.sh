#!/bin/bash

# Setting the optimization flag of g++ in parallelDefs

FILE_PATH="/home/jayasri/fall-2025-sem1/ug-rd/pbbsbench-fall-2025/common/parallelDefs"
LINE=8
WORD=4

COMMAND="$1"
LOG_FILE="$2"
BENCHMARK_TYPE="$3"
PROGRAM="$4"

NUM_RUNS=10

OPTIMIZATION_FLAGS=("-O0" "-O1" "-O2" "-O3" "-Os" "-Ofast")

convert_to_seconds() {
    echo "$1" | awk '{
        minutes = $0;
        gsub(/m.*/,"",minutes);
        seconds = $0;
        gsub(/.*m/,"",seconds);
        gsub(/s/,"",seconds);
        print minutes * 60 + seconds
    }'
}

for NEW_WORD in "${OPTIMIZATION_FLAGS[@]}"; do

    echo "--------------------------------------------------------"
    echo "Testing with optimization flag: $NEW_WORD"
    
    TEMP_FILE=$(mktemp)
    /usr/bin/mawk \
        -v line_no="$LINE" \
        -v word_no="$WORD" \
        -v new_word="$NEW_WORD" \
        'NR == line_no {
            $word_no = new_word        
        } 1' \
        "$FILE_PATH" > "$TEMP_FILE"

    mv "$TEMP_FILE" "$FILE_PATH"

    # sum variables to hold values on add
    total_real_s=$(echo "0.0" | bc)
    total_user_s=$(echo "0.0" | bc)
    total_sys_s=$(echo "0.0" | bc)

    for (( i=1; i<=NUM_RUNS; i++ )); do
        echo "  -> Run $i of $NUM_RUNS"
        
        # Run the command and capture output
        FULL_OUTPUT=$( { time $COMMAND; } 2>&1 )
        
        # Extract time strings
        REAL_TIME_STR=$(echo "$FULL_OUTPUT" | grep 'real' | awk '{print $2}')
        USER_TIME_STR=$(echo "$FULL_OUTPUT" | grep 'user' | awk '{print $2}')
        SYS_TIME_STR=$(echo "$FULL_OUTPUT" | grep 'sys' | awk '{print $2}')
        
        # Convert to seconds
        current_real_s=$(convert_to_seconds "$REAL_TIME_STR")
        current_user_s=$(convert_to_seconds "$USER_TIME_STR")
        current_sys_s=$(convert_to_seconds "$SYS_TIME_STR")
        
        # Add to sum variables
        total_real_s=$(echo "$total_real_s + $current_real_s" | bc)
        total_user_s=$(echo "$total_user_s + $current_user_s" | bc)
        total_sys_s=$(echo "$total_sys_s + $current_sys_s" | bc)
    
    done

    # Calculate the averages
    avg_real_s=$(echo "scale=3; $total_real_s / $NUM_RUNS" | bc)
    avg_user_s=$(echo "scale=3; $total_user_s / $NUM_RUNS" | bc)
    avg_sys_s=$(echo "scale=3; $total_sys_s / $NUM_RUNS" | bc)


    if [ ! -f "$LOG_FILE" ]; then
        echo "optimization,benchmark_type,program,real_time_s,user_time_s,sys_time_s,total_cpu_time_s" > "$LOG_FILE"
    fi

    echo "${NEW_WORD/#-/},$BENCHMARK_TYPE,$PROGRAM,$avg_real_s,$avg_user_s,$avg_sys_s" >> "$LOG_FILE"

done

exit 0