#!/bin/bash

for run in */*/Events/*/; do
    if grep -q . "${run}"mumuvbstata_delphes.log; then
        grep -q "ERROR:" "${run}"mumuvbstata_delphes.log || continue
    fi
    echo $0: "${run}" >&2
    grep "ERROR:" "${run}"mumuvbstata_delphes.log
done
