#!/bin/bash

for run in */*/Events/*/; do
    if grep -q . "${run}"mumuww_delphes.log; then
        grep -q "ERROR:" "${run}"mumuww_delphes.log || continue
    fi
    echo $0: "${run}" >&2
    rm -f "${run}"mumuww_delphes_events.root
    gzip -cd "${run}"*hepmc.gz | \
        DelphesHepMC2 \
        "${run}"../../Cards/delphes_card.dat \
        "${run}"mumuww_delphes_events.root \
        2>&1 | tee "${run}"mumuww_delphes.log
done
