#!/bin/bash

for run in */*/Events/*/; do
    if grep -q . "${run}"mumubbz_delphes.log; then
        grep -q "ERROR:" "${run}"mumubbz_delphes.log || continue
    fi
    echo $0: "${run}" >&2
    grep "ERROR:" "${run}"mumubbz_delphes.log
done
