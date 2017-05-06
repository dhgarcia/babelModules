
"""

Model class

"""

# general Python imports
import pickle
import numpy as np

# Parameters imports
import Parameters.settings as setting
import Parameters.options as option
import Parameters.parameters as param
import Parameters.preferences as preference

if setting.simulator_name == 'brian':
    from pyNN.brian import *               # use if running brian
    #import pyNN.brian as sim

if setting.simulator_name == 'spyNNaker':
    from pyNN.spiNNaker import *            # Imports the pyNN.spiNNaker module
    #import pyNN.spiNNaker as sim



#Taken from PyNN
import copy

class BaseModelType(object):
    """Base class for standard and native cell and synapse model classes."""
    # does not really belong in this module. Some reorganisation required.
    default_parameters = {}
    default_initial_values = {}

    def __init__(self, parameters):
        self.parameters = self.__class__.checkParameters(parameters, with_defaults=True)

    @classmethod
    def has_parameter(cls, name):
        return name in cls.default_parameters

    @classmethod
    def get_parameter_names(cls):
        return cls.default_parameters.keys()

    @classmethod
    def checkParameters(cls, supplied_parameters, with_defaults=False):
        """
        Returns a parameter dictionary, checking that each
        supplied_parameter is in the default_parameters and
        converts to the type of the latter.

        If with_defaults==True, parameters not in
        supplied_parameters are in the returned dictionary
        as in default_parameters.

        """
        default_parameters = cls.default_parameters
        if with_defaults:
            parameters = copy.copy(default_parameters)
        else:
            parameters = {}
        if supplied_parameters:
            for k in supplied_parameters.keys():
                if k in default_parameters.keys():
                    err_msg = "For %s in %s, expected %s, got %s (%s)" % \
                              (k, cls.__name__, type(default_parameters[k]),
                               type(supplied_parameters[k]), supplied_parameters[k])
                    # same type
                    if type(supplied_parameters[k]) == type(default_parameters[k]):
                        parameters[k] = supplied_parameters[k]
                    # float and something that can be converted to a float
                    elif isinstance(default_parameters[k], float):
                        try:
                            parameters[k] = float(supplied_parameters[k])
                        except (ValueError, TypeError):
                            raise errors.InvalidParameterValueError(err_msg)
                    # list and something that can be transformed to a list
                    elif isinstance(default_parameters[k], list):
                        try:
                            parameters[k] = list(supplied_parameters[k])
                        except TypeError:
                            raise errors.InvalidParameterValueError(err_msg)
                    else:
                        raise errors.InvalidParameterValueError(err_msg)
                else:
                    raise errors.NonExistentParameterError(k, cls, cls.default_parameters.keys())
        return parameters

    def describe(self, template='modeltype_default.txt', engine='default'):
        """
        Returns a human-readable description of the cll or synapse type.

        The output may be customized by specifying a different template
        togther with an associated template engine (see ``pyNN.descriptions``).

        If template is None, then a dictionary containing the template context
        will be returned.
        """
        context = {
            "name": self.__class__.__name__,
            "parameters": self.parameters,
        }
        return descriptions.render(engine, template, context)



# Model imports
from Model.areas import ModelArea
from Model.projections import ModelProjection


from Plotting import mappings

