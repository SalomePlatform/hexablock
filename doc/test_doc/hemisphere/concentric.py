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
