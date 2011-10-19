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
elts.saveVtk(file_name)

print "...test pipe OK"
