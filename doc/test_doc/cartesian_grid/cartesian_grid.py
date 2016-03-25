# -*- coding: utf-8 -*-
# Copyright (C) 2009-2016  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

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
