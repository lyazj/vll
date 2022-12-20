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
    "mumubb_collect_bg_3.root",
    "mumubbz_collect_bg_3.root",
    "mumutata_collect_bg_3.root",
    "mumutataz_collect_bg_3.root",
    "mumutt_collect_bg_3.root",
    "mumuttz_collect_bg_3.root",
    "mumuvbsbb_collect_bg_3.root",
    "mumuvbstata_collect_bg_3.root",
    "mumuvbstt_collect_bg_3.root",
    "mumuvbsww_collect_bg_3.root",
    "mumuww_collect_bg_3.root",
    "mumuwwz_collect_bg_3.root",
    "mumuEE_collect_1000_3.root",
    "mumuEE_collect_1500_3.root",
    "mumuEE_collect_2000_3.root",
    "mumuEE_collect_3000_3.root",
    "mumuEE_collect_500_3.root",
    "mumuNN_collect_1000_3.root",
    "mumuNN_collect_1500_3.root",
    "mumuNN_collect_2000_3.root",
    "mumuNN_collect_3000_3.root",
    "mumuNN_collect_500_3.root",
]

root10 = [
    "mumubb_collect_bg_10.root",
    "mumubbz_collect_bg_10.root",
    "mumutata_collect_bg_10.root",
    "mumutataz_collect_bg_10.root",
    "mumutt_collect_bg_10.root",
    "mumuttz_collect_bg_10.root",
    "mumuvbsbb_collect_bg_10.root",
    "mumuvbstata_collect_bg_10.root",
    "mumuvbstt_collect_bg_10.root",
    "mumuvbsww_collect_bg_10.root",
    "mumuww_collect_bg_10.root",
    "mumuwwz_collect_bg_10.root",
    "mumuEE_collect_1000_10.root",
    "mumuEE_collect_1500_10.root",
    "mumuEE_collect_2000_10.root",
    "mumuEE_collect_3000_10.root",
    "mumuEE_collect_500_10.root",
    "mumuNN_collect_1000_10.root",
    "mumuNN_collect_1500_10.root",
    "mumuNN_collect_2000_10.root",
    "mumuNN_collect_3000_10.root",
    "mumuNN_collect_500_10.root",
]

plt.figure(figsize=(10, 6))
for root in root3:
    cs = CS[root]
    with uproot.open(root) as file:
        Collect = file['Collect']
        try:
            JetPT = Collect['JetPT'].array(library='np')
            n = len(JetPT)
            JetPT = np.concatenate(JetPT)
            print(f'{root}: {n} events, {len(JetPT)} jets')
        except ValueError:
            print(f'{root}: {n} events')
            continue
        if root[:6] == 'mumuEE' or root[:6] == 'mumuNN':
            plt.hist(JetPT, 50, weights=[cs / n] * len(JetPT), histtype='step', label=root.replace('_collect_min', '')[:-5])
        else:
            plt.hist(JetPT, 50, weights=[cs / n] * len(JetPT), histtype='barstacked', label=root.replace('_collect_min', '')[:-5])
plt.title('$p_\\mathrm{T}$ distribution of jets')
plt.xlabel('$p_\\mathrm{T}$')
plt.ylabel('relevant strenth')
plt.yscale('log')
plt.legend()
plt.tight_layout()
plt.savefig('JetPTCS3.pdf')

plt.figure(figsize=(10, 6))
for root in root3:
    cs = CS[root]
    with uproot.open(root) as file:
        Collect = file['Collect']
        try:
            JetEta = Collect['JetEta'].array(library='np')
            n = len(JetEta)
            JetEta = np.concatenate(JetEta)
            print(f'{root}: {n} events, {len(JetEta)} jets')
        except ValueError:
            print(f'{root}: {n} events')
            continue
        if root[:6] == 'mumuEE' or root[:6] == 'mumuNN':
            plt.hist(JetEta, 50, weights=[cs / n] * len(JetEta), histtype='step', label=root.replace('_collect_min', '')[:-5])
        else:
            plt.hist(JetEta, 50, weights=[cs / n] * len(JetEta), histtype='barstacked', label=root.replace('_collect_min', '')[:-5])
plt.title('$\\eta$ distribution of jets')
plt.xlabel('$\\eta$')
plt.ylabel('relevant strenth')
plt.yscale('log')
plt.legend()
plt.tight_layout()
plt.savefig('JetEtaCS3.pdf')

