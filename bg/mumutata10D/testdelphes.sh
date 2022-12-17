#!/bin/bash

for run in */*/Events/*/; do
    if grep -q . "${run}"mumutata_delphes.log; then
        grep -q "ERROR:" "${run}"mumutata_delphes.log || continue
    fi
    echo $0: "${run}" >&2
    grep "ERROR:" "${run}"mumutata_delphes.log
done
