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

print "test make elements by transforming elements..."
doc = hexablock.addDocument()

size_x = 1
size_y = 1
size_z = 2

orig = doc.addVertex(0, 0, 0)
dirVr = doc.addVector(1, 1, 1)

grid = doc.makeCartesian1(orig, dirVr, size_x, size_y, size_z, 0, 0,
                           0)
orig.setScalar(2)

file_name = os.path.join(os.environ['TMP'], 'transfo0.vtk')
####  doc.saveVtk(file_name)

devant = doc.addVector(5, 0, 0)
grid2 = doc.makeTranslation(grid, devant)

file_name = os.path.join(os.environ['TMP'], 'transfo_translation.vtk')
####  doc.saveVtk(file_name)

grid4 = doc.makeRotation(grid2, orig, dirVr, 45)

file_name = os.path.join(os.environ['TMP'], 'transfo_rotation.vtk')
####  doc.saveVtk(file_name)


print "...test make elements by transforming elements OK"


