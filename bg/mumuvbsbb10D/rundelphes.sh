#!/bin/bash

for run in */*/Events/*/; do
    if grep -q . "${run}"mumuvbsbb_delphes.log; then
        grep -q "ERROR:" "${run}"mumuvbsbb_delphes.log || continue
    fi
    echo $0: "${run}" >&2
    rm -f "${run}"mumuvbsbb_delphes_events.root
    gzip -cd "${run}"*hepmc.gz | \
        DelphesHepMC2 \
        "${run}"../../Cards/delphes_card.dat \
        "${run}"mumuvbsbb_delphes_events.root \
        2>&1 | tee "${run}"mumuvbsbb_delphes.log
done
