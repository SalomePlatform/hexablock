####### Test for Prism Quad ###############

import hexablock


doc = hexablock.addDocument("default")

orig = doc.addVertex(0, 0, 0)
dx = doc.addVector(1, 0, 0)
dy = doc.addVector(0, 1, 0)
dz = doc.addVector(0, 0, 1)

dimx = 11
dimy = 11
dimz = 2
grid = doc.makeCartesian(orig, dx, dy, dz, dimx, dimy, dimz)

mx = dimx/2
my = dimy/2
prems = grid.getQuadIJ(mx, my, dimz)

liste = [prems]
liste.extend([grid.getQuadIJ(nx, my, dimz) for nx in range(dimx) if nx != mx])
liste.extend([grid.getQuadIJ(mx, ny, dimz) for ny in range(dimy) if ny != my])

vec = doc.addVector(1, 1, 1)
elts = doc.prismQuads(liste, vec, 5) 

#####  elts.saveVtk ("prism_quad.vtk")
