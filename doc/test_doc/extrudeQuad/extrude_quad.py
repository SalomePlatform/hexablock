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

####### Test for Extrude Quad ###############

import hexablock


doc = hexablock.addDocument("Extrude Quad Test")

dimx = 11
dimy = 11
dimz = 2

orig1 = doc.addVertex ( 0,0,0)
vx    = doc.addVector ( 1,0,0)
vy    = doc.addVector ( 0,1,0)
vz    = doc.addVector ( 0,0,1)
dir1  = doc.addVector ( 1,1,1)
dir2  = doc.addVector ( 1,1,-1)

dx =1
dy=1
dz=1

grid1 = doc.makeCartesianUni (orig1, vx,vy,vz, dx,dy,dz, dimx,dimy,dimz)

mx = dimx/2
my = dimy/2
liste1 = []
liste2 = []

liste1.append(grid1.getQuadIJ (mx, my, dimz))
liste2.append(grid1.getQuadIJ (mx, my, 0))
for nx in range(dimx):
    if nx!=mx:     
        liste1.append(grid1.getQuadIJ (nx, my, dimz))
        liste2.append(grid1.getQuadIJ (nx, my, 0))

for ny in range(dimy):
    if ny!=my:
        liste1.append(grid1.getQuadIJ (mx, ny, dimz))
        liste2.append(grid1.getQuadIJ (mx, ny, 0))

tlen = []
dh0  = 1
dh   = 0.02
lh = 0
for nro in range(5):
    dh = 1.5*dh + 1
    lh += dh
    tlen.append(lh)

nbiter = 5
doc.saveVtk ("prisme1.vtk")
prisme2 = doc.extrudeQuadsUni (liste2, dir2, dh0, nbiter)
doc.saveVtk ("prisme2.vtk")

prisme1 = doc.extrudeQuads (liste1, dir1, tlen)

doc.saveVtk ("prisme3.vtk")
