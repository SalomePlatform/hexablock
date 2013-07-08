# -*- coding: utf-8 -*-
####### Rind Grid Test ##########

import hexablock


doc  = hexablock.addDocument ("Rind Grid Test")


# Simple Rind -----


nr = 4
na = 8
nh = 12

rind0 = doc.makeRindTop(nr, na, nh)
rind0.saveVtk("makeRindTop.vtk")

# Uniform Rind -----

center  = doc.addVertex (0, 0, 0)
vplan = doc.addVertex (0, 0, -0.5)
vx    = doc.addVector (1, 0, 0)
vz    = doc.addVector (0, 0, 1)
rtrou = 1
rint  = 8
rext  = 10
angle = 180

rind1 = doc.makeRindUni(center, vx, vz, rtrou, rint, rext, angle, vplan, nr, na, nh)
rind1.saveVtk("makeRindUni.vtk")

# Custom Rind -----

dr     = (rext-rtrou)/nr
dtheta = angle/na
dphi   = 180.0/nh
trad = []
tang = []
tphi = []

trad.append(rtrou)
for nro in range(nr+1):
  trad.append(rint + nro*dr)
  
for nro in range(na+1):
  tang.append(nro*dtheta)
  
for nro in range(nh+1):
  tphi.append(-90 + nro*dphi)

rind2 = doc.makeRind(center, vx, vz, trad, tang, tphi)
rind2.saveVtk("makeRind.vtk")
