#!/bin/bash

for run in */*/Events/*/; do
    if grep -q . "${run}"mumutataz_delphes.log; then
        grep -q "ERROR:" "${run}"mumutataz_delphes.log || continue
    fi
    echo $0: "${run}" >&2
    grep "ERROR:" "${run}"mumutataz_delphes.log
done
