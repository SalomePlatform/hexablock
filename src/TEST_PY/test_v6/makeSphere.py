# Copyright (C) 2009-2024  CEA, EDF
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

doc = hexablock.addDocument ("model")

center = doc.addVertex (0,0,0)
vx     = doc.addVector (1,0,0)
vz     = doc.addVector (0,0,1)

rhole  = 1.0
rext   = 4.0
angle  = 260.0

trayons = [ 1, 2, 3 , 4 ]
tangles = [ 0, 30, 60, 90, 120, 150, 180]
tphi    = [ -30, -10, 0, 10, 30, 45, 60, 75, 80 ]

doc.makeSphere (center,vx,vz, trayons, tangles, tphi)

doc.saveVtk ("model1.vtk")
doc.addLaws (0.5, True)
mesh_hexas = hexablock.mesh(doc)
