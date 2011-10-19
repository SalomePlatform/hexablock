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
grid.saveVtk(file_name)

print "...test grille cartesienne OK"
