
"""

Yarp grabber class
Read from a yarp port
and provide input SpikeSourceArray
for the input_pol

"""

# general Python imports


# Yarp
import yarp


class yarpInputGrabber(object):
    """docstring for yarpInputGrabber"""
    def __init__(self, portName):
        super(yarpInputGrabber, self).__init__()

        #yarp.Network.init()

        self.port_name = portName
        self.port = yarp.BufferedPortBottle()


    def open(self):
        self.port.open(self.port_name)

    def close(self):
        self.port.close()


    def connect(self, portName):
        yarp.Network.connect(portName, self.port_name)

    def read(self):
        return self.port.read()


    def getSpikes(self):
        Data = self.read()
        Spikes = map(int,Data.toString().split())
        return Spikes



class yarpOutGrabber(object):
    """docstring for yarpOutGrabber"""
    def __init__(self, portName):
        super(yarpOutGrabber, self).__init__()
        #yarp.Network.init()

        self.port_name = portName
        self.port = yarp.BufferedPortBottle()
        self.bottle = yarp.Bottle()


    def open(self):
        self.port.open(self.port_name)

    def close(self):
        yarp.port.close()


    def connect(self, portName):
        yarp.Network.connect(self.port_name, portName)

    def write(self):
        self.port.write()


    def setActiveNeurons(self, values):
        self.bottle = self.port.prepare()
        #check if values is a list or a single value
        if isinstance(values, list):
            for val in values:
                self.bottle.addInt(int(val))
        else:
            self.bottle.addInt(int(values))
        self.write()
#
