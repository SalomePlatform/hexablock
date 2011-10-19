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
grid.saveVtk(file_name)

print "...test grille spherique OK"
