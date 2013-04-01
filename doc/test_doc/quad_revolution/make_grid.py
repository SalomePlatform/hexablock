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

#### Make a grid and get its quads #####

import hexablock

doc  = hexablock.addDocument ("default")

ori  = doc.addVertex ( 0, 0, 0)
vx   = doc.addVector ( 1 ,0, 0)
vz   = doc.addVector ( 0, 0, 1)

dr = 1
da = 360
dl = 1

grid = doc.makeCylindrical (ori, vx,vz, dr,da,dl, nr,na,nl, False)

nr = 1
na = 6
nl = 1

liste = [ ]
for nx in range (nr) :
    for ny in range (na) :
        cell = grid.getQuadIJ (nx, ny, nl)
        liste.append (cell);
