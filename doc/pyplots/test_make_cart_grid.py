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

#

# exemple repris de test_HEXABLOCK.py

import os
import GEOM
import SALOMEDS
import hexablock


print "test grille cartesienne..."
doc = hexablock.addDocument()

orig = doc.addVertex(0, 0, 0)

#direction = doc.addVector(1, 1, 1)

dx = doc.addVector(1, 0, 0)
dy = doc.addVector(0, 1, 0)
dz = doc.addVector(0, 0, 1)

ni = 15
nj = 12
nk = 8

grid = doc.makeCartesian(orig, dx, dy, dz, ni, nj, nk)

file_name = os.path.join(os.environ['TMP'], 'grille_cartesienne.vtk')
####  grid.saveVtk(file_name)

print "...test grille cartesienne OK"
