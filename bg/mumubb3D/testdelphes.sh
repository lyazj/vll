#!/bin/bash

for run in */*/Events/*/; do
    if grep -q . "${run}"mumubb_delphes.log; then
        grep -q "ERROR:" "${run}"mumubb_delphes.log || continue
    fi
    echo $0: "${run}" >&2
    grep "ERROR:" "${run}"mumubb_delphes.log
done
