####### Test for Cut Hexa ###############

import hexablock


doc = hexablock.addDocument("default")

size_x = 2
size_y = 1
size_z = 1

orig = doc.addVertex(0, 0, 0)
dirVr = doc.addVector(1, 1, 1)
grid = doc.makeCartesian1(orig, dirVr, size_x, size_y, size_z, 0, 0, 0)

#####  doc.saveVtk ("hexa.vtk")

arete = grid.getEdgeK(0, 0, 0)
doc.cut(arete, 1)

#####  doc.saveVtk ("cut_hexa.vtk")
