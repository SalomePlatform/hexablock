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

####### Test for Join Quad ###############

import hexablock

doc = hexablock.addDocument("default")

orig1 = doc.addVertex(0, 0, 0)
dx = doc.addVector(1, 0, 0)
dy = doc.addVector(0, 1, 0)
dz = doc.addVector(0, 0, 1)

dimx = 11
dimy = 11
dimz = 2
grid1 = doc.makeCartesian(orig1, dx, dy, dz, dimx, dimy, dimz)

orig2 = doc.addVertex(dimx/2.0, 0, 8)
grid2 = doc.makeCylindrical(orig2, dx, dy, 1, 180, 1, dimz, dimy,
                             dimx, False)

mx = dimx/2
my = dimy/2
prems = grid1.getQuadIJ(mx, my, dimz)
cible = grid2.getQuadJK(dimz, mx, my)

v1 = prems.getVertex(0)
v3 = prems.getVertex(1)
v2 = cible.getVertex(1)
v4 = cible.getVertex(2)

liste = [prems]
liste.extend([grid1.getQuadIJ(nx, my, dimz) for nx in range(dimx) if nx != mx])
liste.extend([grid1.getQuadIJ(mx, ny, dimz) for ny in range(dimy) if ny != my])

height = 5

elts = doc.joinQuads(liste, cible, v1, v2, v3, v4, height) 

#####  elts.saveVtk ("join_quads.vtk")
