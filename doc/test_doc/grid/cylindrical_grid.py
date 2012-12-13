####### Test Regular Cylindrical Grid #################    

import hexablock


doc  = hexablock.addDocument ("default")

orig1 = doc.addVertex(0, 0, 0)
orig2 = doc.addVertex(10, 0, 0)
orig3 = doc.addVertex(0, 10, 0)
orig4 = doc.addVertex(10, 10, 0)
orig5 = doc.addVertex(0, 20, 0)
orig6 = doc.addVertex(10, 20, 0)

vz = doc.addVector(0, 0, 1)
vx = doc.addVector(1, 0, 0)

dr = 1
dl = 1
nr = 2
nl = 3

c1 = doc.makeCylindrical(orig1, vx, vz, dr, 360, dl, nr, 4, nl, True)
c2 = doc.makeCylindrical(orig2, vx, vz, dr, 360, dl, nr, 8, nl, True)
c3 = doc.makeCylindrical(orig3, vx, vz, dr, 270, dl, nr, 8, nl, True)
c4 = doc.makeCylindrical(orig4, vx, vz, dr, 270, dl, nr, 7, nl, True)
c5 = doc.makeCylindrical(orig5, vx, vz, dr, 360, dl, nr, 5, nl, True)
c6 = doc.makeCylindrical(orig6, vx, vz, dr, 360, dl, nr, 6, nl, True)

#####  doc.saveVtk ("cylindrical.vtk")
