
"""

functions for doing nice 2D plotting

"""

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cmap

# Plotting imports
#from Plotting.mappings import *
from Plotting.mappings import coord_map_array, ColMap, ColIMap
# Parameters imports
import Parameters.options as option
import Parameters.settings as setting

def plot_spikes(spikes, size, title = ""):
    # make a 2D array for plotting ()
    p_array = coord_map_array(size)
    p_x_array = p_array[0]
    p_y_array = p_array[1]

    #print "%s size is %d x %d" % (title, size[0], size[1])
    title = title + "_Spikes"

    # Analysis and plotting of spikes
    p_spikes = spikes
    #p_spikes = pop.getSpikes()
    print title, len(p_spikes)
    #print pop.meanSpikeCount(gather=True)/(runtime/1000.0)

    p_id = []
    p_times = []
    #p_xvals = []
    #p_yvals = []

    # Do a plot of all spikes
    for sp in p_spikes:
        p_id.append(sp[0])
        p_times.append(sp[1])
        #xpos = p_x_array[int(sp[0])]
        #ypos = p_y_array[int(sp[0])]
        #p_xvals.append(xpos)
        #p_yvals.append(ypos)


    plt.figure()
    plt.plot(p_times, p_id, '.b')
    plt.xlim(0, setting.runtime)
    plt.ylim(0, size[0]*size[1])
    plt.title(title)
    if option.save_plots == True:
        plt.savefig(option.plots_folder + title + "." + option.plotFileFormat, format=option.plotFileFormat)


def plot_map(spikes, size, position=None, title=""):
    p_array = coord_map_array(size)
    p_x_array = p_array[0]
    p_y_array = p_array[1]

    #print "%s size is %d x %d" % (title, size[0], size[1])
    title = title + "_Map"

    # make a 2D array for plotting
    p_array = np.zeros((int(size[0]),int(size[1])),int)

    # Analysis and plotting of spikes
    p_spikes = spikes
    #p_spikes = pop.getSpikes()
    #print title, len(p_spikes)
    #print pop.meanSpikeCount(gather=True)/(runtime/1000.0)

#    p_counts = np.zeros((int(size[0])*int(size[1])),int)

#    p_id = []
#    p_times = []
#    p_xvals = []
#    p_yvals = []
    if position is not None:
        p_counts = np.zeros((int(size[0])*int(size[1])),int)

    # Do a plot of all spikes
    for sp in p_spikes:
#        p_id.append(sp[0])
#        p_counts[int(sp[0])] +=1
#        p_times.append(sp[1])
        xpos = p_x_array[int(sp[0])]
        ypos = p_y_array[int(sp[0])]
#        p_xvals.append(xpos)
#        p_yvals.append(ypos)
        p_array[xpos,ypos] = 3
        if position is not None:
            p_counts[int(sp[0])] +=1

    if position is not None:
        for nrn in xrange(size[0]*size[1]):
            xpos = p_x_array[nrn]
            ypos = p_y_array[nrn]
            if float(sum(p_counts)) > 0.0:
                if float(p_counts[nrn])/float(max(p_counts)) > 0.25:
                    p_array[xpos,ypos] = 3
                else:
                    p_array[xpos,ypos] = 0
            else:
                p_array[xpos,ypos] = 0
            #print nrn, xpos,ypos, p_array[xpos,ypos] #float(p_counts[nrn])/float(p_total)
        x_attend = position[0]
        y_attend = position[1]
        p_array[int(x_attend),int(y_attend)] = 2
        title = title + "Salient_position(" + str(x_attend) + "," + str(y_attend) + ")"

    # Make the plot
    x = np.arange(0,size[0]+1)
    y = np.arange(0,size[1]+1)
    X,Y = np.meshgrid(x,y)

    plt.figure()
    #if position is not None:
    #    plt.pcolor(X, Y, p_array, edgecolors='k', cmap=cmap.spectral,
    #                vmin=0.0, vmax=1)
    #else:
    plt.pcolor(X, Y, p_array, edgecolors = 'k', cmap=cmap.spectral)
    plt.xlim(0, size[0])
    plt.ylim(0, size[1])
    plt.title(title)
    if position is not None:
        plt.axis('off')
    plt.gca().invert_yaxis()
    if option.save_plots == True:
        plt.savefig(option.plots_folder + title + "." + option.plotFileFormat, format=option.plotFileFormat)



