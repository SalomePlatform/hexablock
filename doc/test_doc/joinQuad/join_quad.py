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

####### Test for Join Quad ###############

import hexablock



doc = hexablock.addDocument("Join Quad Test")

dimx = 11
dimy = 11
dimz = 2

grid1 = doc.makeCartesianTop (dimx,dimy,dimz)

orig2 = doc.addVertex(dimx/2.0,0,8)
vectj = doc.addVector (0,1,0);
vecti = doc.addVector (1,0,0);
grid2 = doc.makeCylinderUni (orig2, vecti, vectj, 1.0, 2.0, 180.0, 11.0, dimz,dimy,dimx)

mx = dimx/2
my = dimy/2
prems = grid1.getQuadIJ (mx, my, dimz)
cible = grid2.getQuadJK (dimz, mx, my)

va1 = prems.getVertex (0)
va2 = prems.getVertex (1)

vb1 = cible.getVertex (1)
vb2 = cible.getVertex (2)

hauteur = 5

liste = []

liste.append(prems)
for nx in range(mx):
    if nx!=mx:
        liste.append(grid1.getQuadIJ(nx, my, dimz))

for ny in range(dimy):
    if (ny!=my):
        liste.append(grid1.getQuadIJ (mx, ny, dimz))

doc.saveVtk ("jointQuad1.vtk")
joint = doc.joinQuadsUni (liste, cible, va1,vb1,va2,vb2, hauteur)
doc.saveVtk ("jointQuad2.vtk")
