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

####### Test for Cut Hexa ###############

import hexablock


doc = hexablock.addDocument("default")

size_x = 2
size_y = 1
size_z = 1

orig = doc.addVertex(0, 0, 0)
dirVr = doc.addVector(1, 1, 1)
grid = doc.makeCartesian1(orig, dirVr, size_x, size_y, size_z, 0, 0, 0)

#####  doc.saveVtk ("hexa.vtk")

arete = grid.getEdgeK(0, 0, 0)
doc.cut(arete, 1)

#####  doc.saveVtk ("cut_hexa.vtk")
