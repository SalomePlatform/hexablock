# -*- coding: latin-1 -*-
#  Copyright (C) 2009-2011  CEA/DEN, EDF R&D
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

import hexablock


# ======================================================== test_sphere
def test_sphere ():
  doc = hexablock.addDocument ("default")
  orig = doc.addVertex (0,0,0)

  ncouches = 1
  k = 0.8
  decal  = doc.addVector (1,1,1)
  sphere = doc.makeSpherical (orig, decal, ncouches, k)

  for nc in range(ncouches):
    cell = sphere.getStrate (nc, Q_A)
    cell.remove ()

  sphere.saveVtk ("sphere.vtk")
  print "test_sphere OK"

# ======================================================== test_cartesi1
def test_cartesi1 ():
  size_x = 15
  size_y = 12
  size_z = 8

  doc = hexablock.addDocument ()
  orig = doc.addVertex (0,0,0)

  dirVr = doc.addVector (1,1,1)
  grid  = doc.makeCartesian1 (orig, dirVr, size_x,size_y,size_z, 0, 0, 0 ) #CS_NOT_SPEC (makeCartesian1)

   #for (int nz=0 nz<size_z  nz++)
       #for (int ny=nz+1 ny<size_y-nz-1  ny++)
           #for (int nx=nz+1 nx<size_x-nz-1  nx++)
               
               ##  grid.getHexa (nx, ny, nz).remove ()
               #Hexa* cell = grid.getHexa (nx, ny, nz)
               #cell.remove ()

  grid.saveVtk ("hexa1.vtk")
  print "test_cartesi1 OK"
  # doc.dump ()

## ======================================================== afficher
##define Afficher(elt) afficher (#elt, elt)
#def afficher (cpchar nom, EltBase* elt):

   #if (elt==NULL)
      
      #printf (" .... %s = 0x0\n", nom)
      #return HOK
      

   #printf (" .... %s = 0x%08lx = %03d\n", nom, (unsigned long) elt, elt.getId())
   #return HOK

# ======================================================== test_find
def test_find ():
   size_x = 2
   size_y = 2
   size_z = 2

   doc = hexablock.addDocument ()

   orig = doc.addVertex (0,0,0)
   dirVr  = doc.addVector (1,1,1)
   grid = doc.makeCartesian1 (orig, dirVr, size_x,size_y,size_z, 0, 0, 0 )

   grid.saveVtk ("mini1.vtk")
   #doc.dump()

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
   assert not doc.findQuad (v02, v06)


   assert doc.findHexa (v00, v06)
   assert doc.findHexa (v06, v26)
   assert doc.findHexa (v26, v06)
   print "test_find OK"

# ======================================================== test_joint
def test_joint ():
  dimx = 11
  dimy = 11
  dimz = 2
  doc = hexablock.addDocument ()

  orig1 = doc.addVertex (0,0,0)
  dirVr = doc.addVector (1,1,1)

  grid1 = doc.makeCartesian1(orig1, dirVr, dimx,dimy,dimz , 0, 0, 0 )

  orig2 = doc.addVertex  (dimx/2.0,0,8)
  vectj = doc.addVector (0,1,0)
  vecti = doc.addVector (1,0,0)
  grid2 = doc.makeCylindrical (orig2, vecti, vectj, 1, 180, 1,dimz,dimy,dimx, False)
  
  mx = dimx/2
  my = dimy/2
  prems = grid1.getQuad1 (hexablock.DIR_Z, mx, my, dimz)
  cible = grid2.getQuad1 (hexablock.DIR_X, dimz, mx, my)

  v1 = prems.getVertex (0)
  v2 = cible.getVertex (0)
  v3 = prems.getVertex (1)
  v4 = cible.getVertex (3)

  liste  = []
  miroir = []

  liste += [prems]
  for nx in range (dimx):
    if ( nx != mx ):
      liste += [grid1.getQuad1 (hexablock.DIR_Z, nx, my, dimz)]

  for ny in range (dimy):
    if ( ny != my ):
      liste += [grid1.getQuad1 (hexablock.DIR_Z, mx, ny, dimz)]

   # Elements* joint = 
   #
  doc.joinQuads(liste, cible, v1, v2, v3, v4, 5)
  doc.saveVtk ("joint.vtk")
  print "test_joint OK"

