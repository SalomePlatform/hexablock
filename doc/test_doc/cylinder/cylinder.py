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
