
'''
# This script calls MSISe-00 at a given time, latitude, and longitude 
# over a given altitude range. Although this library can generate 
# n-dimensional arrays over all these abscissa, it will not write to
# the file correctly unless a single time, lat., and long. are chosen.
# The script overwrites the profile every time it's called. 
#
# To call:
#
#   python generate_MSISE00_atmosphere.py arg1
# 
# if arg1 is anything, script will plot data in addition to writing file
# if arg1 is nothing, script will only write file
#
# GB
'''

import msise00
from datetime import datetime
import numpy as np
import sys

################################
##Input parameters to MSISe-00##
################################

altitudeStepSize = 10   # km
lowerAlt         = 0    # km
upperAlt         = 1000 # km

# Datetime for MSIS model run
time = datetime(2018, 3, 31, 12);

# Geographic latitude , longitude
g_lat_lon = [65. , -148.]

################################
################################
################################

# Construct altitude array
altitudeArr = np.linspace(lowerAlt, 
                          upperAlt, 
                      int((upperAlt+altitudeStepSize)/altitudeStepSize));
# Call MSIS model
atmos = msise00.run(time=time, 
                    altkm=altitudeArr, 
                    glat=g_lat_lon[0], 
                    glon=g_lat_lon[1])

# If program gets any arguments it'll plot the profiles
if len(sys.argv) > 1: 
    import matplotlib.pyplot as plt
    fig = plt.figure();
    ax1 = fig.add_subplot(121)
    for var in atmos.species:
        if var is not 'Total':
            atmos[var].plot(y='alt_km', label=var, ax=ax1)
    plt.xscale('log'); plt.legend(fontsize=8);
    plt.xlabel('Species number density [m$^{-3}$]')
    plt.ylabel('Altitude [km]');
    plt.title('Number Density Profile')
    plt.grid();

    ax2 = fig.add_subplot(122)
    atmos['Tn'].plot(y='alt_km', ax=ax2);
    plt.xlabel('Neutral Temperature [K]')
    plt.ylabel('');
    plt.title('Neutral Temperature Profile')
    plt.grid(); 
   
    plt.suptitle('MSISe-00 Results: %s , %.1f$^\circ$N %.1f$^\circ$W' % (time,
                                       g_lat_lon[0],
                                       g_lat_lon[1]))
    plt.show();     

# Write order that I defined in Geant DetectorConstruction
write_order = ['alt_km','O','N2','O2','Total','Tn','He','Ar','H','N']

# Write to atmosphere file
with open('MSISE00_atmosphere.csv', 'w') as f:
    
    for alt in altitudeArr:
        
        # Query all variables at an altitude slice
        line = atmos.sel(time=time,
                     alt_km=alt,
                     lat=g_lat_lon[0],
                     lon=g_lat_lon[1])
        
        for item in write_order:
        
            if item != write_order[-1]:
                f.write(str(line[item].data) + ',')
            else: # write last entry of line with newline instead of comma
                f.write(str(line[item].data) + '\n')
    
    # file closes when scope is left
