# -*- coding: utf-8 -*-
####### Test Cartesian Grid #################    
    
import hexablock


doc  = hexablock.addDocument ("Cartesian Grid Test")

# Simple Cartesian Grid

nbX = 3
nbY = 4
nbZ = 5

grid0 = doc.makeCartesianTop(nbX, nbY, nbZ)
grid0.saveVtk("makeCartesianTop.vtk")

# Uniform Cartesian Grid

orig1 = doc.addVertex(10, 0, 0)

vecX = doc.addVector(1, 0, 0)
vecY = doc.addVector(0, 1, 0)
vecZ = doc.addVector(0, 0, 1)

lenX = 5 
lenY = 3.5
lenZ = 2

grid1 = doc.makeCartesianUni(orig1, vecX, vecY, vecZ, lenX, lenY, lenZ, nbX, nbY, nbZ)
grid1.saveVtk("makeCartesianUni.vtk")

# Custom Cartesian Grid

orig2 = doc.addVertex(20, 0, 0)

tx = [] # a list of x coordinates
ty = [] # a list of y coordinates
tz = [] # a list of z coordinates
for i in range(6):
  tx.append(i+1)
  ty.append(i+1)
  tz.append(i+1)
# tx=ty=tz=[1,2,3,4,5,6]

grid2 = doc.makeCartesian(orig2, vecX, vecY, vecZ, tx, ty, tz)
grid2.saveVtk("makeCartesian.vtk")
