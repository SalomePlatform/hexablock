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
doc.saveVtk(file_name)

doc.disconnectQuad(hexa1, quad)

file_name = os.path.join(os.environ['TMP'], 'disconnect2.vtk')
doc.saveVtk(file_name)


print "...test disconnect elements OK"


