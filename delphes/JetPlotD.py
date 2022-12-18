#!/usr/bin/env python3.6

import uproot
import numpy as np
import matplotlib.pyplot as plt

CS = {}
with open('CS.txt') as f:
    for l in f:
        r, c = l.split()
        CS[r.replace('_collect', '_collect_min')] = CS[r] = float(c)

root3 = [
    "mumubb_collect_min_bg_3.root",
    "mumubbz_collect_min_bg_3.root",
    "mumutata_collect_min_bg_3.root",
    "mumutataz_collect_min_bg_3.root",
    "mumutt_collect_min_bg_3.root",
    "mumuttz_collect_min_bg_3.root",
    "mumuvbsbb_collect_min_bg_3.root",
    "mumuvbstata_collect_min_bg_3.root",
    "mumuvbstt_collect_min_bg_3.root",
    "mumuvbsww_collect_min_bg_3.root",
    "mumuww_collect_min_bg_3.root",
    "mumuwwz_collect_min_bg_3.root",
    "mumuEE_collect_min_1000_3.root",
    "mumuEE_collect_min_1500_3.root",
    "mumuEE_collect_min_2000_3.root",
    "mumuEE_collect_min_3000_3.root",
    "mumuEE_collect_min_500_3.root",
    "mumuNN_collect_min_1000_3.root",
    "mumuNN_collect_min_1500_3.root",
    "mumuNN_collect_min_2000_3.root",
    "mumuNN_collect_min_3000_3.root",
    "mumuNN_collect_min_500_3.root",
]

root10 = [
    "mumubb_collect_min_bg_10.root",
    "mumubbz_collect_min_bg_10.root",
    "mumutata_collect_min_bg_10.root",
    "mumutataz_collect_min_bg_10.root",
    "mumutt_collect_min_bg_10.root",
    "mumuttz_collect_min_bg_10.root",
    "mumuvbsbb_collect_min_bg_10.root",
    "mumuvbstata_collect_min_bg_10.root",
    "mumuvbstt_collect_min_bg_10.root",
    "mumuvbsww_collect_min_bg_10.root",
    "mumuww_collect_min_bg_10.root",
    "mumuwwz_collect_min_bg_10.root",
    "mumuEE_collect_min_1000_10.root",
    "mumuEE_collect_min_1500_10.root",
    "mumuEE_collect_min_2000_10.root",
    "mumuEE_collect_min_3000_10.root",
    "mumuEE_collect_min_500_10.root",
    "mumuNN_collect_min_1000_10.root",
    "mumuNN_collect_min_1500_10.root",
    "mumuNN_collect_min_2000_10.root",
    "mumuNN_collect_min_3000_10.root",
    "mumuNN_collect_min_500_10.root",
]

plt.figure(figsize=(10, 6))
for root in root3:
    cs = CS[root]
    with uproot.open(root) as file:
        Collect = file['Collect']
        try:
            JetPT = Collect['Jet/Jet.PT'].array(library='np')
            n = len(JetPT)
            JetPT = np.concatenate(JetPT)
            print(f'{root}: {n} events, {len(JetPT)} b-jets')
        except ValueError:
            print(f'{root}: {n} events')
            continue
        if root[:6] == 'mumuEE' or root[:6] == 'mumuNN':
            plt.hist(JetPT, 50, density=True, histtype='step', label=root.replace('_collect_min', '')[:-5])
        else:
            plt.hist(JetPT, 50, density=True, histtype='barstacked', label=root.replace('_collect_min', '')[:-5])
plt.title('$p_\\mathrm{T}$ distribution of b-tagged jets')
plt.xlabel('$p_\\mathrm{T}$')
plt.ylabel('relevant strenth')
plt.yscale('linear')
plt.ylim(0, 0.005)
plt.legend()
plt.tight_layout()
plt.savefig('JetPTD3.pdf')

plt.figure(figsize=(10, 6))
for root in root3:
    cs = CS[root]
    with uproot.open(root) as file:
        Collect = file['Collect']
        try:
            JetEta = Collect['Jet/Jet.Eta'].array(library='np')
            n = len(JetEta)
            JetEta = np.concatenate(JetEta)
            print(f'{root}: {n} events, {len(JetEta)} b-jets')
        except ValueError:
            print(f'{root}: {n} events')
            continue
        if root[:6] == 'mumuEE' or root[:6] == 'mumuNN':
            plt.hist(JetEta, 50, density=True, histtype='step', label=root.replace('_collect_min', '')[:-5])
        else:
            plt.hist(JetEta, 50, density=True, histtype='barstacked', label=root.replace('_collect_min', '')[:-5])
