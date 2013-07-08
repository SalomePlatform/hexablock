# -*- coding: utf-8 -*-
####### Sphere Grid Test ##########

import hexablock


doc  = hexablock.addDocument ("Sphere Grid Test")

# Simple Sphere -----

nr = 4
na = 8
nh = 12

sphere0 = doc.makeSphereTop(nr, na, nh)
sphere0.saveVtk("makeSphereTop.vtk")

# Uniform Sphere -----

center  = doc.addVertex (0, 0, 0)
vplan = doc.addVertex (0, 0, -0.5)
vx    = doc.addVector (1, 0, 0)
vz    = doc.addVector (0, 0, 1)
rtrou = 1
rext  = 10
angle = 180

sphere1 = doc.makeSphereUni (center, vx, vz, rtrou, rext, angle, vplan, nr, na, nh)
sphere1.saveVtk("makeSphereUni.vtk")

# Custom Sphere -----

dr     = (rext-rtrou)/nr
dtheta = angle/na
dphi   = 180.0/nh
trad = []
tang = []
tphi = []

for nro in range(nr+1):
  trad.append(rtrou + nro*dr)
  
for nro in range(na+1):
  tang.append(nro*dtheta)
  
for nro in range(nh+1):
  tphi.append(-90 + nro*dphi)

sphere2 = doc.makeSphere (center, vx, vz, trad, tang, tphi)
sphere2.saveVtk("makeSphere.vtk")