# ======================================================== test_hexa1
def test_hexa1 ():
   size_x = 1
   size_y = 1
   size_z = 1

   doc = hexablock.addDocument ()
   orig  = doc.addVertex (0,0,0)
   dirVr = doc.addVector (1,1,1)
   #  Elements*  grid = 
   doc.makeCartesian1 (orig, dirVr, size_x,size_y,size_z, 0, 0, 0 )

   #doc.dump ()
   print "test_hexa1 OK"

# ======================================================== test_decoupage
def test_decoupage ():
  size_x = 2
  size_y = 1
  size_z = 1

  doc = hexablock.addDocument ()

  orig = doc.addVertex (0,0,0)
  dirVr = doc.addVector (1,1,1)

  grid  = doc.makeCartesian1 (orig, dirVr, size_x,size_y,size_z, 0, 0, 0 )
  #arete = grid.getEdge ( HEXA_ORB.hexablock.DIR_Z, 0, 0, 0)
  arete = grid.getEdge1( hexablock.DIR_Z, 0, 0, 0 )

  #doc.dump()
  doc.saveVtk ("no_decoupe.vtk")
  doc.cut (arete, 1)

  #doc .dump ()
  doc .saveVtk ("decoupe.vtk")
  # doc .saveFile ()
  print "test_decoupage OK"

# ======================================================== test_gen_xml
def test_gen_xml ():

  size_x = 2
  size_y = 2
  size_z = 2

  doc = hexablock.addDocument ()

  orig  = doc.addVertex (0,0,0)
  dirVr = doc.addVector (1,1,1)
  grid  = doc.makeCartesian1 (orig, dirVr, size_x,size_y,size_z, 0, 0, 0 )

  cell    = grid.getHexa (0)
  face    = cell.getQuad (0)
  arete   = cell.getEdge (0)
  noeud   = cell.getVertex (0)

  #CS_TODO?
  #shape1 = Shape()
  #shape2 = Shape()
  #shape3 = Shape()

  #noeud.setAssociation (shape1)
  #arete.setAssociation (shape2)
  #face .setAssociation (shape3)

  law1 = doc.addLaw("loi1", 1)
  law2 = doc.addLaw("loi2", 2)
  law3 = doc.addLaw("loi3", 3)

  law1.setKind (hexablock.UNIFORM)
  law2.setKind (hexablock.ARITHMETIC)
  law3.setKind (hexablock.GEOMETRIC)

  prop1 = doc.getPropagation (0)
  prop2 = doc.getPropagation (1)
  prop3 = doc.getPropagation (2)

  prop1.setLaw (law1)
  prop2.setLaw (law2)
  prop3.setLaw (law3)

  prop1.setWay (True)
  prop2.setWay (False)
  prop3.setWay (True)

  doc .saveVtk ("mini.vtk")
  doc .setFile ("Essai")
  doc .saveFile ()
  print "test_gen_xml OK"



# ======================================================== test_relecture
def test_relecture ():
  doc = hexablock.loadDocument ("Essai")

  #doc .dump ()
  doc .saveVtk ("restore.vtk")
  print "test_relecture OK"

# ======================================================== test_clone
def test_clone ():
  size_x = 2
  size_y = 2
  size_z = 2
  doc = hexablock.addDocument ()

  orig = doc.addVertex (0,0,0)
  dirVr = doc.addVector (1,1,1)
  grid  = doc.makeCartesian1 (orig, dirVr, size_x,size_y,size_z, 0, 0, 0 )

  bond  = doc.addVector (0, 0, 7)
  #  Elements* grid2 = doc.makeTranslation (grid, bond)
  doc.makeTranslation (grid, bond)

  doc.saveVtk ("clonage.vtk")
  #doc.dump()
  print "test_clone OK"

