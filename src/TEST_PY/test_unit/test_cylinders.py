# -*- coding: latin-1 -*-
# Copyright (C) 2009-2012  CEA/DEN, EDF R&D
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

# Hexa : Creation d'hexaedres 

import hexablock
import os
import geompy

#---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8


# ======================================================= test_cylindricals
def test_cylindricals () :

    doc  = hexablock.addDocument ("rind:hexas")

    orig = doc.addVertex (0, 0, 0)
    vz   = doc.addVector (0, 0, 1)
    vx   = doc.addVector (1 ,0, 0)


    tdr = [ 1, 2, 0.5 ]
    tda = [ 30, 30, 30, 10, 10, 10,  10, 10, 10,  10, 10, 10, 
            30, 30, 30, 10, 10, 10,  10, 10, 10,  10, 10, 10 ]
    tdl = [ 1, 2, 3  ]

    tdr = [ 1, 1, 1, 1 ]
    tda = [ 45, 45,  45, 45,  45, 45,  45, 45 ]
    tdl = [ 1, 1, 1  ]


    c1 = doc.makeCylindricals (orig, vx,vz, tdr, tda, tdl, False)
    ## c1 = doc.makeCylindrical (orig, vx,vz, 1.0, 360.0, 1.0, 3, 8, 3, False)


    doc.saveVtk ("cylindricals.vtk")
    return doc

# ================================================================= Begin

doc = test_cylindricals  ()

law = doc.addLaw("Uniform", 4)

for j in range(doc.countPropagation()):
    propa = doc.getPropagation(j)
    propa.setLaw(law) 

mesh_hexas = hexablock.mesh(doc)


