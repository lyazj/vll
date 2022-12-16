#!/bin/bash

for run in */*/Events/*/; do
    if grep -q . "${run}"mumutt_delphes.log; then
        grep -q "ERROR:" "${run}"mumutt_delphes.log || continue
    fi
    echo $0: "${run}" >&2
    grep "ERROR:" "${run}"mumutt_delphes.log
done
