
"""

Create some arrays for neuron ID to x-y coord mappings
for doing nice 2D plotting

"""

import numpy
import matplotlib.cm as cmap
from matplotlib.colors import ListedColormap

# make a custom colormap for plotting greyscale

colormap = numpy.array([(0.0,0.0,0.0),
                        (0.1,0.1,0.1),
                        (0.2,0.2,0.2),
                        (0.3,0.3,0.3),
                        (0.4,0.4,0.4),
                        (0.5,0.5,0.5),
                        (0.6,0.6,0.6),
                        (0.7,0.7,0.7),
                        (0.8,0.8,0.8),
                        (0.9,0.9,0.9),
                        (1.0,1.0,1.0)])

ColMap = ListedColormap(colormap, name='attncolmap')
ColIMap = ListedColormap(ColMap.colors[::-1], name='attninvcolmap')

cmap.register_cmap(cmap=ColMap)
cmap.register_cmap(cmap=ColIMap)

# Create some arrays for neuron ID to x-y coord mappings
# for doing nice 2D plotting

def coord_map_array(size):
    map_array = []

    x_array = numpy.zeros((int(size[0]*size[1])),int)
    y_array = numpy.zeros((int(size[0]*size[1])),int)

    nrn = 0
    for xcoord in xrange(0,size[0]):
       for ycoord in xrange(0,size[1]):
          x_array[nrn] = xcoord
          y_array[nrn] = ycoord
          nrn+=1

    map_array.append(x_array)
    map_array.append(y_array)



    return map_array



#
