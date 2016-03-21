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


# ======================================================= test_croix
def test_croix () :

    nom  = "test_croix"
    doc  = hexablock.addDocument(nom)
    doc.setLevel (1)
    nvtk = 0

    ori = doc.addVertex ( 0, 0, 0)
    vz  = doc.addVector ( 0, 0, 1)
    vx  = doc.addVector ( 1 ,0, 0)

    dr    = 1.0
    angle = 360.0
    dl    = 1.0

    nr = 1
    na = 4
    nl = 5
    fill = True

    grid = doc.makeCylindrical (ori, vx, vz, dr, angle, dl, nr, na, nl, fill)

    doc.saveVtk ("carre.vtk")
    return doc

# ================================================================= Begin

doc = test_croix  ()
###  doc = test_pipes  ()

law = doc.addLaw("Uniform", 4)

for j in range(doc.countPropagation()):
    propa = doc.getPropagation(j)
    propa.setLaw(law) 

mesh_hexas = hexablock.mesh(doc)
muv, mue, muq, muh = hexablock.dump(doc, mesh_hexas)
