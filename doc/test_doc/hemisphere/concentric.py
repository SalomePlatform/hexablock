# -*- coding: utf-8 -*-
####### Concentric (Spherical) Grid Test ##########

import hexablock


doc  = hexablock.addDocument ("Spherical Grid Test")

# Simple Spherical Grid -----

nbLayers = 3
crit = 0

grid0 = doc.makeSphericalTop(nbLayers, crit)
grid0.saveVtk("makeSphericalTop.vtk")

# Uniform Spherical Grid -----

center = doc.addVertex (0, 0, 10)
vx     = doc.addVector (1, 0, 0)
vz     = doc.addVector (0, 1, 1)
rayon  = 1

grid1 = doc.makeSphericalUni(center, vx, vz, rayon, nbLayers, crit);
grid1.saveVtk("makeSphericalUni.vtk")

# Custom Spherical Grid-----

tr = [10, 20, 30, 40] # a list of radiuses (one radius for each layer)

grid2 = doc.makeSpherical (center, vx, vz, tr, crit)
grid2.saveVtk("makeSpherical.vtk")
