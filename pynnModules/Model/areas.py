

"""

Model areas class
V1, V2, V4, PFC, LIP

"""

import numpy as np

# Parameters imports
import Parameters.settings as setting
import Parameters.parameters as param
import Parameters.preferences as preference
import Parameters.options as option

# pyNN imports
#from pyNN import models, errors
if setting.simulator_name == 'brian':
    from pyNN.brian import *               # use if running brian

if setting.simulator_name == 'spyNNaker':
    from pyNN.spiNNaker import *            # Imports the pyNN.spiNNaker module


# Model imports
from Model.model import BaseModelType


# Plots import
from Plotting import plots


class ModelArea(BaseModelType):
    """ brain model area class """

    default_parameters = {
        'size':             [0, 0], #
        'layers':           0,      #
        'neuron':           {},     #   neuron: {'type': IF_curr_exp,'params': cell_params}
#        'neuron_type':      IF_curr_exp,  #
#        'neuron_params':    {},     #
        'optional':         {},     # either initialize, bias, or spike_times
        'label':            "area", #
    }

    def __init__(self, parameters):
        BaseModelType.__init__(self, parameters)

        #check parameters
        self.parameters = self.checkParameters(parameters, with_defaults=False)
        #self.pop_size = size
        #self.layers = layers
        #self.label = label
        self.pop = []


        #create populations
        if 'neuron' in self.parameters:
            self.set_neuron(self.parameters['neuron'])
            self.create_populations()


    def create_populations(cls):
        size = cls.parameters['size']

        neuron_type = cls.parameters['neuron_type']
        neuron_params = cls.parameters['neuron_params']

        for i in range(cls.parameters['layers']):    # Cycles orientations
        # creates a population for each connection
            cls.pop.append(Population(size[0]*size[1],   #Size
                neuron_type,                        # Neuron Type
                neuron_params,                      # Neuron Parameters
                label=cls.parameters['label'] + "_" + str(i))    # Label
                )
            print "-- Creating Population", cls.parameters['label'], i

        if 'optional' in cls.parameters:
            options = cls.parameters['optional']
            #cls.rng = NumpyRNG(seed=28374)
            #if 'initialize' in options:
            #    distr = options['initialize']
            #    #cls.initialize()
            if 'bias' in options:
                biases = options['bias']
                pref = biases['preferred']
                aver = biases['aversive']
                neut = biases['neutral']
                r_distr = biases['rest_distr']
                i_distr = biases['init_distr']
                bias = biases['bias']
                cls.set_bias(pref, aver, neut, r_distr, i_distr, bias)
            if 'spike_times' in options:
                spk = options['spike_times']
                spike_list = spk['spike_list']
                in_spikes = spk['spikes']
                cls.set_spike_times(in_spikes, spike_list)



    def set(cls, param, val=None):
        if param in cls.parameters.keys():
            cls.parameters[param] = val
        else:
            raise errors.NonExistentParameterError(param, cls, cls.parameters.keys())

    def get(cls, param):
        val = []
        if param in cls.parameters.keys():
            val = cls.parameters[param]
        else:
            raise errors.NonExistentParameterError(param, cls, cls.parameters.keys())

        return val

    def set_neuron(cls, neuron):
        if neuron:
            cls.parameters['neuron_type'] = neuron['neuron_type']
            cls.parameters['neuron_params'] = neuron['neuron_params']
        else:
            raise errors.InvalidParameterValueError("No value")

    def set_population_params(cls, param, val=None):
        for i in range(cls.parameters['layers']):
            cls.pop[i].set(param, val)

    def initialize(cls):
        print "Initialize"
        v_init_distr = RandomDistribution('uniform', [-55,-95], cls.rng)
        for i in range(cls.parameters['layers']):
            #cls.pop[i].initialize('v', random_distribution)
            cls.pop[i].initialize('v', v_init_distr)
            #cls.pop[i].randomInit(random_distribution)

    def record(cls, idx):   #record ith population area layer
        print "-- observing %s %d"  % (cls.pop[idx].label, idx)
        cls.pop[idx].record()

    def record_all(cls):   #record() all population area layers
        for i in range(cls.parameters['layers']):
            cls.record(i)

    def get_spikes(cls, idx):
        return cls.pop[idx].getSpikes() #.getSpikes(gather=True, compatible_output=True)

    def get_population_spikes(cls):
        spikes = []
        for i in range(cls.parameters['layers']):
            spikes.append(cls.get_spikes(i))

        return spikes

    def plot_spikes(cls, spikes, title = ""):
        size = cls.parameters['size']
        plots.plot_spikes(spikes, size, title = title)

    def plot_population_spikes(cls, time=None):
        spikes = cls.get_population_spikes()
        if time is not None:
            title = str(time) + "_" + cls.parameters['label'] + "_"
        else:
            title = cls.parameters['label'] + "_"
        for i in range(cls.parameters['layers']):
            cls.plot_spikes(spikes[i], title = title + str(i))

    def plot_map(cls, spikes, position=None, title = ""):
        size = cls.parameters['size']
        plots.plot_map(spikes, size, position, title = title)

    def plot_population_map(cls, position=None, time=None):
        spikes = cls.get_population_spikes()
        if time is not None:
            title = str(time) + "_" + cls.parameters['label'] + "_"
        else:
            title = cls.parameters['label'] + "_"
        for i in range(cls.parameters['layers']):
            cls.plot_map(spikes[i], position=position, title = title + str(i))

    def plot_saliency_map(cls, spikes, title = ""):
        size = cls.parameters['size']
        plots.plot_saliency_map(spikes, size, title = title)

    def plot_population_saliency_map(cls, time=None):
        spikes = cls.get_population_spikes()
        if time is not None:
            title = str(time) + "_" + cls.parameters['label'] + "_"
        else:
            title = cls.parameters['label'] + "_"
        for i in range(cls.parameters['layers']):
            cls.plot_saliency_map(spikes[i], title = title + str(i))


    def set_bias(cls, preferred, aversive, neutral, r_distr, i_distr, bias):
        print "set biasses"
        # set biasses to hardwire preference ADR
        if bias == "colour":
            aversive_bias = preference.aversive_colour
            preferred_bias = preference.preferred_colour
        else: # elif bias == "orientation":
            aversive_bias = preference.aversive_orientation
            preferred_bias = preference.preferred_orientation
        #v_init_distr = RandomDistribution('uniform', [-55,-95], cls.rng)
        #v_rest_distr = RandomDistribution('uniform', [-55,-65], cls.rng)
        v_init_distr = i_distr
        v_rest_distr = r_distr
        size = cls.parameters['size']
        for i in range(cls.parameters['layers']):
            cls.pop[i].initialize('v', v_init_distr)
            if i == preferred_bias:
                cls.pop[i].set('i_offset', preferred)
                #print "preferred", i, preferred
            elif i == aversive_bias:
                cls.pop[i].set('i_offset', aversive)
                #print "aversive", i, aversive
            else:
                cls.pop[i].set('i_offset', neutral)
                #print "neutral", i, neutral

            if option.active_pfc == False:
                v_rest_or = []
                for j in range(size[0]*size[1]):
                    v_rest_or.append(v_rest_distr.next())
                #cls.pop[i].set('v_rest', v_rest_or)
                cls.pop[i].tset('v_rest', np.array(v_rest_or))

    def set_spike_times(cls, in_spikes, spike_list):
        if (len(in_spikes) > 0):
            if setting.simulator_name == 'brian':
                for sp in in_spikes:
                    cls.pop[0][sp].spike_times = spike_list

            if setting.simulator_name == 'spyNNaker':
                size = cls.parameters['size']
                spike_times = []
                for sp in range(size[0]*size[1]):
                    if sp in in_spikes:
                        spike_times.append(spike_list)
                    else:
                        spike_times.append([])
                cls.pop[0].set('spike_times', spike_times)


    #
    # def get_weights(self, format):
    #     self.wts = []
    #     for proj in range(len(self.proj)):
    #         self.wts.append(self.proj[proj].getWeights(format=format)) #get('weight','array')
    #     return self.wts
    #
    # def create_projection_11(self, presyn_area, presyn_idx, postsyn_idx, connector, target = None, synapse_dynamics = None):
    #     self.proj.append(Projection(presyn_area.pop[presyn_idx], self.pop[postsyn_idx],
    #                 connector,
    #                 target=target,
    #                 synapse_dynamics=synapse_dynamics,
    #                 label=presyn_area.label +"_"+ str(presyn_idx) +"->"+self.label +"_"+ str(postsyn_idx)
    #                 ))
    #
    # def create_projection_1L(self, presyn_area, presyn_idx, connector, target = None, synapse_dynamics = None):
    #     for i in range(self.layers):
    #         self.create_projection_11(presyn_area, presyn_idx, i, connector, target, synapse_dynamics)
    #
    # def create_projection_LL(self, presyn_area, connector, target = None, synapse_dynamics = None):
    #     for i in range(self.layers):
    #         self.create_projection_11(presyn_area, i, i, connector, target, synapse_dynamics)
    #
    # def create_projection_L1(self, presyn_area, postsyn_idx, connector, target = None, synapse_dynamics = None):
    #     for i in range(presyn_area.layers):
    #         self.create_projection_11(presyn_area, i, postsyn_idx, connector, target, synapse_dynamics)
    #
    #
    # def create_within_lateral_projection(self, connector, target = None, synapse_dynamics = None):
    #     for i in range(self.layers):
    #         for j in range(self.layers):
    #             if (i!=j):
    #                 self.proj.append(Projection(self.pop[i], self.pop[j],
    #                             connector,
    #                             target=target,
    #                             synapse_dynamics=synapse_dynamics,
    #                             label=self.label +"_"+ str(i) +"->"+ self.label +"_"+ str(j)
    #                             ))
    #
    # def create_within_projection(self, connector, target = None, synapse_dynamics = None):
    #     for i in range(self.layers):
    #         self.proj.append(Projection(self.pop[i], self.pop[i],
    #                     connector,
    #                     target=target,
    #                     synapse_dynamics=synapse_dynamics,
    #                     label=self.label +"_"+ str(i) +"->"+ self.label +"_"+ str(i)
    #                     ))
    #





##
