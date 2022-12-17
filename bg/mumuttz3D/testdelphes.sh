#!/bin/bash

for run in */*/Events/*/; do
    if grep -q . "${run}"mumuttz_delphes.log; then
        grep -q "ERROR:" "${run}"mumuttz_delphes.log || continue
    fi
    echo $0: "${run}" >&2
    grep "ERROR:" "${run}"mumuttz_delphes.log
done
