#!/bin/bash

for run in */*/Events/*/; do
    if grep -q . "${run}"mumuvbsbb_delphes.log; then
        grep -q "ERROR:" "${run}"mumuvbsbb_delphes.log || continue
    fi
    echo $0: "${run}" >&2
    grep "ERROR:" "${run}"mumuvbsbb_delphes.log
done
