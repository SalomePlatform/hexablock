####### Test for spherical grid ###########
  
import hexablock


doc  = hexablock.addDocument ("default")

orig = doc.addVertex(0, 0, 0)
radius = 5
n = 2
k = 1

grid = doc.makeSpherical(orig, radius, n, k)

doc.saveVtk ("spherical_grid.vtk")
