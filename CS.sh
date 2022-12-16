#!/bin/bash

for log in */*/*-1/runmg5-1.log; do
    printf '%-32s' "$(dirname ${log}): "
    (grep Cross-section ${log} || echo - - failed) | uniq | awk '{print $3" "$4" "$5" "$6}'
done
