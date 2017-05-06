

"""

Model projections class
V2_V1, V4_V2, PFC_V4, LIP_V4

"""

import numpy as np

# Parameters imports
import Parameters.settings as setting

# pyNN imports
#from pyNN import models, connectors
if setting.simulator_name == 'brian':
    from pyNN.brian import *               # use if running brian

if setting.simulator_name == 'spyNNaker':
    from pyNN.spiNNaker import *            # Imports the pyNN.spiNNaker module


# Model imports
from Model.model import BaseModelType
from Model.areas import ModelArea

# Plots import
from Plotting import plots


class ModelProjection(BaseModelType):
    """ brain model projection class """

    default_parameters = {
        'presyn_area':  ModelArea({}),   #
        'postsyn_area': ModelArea({}),   #
        'connector':    {},   #
        'target':       "excitatory",   #
        'synapses':     {},   #
        'pop_idx':      [],   #
        'label':        "proj", #
    }


    def __init__(self, parameters):
        BaseModelType.__init__(self, parameters)

        #self.parameters = parameters
        #check parameters
        self.parameters = self.checkParameters(parameters, with_defaults=True)
        self.presyn_area = self.parameters['presyn_area']
        self.postsyn_area = self.parameters['postsyn_area']
        self.indexes = self.parameters['pop_idx']
        self.set_connector()
        self.set_stdp()
        self.proj = []
        self.start_wts = []
        self.global_min = 9999.0
        self.global_max = 0.0

        #create projections
        if self.connector:
            self.create_projection()


    def set(cls, param, val=None):
        if  param in cls.parameters.keys():
            cls.parameters[param] = val
        else:
            raise errors.NonExistentParameterError(param, cls, cls.parameters.keys())

    def get(cls, param):
        val = []
        if  param in cls.parameters.keys():
            val = cls.parameters[param]
        else:
            raise errors.NonExistentParameterError(param, cls, cls.parameters.keys())

        return val


    def create_projection(cls):
        #
        if cls.stdp is not None:
            plasticity = SynapseDynamics(slow=cls.stdp)
        else:
            plasticity = cls.stdp
        #idx = cls.parameters['pop_idx']
        for i in range(len(cls.indexes)):
            pre_idx=cls.indexes[i][0]
            post_idx=cls.indexes[i][1]
            con_idx = 0 if len(cls.connector)==1 else i
            cls.proj.append(Projection(cls.presyn_area.pop[pre_idx], cls.postsyn_area.pop[post_idx],
                            cls.connector[con_idx],
                            target=cls.parameters['target'],
                            synapse_dynamics=plasticity,
                            label=cls.parameters['label'] + str(pre_idx) + str(post_idx)
                            ))
            #cls.start_wts.append(cls.proj[-1].getWeights(format='array')) #get('weight','array')
            cls.start_wts.append(cls.proj[-1].getWeights(format='list')) #get('weight','array')
            print "-- Creating Projection", cls.parameters['label'], i

        #cls.start_wts.append(cls.get_weight(format = 'array'))


    def set_connector(cls, connector_parameters=None):
        # create the connector ?
        connection_method = []
        if connector_parameters is not None:
            cls.set('connector', connector_parameters)
        params = cls.parameters['connector']

        if 'connectors' in params:
            connection_method = params['connectors']
        else:
            if 'method' in params:
                if params['method'] == "FromListConnector":
                    conn_list = params['conn_list']
                    connection_method.insert(0,FromListConnector(conn_list))
                elif params['method'] == "OneToOneConnector":
                    weights = params['weights'] if isinstance(params['weights'], list) else [params['weights']] #check if weights is a single number
                    delays = params['delays']
                    for w in range(len(weights)):
                        connection_method.insert(0,OneToOneConnector(weights = weights[w], delays = delays))
                elif params['method'] == "AllToAllConnector":
                    weights = params['weights'] if isinstance(params['weights'], list) else [params['weights']] #check if weights is a single number
                    delays = params['delays']
                    allow_self_connections = params['allow_self_connections']
                    for w in range(len(weights)):
                        connection_method.insert(0,AllToAllConnector(
                                            allow_self_connections = allow_self_connections,
                                            weights = weights[w], delays = delays
                                            ))
                elif params['method'] == "FixedProbabilityConnector":
                    weights = params['weights'] if isinstance(params['weights'], list) else [params['weights']] #check if weights is a single number
                    delays = params['delays']
                    allow_self_connections = params['allow_self_connections']
                    p_connect = params['p_connect']
                    for w in range(len(weights)):
                        connection_method.insert(0,FixedProbabilityConnector(p_connect = p_connect,
                                            allow_self_connections = allow_self_connections,
                                            weights = weights[w], delays = delays
                                            ))

        cls.connector = connection_method



    def set_stdp(cls, stdp_params=None):
        # create the synapse dynamics
        stdp_model = None
        timing_dependence = None
        weight_dependence = None
        voltage_dependence = None
        if stdp_params is not None:
            cls.set('synapses', stdp_params)
        params = cls.parameters['synapses']

        if 'stdp_model' in params:
            stdp_model = params['stdp_model']
        else:
            if 'timing_dependence' in params:
                timing_dependence = params['timing_dependence']
            else:
                if 'tau_plus' in params or 'tau_minus' in params:
                    tau_plus = 20.0     #defaults
                    tau_minus = 20.0    #defaults
                    if 'tau_plus' in params:
                        tau_plus = params['tau_plus']
                    if 'tau_minus' in params:
                        tau_minus = params['tau_minus']
                    timing_dependence = SpikePairRule(tau_plus = tau_plus, tau_minus = tau_minus)

            if 'weight_dependence' in params:
                weight_dependence = params['weight_dependence']
            else:
                if 'w_min' in params or 'w_max' in params:
                    w_min = 0.0
                    w_max = 1.0
                    A_plus = 0.01
                    A_minus = 0.01
                    if 'w_min' in params:
                        w_min = params['w_min']
                    if 'w_max' in params:
                        w_max = params['w_max']
                    if 'A_plus' in params:
                        A_plus = params['A_plus']
                    if 'A_minus' in params:
                        A_minus = params['A_minus']
                    weight_dependence = AdditiveWeightDependence(w_min = w_min, w_max = w_max, A_plus = A_plus, A_minus = A_minus)

            if 'dendritic_delay_fraction' in params:
                dendritic_delay_fraction = params['dendritic_delay_fraction']
                stdp_model = STDPMechanism(
                timing_dependence = timing_dependence,
                weight_dependence = weight_dependence,
                voltage_dependence = voltage_dependence,
                #dendritic_delay_fraction=dendritic_delay_fraction
                )

        cls.stdp = stdp_model

    def set_weight(cls):
        print "model.projections.set_weight"

    def get_weight(cls, format):
        cls.wts = []
        #cls.wts_list = []
        for proj in range(len(cls.proj)):
            cls.wts.append(cls.proj[proj].getWeights(format=format)) #get('weight','array')
            #cls.wts_list.append(cls.proj[proj].getWeights(format='list')) #get('weight','array')
            #cls.proj[proj].printWeights(file = cls.parameters['label'] + str(proj) + '_w.txt',format='list') #get('weight','array')

        #return cls.wts


    def get_weight_diff(cls):
        #cls.get_weight(format = 'array')
        cls.get_weight(format = 'list')

        cls.weight_diffs = []
        cls.weight_diffs_array = []
        for w in range(len(cls.wts)):
            #print "Projection", cls.parameters['label'], w, "in weight", cls.start_wts[w], "end weight", cls.wts[w]
            diffs = np.subtract(cls.wts[w], cls.start_wts[w]) # list
            #diffs = cls.wts[w] - cls.start_wts[w] # array

            if min(diffs.ravel()) < cls.global_min:
                cls.global_min = min(diffs.ravel())
            if max(diffs.ravel()) > cls.global_max:
                cls.global_max = max(diffs.ravel())
            print "Projection", cls.parameters['label'], w, "min weight", min(diffs.ravel()), "max weight", max(diffs.ravel())

            cls.weight_diffs.append(diffs)
            diffs = cls.weights_to_array(diffs, w)
            cls.weight_diffs_array.append(diffs)


    def plot_weights(cls, weight_diffs, title):
        pre_size = cls.presyn_area.get('size')
        post_size = cls.postsyn_area.get('size')
        plots.plot_weights(weight_diffs, pre_size, post_size, cls.global_max, cls.global_min, title = title)

    def plot_projection_weights(cls):
        cls.get_weight_diff()
        for w in range(len(cls.wts)):
            prelabel = cls.presyn_area.get('label')
            postlabel = cls.postsyn_area.get('label')
            pre_idx=cls.indexes[w][0]
            post_idx=cls.indexes[w][1]
            title = prelabel + "_" + str(pre_idx) + "->" + postlabel + "_" + str(post_idx)
            cls.plot_weights(cls.weight_diffs_array[w], title = title)


    def weights_to_array(cls, weight, proj_idx):
        #params = cls.parameters['connector']

        pre_size = cls.presyn_area.get('size')
        post_size = cls.postsyn_area.get('size')

        weight_array = np.empty((pre_size[0]*pre_size[1],
                            post_size[0]*post_size[1]))
        weight_array.fill(np.nan)

        if setting.simulator_name == 'brian':
            conn_list = []
            for i in range(cls.proj[proj_idx].size()):
                conn_list.append(cls.proj[proj_idx].__getitem__(i).addr)

        if setting.simulator_name == 'spyNNaker':
            #source = cls.proj[proj_idx]._get_synaptic_data(True, "source")
            #target = cls.proj[proj_idx]._get_synaptic_data(True, "target")
            #source = cls.proj[proj_idx].getSources(format='list')
            #target = cls.proj[proj_idx].getTargets(format='list')
            conn_list = cls.proj[proj_idx]._get_synaptic_data(True, None)


            #conn_list = zip(source, target)

        for i, conn in enumerate(conn_list):
            weight_array[conn[0]][conn[1]] = weight[i]

        # if 'connectors' in params:
        #     print "Don't know what to do with connectors param ", params['connectors']
        # else:
        #     if 'method' in params:
        #         if params['method'] == "FromListConnector":
        #             print "Do the From list stuff"
        #             conn_list = params['conn_list']
        #             for i, conn in enumerate(conn_list):
        #                 weight_array[conn[0]][conn[1]] = weight[i]
        #         elif params['method'] == "OneToOneConnector":
        #             print "Do the One to One stuff"
        #             np.fill_diagonal(weight_array, weight)
        #         elif params['method'] == "AllToAllConnector":
        #             print "Do the All to All stuff"
        #             weight_array = np.reshape(weight,
        #                             (pre_size[0]*pre_size[1],
        #                             post_size[0]*post_size[1]))
        #         elif params['method'] == "FixedProbabilityConnector":
        #             print "Don't know what to do with param ", params['method']

        return weight_array


    def get_connection_data(cls):

        connections = []
        for proj in range(len(cls.proj)):
            if setting.simulator_name == 'brian':
                conn_list = []
                for i in range(cls.proj[proj].size()):
                    conn = (cls.proj[proj].__getitem__(i).addr[0],
                            cls.proj[proj].__getitem__(i).addr[1],
                            cls.proj[proj].__getitem__(i).weight,
                            cls.proj[proj].__getitem__(i).delay)
                    conn_list.append(conn)

            if setting.simulator_name == 'spyNNaker':
                syn_data = cls.proj[proj]._get_synaptic_data(True, None)
                #source = cls.proj[proj]._get_synaptic_data(True, "source")
                #target = cls.proj[proj]._get_synaptic_data(True, "target")
                #weight = cls.proj[proj]._get_synaptic_data(True, "weight")
                #delay = cls.proj[proj]._get_synaptic_data(True, "delay")

                #conn_list = zip(source, target, weight, delay)
                conn_list = []
                for data in syn_data:
                    conn = (data[0], data[1], data[2], data[3])
                    conn_list.append(conn)

            connections.append(conn_list)

        return connections
#
