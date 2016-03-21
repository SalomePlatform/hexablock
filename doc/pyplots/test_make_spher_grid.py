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


print "test grille spherique..."
doc = hexablock.addDocument()

orig = doc.addVertex(0, 0, 0)

direction = doc.addVector(1, 1, 1)

#n = 2
n = 2
# k = 0.8  # amincit les couches (ou epaissit : si < 1 ou > 1)

k = 0.8  # amincit les couches (ou epaissit : si < 1 ou > 1)

grid = doc.makeSpherical(orig, direction, n, k)



file_name = os.path.join(os.environ['TMP'], 'grille_spherique.vtk')
####  grid.saveVtk(file_name)

print "...test grille spherique OK"
