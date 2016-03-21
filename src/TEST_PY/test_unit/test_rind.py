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


# ======================================================= test_sphere
def test_sphere () :

    doc  = hexablock.addDocument ("default")

    center  = doc.addVertex (0,0,0)
    radius  = 8
    orig    = doc.addVertex (0,0,0)
    vz      = doc.addVector (0,0,1)
    vx      = doc.addVector (1,0,0)
    radhole = 1
    nrad    = 3
    nang    = 16
    nhaut   = 8

    doc.makeSphere (center, vx, vz, radius, radhole, orig, nrad, nang, nhaut)
    doc.saveVtk ("Rind1.vtk")
    return doc

# ======================================================= test_part_sphere
def test_part_sphere () :

    doc  = hexablock.addDocument ("default")

    center  = doc.addVertex (0,0,0)
    radius  = 8
    orig    = doc.addVertex (0,0,-4)
    vz      = doc.addVector (0,0,1)
    vx      = doc.addVector (1,0,0)
    angle   = 90
    radhole = 1
    nrad    = 3
    nang    = 4
    nhaut   = 8

    doc.makePartSphere (center, vx, vz, radius, radhole, orig, angle, 
                        nrad, nang, nhaut)
    doc.saveVtk ("Rind2.vtk")
    return doc

# ======================================================= test_rind
def test_rind () :

    doc  = hexablock.addDocument ("default")

    center  = doc.addVertex (0,0,0)
    radius  = 8
    radint  = 7
    orig    = doc.addVertex (0,0,0)
    vz      = doc.addVector (0,0,1)
    vx      = doc.addVector (1,0,0)
    radhole = 1
    nrad    = 3
    nang    = 16
    nhaut   = 8

    doc.makeRind (center, vx, vz, radius, radint, radhole, orig, 
                  nrad, nang, nhaut)
    doc.saveVtk ("Rind3.vtk")
    return doc

# ======================================================= test_part_rind
def test_part_rind () :

    doc  = hexablock.addDocument ("default")

    center  = doc.addVertex (0,0,0)
    radius  = 8
    radint  = 7
    orig    = doc.addVertex (0,0,0)
    vz      = doc.addVector (0,0,1)
    vx      = doc.addVector (1,0,0)
    angle   = 90
    radhole = 1
    nrad    = 3
    nang    = 14
    nhaut   = 8

    doc.makePartRind (center, vx, vz, radius, radint, radhole, orig, angle,
                      nrad, nang, nhaut)
    doc.saveVtk ("Rind4.vtk")
    return doc

# ================================================================= Begin

##test_sphere  ()
##test_part_sphere  ()
doc = test_part_rind  ()
##test_rind  ()

law = doc.addLaw("Uniform", 4)

for j in range(doc.countPropagation()):
    propa = doc.getPropagation(j)
    propa.setLaw(law) 

mesh_hexas = hexablock.mesh(doc, "rind:hexas")
