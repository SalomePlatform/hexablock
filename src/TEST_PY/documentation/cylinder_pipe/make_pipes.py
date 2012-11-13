    
####### Make Pipes Test ########## 

import hexablock


doc  = hexablock.addDocument ("default")

ori1 = doc.addVertex ( 0, 0, 0)
ori2 = doc.addVertex (-5, 0, 5)
vz   = doc.addVector ( 0, 0, 1)
vx   = doc.addVector ( 1 ,0, 0)

nr1 = 2
ni1 = 1
nl1 = 10
nr2 = 1
ni2 = 0.5
nl2 = 10

pipe1 = doc.addPipe   (ori1, vz, ni1, nr1, nl1)
pipe2 = doc.addPipe   (ori2, vx, ni2, nr2, nl2)
elts  = doc.makePipes (pipe1, pipe2)

doc.saveVtk ("make_pipes.vtk")
