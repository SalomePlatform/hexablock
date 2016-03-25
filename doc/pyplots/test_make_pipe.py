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

# exemple repris de test_hexa1.cxx

import os
import GEOM
import SALOMEDS
import hexablock

print "test pipe..."
doc = hexablock.addDocument()

orig = doc.addVertex(50, 0, 0)

vz = doc.addVector(0, 0, 1)

int_radius = 3
ext_radius = 4
height = 5

pi = doc.addPipe(orig, vz, int_radius, ext_radius, height)

vb = doc.addVector(1, 0, 0)
nr = 4
na = 9
nl = 5
elts = doc.makePipe(pi, vb, nr, na, nl)

file_name = os.path.join(os.environ['TMP'], 'pipe.vtk')
####  elts.saveVtk(file_name)

print "...test pipe OK"
