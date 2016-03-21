# -*- coding: latin-1 -*-
# Copyright (C) 2009-2016  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
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

# Hexa : Creation d'hexaedres 

import hexablock
import os

#---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8


# ======================================================= test_double_t
def test_double_t () :

    doc  = hexablock.addDocument ("default")

    ori1 = doc.addVertex ( 0, 0, 0)
    ori2 = doc.addVertex (-5, 0, 5)
    ori3 = doc.addVertex ( 0, 0, 12)
    ori4 = doc.addVertex (-5, 0, 17)

    vx   = doc.addVector ( 1 ,0, 0)
    vz   = doc.addVector ( 0, 0, 1)

    nl1 = 10
    nl2 = 10

    rsmall = 1
    rmoy   = 2
    rbig   = 3

    cyl1  = doc.addCylinder (ori1, vz, rbig, nl1)
    cyl2  = doc.addCylinder (ori2, vx, rmoy, nl2)

    cyl3  = doc.addCylinder (ori3, vz, rbig, nl1)
    cyl4  = doc.addCylinder (ori4, vx, rmoy, nl2)

    grid1 = doc.makeCylinders (cyl1, cyl2)
    grid2 = doc.makeCylinders (cyl4, cyl3)

    nx_int = 0
    nx_ext = 1
    cyl_big   = 1
    cyl_small = 0
    S_E     = 0
    S_NE    = 1
    S_N     = 2
    S_NW    = 3
    S_MAXI  = 8
                  ##    Cyl       i     j     k
    qb = grid1. getQuadIJ (cyl_big, nx_ext, S_E, 4)
    qh = grid2. getQuadIJ (cyl_big, nx_ext, S_E, 0) 

    vb0 = grid1.getVertexIJK (cyl_big, 2, S_E,  4)  ## cible
    vb1 = grid1.getVertexIJK (cyl_big, 2, S_NE, 4)
    vh0 = grid2.getVertexIJK (cyl_big, 2, S_E,  0)   ## depart
    vh1 = grid2.getVertexIJK (cyl_big, 2, S_NE, 0)

    hliste = []

    hliste.append (qh)
    for ny in range (S_MAXI) :
        if ny != S_E :
          hliste.append (grid2.getQuadIJ (cyl_big, nx_ext, ny, 0)) 

    for ny in range (4) :
        hliste.append (grid2.getQuadIJ (cyl_big, nx_int, ny, 0)) 

    hauteur = 3
    doc.joinQuads  (hliste, qb, vh0, vb0, vh1, vb1, hauteur)

    doc.saveVtk ("double_t.vtk")
    return doc

# ======================================================= test_lorraine
def test_lorraine () :

    doc  = hexablock.addDocument ("default")

    ori1 = doc.addVertex ( 0, 0, 0)
    ori2 = doc.addVertex (-5, 0, 5)
    ori3 = doc.addVertex ( 0, 0, 12)
    ori4 = doc.addVertex (-5, 0, 17)

    vx   = doc.addVector ( 1 ,0, 0)
    vz   = doc.addVector ( 0, 0, 1)

    nl1 = 10
    nl2 = 10

    rsmall = 1
    rmoy   = 3
    rbig   = 4

    cyl1  = doc.addCylinder (ori1, vz, rmoy,   nl1)
    cyl2  = doc.addCylinder (ori2, vx, rsmall, nl2)

    cyl3  = doc.addCylinder (ori3, vz, rmoy, nl1)
    cyl4  = doc.addCylinder (ori4, vx, rbig, nl2)

    grid1 = doc.makeCylinders (cyl1, cyl2)
    grid2 = doc.makeCylinders (cyl4, cyl3)

    nx_int = 0
    nx_ext = 1
    cyl_big   = 1
    cyl_small = 0
    S_E     = 0
    S_NE    = 1
    S_N     = 2
    S_NW    = 3
    S_MAXI  = 8
                  ##    Cyl       i     j     k
    qb = grid1. getQuadIJ (cyl_big,   nx_ext, S_E, 4)
    qh = grid2. getQuadIJ (cyl_small, nx_ext, S_N, 0) 

    vb0 = grid1.getVertexIJK (cyl_big,   2, S_E,  4)  ## cible
    vb1 = grid1.getVertexIJK (cyl_big,   2, S_NE, 4)
    vh0 = grid2.getVertexIJK (cyl_small, 2, S_N,  0)   ## depart
    vh1 = grid2.getVertexIJK (cyl_small, 2, S_NW, 0)

    hliste = []

    hliste.append (qh)
    for ny in range (S_MAXI) :
        if ny != S_N :
          hliste.append (grid2.getQuadIJ (cyl_small, nx_ext, ny, 0)) 

    for ny in range (4) :
        hliste.append (grid2.getQuadIJ (cyl_small, nx_int, ny, 0)) 

    hauteur = 3
    doc.joinQuads  (hliste, qb, vh0, vb0, vh1, vb1, hauteur)

    doc.saveVtk ("lorraine.vtk")
    return doc

# ================================================================= Begin

doc = test_double_t  ()

law = doc.addLaw("Uniform", 4)

for j in range(doc.countPropagation()):
    propa = doc.getPropagation(j)
    propa.setLaw(law) 

mesh_hexas = hexablock.mesh(doc, "maillage:hexas")
