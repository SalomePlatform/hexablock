####### Test make translation ###############

import hexablock

doc = hexablock.addDocument("default")

size_x = 1
size_y = 1
size_z = 2

orig = doc.addVertex(0, 0, 0)
dirVr = doc.addVector(1, 1, 1)

grid = doc.makeCartesian1(orig, dirVr, size_x, size_y, size_z, 0, 0, 0)

doc.saveVtk ("cartesian.vtk")

devant = doc.addVector(5, 0, 0)
grid1 = doc.makeTranslation(grid, devant)

doc.saveVtk ("translation.vtk")
