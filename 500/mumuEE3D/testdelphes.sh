#!/bin/bash

for run in */*/Events/*/; do
    if grep -q . "${run}"mumuEE_delphes.log; then
        grep -q "ERROR:" "${run}"mumuEE_delphes.log || continue
    fi
    echo $0: "${run}" >&2
    grep "ERROR:" "${run}"mumuEE_delphes.log
done
