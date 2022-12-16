#!/bin/bash

for run in */*/Events/*/; do
    if grep -q . "${run}"mumutt_delphes.log; then
        grep -q "ERROR:" "${run}"mumutt_delphes.log || continue
    fi
    echo $0: "${run}" >&2
    rm -f "${run}"mumutt_delphes_events.root
    gzip -cd "${run}"*hepmc.gz | \
        DelphesHepMC2 \
        "${run}"../../Cards/delphes_card.dat \
        "${run}"mumutt_delphes_events.root \
        2>&1 | tee "${run}"mumutt_delphes.log
done
