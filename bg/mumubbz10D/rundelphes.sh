#!/bin/bash

for run in */*/Events/*/; do
    if grep -q . "${run}"mumubbz_delphes.log; then
        grep -q "ERROR:" "${run}"mumubbz_delphes.log || continue
    fi
    echo $0: "${run}" >&2
    rm -f "${run}"mumubbz_delphes_events.root
    gzip -cd "${run}"*hepmc.gz | \
        DelphesHepMC2 \
        "${run}"../../Cards/delphes_card.dat \
        "${run}"mumubbz_delphes_events.root \
        2>&1 | tee "${run}"mumubbz_delphes.log
done
