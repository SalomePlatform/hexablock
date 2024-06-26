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
rint   = 3.0
rext   = 4.0
angle  = 180.0

nr  = 3
na  = 8
nh  = 6

vplan = doc.addVertex (0, 0, -rext*0.7)

doc.makeRindUni (center,vx,vz, rhole,rint,rext, angle,vplan,nr,na,nh)

## doc.saveVtk ("model1.vtk")
doc.addLaws (1, True)
mesh_hexas = hexablock.mesh(doc)
