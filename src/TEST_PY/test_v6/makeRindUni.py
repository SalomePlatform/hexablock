
import hexablock

doc = hexablock.addDocument ("model")

center = doc.addVertex (0,0,0)
vx     = doc.addVector (1,0,0)
vz     = doc.addVector (0,0,1)

rhole  = 1.0
rint   = 3.0
rext   = 4.0
angle  = 180.0

nr  = 3
na  = 8
nh  = 6

vplan = doc.addVertex (0, 0, -rext*0.7)

doc.makeRindUni (center,vx,vz, rhole,rint,rext, angle,vplan,nr,na,nh)

## doc.saveVtk ("model1.vtk")
doc.addLaws (1, True)
mesh_hexas = hexablock.mesh(doc)
