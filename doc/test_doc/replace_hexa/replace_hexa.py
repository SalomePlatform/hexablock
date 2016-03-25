# -*- coding: latin-1 -*-
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

####### Test replace hexa ###############

import hexablock

doc  = hexablock.addDocument ("default")


# Hexa : Grid construction

orig = doc.addVertex ( 0, 0, 0)
vx   = doc.addVector ( 1 ,0, 0)
vy   = doc.addVector ( 0, 1, 0)
vz   = doc.addVector ( 0, 0, 1)

size_x = 3
size_y = 3
size_z = 3

grid = doc.makeCartesian (orig, vx, vy, vz, size_x, size_y, size_z)

c1 = grid.getVertexIJK (1, 2, size_z)
c2 = grid.getVertexIJK (1, 1, size_z)
c3 = grid.getVertexIJK (2, 1, size_z)


# Hexa : Hexa construction

pa1 = doc.addVertex (-1, -1, 0)
pb1 = doc.addVertex ( 1, -1, 0)
pc1 = doc.addVertex ( 1,  1, 0)
pd1 = doc.addVertex (-1,  1, 0)

pa2 = doc.addVertex (-2, -2, 0)
pb2 = doc.addVertex ( 2, -2, 0)
pc2 = doc.addVertex ( 2,  2, 0)
pd2 = doc.addVertex (-2,  2, 0)

edab1 = doc.addEdge (pa1, pb1)
edbc1 = doc.addEdge (pb1, pc1)
edcd1 = doc.addEdge (pc1, pd1)
edda1 = doc.addEdge (pd1, pa1)

edab2 = doc.addEdge (pa2, pb2)
edbc2 = doc.addEdge (pb2, pc2)
edcd2 = doc.addEdge (pc2, pd2)
edda2 = doc.addEdge (pd2, pa2)

edaa = doc.addEdge (pa1, pa2)
edbb = doc.addEdge (pb1, pb2)
edcc = doc.addEdge (pc1, pc2)
eddd = doc.addEdge (pd1, pd2)

qpattern = []
qpattern.append (doc.addQuad (edab1, edbc1, edcd1, edda1))
qpattern.append (doc.addQuad (edab1, edbb,  edab2, edaa))
qpattern.append (doc.addQuad (edbc1, edcc,  edbc2, edbb))
qpattern.append (doc.addQuad (edcd1, eddd,  edcd2, edcc))
qpattern.append (doc.addQuad (edda1, edaa,  edda2, eddd))

#####  doc.saveVtk ("replace0.vtk")

# Hexa replacement (on the grid)
doc.replace (qpattern, pd2,c1, pa2,c2, pb2,c3)

#####  doc.saveVtk ("replace1.vtk")

