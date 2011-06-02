# -*- coding: latin-1 -*-
# Copyright (C) 2009-2011  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#


import hexablock

# ======================================================== test_sphere
def test_sphere():
   doc = hexablock.addDocument()
   orig = doc.addVertex (0,0,0)

   ncouches = 1
   k = 0.8
   decal  = doc.addVector (1,1,1)
   sphere = doc.makeSpherical(orig, decal, ncouches, k)

   sphere.saveVtk ("test_sphere.vtk")
   # doc.dump ()
   print "test_sphere OK"
   
   
# ======================================================== test_cartesi1
def test_cartesi1():
  size_x = 15
  size_y = 12
  size_z = 8

  doc  = hexablock.addDocument()
  orig = doc.addVertex (0,0,0)
  direc= doc.addVector (1,1,1)
  grid = doc.makeCartesian1(orig, direc, size_x,size_y,size_z,0,0,0)
  grid.saveVtk ("test_cartesi1.vtk")
  # doc.dump ()
  print "test_cartesi1 OK"



#// ======================================================== test_find
def test_find():
   size_x = 2
   size_y = 2
   size_z = 2

   doc  = hexablock.addDocument()
   
   orig = doc.addVertex(0,0,0)
   direc = doc.addVector (1,1,1)
   grid = doc.makeCartesian1 (orig, direc, size_x,size_y,size_z,0,0,0)

   grid.saveVtk ("mini1.vtk")
   #doc.dump ().

   v00 = doc.findVertex (0, 0, 0)   
   v02 = doc.findVertex (1, 1, 0)
   v06 = doc.findVertex (1, 1, 1)
   v08 = doc.findVertex (2, 1, 0)
   v10 = doc.findVertex (2, 1, 1)
   v22 = doc.findVertex (2, 1, 2)
   v26 = doc.findVertex (2, 2, 2)

   assert v00
   assert v02
   assert v06
   assert v08
   assert v10
   assert v22
   assert v26

   assert doc.findEdge (v06, v10)
   assert doc.findEdge (v10, v06)

   assert doc.findQuad (v02, v10)
   assert doc.findQuad (v06, v08)
   assert not doc.findQuad (v02, v06)  #CS_FAILS
   
   assert doc.findHexa (v00, v06)
   assert doc.findHexa (v06, v26)
   assert doc.findHexa (v26, v06)
   print "test_find OK"

#// ======================================================== test_joint
def test_joint():
   import HEXA_ORB
   dimx = 11
   dimy = 11
   dimz = 2

   doc = hexablock.addDocument()

   orig1 = doc.addVertex (0,0,0)
   direc = doc.addVector (1,1,1)

   grid1 = doc.makeCartesian1(orig1, direc, dimx, dimy, dimz, 0, 0, 0 )

   orig2 = doc.addVertex  (dimx/2.0,0,8)
   vectj = doc.addVector (0,1,0)
   vecti = doc.addVector (1,0,0)
   grid2 = doc.makeCylindrical (orig2, vecti, vectj, 1, 180, 1,dimz,dimy,dimx, False)

   mx = dimx/2
   my = dimy/2
   prems = grid1.getQuad1 (HEXA_ORB.DIR_Z, mx, my, dimz)
   cible = grid2.getQuad1 (HEXA_ORB.DIR_X, dimz, mx, my)


   v1 = prems.getVertex (0)
   v2 = cible.getVertex (0)
   v3 = prems.getVertex (1)
   v4 = cible.getVertex (3)

   liste = []
   miroir = []

   liste.append(prems)
   
   for nx in range(dimx):
      if ( nx != mx ):
        liste.append(grid1.getQuad1 (HEXA_ORB.DIR_Z, nx, my, dimz))
   
   #for (int nx=0. nx<dimx. nx++)
       #if (nx!=mx) 
          #liste.append(grid1.getQuad (dir_z, nx, my, dimz)). 

   for ny in range(dimy):
      if ( ny != my ):
        liste.append(grid1.getQuad1 (HEXA_ORB.DIR_Z, mx, ny, dimz))
   #for (int ny=0. ny<dimy. ny++)
       #if (ny!=my) 
          #liste.push_back (grid1.getQuad (dir_z, mx, ny, dimz)). 

   joint = doc.joinQuads  (liste, cible, v1, v2, v3, v4, 5)

   joint.saveVtk ("test_joint.vtk")
   print "test_joint OK"

