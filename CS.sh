#!/bin/bash

for log in */*/*-1/runmg5-1.log; do
    CS="$(grep Cross-section ${log} | uniq)"
    if [ -z "${CS}" ]; then
        CS="failed"
    else
        CS="${CS:23}"
    fi
    printf '%-32s%s\n' "$(dirname ${log}): " "${CS}"
done
