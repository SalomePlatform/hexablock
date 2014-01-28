
import hexablock

doc = hexablock.addDocument ("model")

center = doc.addVertex (0,0,0)
vx     = doc.addVector (1,0,0)
vz     = doc.addVector (0,0,1)

rhole  = 1.0
rext   = 4.0
angle  = 260.0

nr  = 3
na  = 8
nh  = 6

vplan = doc.addVertex (0, 0, -rext*0.5)

doc.makeSphereUni (center,vx,vz, rhole,rext,angle,vplan, nr,na,nh)

doc.saveVtk ("model1.vtk")
