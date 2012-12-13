####### Test make scale ###############

import hexablock


doc  = hexablock.addDocument ("default")
ori  = doc.addVertex ( 0, 0, 0)
vz   = doc.addVector ( 0, 0, 1)
vx   = doc.addVector ( 1 ,0, 0)

dr = 1
da = 360
dl = 1

nr = 1
na = 6
nl = 1

grid = doc.makeCylindrical (ori, vx,vz, dr,da,dl, nr,na,nl, False)

dest   = doc.addVertex (15, 0, 0)
grid2  = doc.makeScale (grid, dest, 0.5)

#####  doc .saveVtk ("make_scale.vtk")
