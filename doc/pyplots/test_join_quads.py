#

# exemple repris de test_hexa1.cxx (test_joint())

import os
import GEOM
import SALOMEDS
import hexablock

print "test join..."
doc = hexablock.addDocument()

orig1 = doc.addVertex(0, 0, 0)
dx = doc.addVector(1, 0, 0)
dy = doc.addVector(0, 1, 0)
dz = doc.addVector(0, 0, 1)

dimx = 11
dimy = 11
dimz = 2
grid1 = doc.makeCartesian(orig1, dx, dy, dz, dimx, dimy, dimz)

orig2 = doc.addVertex(dimx/2.0, 0, 8)
grid2 = doc.makeCylindrical(orig2, dx, dy, 1, 180, 1, dimz, dimy,
                            dimx, False)

mx = dimx/2
my = dimy/2
prems = grid1.getQuadIJ(mx, my, dimz)
cible = grid2.getQuadJK(dimz, mx, my)

# @todo reference = test_hexa1.cxx ou ModelTest.cxx ?
v1 = prems.getVertex(0)
v2 = cible.getVertex(0)
v3 = prems.getVertex(1)
v4 = cible.getVertex(3)

liste = [prems]
liste.extend([grid1.getQuadIJ(nx, my, dimz) for nx in range(dimx) if nx != mx])
liste.extend([grid1.getQuadIJ(mx, ny, dimz) for ny in range(dimy) if ny != my])

height = 5

elts = doc.joinQuads(liste, cible, v1, v2, v3, v4, height) 

## indispensable ???
nbr_joint_vertex = elts.countVertex()
nbr_surf_vertex = nbr_joint_vertex/(height + 1)

ind0 = elts.findVertex(v1)
for nh in range(nbr_surf_vertex):
    elts.getVertex(nh).setScalar(5)
for nh in range(height):
    elts.getVertex(nh*nbr_surf_vertex).setScalar(3)


file_name = os.path.join(os.environ['TMP'], 'joint.vtk')
elts.saveVtk(file_name)

print "...test join OK"
