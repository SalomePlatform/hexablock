

import hexablock
 
doc  = hexablock.addDocument ("default")

orig = doc.addVertex(0, 0, 0)

direction = doc.addVector(1, 1, 1)

dx = doc.addVector(1, 0, 0)
dy = doc.addVector(0, 1, 0)
dz = doc.addVector(0, 0, 1)

n = 2
k = 0.8

grid = doc.makeSpherical(orig, dx, dy, dz, n, k)

doc.saveVtk ("spherical_grid2.vtk")
