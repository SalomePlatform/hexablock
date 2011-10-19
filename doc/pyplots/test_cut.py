#

import os
import GEOM
import SALOMEDS
import hexablock

print "test cut hexaedra..."
doc = hexablock.addDocument()

size_x = 2
size_y = 1
size_z = 1

orig = doc.addVertex(0, 0, 0)
dirVr = doc.addVector(1, 1, 1)

grid = doc.makeCartesian1(orig, dirVr, size_x, size_y, size_z, 0, 0,
                           0)
arete = grid.getEdgeK(0, 0, 0)

## doc.dump()

file_name = os.path.join(os.environ['TMP'], 'no_decoupe.vtk')
doc.saveVtk(file_name)

doc.cut(arete, 1)

# doc.dump()
file_name = os.path.join(os.environ['TMP'], 'decoupe.vtk')
doc.saveVtk(file_name)


print "...test cut hexaedra OK"


