####### Test prismQuadsVec ###############

import hexablock

doc  = hexablock.addDocument("prism_vec")
    
ori  = doc.addVertex ( 0, 0, 0)
vx   = doc.addVector ( 1 ,0, 0)
vz   = doc.addVector ( 0, 0, 1)

dr = 1
da = 360
dl = 1

nr = 1
na = 6
nl = 1

grid = doc.makeCylindrical (ori, vx,vz, dr,da,dl, nr,na,nl, False)

liste = [ ]
for nx in range (nr) :
    for ny in range (na) :
        cell = grid.getQuadIJ (nx, ny, nl)
        liste.append (cell);

axis = doc.addVector (1, 1, 1);
hauteurs = [1, 3, 7, 15]

elts = doc.prismQuadsVec (liste, axis, hauteurs, 0)

#####  elts.saveVtk("prism_quads_vec.vtk")
