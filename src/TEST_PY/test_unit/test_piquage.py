# -*- coding: latin-1 -*-
# Copyright (C) 2009-2023  CEA/DEN, EDF R&D
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

#### import os
import hexablock
geompy = hexablock.geompy

#---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8

use_paraview = False

# ======================================================= test_pipes
def test_piquage () :

    name = "piquage"
    doc  = hexablock.addDocument (name)

    orig = doc.addVertex ( 8, 0, 0)
    vx   = doc.addVector ( 1 ,0, 0)
    vy   = doc.addVector ( 0, 1, 0)
    vz   = doc.addVector ( 0, 0, 1)

    size_x   = 5
    size_y   = 5
    size_z   = 3
    size_cyl = 12

    rint     = 2
    rext     = 3
    angle    = 360
    haut     = 1
    nr       = 1
    nh       = 1

    grid = doc.makeCartesianTop (size_x, size_y, size_z)
    pipe = doc.makePipeUni  (orig, vx, vz, rint, rext, angle,
                             haut, nr, size_cyl, nh) 

    c1 = grid.getVertexIJK (2, 1, size_z)
    c2 = grid.getVertexIJK (3, 1, size_z)

    p1 = pipe.getVertexIJK (1, 7, 1)
    p2 = pipe.getVertexIJK (1, 8, 1)

    qpattern = []
    qtarget  = []
    for na in range (size_cyl) :
       quad = pipe.getQuadIJ (0, na, 1)
       quad.setColor (2)
       qpattern.append (quad)

    for ni in range (1, size_x-1) :
        for nj in range (1, size_y-1) :
            quad = grid.getQuadIJ (ni, nj, size_z)
            quad.setColor (2)
            qtarget.append (quad)

    c1.setColor (6)
    c2.setColor (4)

    p1.setColor (6)
    p2.setColor (4)
    if use_paraview :
       doc.saveVtk ("replace0.vtk")

    doc.replace (qpattern, qtarget, p1,c1, p2,c2)
    if use_paraview :
       doc.saveVtk ("replace1.vtk")

    pipe.remove()
    if use_paraview :
       doc.saveVtk ("replace2.vtk")
    return doc

# ================================================================= Begin

doc = test_piquage  ()

doc.addLaws (0.9, True)

mesh_hexas = hexablock.mesh (doc, "maillage:hexas")