plt.figure(figsize=(10, 6))
for root in root3:
    cs = CS[root]
    with uproot.open(root) as file:
        Collect = file['Collect']
        try:
            JetE = Collect['JetE'].array(library='np')
            n = len(JetE)
            JetE = np.concatenate(JetE)
            print(f'{root}: {n} events, {len(JetE)} jets')
        except ValueError:
            print(f'{root}: {n} events')
            continue
        if root[:6] == 'mumuEE' or root[:6] == 'mumuNN':
            plt.hist(JetE, 50, weights=[cs / n] * len(JetE), histtype='step', label=root.replace('_collect_min', '')[:-5])
        else:
            plt.hist(JetE, 50, weights=[cs / n] * len(JetE), histtype='barstacked', label=root.replace('_collect_min', '')[:-5])
plt.title('$E$ distribution of jets')
plt.xlabel('$E$')
plt.ylabel('relevant strenth')
plt.yscale('log')
plt.legend()
plt.tight_layout()
plt.savefig('JetECS3.pdf')

plt.figure(figsize=(10, 6))
for root in root10:
    cs = CS[root]
    with uproot.open(root) as file:
        Collect = file['Collect']
        try:
            JetPT = Collect['JetPT'].array(library='np')
            n = len(JetPT)
            if n < 1000:
                raise ValueError(f'n = {n} too small')
            JetPT = np.concatenate(JetPT)
            print(f'{root}: {n} events, {len(JetPT)} jets')
        except ValueError:
            print(f'{root}: {n} events')
            continue
        if root[:6] == 'mumuEE' or root[:6] == 'mumuNN':
            plt.hist(JetPT, 50, weights=[cs / n] * len(JetPT), histtype='step', label=root.replace('_collect_min', '')[:-5])
        else:
            plt.hist(JetPT, 50, weights=[cs / n] * len(JetPT), histtype='barstacked', label=root.replace('_collect_min', '')[:-5])
plt.title('$p_\\mathrm{T}$ distribution of jets')
plt.xlabel('$p_\\mathrm{T}$')
plt.ylabel('relevant strenth')
plt.yscale('log')
plt.legend()
plt.tight_layout()
plt.savefig('JetPTCS10.pdf')

plt.figure(figsize=(10, 6))
for root in root10:
    cs = CS[root]
    with uproot.open(root) as file:
        Collect = file['Collect']
        try:
            JetEta = Collect['JetEta'].array(library='np')
            n = len(JetEta)
            if n < 1000:
                raise ValueError(f'n = {n} too small')
            JetEta = np.concatenate(JetEta)
            print(f'{root}: {n} events, {len(JetEta)} jets')
        except ValueError:
            print(f'{root}: {n} events')
            continue
        if root[:6] == 'mumuEE' or root[:6] == 'mumuNN':
            plt.hist(JetEta, 50, weights=[cs / n] * len(JetEta), histtype='step', label=root.replace('_collect_min', '')[:-5])
        else:
            plt.hist(JetEta, 50, weights=[cs / n] * len(JetEta), histtype='barstacked', label=root.replace('_collect_min', '')[:-5])
plt.title('$\\eta$ distribution of jets')
plt.xlabel('$\\eta$')
plt.ylabel('relevant strenth')
plt.yscale('log')
plt.legend()
plt.tight_layout()
plt.savefig('JetEtaCS10.pdf')

plt.figure(figsize=(10, 6))
for root in root10:
    cs = CS[root]
    with uproot.open(root) as file:
        Collect = file['Collect']
        try:
            JetE = Collect['JetE'].array(library='np')
            n = len(JetE)
            JetE = np.concatenate(JetE)
            print(f'{root}: {n} events, {len(JetE)} jets')
        except ValueError:
            print(f'{root}: {n} events')
            continue
        if root[:6] == 'mumuEE' or root[:6] == 'mumuNN':
            plt.hist(JetE, 50, weights=[cs / n] * len(JetE), histtype='step', label=root.replace('_collect_min', '')[:-5])
        else:
            plt.hist(JetE, 50, weights=[cs / n] * len(JetE), histtype='barstacked', label=root.replace('_collect_min', '')[:-5])
plt.title('$E$ distribution of jets')
plt.xlabel('$E$')
plt.ylabel('relevant strenth')
plt.yscale('log')
plt.legend()
plt.tight_layout()
plt.savefig('JetECS10.pdf')