class Model(object):
    """ brain model class """
    def __init__(self, areas, projections):

        self.model_areas = {}
        self.model_projections = {}

        if areas:
            for area in areas:
                self.add_area(area)

        if projections:
            for proj in projections:
                self.add_projection(proj)


    def add_area(cls, area):
        a = ModelArea(area)
        cls.model_areas.update({a.get('label'): a})

    def add_projection(cls, projection):
        #need to get the presyn and postsyn areas
        presyn_area = cls.model_areas[projection['presyn_area']]
        postsyn_area = cls.model_areas[projection['postsyn_area']]
        projection['presyn_area'] = presyn_area
        projection['postsyn_area'] = postsyn_area
        proj = ModelProjection(projection)
        cls.model_projections.update({proj.get('label'): proj})


    def record_area(cls, area_label):
        for label, area in cls.model_areas.iteritems():
            if label == area_label:
                area.record_all()

    def record_areas(cls):
        #for area in cls.model_areas.itervalues():
            for label, area in cls.model_areas.iteritems():
                if 'injector' not in label:
                    area.record_all()


    def set_area_param(cls, area_label, param, value=None):
        for label, area in cls.model_areas.iteritems():
            if label == area_label:
                area.set_population_params(param, value)

    def get_projection_weight(cls, proj_label, format):
        w_proj = cls.model_projections[proj_label]
        w_proj.get_weight(format = format)

        return w_proj.wts

    def save_projection_weights(cls, initial=None, final=None):
        if initial is not None: #initial file given
            i_weights = {}
            for label, proj in cls.model_projections.iteritems():
                i_weights[label] = proj.start_wts
            pickle.dump(i_weights, open(initial, 'wb'), -1)
        if final is not None:
            f_weights = {}
            for label, proj in cls.model_projections.iteritems():
                #f_weights[label] = proj.get_projection_weight(label, 'array')
                proj.get_weight(format = 'list')
                f_weights[label] = proj.wts
            pickle.dump(f_weights, open(final, 'wb'), -1)

    def save_projections(cls, pfile=None):

        if pfile is not None: # file given
            projections = {}
            for label, proj in cls.model_projections.iteritems():
                conn_list = proj.get_connection_data()
                projections[label] = conn_list
            pickle.dump(projections, open(pfile, 'wb'), -1)


    def save_output(cls, setup_time, run_time, pfile=None):

        if pfile is not None: # file given
            output_file = open(pfile, "a+")
            output_file.write("--------------------------------------------------------------------------------\n")
            output_file.write(option.date)
            output_file.write("\n")
            output_file.write("SIMULATOR NAME: %s\n" % setting.simulator_name)
            output_file.write("-------------------------------\n")
            output_file.write("NETWORK PARAMETERS:\n")
            output_file.write("Network base input size: %d\n" % preference.base_num_neurons)
            output_file.write("Feedback on? %s\n" % option.feedback_on)
            output_file.write("FEF PFC? %s\n" % option.active_pfc)
            output_file.write("Aversive Inhibitory? %s\n" % option.aversive_inhibitory)
            output_file.write("Top Down Priming? %s\n" % option.top_down_priming)
            output_file.write("Learning Attention on? %s\n" % option.plasticity_on)
            output_file.write("Learning Audio on? %s\n" % option.plasticity_au_on)
            output_file.write("Preferred orientation: %d\n" % preference.preferred_orientation)
            output_file.write("Aversive orientation: %d\n" % preference.aversive_orientation)
            output_file.write("-------------------------------\n")
            output_file.write("SAVE PARAMETERS:\n")
            output_file.write("Load Learned weights? %s\n" % option.Load)
            output_file.write("Save Learned weights? %s\n" % option.Save)
            output_file.write("Recalling Learned Label? %s\n" % option.Naming)

            output_file.write("-------------------------------\n")
            output_file.write("TIMINGS:\n")
            output_file.write("Setup time: %f s\n" % setup_time)
            output_file.write("Load time: %f s \n" % (run_time-setup_time-(setting.runtime/1000.0)))
            output_file.write("Run time: %f s \n" % (setting.runtime/1000.0))

            output_file.write("-------------------------------\n")
            output_file.write("PFC PARAMETERS::\n")
            output_file.write("Bias Preferred: %f \n" % param.pfc_bias_pref)
            output_file.write("Bias Aversive: %f \n" % param.pfc_bias_avert)
            output_file.write("Bias Neutral: %f \n" % param.pfc_bias_neut)

            output_file.write("-------------------------------\n")
            output_file.write("ATTENTION STDP PARAMETERS::\n")
            output_file.write("Tau Plus: %f ms\n" % param.tau_plus_at)
            output_file.write("Tau Minus: %f ms\n" % param.tau_minus_at)
            output_file.write("dA Plus: %f nA\n" % param.dA_plus_at)
            output_file.write("dA Minus: %f nA\n" % param.dA_minus_at)
            output_file.write("gMax: %f nA\n" % param.gmax_at)
            output_file.write("AUDITORY STDP PARAMETERS::\n")
            output_file.write("Tau Plus: %f ms\n" % param.tau_plus_ac)
            output_file.write("Tau Minus: %f ms\n" % param.tau_minus_ac)
            output_file.write("dA Plus: %f nA\n" % param.dA_plus_ac)
            output_file.write("dA Minus: %f nA\n" % param.dA_minus_ac)
            output_file.write("gMax: %f nA\n" % param.gmax_ac)


            output_file.write("-------------------------------\n")
            output_file.write("WEIGHTS:\n")
            for label, proj in cls.model_projections.iteritems():
                if (label == "V2->V4" and option.plasticity_on) or (label == "STP->PBR" and option.plasticity_au_on):
                    proj.get_weight_diff()
                    for w in range(len(proj.wts)):
                        output_file.write(label + str(w) + ": min weight_diff %f max weight_diff %f\n" % (min(proj.weight_diffs[w].ravel()), max(proj.weight_diffs[w].ravel())))
                        output_file.write(label + str(w) + "_init: min weight %f max weight %f\n" % (min(proj.start_wts[w]), max(proj.start_wts[w])))
                        output_file.write(label + str(w) + "_final: min weight %f max weight %f\n" % (min(proj.wts[w]), max(proj.wts[w])))

            output_file.write("-------------------------------\n")
            output_file.write("POPULATION SPIKES:\n")
            # for label, area in cls.model_areas.iteritems():
            #     size = area.get('size')
            #     for i in range(area.get('layers')):
            #         spikes = area.get_spikes(i)
            #
            #         active = []
            #         counts = np.zeros((int(size[0])*int(size[1])),int)
            #         # Do a plot of all spikes
            #         for sp in spikes:
            #             counts[int(sp[0])] +=1
            #             if sp[0] not in active:
            #                 active.append(sp[0])
            #
            #         #print p_counts
            #         #p_total = sum(p_counts)
            #         #print "Total", title, "activity", p_total, min(p_counts), max(p_counts)
            #
            #         output_file.write(label + str(i) + " Spike# : %f \n" % len(spikes))
            #         output_file.write(label + str(i) + " spike_count: max %f min %f \n" % (min(counts), max(counts)))
            #         #output_file.write(label + str(i) + " spike_count: %f \n" % counts)



            output_file.write("-------------------------------\n")
            output_file.write("NEURONS:\n")
            output_file.write("PBR speech active neurons: ")
            for item in cls.get_salient_position("PBR"):
                output_file.write("{},".format(item))
            output_file.write("\n")
            output_file.write("LIP salient active neurons: ")
            for item in cls.get_salient_position("LIP"):
                output_file.write("{},".format(item))
            output_file.write("\n")




    def plot_area(cls, area_label, spikes=False, maps=False, saliency=False, time=None):
        for label, area in cls.model_areas.iteritems():
            if label == area_label:
                if spikes == True:
                    area.plot_population_spikes(time)
                if maps == True:
                    area.plot_population_map(time)
                if saliency == True:
                    area.plot_population_saliency_map(time)


    def plot_areas(cls, spikes=False, maps=False, saliency=False, time=None):
        for area in cls.model_areas.itervalues():
            if spikes == True:
                area.plot_population_spikes(time)
            if maps == True:
                area.plot_population_map(time)
            if saliency == True:
                area.plot_population_saliency_map(time)

    def plot_projection_weight(cls, proj_label):
        for label, proj in cls.model_projections.iteritems():
            if label == proj_label:
                proj.plot_projection_weights()

    def plot_projections_weights(cls):
        for proj in cls.model_projections.itervalues():
            proj.plot_projection_weights()


    def get_salient_position(cls, salient_area):
        s_area = cls.model_areas[salient_area]
        s_size = s_area.get('size')

        s_spikes = s_area.get_spikes(0) #change for many layers

        s_counts = np.zeros((int(s_size[0])*int(s_size[1])),int)
        # Do a plot of all spikes
        for sp in s_spikes:
            s_counts[int(sp[0])] +=1

        # Get the coordinates of the most active area
        active = [np.argmax(s_counts), float(s_counts[np.argmax(s_counts)]) ]
        return active

    def get_active_spikes(cls, area):
        area = cls.model_areas[area]
        size = area.get('size')

        spikes = area.get_spikes(0) #change for many layers

        active = []
        # Get spikes that were active
        for sp in spikes:
            #print "spike ", sp[0], sp[1]
            if sp[0] not in active:
                active.append(sp[0])

        return active

    def plot_salient_position(cls, input_area, active_area, time = None):
        # Get the coordinates of the most active area
        active = cls.get_salient_position(active_area)

        p_area = cls.model_areas[active_area]
        i_area = cls.model_areas[input_area]

        p_size = p_area.get('size')
        i_size = i_area.get('size')

        p_array = mappings.coord_map_array(p_size)
        p_x_array = p_array[0]
        p_y_array = p_array[1]

        # indicate salient position in input space
        # calculated from LIP activity
        # again have to swap x and y as it is for plotting
        p_mag = round(float(i_size[0])/float(p_size[0]))
        x_attend = float(p_x_array[active[0]]*p_mag)
        y_attend = float(p_y_array[active[0]]*p_mag)
        #p_array[int(x_attend),int(y_attend)] = 2
        print "Salient position in Input Space", x_attend, y_attend

        salient_position = [x_attend, y_attend]

        i_area.plot_population_map(salient_position, time)

    #def connectInput (cls, label, area):
    #    print "try"
    #    p_area = cls.model_areas[active_area]
    #    #TODO

    def get_area_population(cls, area_label, pop_idx):
        p_area = cls.model_areas[area_label]
        return p_area.pop[pop_idx]

#
