##>>>The code is used to read data from hdf5 file
##>>>and plot on the screen and output figure file using matplotlib-python

import matplotlib
matplotlib.use('Agg')


from matplotlib.ticker import MaxNLocator
from matplotlib import cm

from matplotlib.figure import Figure

from matplotlib.ticker import ScalarFormatter

from mpl_toolkits.axes_grid1.inset_locator import inset_axes
import matplotlib.pyplot as plt
import matplotlib as mpl
import matplotlib.ticker as ticker

import numpy as np
from numpy import arange, sin, pi
import math
import h5py as h5
from scipy.interpolate import spline


font={	'family' : 'serif',
	'weight' : 'bold',
	'size' : 8,
	}

mpl.rcParams['font.family'] = 'serif'
mpl.rcParams['mathtext.fontset'] = 'stix'
mpl.rcParams['mathtext.it'] = 'serif'

mpl.rcParams['axes.linewidth'] = 2.0
mpl.rcParams['xtick.major.size'] = 2
mpl.rcParams['ytick.major.size'] = 2
mpl.rcParams['lines.linewidth'] = 3.0

mpl.rcParams['grid.linestyle'] = ":"
mpl.rcParams['grid.color'] = "black"

#mpl.rcParams['font.serif'] = ['SimHei, Bitstream Vera Sans, Lucida Grande, Verdana, Geneva, Lucid, Arial, Helvetica, Avant Garde, sans-serif']  # 设置matplotlib可以显示汉语
#mpl.rcParams['axes.unicode_minus'] = False

#for paper
mpl.rcParams['font.size'] = 18
label_fontsize = 21
legend_fontsize = 15
inset_label_fontsize = 15

# for poster
'''
mpl.rcParams['font.size'] = 25
label_fontsize = 25
legend_fontsize = 25
inset_label_fontsize = 25
'''

linestyles = ['-', '--', '-.', ':']
markers = ['x', '^', 's', '*']
markevery = 3
markersize = 10

ylabel_x = -0.058


# contants
const_e = 1.602e-19

def get_axis_limits(ax, x_scale=0.0, y_scale=1.08):
    return ax.get_xlim()[0] + (ax.get_xlim()[1] - ax.get_xlim()[0]) * x_scale, ax.get_ylim()[1] + ( ax.get_ylim()[1] - ax.get_ylim()[0] ) * (y_scale - 1.0)

yformatter = ScalarFormatter()
yformatter.set_powerlimits((-3,3))


print("template.py is imported!")
