import time
import timeit
import json
import os
import numpy as np
import numba
from PyMPDATA import Options
from PyMPDATA_examples.Smolarkiewicz_1984 import Settings, Simulation

def run_simulation(*, static, n_threads, nx, dt, nt):
    settings = Settings(n=nx, dt=dt)
    options = Options(n_iters=int(os.environ['N_ITERS']))
    simulation = Simulation(settings, options, static=static, n_threads=n_threads)
    cpu_time_per_timestep = simulation.run(nt=nt)

def time_pympdata(*, static, n_threads, nx, dt, nt, repeat):
    cmd = f"run_simulation(static={static}, n_threads={n_threads}, nx={nx}, dt={dt}, nt={nt})"
    time_init = min(timeit.repeat(
        cmd.replace(f"nt={nt}", "nt=1"),
	globals=globals(),
	number=1,
	repeat=repeat
    ))
    time_full = min(timeit.repeat(
        cmd,
	globals=globals(),
	number=1,
	repeat=repeat
    ))
    return (time_full - time_init) / (nt - 1)

if __name__ == '__main__':
    ticks = range(1, int(os.environ['MAX_THREADS'])+1) if os.environ["NUMBA_DISABLE_JIT"] != '1' else (1,)

    if os.environ["NUMBA_DISABLE_JIT"] == '1':
        with np.errstate(all="ignore"):
            data = {'nojit': time_pympdata(
                static=False,
                n_threads=1,
                nx=int(os.environ['NX']),
                dt=float(os.environ['DT']),
                nt=int(os.environ['N_STEPS']),
                repeat=2
            )}
    else:
        data = {
            'static': {i:-1 for i in ticks},
            'dynamic': {i:-1 for i in ticks},
        }
        for n_threads in ticks:
            numba.set_num_threads(n_threads)
            print(f'n_threads={n_threads}')
            for grid, flag in {'static': True, 'dynamic': False}.items():
                data[grid][n_threads] = time_pympdata(
                    static=flag,
                    n_threads=n_threads,
                    nx=int(os.environ['NX']),
                    dt=float(os.environ['DT']),
                    nt=int(os.environ['N_STEPS']),
                    repeat=int(os.environ['N_REPEATS'])
                )

    with open(f'data_NDJ={os.environ["NUMBA_DISABLE_JIT"]}.json', 'w') as file:
        json.dump(data, file)
