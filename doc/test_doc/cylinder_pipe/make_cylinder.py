
####### Make Cylinder Test ########## 

import hexablock


doc  = hexablock.addDocument ("default")

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

doc.saveVtk ("make_cylinder.vtk")
