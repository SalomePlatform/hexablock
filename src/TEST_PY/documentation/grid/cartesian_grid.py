####### Test Cartesian Grid #################    
    
import hexablock


doc  = hexablock.addDocument ("default")

orig = doc.addVertex(0, 0, 0)

dx = doc.addVector(1, 0, 0)
dy = doc.addVector(0, 1, 0)
dz = doc.addVector(0, 0, 1)

ni = 15 
nj = 12
nk = 8

grid = doc.makeCartesian(orig, dx, dy, dz, ni, nj, nk)

doc.saveVtk ("cartesian_grid.vtk")
