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
doc.saveVtk(file_name)

devant = doc.addVector(5, 0, 0)
grid2 = doc.makeTranslation(grid, devant)

file_name = os.path.join(os.environ['TMP'], 'transfo_translation.vtk')
doc.saveVtk(file_name)

grid4 = doc.makeRotation(grid2, orig, dirVr, 45)

file_name = os.path.join(os.environ['TMP'], 'transfo_rotation.vtk')
doc.saveVtk(file_name)


print "...test make elements by transforming elements OK"


