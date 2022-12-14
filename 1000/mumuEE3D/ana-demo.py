#!/usr/bin/env python3.6

import os
import re

rootfiles = []
with os.popen('ls mumuEE-*/mumuEE/Events/run_01/mumuEE_delphes_events.root') as p:
    for l in p:
        rootfiles.append(l.strip())
rootfiles.sort(key=lambda x: int(re.search(r'-(\d+)', x).group(1)))

import ROOT

ROOT.gSystem.Load('libDelphes.so')
Delphes = ROOT.TChain('Delphes')
for rootfile in rootfiles:
    Delphes.Add(rootfile)
# Delphes.GetFile().Get('Delphes').Print('*VLC*')
reader = ROOT.ExRootTreeReader(Delphes)

VLCjetR05N2 = reader.UseBranch('VLCjetR05N2')
# VLCjetR05N3 = reader.UseBranch('VLCjetR05N3')
# VLCjetR05N4 = reader.UseBranch('VLCjetR05N4')
# VLCjetR05N5 = reader.UseBranch('VLCjetR05N5')
# VLCjetR05N6 = reader.UseBranch('VLCjetR05N6')
# VLCjetR07N2 = reader.UseBranch('VLCjetR07N2')
# VLCjetR07N3 = reader.UseBranch('VLCjetR07N3')
# VLCjetR07N4 = reader.UseBranch('VLCjetR07N4')
# VLCjetR07N5 = reader.UseBranch('VLCjetR07N5')
# VLCjetR07N6 = reader.UseBranch('VLCjetR07N6')
# VLCjetR10N2 = reader.UseBranch('VLCjetR10N2')
# VLCjetR10N3 = reader.UseBranch('VLCjetR10N3')
# VLCjetR10N4 = reader.UseBranch('VLCjetR10N4')
# VLCjetR10N5 = reader.UseBranch('VLCjetR10N5')
# VLCjetR10N6 = reader.UseBranch('VLCjetR10N6')
# VLCjetR12N2 = reader.UseBranch('VLCjetR12N2')
# VLCjetR12N3 = reader.UseBranch('VLCjetR12N3')
# VLCjetR12N4 = reader.UseBranch('VLCjetR12N4')
# VLCjetR12N5 = reader.UseBranch('VLCjetR12N5')
# VLCjetR12N6 = reader.UseBranch('VLCjetR12N6')
# VLCjetR15N2 = reader.UseBranch('VLCjetR15N2')
# VLCjetR15N3 = reader.UseBranch('VLCjetR15N3')
# VLCjetR15N4 = reader.UseBranch('VLCjetR15N4')
# VLCjetR15N5 = reader.UseBranch('VLCjetR15N5')
# VLCjetR15N6 = reader.UseBranch('VLCjetR15N6')
# VLCjetR02_inclusive = reader.UseBranch('VLCjetR02_inclusive')
# VLCjetR05_inclusive = reader.UseBranch('VLCjetR05_inclusive')
# VLCjetR07_inclusive = reader.UseBranch('VLCjetR07_inclusive')
# VLCjetR10_inclusive = reader.UseBranch('VLCjetR10_inclusive')
# VLCjetR12_inclusive = reader.UseBranch('VLCjetR12_inclusive')
# VLCjetR15_inclusive = reader.UseBranch('VLCjetR15_inclusive')

for entry in range(reader.GetEntries()):
    reader.ReadEntry(entry)
    print(VLCjetR05N2[0].BTag, VLCjetR05N2[1].BTag)