# ======================================================== test_separ
def test_separ ():
  size_x = 2
  size_y = 2
  size_z = 2

  doc = hexablock.addDocument ()

  orig  = doc.addVertex (0,0,0)
  dirVr = doc.addVector (1,1,1)

  doc.makeCartesian1 (orig, dirVr, size_x,size_y,size_z, 0, 0, 0 )

  doc .saveVtk ("separ.vtk")
  #doc .dump()
  print "test_separ OK"


# ======================================================== test_cyl
def test_cyl ():
  doc = hexablock.addDocument ()

  orig = doc.addVertex (0,0,0)
  haut = doc.addVector (0,0,1)
  base = doc.addVector (1,0,0)

  dr = 1
  # double da = 360 # degres
  dl = 1
  nr = 2
  na = 9
  nl = 5

  cyl1 = doc.makeCylindrical(orig,base,haut,dr, 360,dl,nr,na,nl)
  cyl1.saveVtk ("cylindre_full.vtk")

  cyl2 = doc.makeCylindrical(orig,haut,base,dr, 270,dl,nr,na,nl)
  cyl2.saveVtk ("cylindre_34.vtk")

  # doc.dump ()
  print "test_cyl OK"

# ======================================================== test_propagation
def test_propagation ():
  size_x = 2
  size_y = 1
  size_z = 1

  doc = hexablock.addDocument ()

  orig = doc.addVertex (0,0,0)
  dirVr = doc.addVector (1,1,1)

  doc.makeCartesian1 (orig, dirVr, size_x,size_y,size_z, 0, 0, 0 )

  nb = doc.countPropagation ()

  for nro in range(nb):
    prop  = doc .getPropagation (nro)
    edges = prop.getEdges ()
    print " ____________________________________ Prop nro %s\n"%nro
    for e in edges:
      way = e.getWay ()
      if (way):
        print "     (%s,%s)\n"%( e.getVertex(0).name, e.getVertex(1).name )
      else:
        print "     (%s,%s)\n"%( e.getVertex(1).name, e.getVertex(0).name )

  #doc.dump  ()
  doc.saveVtk ("test_propagation.vtk")
  doc.setFile ("test_propagation")
  doc.saveFile ()
  print "test_propagation OK"


# ======================================================== test_move
def test_move ():
  size_x = 1
  size_y = 1
  size_z = 2

  doc = hexablock.addDocument ()

  orig  = doc.addVertex (0,0,0)
  dirVr = doc.addVector (1,1,1)
  grid  = doc.makeCartesian1 (orig, dirVr, size_x,size_y,size_z, 0, 0, 0 )

  enhaut = doc.addVector (0, 0, 5)
  devant = doc.addVector (5, 0, 0)
  agauche = doc.addVector (0, 5, 0)

  #CS_TODO?
  #matrice = Matrix() 
  #matrice.defTranslation (enhaut)

  cube    = grid.getHexa (1)
  dessous = cube.getQuad (0)#(Q_A) #CS_TODO?
  #dessous.dump()

  grid2 = doc.makeTranslation (grid, devant)
  doc.makeTranslation (grid, agauche)
  cube2 = grid2.getHexa (1)

  cube .disconnectQuad (dessous)
  
  #CS_TODO?
  #cube .transform( matrice )
  #cube2.transform( matrice )


  doc .saveVtk ("move.vtk")
  #doc .dump()
  print "test_move OK, CS_TODO?"




#test_sphere () KO
test_cartesi1 ()
test_find ()
test_joint ()
test_hexa1 ()
test_decoupage ()
test_gen_xml ()
#test_relecture () KO
#test_clone ()
#test_separ ()
#test_cyl ()
#test_propagation ()
#test_move ()

