# usage:
# dataframe_filename

import sys
import os
import seaborn as sns
import pandas as pd
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np
from keyname import keyname as kn
from fileshash import fileshash as fsh

matplotlib.rcParams['pdf.fonttype'] = 42
sns.set(style='whitegrid')

dataframe_filename = sys.argv[1]

df = pd.read_csv(dataframe_filename)

plt.clf()

ax = sns.barplot(
    x="Genotype",
    y="Fraction Resource Shared",
    data=df
)


outfile = kn.pack({
    '_data_hathash_hash' : fsh.FilesHash().hash_files([dataframe_filename]),
    '_script_fullcat_hash' : fsh.FilesHash(
                                    file_parcel="full_parcel",
                                    files_join="cat_join"
                                ).hash_files([sys.argv[0]]),
    '_source_hash' :kn.unpack(dataframe_filename)['_source_hash'],
    'title' : 'sharingfraction',
    'ext' : '.pdf'
})

ax.get_figure().savefig(
    outfile,
    transparent=True,
    bbox_inches='tight',
    pad_inches=0
)

print('Output saved to', outfile)
