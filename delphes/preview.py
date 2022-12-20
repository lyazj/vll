#!/usr/bin/env python3.6

import uproot

Collect = uproot.open('mumuEE_collect_1000_3.root')['Collect']
HepMCWeight = Collect['HepMCWeight'].array(library='np')
Weight = Collect['Weight'].array(library='np')
JetBTag = Collect['JetBTag'].array(library='np')
TauNumber = Collect['TauNumber'].array(library='np')
TauBNumber = Collect['TauBNumber'].array(library='np')
B50Number = Collect['B50Number'].array(library='np')
B70Number = Collect['B70Number'].array(library='np')
B90Number = Collect['B90Number'].array(library='np')

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
print('TauNumber.mean()', TauNumber.mean(), sep='\n')
print()
print('TauBNumber.mean()  # b-tagged tau jet number', TauBNumber.mean(), sep='\n')
print()
print('B50Number.mean()  # btagging efficiency: 50%', B50Number.mean(), sep='\n')
print()
print('B70Number.mean()', B70Number.mean(), sep='\n')
print()
print('B90Number.mean()', B90Number.mean(), sep='\n')
