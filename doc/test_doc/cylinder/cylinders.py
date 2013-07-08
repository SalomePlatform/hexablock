# -*- coding: utf-8 -*-
####### Make Cylinders Test ########## 

import hexablock
import math

    
doc  = hexablock.addDocument ("Make Cylinders Test")

rext1 =  2*math.sqrt (2.0)
rext2 =  3*math.sqrt (2.0)
h1   =  16
h2   =  16
xl1  =  -8

orig1  = doc.addVertex ( 0, 0,  xl1)
orig2  = doc.addVertex (-8, 0,  0)
vz1   = doc.addVector ( 0, 0,  1)
vz2   = doc.addVector ( 1, 0,  0)
   
cylinders = doc.makeCylinders (orig1, vz1, rext1, h1, orig2, vz2, rext2, h2)
cylinders.saveVtk("makeCylinders.vtk")
