# -*- coding: utf-8 -*-
####### Make Pipes Test ########## 

import hexablock
import math

    
doc  = hexablock.addDocument ("Make Pipes Test")

rext1 =  2*math.sqrt (2.0)
rext2 =  3*math.sqrt (2.0)
rint1 =  rext1/2
rint2 =  rext1
h1   =  16
h2   =  16
xl1  =  -8

orig1  = doc.addVertex ( 0, 0,  xl1)
orig2  = doc.addVertex (-8, 0,  0)
vz1   = doc.addVector ( 0, 0,  1)
vz2   = doc.addVector ( 1, 0,  0)
   
pipes = doc.makePipes (orig1, vz1, rint1, rext1, h1, orig2, vz2, rint2, rext2, h2);
pipes.saveVtk("makePipes.vtk")
