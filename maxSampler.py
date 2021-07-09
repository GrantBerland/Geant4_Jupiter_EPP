#!/usr/bin/env python

import sys
import numpy as np
from scipy.special import lambertw

# Parse inputs for E0 and (optionally) nParts
if len(sys.argv) is 2:
    E0     = float(sys.argv[1]);
    nParts = 10000;
elif len(sys.argv) is 3:
    E0     = float(sys.argv[1]);
    nParts = int(float(sys.argv[2])); # allows for writing in as 1e6, etc.
else:
    raise ValueError("Incorrect number of arguments input into Maxwellian sampler!");

# Sample Singhal et al., 1992 "Maxwellian" distribution of
# f ~ E exp(-E/E0)
sampler = np.real(-E0*(lambertw(-np.random.rand(nParts)/np.exp(1), -1) + 1))

# Write sampled values to be read in by Geant program
with open("energyFile.csv", 'w') as f:
    for i in range(0, nParts):
        f.write("%.3f\n" % sampler[i]);
        
print("Maxwellian with E0 = %.1f keV and %.0f particles sampled!" % (E0, nParts));

