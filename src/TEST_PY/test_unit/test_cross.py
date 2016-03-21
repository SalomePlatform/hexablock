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


# ======================================================= test_pipes
def test_pipes () :

    doc  = hexablock.addDocument ("default")

    ori1 = doc.addVertex ( 0, 0, 0)
    ori2 = doc.addVertex (-5, 0, 5)
    vz   = doc.addVector ( 0, 0, 1)
    vx   = doc.addVector ( 1 ,0, 0)

    nr1 = 2
    ni1 = 1
    nl1 = 10
    nr2 = 1
    ni2 = 0.5
    nl2 = 10

    pipe1 = doc.addPipe   (ori1, vz, ni1, nr1, nl1)
    pipe2 = doc.addPipe   (ori2, vx, ni2, nr2, nl2)
    grid  = doc.makePipes (pipe1, pipe2)

    doc.save    ("pipes")
    doc.saveVtk ("pipes.vtk")
    return doc


# ======================================================= test_croix
def test_croix () :

    doc  = hexablock.addDocument ("default")

    ori1 = doc.addVertex ( 0, 0, 0)
    ori2 = doc.addVertex (-5, 0, 5)
    vz   = doc.addVector ( 0, 0, 1)
    vx   = doc.addVector ( 1 ,0, 0)

    nr1 = 2
    nl1 = 10
    nr2 = 1.8
    nl2 = 10

    cyl1 = doc.addCylinder (ori1, vz, nr1, nl1)
    cyl2 = doc.addCylinder (ori2, vx, nr2, nl2)
    grid = doc.makeCylinders (cyl1, cyl2)
    ###  grid = doc.makePipes (cyl1, cyl2)

    doc.saveVtk ("croix.vtk")
    return doc

# ================================================================= Begin

## doc = test_pipes  ()
doc = test_croix  ()
## doc.setLevel (747)

law = doc.addLaw("Uniform", 4)

for j in range(doc.countPropagation()):
    propa = doc.getPropagation(j)
    propa.setLaw(law) 

mesh_hexas = hexablock.mesh (doc, "maillage:hexas")

