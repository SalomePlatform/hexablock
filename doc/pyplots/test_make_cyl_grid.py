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

import os
import GEOM
import SALOMEDS
import hexablock

# differents cas possibles de remplissage possibles :


print "test grille cylindrique..."
doc = hexablock.addDocument()

orig1 = doc.addVertex(0, 0, 0)
orig2 = doc.addVertex(10, 0, 0)
orig3 = doc.addVertex(0, 10, 0)
orig4 = doc.addVertex(10, 10, 0)
orig5 = doc.addVertex(0, 20, 0)
orig6 = doc.addVertex(10, 20, 0)

vz = doc.addVector(0, 0, 1)
vx = doc.addVector(1, 0, 0)

dr = 1
dl = 1
nr = 2
nl = 3

c1 = doc.makeCylindrical(orig1, vx, vz, dr, 360, dl, nr, 4, nl, True)
c2 = doc.makeCylindrical(orig2, vx, vz, dr, 360, dl, nr, 8, nl, True)
c3 = doc.makeCylindrical(orig3, vx, vz, dr, 270, dl, nr, 8, nl, True)
c4 = doc.makeCylindrical(orig4, vx, vz, dr, 270, dl, nr, 7, nl, True)
c5 = doc.makeCylindrical(orig5, vx, vz, dr, 360, dl, nr, 5, nl, True)
c6 = doc.makeCylindrical(orig6, vx, vz, dr, 360, dl, nr, 6, nl, True)

base2 = nr * nl * 8
for i in range(2):
    c2.getHexa(base2 + i).setScalar(5)

file_name = os.path.join(os.environ['TMP'], 'grilles_cylindriques.vtk')
####  doc.saveVtk(file_name)

print "...test grille cylindrique OK"


