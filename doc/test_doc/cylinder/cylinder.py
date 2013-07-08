# -*- coding: utf-8 -*-
####### Make Cylinder Test ########## 

import hexablock


# Simple Cylinder -----

doc  = hexablock.addDocument ("Make Cylinder Test")

nbR = 8
nbA = 10
nbH = 8

cylinder0 = doc.makeCylinderTop (nbR, nbA, nbH)
cylinder0.saveVtk ("makeCylinderTop.vtk")
   
# Uniform Cylinder -----

origin = doc.addVertex (0, 0, 5)
vx     = doc.addVector (1, 0, 0)
vz     = doc.addVector (0, 0, 1)
rint   = 2
rext   = 4
angle  = 300
hauteur = 1


cylinder1 = doc.makeCylinderUni (origin, vx, vz, rint, rext, angle, hauteur, nbR, nbA, nbH)
cylinder1.saveVtk ("makeCylinderUni.vtk")

# Custom Cylinder

origin = doc.addVertex (5, 0, 0)
tr = [10, 20, 30, 40]
ta = [45, 90, 135, 180, 225]
th = [5, 30, 40, 60]

cylinder2 = doc.makeCylinder(origin, vx, vz, tr, ta, th)
cylinder2.saveVtk("makeCylinder.vtk")
