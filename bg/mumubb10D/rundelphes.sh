#!/bin/bash

for run in */*/Events/*/; do
    if grep -q . "${run}"mumubb_delphes.log; then
        grep -q "ERROR:" "${run}"mumubb_delphes.log || continue
    fi
    echo $0: "${run}" >&2
    rm -f "${run}"mumubb_delphes_events.root
    gzip -cd "${run}"*hepmc.gz | \
        DelphesHepMC2 \
        "${run}"../../Cards/delphes_card.dat \
        "${run}"mumubb_delphes_events.root \
        2>&1 | tee "${run}"mumubb_delphes.log
done
