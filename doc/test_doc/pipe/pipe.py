# -*- coding: utf-8 -*-
####### Make Pipe Test ########## 

import hexablock
        
doc  = hexablock.addDocument ("Make Pipe Test")

# Simple Pipe -----

nbR = 8
nbA = 10
nbH = 8

pipe0 = doc.makePipeTop (nbR, nbA, nbH)
pipe0.saveVtk ("makePipeTop.vtk")

# Uniform Pipe -----

orig = doc.addVertex (0, 0, 0)
vx   = doc.addVector (1, 0, 0)
vz   = doc.addVector (0, 0, 1)
rint = 1
rext = 3
angle = 360
hauteur = 2

pipe1 = doc.makePipeUni (orig, vx, vz, rint, rext, angle, hauteur, nbR, nbA, nbH)
pipe1.saveVtk ("makePipeUni.vtk");
   

# Custom Pipe -----

origin = doc.addVertex (0, 5, 0)
tr = [10, 20, 30, 40]
ta = [45, 90, 135, 180, 225]
th = [5, 30, 40, 60]

pipe2 = doc.makePipe(origin, vx, vz, tr, ta, th)
pipe2.saveVtk("makePipe.vtk")
