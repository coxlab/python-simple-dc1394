from numpy import *
from matplotlib import pylab

from dc1394simple import *

cams = enumerate_cameras()
print(cams)

if len(cams) >  0:
    cam = SimpleCamera(cams[0])
else:
    cam = SimpleCamera()

f = cam.capture_fake_frame()

print f.width
print f.height

f_1d = f.to_1D_list()

print len(f_1d)

pylab.imshow(f.array)