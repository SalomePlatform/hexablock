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

import hexablock



doc  = hexablock.addDocument ("test_cone")

orig = doc.addVertex (0, 0, 0)
ori2 = doc.addVertex (0, 0, 10)
vz   = doc.addVector (0, 0, 1)
vx   = doc.addVector (1 ,0, 0)

rad1   = 2.0
rad2   = 1.0
height = 5.0
angle  = 270.0
hollow = False

nr = 2
na = 8
nl = 1

c1 = doc.makeCylindrical (orig, vx,vz, rad1, angle, height, nr,na,nl, hollow)
c2 = doc.makeCylindrical (ori2, vx,vz, rad2, angle, height, nr,na,nl, hollow)

vh0 = c2.getVertexIJK (0, 0, 0)
vh1 = c2.getVertexIJK (1, 0, 0)

vb0 = c1.getVertexIJK (0, 0, nl)
vb1 = c1.getVertexIJK (1, 0, nl)

qcible = c2.getQuadIJ (0, 0, 0)
qliste = []
for ni in range (nr) :
    for nj in range (na) :
        quad = c1.getQuadIJ (ni, nj, nl)
        qliste.append (quad)

## Join quads to make a cone between the 2 cylinders
doc.joinQuads  (qliste, qcible, vb0, vh0, vb1, vh1, 1)

### -------------------- Mesh 
law = doc.addLaw ("Uniform", 4)

for np in range(doc.countPropagation()):
    propa = doc.getPropagation (np)
    propa.setLaw (law) 

mesh_hexas = hexablock.mesh(doc)

