#!/usr/bin/env python3

import re
import argparse
import math

parser = argparse.ArgumentParser(**{
    'description': 'Concurrently launch mg5_aMC for specified number of events.'
})
# parser.add_argument('-d', '--directory', metavar='dirpath', type=str, required=True, help='event source directory')
# parser.add_argument('-n', '--number', metavar='integer', type=int, required=True, help='target event number')
# parser.add_argument('-s', '--script', metavar='datpath', type=str, required=True, help='script passed to mg5_aMC')
parser.add_argument('-t', '--thread', metavar='integer', type=int, default=None, help='worker thread number')
args = parser.parse_args()
# directory = args.directory
# number = args.number
# script = args.script
thread = args.thread or int(math.ceil(__import__('multiprocessing').cpu_count() / 2))

# debug
script = 'event.dat'
directory = 'mumuvbsww'
number = 50000

batch_min = 1000
# batch = max(int(math.ceil(number / thread)), batch_min)
batch = batch_min

with open(script) as f:
    script_content = f.read()

import os
import time
import logging
import signal
import traceback

# logging.basicConfig(level=logging.DEBUG)
logging.basicConfig(level=logging.INFO)
# logging.basicConfig(level=logging.WARNING)

def waitpid(pid=-1, options=0):
    pid, status = os.waitpid(pid, options)
    if os.WIFEXITED(status):
        code = os.WEXITSTATUS(status)
        if code:
            logging.warning(f'process {pid} exited with code {code}')
            return pid, 1
        else:
            logging.debug(f'process {pid} exited with code {code}')
            return pid, 0
    signal = os.WTERMSIG(status)
    logging.warning(f'process {pid} terminated by signal {signal}')
    return pid, 1

def waitall():
    while True:
        try:
            waitpid(-1)
        except ChildProcessError:
            break

def run_async(init, exec, *args, **kwargs):
    pid = os.fork()
    if pid == 0:  # child
        try:
            if init is not None:
                args, kwargs = init(*args, **kwargs)
            exec(*args)
        except:
            traceback.print_exc()
            os._exit(1)
    return pid, args, kwargs

def run_sync(init, exec, *args, **kwargs):
    pid, args, kwargs = run_async(init, exec, *args, **kwargs)
    return waitpid(pid), args, kwargs

def redirect(fd_new, path, flags=None, mode=0o644):
    if flags is None:
        if fd_new == 0:
            flags = os.O_RDONLY
        elif fd_new == 1 or fd_new == 2:
            flags = os.O_WRONLY | os.O_TRUNC | os.O_CREAT
    fd = os.open(path, flags, mode)
    os.dup2(fd, fd_new)
    os.close(fd)

def cp(src, dst, *options):
    return run_sync(None, os.execlp, 'cp', 'cp', src, dst, *options)[0][1]

def rm(*args):
    return run_sync(None, os.execlp, 'rm', 'rm', *args)[0][1]

def rm_d(*args):
    return run_sync(None, os.execlp, 'rm', 'rm', *['-d', *args])[0][1]

def cp_r(src, dst, *options):
    return cp(src, dst, *['-r', *options])

def mkdir(path, *options):
    return run_sync(None, os.execlp, 'mkdir', 'mkdir', path, *options)[0][1]

def mkdir_p(path, *options):
    return mkdir(path, *['-p', *options])

count = 0
pending = []
running = {}
success = []
failed = []

def pend_job(*args, **kwargs):
    pending.append((args, kwargs))
    logging.info(f'[P] job pended: args={repr(args)} kwargs={repr(kwargs)}')

def run_job():
    args, kwargs = pending.pop(0)
    pid, args, kwargs = run_async(*args, **kwargs)
    assert pid not in running
    running[pid] = (args, kwargs)
    logging.info(f'[R] job running: pid={repr(pid)} args={repr(args)} kwargs={repr(kwargs)}')

def finish_job(pid):
    global count
    pid, code = waitpid(pid)
    args, kwargs = running.pop(pid)
    (failed if code else success).append((args, kwargs))
    logging.info(f'[{"F" if code else "S"}] job finished: pid={repr(pid)} code={repr(code)} args={repr(args)} kwargs={repr(kwargs)}')
    nb = get_number(kwargs['nwd'])
    count += nb
    logging.info(f'current event number: {count}')

def launch():
    for i in range(thread):
        submit_run()
    while pending:
        while pending:
            while len(running) >= thread:
                finish_job(-1)
                if count < number:
                    submit_run()
            run_job()
        while running:
            finish_job(-1)
            if count < number:
                submit_run()
                break

seeds = {0}
def get_seed():
    seed = 0
    while seed in seeds:
        seed += 1
    seeds.add(seed)
    return seed

def get_nwd(directory):
    while True:
        seed = get_seed()
        d, b = os.path.split(directory)
        if not b:
            while d[-1] == '/':  # XXX: double checking
                d = d[:-1]
            d, b = os.path.split(d)
        nwd = os.path.join(d, f'{b}-{seed}')
        code = mkdir(nwd)
        if code == 0:
            logging.debug(f'new working directory: {nwd}')
            return seed, nwd

def write_runcard(directory, seed, number):
    path = os.path.join(directory, 'run_card.dat')
    with open(path) as f:
        card = f.read()
    card, n = re.subn(r'^.*?(=\s*nevents.*?)$', str(number) + r' \1  # runmg5', card, flags=re.M)
    assert n == 1
    card, n = re.subn(r'^.*?(=\s*iseed.*?)$', str(seed) + r' \1  # runmg5', card, flags=re.M)
    assert n == 1
    with open(path, 'w') as f:
        f.write(card)

def write_script(directory):
    script_new = os.path.join(directory, os.path.basename(script))
    content = script_content
    with open(script_new, 'w') as f:
        f.write(content)
    return script_new

def init_run(*args, **kwargs):
    seed = kwargs['seed']
    nwd = kwargs['nwd']
    number = kwargs['number']
    if cp_r(directory, nwd):
        rm_d(nwd)
        raise RuntimeError('cp_r error')
    write_runcard(os.path.join(nwd, directory, 'Cards'), seed, number)
    write_script(nwd)
    os.chdir(nwd)
    logging.debug(f'working directory ready: {nwd}')
    rd0 = '/dev/null'
    rd1 = 'runmg5-1.log'
    rd2 = 'runmg5-2.log'
    logging.info(f'job submitted: {nwd} rd0={rd0} rd1={rd1}, rd2={rd2}')
    redirect(0, rd0)
    redirect(1, rd1)
    redirect(2, rd2)
    return args, {'rd0': rd0, 'rd1': rd1, 'rd2': rd2}

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

def submit_run():
    seed, nwd = get_nwd(directory)
    pend_job(init_run, os.execlp, 'mg5_aMC', 'mg5_aMC', 'event.dat', seed=seed, nwd=nwd, number=batch)

try:
    launch()
except BaseException as e:
    if type(e) is not SystemExit:
        traceback.print_exc()
    while True:
        try:
            signal.pthread_sigmask(signal.SIG_BLOCK, signal.valid_signals())
            break
        except:
            pass
    # TODO cleanup
