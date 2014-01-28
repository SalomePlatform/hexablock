
import hexablock

doc = hexablock.addDocument ("model")

center = doc.addVertex (0,0,0)
vx     = doc.addVector (1,0,0)
vz     = doc.addVector (0,0,1)

rhole  = 1.0
rext   = 4.0
angle  = 260.0

trayons = [ 1, 2, 3 , 4 ]
tangles = [ 0, 30, 60, 90, 120, 150, 180]
tphi    = [ -30, -10, 0, 10, 30, 45, 60, 75, 80 ]

doc.makeSphere (center,vx,vz, trayons, tangles, tphi)

doc.saveVtk ("model1.vtk")
