#!/bin/bash

for run in */*/Events/*/; do
    if grep -q . "${run}"mumuww_delphes.log; then
        grep -q "ERROR:" "${run}"mumuww_delphes.log || continue
    fi
    echo $0: "${run}" >&2
    grep "ERROR:" "${run}"mumuww_delphes.log
done
