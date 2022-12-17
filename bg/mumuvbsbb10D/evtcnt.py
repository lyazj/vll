#!/usr/bin/env python3

import os
import re
import logging

def get_number(directory):
    with open(os.path.join(directory, 'runmg5-1.log')) as f:
        log = f.read()
    nb = re.findall(r'^\s*Nb of events\s*:\s*(\S+)\s*$', log, re.M)
    if not nb:
        logging.warning(f'result: {directory}: {nb} (failed)')
        return 0
    for i in nb[1:]:
        if i != nb[0]:
            logging.warning(f'result: {directory}: {nb} (failed)')
            return 0
    try:
        nb = int(nb[0])
    except ValueError:
        logging.warning(f'result: {directory}: {nb} (failed)')
        return 0
    logging.info(f'result: {directory}: {nb}')
    return nb

f = 0
r = 0
n = 0
with os.popen('ls -d *-*/') as p:
    for d in p:
        d = d.strip()
        if os.path.exists(os.path.join(d, 'py.py')):
            r += 1
        else:
            f += 1
            n += get_number(d)
print(f'{n} events generated, {f} finished, {r} running')
