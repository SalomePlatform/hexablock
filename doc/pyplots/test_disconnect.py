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


print "test disconnect elements..."
doc = hexablock.addDocument()

size_x = 2;
size_y = 2;
size_z = 1;

orig1 = doc.addVertex(0, 0, 0);
orig2 = doc.addVertex(4, 0, 0);
orig3 = doc.addVertex(8, 0, 0);


dirVr = doc.addVector(1, 1, 1);

grid1 = doc.makeCartesian1(orig1, dirVr, size_x, size_y, size_z, 0, 0,
                           0)
grid2 = doc.makeCartesian1(orig2, dirVr, size_x, size_y, size_z, 0, 0,
                           0)
grid3 = doc.makeCartesian1(orig3, dirVr, size_x, size_y, size_z, 0, 0,
                           0)


nvtk = 0
#doc.setLevel(1)

# matrice =  ???
# ecart = doc.addVector(0.5, 0.5, 0)

hexa1 = grid1.getHexaIJK(1, 1, 0)
hexa2 = grid2.getHexaIJK(1, 1, 0)
hexa3 = grid3.getHexaIJK(1, 1, 0)

quad = grid1.getQuadJK(1, 1, 0)
edge = grid2.getEdgeK(1, 2, 0)
vertex = grid3.getVertexIJK(1, 1, 1)

quad.setScalar(5)
edge.setScalar(5)
vertex.setScalar(5)

file_name = os.path.join(os.environ['TMP'], 'disconnect1.vtk')
####  doc.saveVtk(file_name)

doc.disconnectQuad(hexa1, quad)

file_name = os.path.join(os.environ['TMP'], 'disconnect2.vtk')
####  doc.saveVtk(file_name)


print "...test disconnect elements OK"


