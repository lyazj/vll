#!/bin/bash

for log in */*/*-1/runmg5-1.log; do
    CS="$(grep Cross-section ${log} | uniq)"
    if [ -z "${CS}" ]; then
        CS="N/A"
    else
        CS="${CS:23}"
    fi
    printf '%-36s%s\n' "$(dirname ${log}): " "${CS}"
done
