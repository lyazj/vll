#!/usr/bin/env python3

import argparse

parser = argparse.ArgumentParser(**{
    'description': 'Concurrently launch mg5_aMC for specified number of events.'
})
# parser.add_argument('-d', '--directory', metavar='dirpath', type=str, required=True, help='event source directory')
# parser.add_argument('-s', '--script', metavar='datpath', type=str, required=True, help='script passed to mg5_aMC')
# parser.add_argument('-n', '--number', metavar='integer', type=int, required=True, help='target event number')
parser.add_argument('-t', '--thread', metavar='integer', type=int, default=None, help='worker thread number')
args = parser.parse_args()
# directory = args.directory
# script = args.script
# number = args.number
thread = args.thread or int(__import__('multiprocessing').cpu_count())

import os
import time
import logging

logging.basicConfig(level=logging.DEBUG)

def waitpid(pid=-1, options=0):
    pid, status = os.waitpid(pid, options)
    if os.WIFEXITED(status):
        code = os.WEXITSTATUS(status)
        if code:
            logging.warning(f'process {pid} exited with code {code}')
            return pid, 1
        else:
            logging.info(f'process {pid} exited with code {code}')
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
        if init is not None:
            init()
        exec(*args, **kwargs)
    return pid

def run_sync(init, exec, *args, **kwargs):
    pid = run_async(init, exec, *args, **kwargs)
    return waitpid(pid)

def redirect(fd_new, path, flags=None, mode=0o644, *args, **kwargs):
    if flags is None:
        if fd_new == 0:
            flags = os.O_RDONLY
        elif fd_new == 1 or fd_new == 2:
            flags = os.O_WRONLY | os.O_TRUNC | os.O_CREAT
    fd = os.open(path, flags, mode, *args, **kwargs)
    os.dup2(fd, fd_new)
    os.close(fd)

def cp(src, dst, *options):
    return run_sync(None, os.execlp, 'cp', 'cp', src, dst, *options)

def cp_r(src, dst, *options):
    return cp(src, dst, ['-r', *options])

def mkdir(path, *options):
    return run_sync(None, os.execlp, 'mkdir', 'mkdir', path, *options)

def mkdir_p(path, *options):
    return mkdir(path, ['-p', *options])

pending = []
running = {}
success = []
failed = []

def pend_job(*args, **kwargs):
    pending.append((args, kwargs))
    logging.info(f'[P] job pended: args={repr(args)} kwargs={repr(kwargs)}')

def run_job():
    args, kwargs = pending.pop(0)
    pid = run_async(*args, **kwargs)
    assert pid not in running
    running[pid] = (args, kwargs)
    logging.info(f'[R] job running: pid={repr(pid)} args={repr(args)} kwargs={repr(kwargs)}')

def finish_job(pid):
    pid, code = waitpid(pid)
    args, kwargs = running.pop(pid)
    (failed if code else success).append((args, kwargs))
    logging.info(f'[{"F" if code else "S"}] job finished: pid={repr(pid)} code={repr(code)} args={repr(args)} kwargs={repr(kwargs)}')

def launch():
    while pending:
        if len(running) >= thread:
            finish_job(-1)
        run_job()
    while running:
        finish_job(-1)

seeds = {0}
def get_seed():
    seed = 0
    while seed in seeds:
        seed = int(time.time()) ^ os.getpid()
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
        pid, code = mkdir(nwd)
        if code == 0:
            return nwd

get_nwd('mumutt')
# pend_job(None, os.execlp, 'ls', 'ls')
# launch()
