
####### Make Pipe Test ########## 

import hexablock
        
doc  = hexablock.addDocument ("default")

orig = doc.addVertex(50, 0, 0)
vz = doc.addVector(0, 0, 1)
int_radius = 3
ext_radius = 4
height = 5

pipe = doc.addPipe(orig, vz, int_radius, ext_radius, height)

vb = doc.addVector(1, 0, 0)
nr = 4
na = 9
nl = 5

elts = doc.makePipe(pipe, vb, nr, na, nl)

#####  doc.saveVtk ("make_pipe.vtk")
