#### Make a grid and get its quads #####

import hexablock

doc  = hexablock.addDocument ("default")

ori  = doc.addVertex ( 0, 0, 0)
vx   = doc.addVector ( 1 ,0, 0)
vz   = doc.addVector ( 0, 0, 1)

dr = 1
da = 360
dl = 1

grid = doc.makeCylindrical (ori, vx,vz, dr,da,dl, nr,na,nl, False)

nr = 1
na = 6
nl = 1

liste = [ ]
for nx in range (nr) :
    for ny in range (na) :
        cell = grid.getQuadIJ (nx, ny, nl)
        liste.append (cell);
