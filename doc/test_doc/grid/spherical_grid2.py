# -*- coding: latin-1 -*-
# Copyright (C) 2009-2013  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
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

import hexablock
 
doc  = hexablock.addDocument ("default")

orig = doc.addVertex(0, 0, 0)

direction = doc.addVector(1, 1, 1)

dx = doc.addVector(1, 0, 0)
dy = doc.addVector(0, 1, 0)
dz = doc.addVector(0, 0, 1)

n = 2
k = 0.8

grid = doc.makeSpherical(orig, dx, dy, dz, n, k)

#####  doc.saveVtk ("spherical_grid2.vtk")
