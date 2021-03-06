# usage:
# h5_filenames

import numpy as np
import h5py
import sys
import os
from tqdm import tqdm
import pandas as pd
from keyname import keyname as kn
from fileshash import fileshash as fsh
import re
from collections import Counter
from joblib import delayed, Parallel

first_update = int(sys.argv[1])
last_update = int(sys.argv[2])
filenames = sys.argv[3:]

# check all data is from same software source
assert len({kn.unpack(filename)['_source_hash'] for filename in filenames}) == 1

def CauseOfDeath(filename):

    file = h5py.File(filename, 'r')
    deaths = [
        {
            0 : None,
            1 : 'Apoptosis',
            2 : 'Bankrupt',
            3 : 'Replaced'
        }[death[idx]]
        for death in [
            np.array(
                file['Death']['upd_'+str(upd)]
            ).flatten()
            for upd in range(first_update, last_update)
        ]
        for idx in range(file['Index']['own'].size)
    ]

    res = Counter(deaths)

    for cause in (
        'Replaced',
        'Bankrupt',
        'Apoptosis',
        None
    ):
        if cause not in res:
            res[cause] = 0

    res.pop(None)

    res = {
        k : v / (file['Index']['own'].size * (last_update - first_update))
        for k, v in res.items()
    }

    return res

def SafeCauseOfDeath(filename):
    try:
        return CauseOfDeath(filename)
    except Exception as e:
        print("warning: corrupt or incomplete data file... skipping")
        print("   ", e)
        return None


print("num files:" , len(filenames))

outfile = kn.pack({
    'title' : 'cell_death',
    '_data_hathash_hash' : fsh.FilesHash().hash_files(filenames),
    '_script_fullcat_hash' : fsh.FilesHash(
                                file_parcel="full_parcel",
                                files_join="cat_join"
                            ).hash_files([sys.argv[0]]),
    '_source_hash' :kn.unpack(filenames[0])['_source_hash'],
    'ext' : '.csv'
})

pd.DataFrame.from_dict([
    {
        'Treatment' : kn.unpack(filename)['treat'],
        'Seed' : kn.unpack(filename)['seed'],
        'Per-Cell-Update Death Rate' : val,
        'Cause' : cause
    }
    for filename, res in zip(
        filenames,
        Parallel(n_jobs=-1)(
            delayed(SafeCauseOfDeath)(filename)
            for filename in tqdm(filenames)
        )
    )
    if res is not None
    for cause, val in res.items()
]).to_csv(outfile, index=False)

print('Output saved to', outfile)
