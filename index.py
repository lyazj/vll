#!/usr/bin/env python3

import re
import os
from xml.dom import minidom

def index():
    document = minidom.Document()
    events = document.createElement('events')
    document.appendChild(events)
    for d in sorted(os.listdir()):
        try:
            vll_mass = d if d == 'bg' else float(d)
            for d in sorted(os.path.join(d, i) for i in os.listdir(d)):
                r = re.search(r'/([^/]*?)(\d+)(D?)$', d)
                if not r:
                    continue
                label, cme, delphes = r.groups()
                records = []
                for d in sorted(os.path.join(d, i) for i in os.listdir(d)):
                    r = re.search(r'/' + re.escape(label) + r'-(\d+)$', d)
                    if not r:
                        continue
                    index = int(r.group(1))
                    records.append((index, d))
                records.sort()
                for index, path in records:
                    record = document.createElement('record')
                    record.setAttribute('vll-mass', str(vll_mass))
                    record.setAttribute('label', str(label))
                    record.setAttribute('cme', str(cme))
                    record.setAttribute('delphes', str(bool(delphes)))
                    record.setAttribute('index', str(index))
                    text = document.createTextNode(path)
                    record.appendChild(text)
                    events.appendChild(record)
        except ValueError:
            pass
    document.writexml(open('index.xml', 'w'), encoding='utf-8', newl='\n')

if __name__ == '__main__':
    index()