plt.title('$\\eta$ distribution of b-tagged jets')
plt.xlabel('$\\eta$')
plt.ylabel('relevant strenth')
plt.yscale('linear')
plt.legend()
plt.tight_layout()
plt.savefig('JetEtaD3.pdf')

plt.figure(figsize=(10, 6))
for root in root3:
    cs = CS[root]
    with uproot.open(root) as file:
        Collect = file['Collect']
        try:
            JetMass = Collect['Jet/Jet.Mass'].array(library='np')
            n = len(JetMass)
            JetMass = np.concatenate(JetMass)
            print(f'{root}: {n} events, {len(JetMass)} b-jets')
        except ValueError:
            print(f'{root}: {n} events')
            continue
        if root[:6] == 'mumuEE' or root[:6] == 'mumuNN':
            plt.hist(JetMass, 50, density=True, histtype='step', label=root.replace('_collect_min', '')[:-5])
        else:
            plt.hist(JetMass, 50, density=True, histtype='barstacked', label=root.replace('_collect_min', '')[:-5])
plt.title('mass distribution of b-tagged jets')
plt.xlim(0, 200)
plt.ylim(0, 0.05)
plt.xlabel('mass')
plt.ylabel('relevant strenth')
plt.yscale('linear')
plt.legend()
plt.tight_layout()
plt.savefig('JetMassD3.pdf')

plt.figure(figsize=(10, 6))
for root in root10:
    cs = CS[root]
    with uproot.open(root) as file:
        Collect = file['Collect']
        try:
            JetPT = Collect['Jet/Jet.PT'].array(library='np')
            n = len(JetPT)
            if n < 1000:
                raise ValueError(f'n = {n} too small')
            JetPT = np.concatenate(JetPT)
            print(f'{root}: {n} events, {len(JetPT)} b-jets')
        except ValueError:
            print(f'{root}: {n} events')
            continue
        if root[:6] == 'mumuEE' or root[:6] == 'mumuNN':
            plt.hist(JetPT, 50, density=True, histtype='step', label=root.replace('_collect_min', '')[:-5])
        else:
            plt.hist(JetPT, 50, density=True, histtype='barstacked', label=root.replace('_collect_min', '')[:-5])
plt.title('$p_\\mathrm{T}$ distribution of b-tagged jets')
plt.xlabel('$p_\\mathrm{T}$')
plt.ylabel('relevant strenth')
plt.yscale('linear')
plt.ylim(0, 0.002)
plt.legend()
plt.tight_layout()
plt.savefig('JetPTD10.pdf')

plt.figure(figsize=(10, 6))
for root in root10:
    cs = CS[root]
    with uproot.open(root) as file:
        Collect = file['Collect']
        try:
            JetEta = Collect['Jet/Jet.Eta'].array(library='np')
            n = len(JetEta)
            if n < 1000:
                raise ValueError(f'n = {n} too small')
            JetEta = np.concatenate(JetEta)
            print(f'{root}: {n} events, {len(JetEta)} b-jets')
        except ValueError:
            print(f'{root}: {n} events')
            continue
        if root[:6] == 'mumuEE' or root[:6] == 'mumuNN':
            plt.hist(JetEta, 50, density=True, histtype='step', label=root.replace('_collect_min', '')[:-5])
        else:
            plt.hist(JetEta, 50, density=True, histtype='barstacked', label=root.replace('_collect_min', '')[:-5])
plt.title('$\\eta$ distribution of b-tagged jets')
plt.xlabel('$\\eta$')
plt.ylabel('relevant strenth')
plt.yscale('linear')
plt.legend()
plt.tight_layout()
plt.savefig('JetEtaD10.pdf')

plt.figure(figsize=(10, 6))
for root in root10:
    cs = CS[root]
    with uproot.open(root) as file:
        Collect = file['Collect']
        try:
            JetMass = Collect['Jet/Jet.Mass'].array(library='np')
            n = len(JetMass)
            JetMass = np.concatenate(JetMass)
            print(f'{root}: {n} events, {len(JetMass)} b-jets')
        except ValueError:
            print(f'{root}: {n} events')
            continue
        if root[:6] == 'mumuEE' or root[:6] == 'mumuNN':
            plt.hist(JetMass, 50, density=True, histtype='step', label=root.replace('_collect_min', '')[:-5])
        else:
            plt.hist(JetMass, 50, density=True, histtype='barstacked', label=root.replace('_collect_min', '')[:-5])
plt.title('mass distribution of b-tagged jets')
plt.xlim(0, 500)
plt.ylim(0, 0.02)
plt.xlabel('mass')
plt.ylabel('relevant strenth')
plt.yscale('linear')
plt.legend()
plt.tight_layout()
plt.savefig('JetMassD10.pdf')