def plot_saliency_map(spikes, size, title = ""):
    p_array = coord_map_array(size)
    p_x_array = p_array[0]
    p_y_array = p_array[1]

    #print "%s size is %d x %d" % (title, size[0], size[1])
    title = title  + "_SaliencyMap"

    # make a 2D array for plotting
    p_array = np.zeros((int(size[0]),int(size[1])),float)

    # Analysis and plotting of spikes
    #p_spikes = pop.getSpikes()
    #print title, len(p_spikes)
    #print pop.meanSpikeCount(gather=True)/(runtime/1000.0)
    p_spikes = spikes

    p_counts = np.zeros((int(size[0])*int(size[1])),int)
    # Do a plot of all spikes
    for sp in p_spikes:
        p_counts[int(sp[0])] +=1

    print p_counts
    p_total = sum(p_counts)
    print "Total", title, "activity", p_total, min(p_counts), max(p_counts)

    # Get the coordinates of the most active area
    # and do a coarse mapping up to input resolution
    active = np.argmax(p_counts)
    print "active", title, active, "active", title, "count", p_counts[active]
    max_p_activation = float(p_counts[active])/float(p_total) if float(p_total) > 0.0 else 0.0
    print "Max", title, " Activation", max_p_activation

    # indicate salient position in input space
    # calculated from LIP activity
    # again have to swap x and y as it is for plotting
    #p_mag = round(float(size[0])/float(size[0]))
    #x_attend = double(p_x_array[active]*p_mag)
    #y_attend = double(p_y_array[active]*p_mag)
    #p_array[int(x_attend),int(y_attend)] = 2
    #print "Salient position in Input Space", x_attend, y_attend


    for nrn in xrange(size[0]*size[1]):
        xpos = p_x_array[nrn]
        ypos = p_y_array[nrn]
        p_array[xpos,ypos] = float(p_counts[nrn])/float(p_total) if float(p_total) > 0.0 else 0.0
        #print nrn, xpos,ypos, p_array[xpos,ypos] #float(p_counts[nrn])/float(p_total)




    # Make the plot
    x = np.arange(0,size[0]+1)
    y = np.arange(0,size[1]+1)
    X,Y = np.meshgrid(x,y)

    plt.figure()
    plt.pcolor(X, Y, p_array, edgecolors='k', cmap=ColMap,
                vmin=0.0, vmax=max_p_activation)
    plt.axis('off')
    plt.colorbar()
    plt.xlim(0, size[0])
    plt.ylim(0, size[1])
    plt.title(title)
    plt.gca().invert_yaxis()
    if option.save_plots == True:
        plt.savefig(option.plots_folder + title + "." + option.plotFileFormat, format=option.plotFileFormat)



def plot_weights(diffs, size_pre, size_post, gmax, gmin, title = ""):

    # make a 2D array for plotting weight changes
    # right now we are splitting pos and neg to see
    # what is what!
    w_array = coord_map_array(size_pre)
    w_x_array = w_array[0]
    w_y_array = w_array[1]

    w_array_pos = np.zeros((int(size_pre[0]),int(size_pre[1])),float)
    w_array_neg = np.zeros((int(size_pre[0]),int(size_pre[1])),float)

    print "%s size is %d x %d" % (title, size_pre[0], size_pre[1])
    titlepos = title + "_PositiveWeightMap"
    titleneg = title + "_NegativeWeightMap"

    #np.reshape( diffs, (size_pre[0]*size_pre[1], size_post[0]*size_post[1]))

    # Calculate weight map
    for x in xrange(diffs.shape[0]):
       for y in xrange(diffs.shape[1]):
          # get the x,y coords of the v2, v4 neurons
            xpos = w_x_array[x]
            ypos = w_y_array[x]
            if (diffs[x][y] >= 0.0):
             # is this weight change more positive than one
             # already in the plotting array
                if (w_array_pos[xpos][ypos] < diffs[x][y]):
                    w_array_pos[xpos][ypos] = diffs[x][y]
            if (diffs[x][y] < 0.0):
                # is this weight change more negative than one
                # already in the plotting array
                if (w_array_neg[xpos][ypos] > diffs[x][y]):
                    w_array_neg[xpos][ypos] = diffs[x][y]

    # for x in xrange(diffs.shape[0]):
    #     xpos = w_x_array[x]
    #     ypos = w_y_array[x]
    #     if (diffs[x] >= 0.0):
    #         # is this weight change more positive than one
    #         # already in the plotting array
    #         if (w_array_pos[xpos][ypos] < diffs[x]):
    #             w_array_pos[xpos][ypos] = diffs[x]
    #     if (diffs[x] < 0.0):
    #         # is this weight change more negative than one
    #         # already in the plotting array
    #         if (w_array_neg[xpos][ypos] > diffs[x]):
    #             w_array_neg[xpos][ypos] = diffs[x]

    # Make the plot
    x = np.arange(0,size_pre[0]+1)
    y = np.arange(0,size_pre[1]+1)
    X,Y = np.meshgrid(x,y)

    plt.figure()
    plt.pcolor(X, Y, w_array_pos, edgecolors='k', cmap=ColMap, vmin=0.0, vmax=gmax)
    plt.axis('off')
    plt.colorbar()
    plt.xlim(0, size_pre[0])
    plt.ylim(0, size_pre[1])
    plt.title(titlepos)
    plt.gca().invert_yaxis()
    if option.save_plots == True:
        plt.savefig(option.plots_folder + titlepos + "." + option.plotFileFormat, format=option.plotFileFormat)

    plt.figure()
    plt.pcolor(X, Y, w_array_neg, edgecolors='k', cmap=ColIMap, vmin=gmin, vmax=0.0)
    plt.axis('off')
    plt.colorbar()
    plt.xlim(0, size_pre[0])
    plt.ylim(0, size_pre[1])
    plt.title(titleneg)
    plt.gca().invert_yaxis()
    if option.save_plots == True:
        plt.savefig(option.plots_folder + titleneg + "." + option.plotFileFormat, format=option.plotFileFormat)


#
