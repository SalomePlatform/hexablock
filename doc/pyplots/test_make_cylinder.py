#

# exemple repris de test_HEXABLOCK.py

import os
import GEOM
import SALOMEDS
import hexablock

print "test cylindre..."
doc = hexablock.addDocument()

base = doc.addVertex(0, 0, 0)

direction = doc.addVector(0, 0, 1)

radius = 4
height = 5

cyl = doc.addCylinder(base, direction, radius, height)

vb = doc.addVector(1, 0, 0)
nr = radius
na = 9
nl = height
elts = doc.makeCylinder(cyl, vb, nr, na, nl)

file_name = os.path.join(os.environ['TMP'], 'cylindre.vtk')
elts.saveVtk(file_name)

print "...test cylindre OK"
