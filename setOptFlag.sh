#!/bin/bash

FILE_PATH="/home/jayasri/pbbsbench-fall-2025/common/parallelDefs" # Based on MS machine

LINE=8
WORD=4
NEW_WORD="$1"

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

