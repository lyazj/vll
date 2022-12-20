#!/usr/bin/env python3.6

import uproot

Collect = uproot.open('mumuEE_collect_1000_3.root')['Collect']
HepMCWeight = Collect['HepMCWeight'].array(library='np')
Weight = Collect['Weight'].array(library='np')
JetBTag = Collect['JetBTag'].array(library='np')
JetTauTag = Collect['JetTauTag'].array(library='np')

print('Collect.keys()', Collect.keys(), sep='\n')
print()
print('HepMCWeight', HepMCWeight, sep='\n')
print()
print('Weight', Weight, sep='\n')
print()
print('(Weight[:,0] == HepMCWeight).all()', (Weight[:,0] == HepMCWeight).all(), sep='\n')
print()
print('JetBTag', JetBTag, sep='\n')
print()
print('JetTauTag', JetTauTag, sep='\n')
