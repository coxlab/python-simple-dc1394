#!/usr/bin/env python
from numpy import *
from matplotlib import pylab

from dc1394simple import *

cams = enumerate_cameras()
print(cams)

if len(cams) >  0:
    cameras = []
    for (i,cam) in enumerate(cams):
        cameras += [SimpleCamera(cam),]
    for (i,cam) in enumerate(cameras):
        f = cam.capture_frame()
        pylab.figure(i)
        pylab.imshow(f, cmap=pylab.cm.gray)
    pylab.show()
else:
    print "Only 1 camera found, oneCamTest.py might be more useful"
