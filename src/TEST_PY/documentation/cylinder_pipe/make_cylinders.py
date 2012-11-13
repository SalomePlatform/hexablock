    
####### Make Cylinders Test ########## 

import hexablock

    
doc  = hexablock.addDocument ("default")

ori1 = doc.addVertex ( 0, 0, 0)
ori2 = doc.addVertex (-5, 0, 5)
vz   = doc.addVector ( 0, 0, 1)
vx   = doc.addVector ( 1 ,0, 0)

nr1 = 2
nl1 = 10
nr2 = 1
nl2 = 10

cyl1 = doc.addCylinder (ori1, vz, nr1, nl1)
cyl2 = doc.addCylinder (ori2, vx, nr2, nl2)
elts = doc.makeCylinders (cyl1, cyl2)

doc.saveVtk ("make_cylinders.vtk")
