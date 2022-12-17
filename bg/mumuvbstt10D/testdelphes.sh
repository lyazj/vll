#!/bin/bash

for run in */*/Events/*/; do
    if grep -q . "${run}"mumuvbsww_delphes.log; then
        grep -q "ERROR:" "${run}"mumuvbsww_delphes.log || continue
    fi
    echo $0: "${run}" >&2
    grep "ERROR:" "${run}"mumuvbsww_delphes.log
done
