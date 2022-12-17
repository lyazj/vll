#!/bin/bash

for run in */*/Events/*/; do
    if grep -q . "${run}"mumuwwz_delphes.log; then
        grep -q "ERROR:" "${run}"mumuwwz_delphes.log || continue
    fi
    echo $0: "${run}" >&2
    grep "ERROR:" "${run}"mumuwwz_delphes.log
done
