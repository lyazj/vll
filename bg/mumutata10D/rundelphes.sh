#!/bin/bash

for run in */*/Events/*/; do
    if grep -q . "${run}"mumutata_delphes.log; then
        grep -q "ERROR:" "${run}"mumutata_delphes.log || continue
    fi
    echo $0: "${run}" >&2
    rm -f "${run}"mumutata_delphes_events.root
    gzip -cd "${run}"*hepmc.gz | \
        DelphesHepMC2 \
        "${run}"../../Cards/delphes_card.dat \
        "${run}"mumutata_delphes_events.root \
        2>&1 | tee "${run}"mumutata_delphes.log
done
