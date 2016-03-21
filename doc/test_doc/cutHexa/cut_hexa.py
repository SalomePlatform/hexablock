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

####### Test for Cut Hexa ###############

import hexablock


doc = hexablock.addDocument("Cut Hexa Test")

size_x = 2
size_y = 1
size_z = 1

grid  = doc.makeCartesianTop (size_x,size_y,size_z)
arete = grid.getEdgeK (0, 0, 0)

doc.saveVtk ("decoupe1.vtk")
nbCuts = 2
grid2 = doc.cutUni (arete, nbCuts)
doc.saveVtk ("decoupe2.vtk")

#tablen = []
#reste  = 1
#abscisse = 0
#for nl in range(5):
#    reste     /= 2
#    abscisse  += reste
#    tablen.append (abscisse)
tablen = [2, 3, 8, 14, 18]
arete = grid2.getEdge(0)
grid3 = doc.cut(arete, tablen)
doc.saveVtk ("decoupe3.vtk")