#// ======================================================== test_mini
def test_mini():  
   size_x = 2
   size_y = 2
   size_z = 2

   doc = hexablock.addDocument ()

   orig  = doc.addVertex (0,0,0)
   direc = doc.addVector (1,1,1)
   grid  = doc.makeCartesian1( orig, direc, size_x,size_y,size_z, 0, 0, 0 )

   orig2 = doc.addVertex (0,0,5)
   grid2 = doc.makeCartesian1( orig2, direc, size_x,size_y,size_z, 0, 0, 0 )

   #doc.saveVtk ("mini.vtk").
   grid2.saveVtk ("test_mini.vtk")
   print "test_mini OK"

#// ======================================================== test_clone
def test_clone():
   size_x = 2
   size_y = 2
   size_z = 2

   doc = hexablock.addDocument()

   orig = doc.addVertex (0,0,0)
   direc = doc.addVector (1,1,1)
   grid1 = doc.makeCartesian1(orig, direc, size_x,size_y,size_z, 0, 0, 0 )

   bond = doc.addVector (0, 0, 7)
   grid2 = doc.makeTranslation(grid1, bond)

   grid1.saveVtk ("test_clone.grid1.vtk")
   grid2.saveVtk ("test_clone.grid2.vtk")
   #doc .saveVtk ("clonage.vtk").
   #doc .dump().
   print "test_clone OK"

#// ======================================================== test_separ
def test_separ():
   size_x = 2
   size_y = 2
   size_z = 2

   doc = hexablock.addDocument()

   orig = doc.addVertex (0,0,0)
   direc = doc.addVector (1,1,1)
   grid = doc.makeCartesian1(orig, direc, size_x,size_y,size_z, 0, 0, 0 )

   grid.saveVtk ("test_separ.vtk")
   #doc .saveVtk ("separ.vtk").
   #doc .dump().
   print "test_separ OK"

#// ======================================================== test_cyl
def test_cyl():
   doc = hexablock.addDocument()

   orig = doc.addVertex (0,0,0)
   haut = doc.addVector (0,0,1)
   base = doc.addVector (1,0,0)

   dr = 1
   #// double da = 360. // degres
   dl = 1
   nr = 2
   na = 9
   nl = 5

   cyl1 = doc.makeCylindrical(orig,base,haut,dr, 360,  dl,nr,na,nl, False )
   cyl1.saveVtk ("test_cyl.full.vtk")

   cyl2 = doc.makeCylindrical(orig,haut,base,dr, 270, dl,nr,na,nl, False )
   cyl2.saveVtk ("test_cyl.34.vtk")

   #// doc.dump ()
   print "test_cyl OK"


#// ======================================================== test_propagation
def test_propagation():
   size_x = 2
   size_y = 1
   size_z = 1

   doc = hexablock.addDocument()

   orig  = doc.addVertex(0,0,0)
   direc = doc.addVector(1,1,1)
   grid  = doc.makeCartesian1(orig, direc, size_x,size_y,size_z, 0, 0, 0 )

   #doc.dump  ().
   nb = doc.countPropagation()

   for nro in range(nb):
       prop  = doc.getPropagation (nro)
       edges = prop.getEdges()
       #ways  = prop.getWays()
       
       for i, anEdge in enumerate(edges):
         if anEdge.getWay():
	   print "( ", anEdge.getVertex (0), ",", anEdge.getVertex (1), " )"
	 else:
	   print "( ", anEdge.getVertex (1), ",", anEdge.getVertex (0), " )"
           
   print "test_propagation OK"










#test_sphere()
#test_cartesi1()
#test_find()
#test_joint()
#test_mini()  
#test_clone()
#test_separ()
#test_cyl()
#test_propagation()

