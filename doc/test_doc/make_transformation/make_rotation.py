####### Test make rotation ###############

import hexablock

doc = hexablock.addDocument("default")

size_x = 1
size_y = 1
size_z = 2

orig = doc.addVertex(0, 0, 0)
dirVr = doc.addVector(1, 1, 1)

grid = doc.makeCartesian1(orig, dirVr, size_x, size_y, size_z, 0, 0, 0)
#####  grid.saveVtk ("initial.vtk")

orig.setScalar(2)
grid1 = doc.makeRotation(grid, orig, dirVr, 60)

#####  grid1.saveVtk ("rotation.vtk")
