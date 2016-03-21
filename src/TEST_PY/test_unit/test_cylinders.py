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


# ======================================================= test_cylindricals
def test_cylindricals () :

    doc  = hexablock.addDocument ("cyl:hexas")

    orig = doc.addVertex (0, 0, 0)
    vz   = doc.addVector (0, 0, 1)
    vx   = doc.addVector (1 ,0, 0)


    tdr = [ 1, 1, 1, 1 ]
    tda = [ 45, 45,  45, 45,  45, 45,  45, 45 ]
    tdl = [ 1, 1, 1  ]

    tdr = [ 1, 2, 0.5 ]
    tda = [ 30, 30, 30, 10, 10, 10,  10, 10, 10,  10, 10, 10, 
            30, 30, 30, 10, 10, 10,  10, 10, 10,  10, 10, 10 ]
    tdl = [ 1, 2, 3  ]


    c1 = doc.makeCylindricals (orig, vx,vz, tdr, tda, tdl, False)

    doc.saveVtk ("cylindricals.vtk")
    return doc

# ======================================================= test_cylindrical
def test_cylindrical () :

    doc  = hexablock.addDocument ("cyl:hexas")

    orig = doc.addVertex (0, 0, 0)
    vz   = doc.addVector (0, 0, 1)
    vx   = doc.addVector (1 ,0, 0)

    nr = 3
    na = 8
    nl = 3
    c1 = doc.makeCylindrical (orig, vx,vz, 1.0, 360.0, 1.0, nr,na,nl, False)


    doc.saveVtk ("cylindricals.vtk")
    return doc


# ======================================================= test_cone
def test_cone () :

    doc  = hexablock.addDocument ("cyl:hexas")

    orig = doc.addVertex (0, 0, 0)
    ori2 = doc.addVertex (0, 0, 10)
    vz   = doc.addVector (0, 0, 1)
    vx   = doc.addVector (1 ,0, 0)

    nr = 2
    na = 8
    nl = 1
    c1 = doc.makeCylindrical (orig, vx,vz, 2.0, 360.0, 5.0, nr,na,nl, False)
    c2 = doc.makeCylindrical (ori2, vx,vz, 1.0, 360.0, 5.0, nr,na,nl, False)

    vh0 = c2.getVertexIJK (0, 0, 0)
    vh1 = c2.getVertexIJK (1, 0, 0)

    vb0 = c1.getVertexIJK (0, 0, nl)
    vb1 = c1.getVertexIJK (1, 0, nl)

    qcible = c2.getQuadIJ (0, 0, 0)
    qliste = []
    for ni in range (nr) :
        for nj in range (na) :
            quad = c1.getQuadIJ (ni, nj, nl)
            qliste.append (quad)
            quad .setColor (3);

    qcible.setColor (3);
    vb0.setColor (5);
    vh0.setColor (5);
    vb1.setColor (5);
    vh1.setColor (5);

    doc.joinQuads  (qliste, qcible, vb0, vh0, vb1, vh1, 1)

    doc.saveVtk ("cone.vtk")
    return doc

# ================================================================= Begin

####  doc = test_cylindricals  ()
####  doc = test_cylindrical  ()
doc = test_cone  ()

law = doc.addLaw("Uniform", 4)

for j in range(doc.countPropagation()):
    propa = doc.getPropagation(j)
    propa.setLaw(law) 

mesh_hexas = hexablock.mesh(doc)


