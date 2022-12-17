#!/bin/bash

for run in */*/Events/*/; do
    if grep -q . "${run}"mumuvbstata_delphes.log; then
        grep -q "ERROR:" "${run}"mumuvbstata_delphes.log || continue
    fi
    echo $0: "${run}" >&2
    rm -f "${run}"mumuvbstata_delphes_events.root
    gzip -cd "${run}"*hepmc.gz | \
        DelphesHepMC2 \
        "${run}"../../Cards/delphes_card.dat \
        "${run}"mumuvbstata_delphes_events.root \
        2>&1 | tee "${run}"mumuvbstata_delphes.log
done
