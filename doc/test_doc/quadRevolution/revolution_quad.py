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

#### Quad Revolution Test #####

import hexablock


doc = hexablock.addDocument("Quad Revolution Test")

center = doc.addVertex(0, 0, 0)
vz     = doc.addVector(0, 0, 1)

v1     = doc.addVertex (10, 0, 0)
v2     = doc.addVertex (11, 0, 0)
v3     = doc.addVertex (11, 0, 2)
v4     = doc.addVertex (10, 0, 2)
quad   = doc.addQuadVertices (v1, v2, v3, v4)
doc.saveVtk("revolution1.vtk")

angle = 180
nbLayers = 8
grid0 = doc.revolutionQuadUni  (quad, center, vz, angle, nbLayers)
doc.saveVtk ("revolution2.vtk")

nr = 1
na = 6
nl = 1

grid = doc.makeCylinderTop (nr,na,nl)

liste = []
for nx in range(nr):
    for ny in range(na):
        cell = grid.getQuadIJ (nx, ny, nl)
        liste.append(cell)

center = doc.addVertex(0, -10, 0)
axis = doc.addVector (1, 0, 0)
angle = 180
nbLayers = 9
grid1 = doc.revolutionQuadsUni  (liste, center, axis, angle, nbLayers)
if grid1.isValid():
    doc.saveVtk ("revolution3.vtk")
