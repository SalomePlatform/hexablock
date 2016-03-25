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


# ======================================================= make_grid
def make_grid (doc) :

    ori  = doc.addVertex ( 0, 0, 0)
    vz   = doc.addVector ( 0, 0, 1)
    vx   = doc.addVector ( 1 ,0, 0)

    dr = 1
    da = 360
    dl = 1

    nr = 1
    na = 6
    nl = 1
    grid = doc.makeCylindrical (ori, vx,vz, dr,da,dl, nr,na,nl, False)
    doc .saveVtk ("transfo1.vtk")
    return grid

# ======================================================= test_translation
def test_translation () :

    doc  = hexablock.addDocument ("default")
    grid = make_grid (doc)

    devant = doc.addVector (10, 0, 0)
    grid2  = doc.performTranslation (grid, devant)

    doc .saveVtk ("transfo2.vtk")
    return doc

# ======================================================= test_scale
def test_scale () :

    doc  = hexablock.addDocument ("default")
    grid = make_grid (doc)

    dest   = doc.addVertex (15, 0, 0)
    grid2  = doc.performScale (grid, dest, 0.5)

    doc .saveVtk ("transfo3.vtk")
    return doc

# ======================================================= test_sym_point
def test_sym_point () :

    doc  = hexablock.addDocument ("default")
    grid = make_grid (doc)

    orig  = doc.addVertex (5, 0, 0)
    ier   = doc.performSymmetryPoint (grid, orig)

    doc .saveVtk ("transfo4.vtk")
    return doc

# ======================================================= test_sym_line
def test_sym_line () :

    doc  = hexablock.addDocument ("default")
    grid = make_grid (doc)

    orig  = doc.addVertex (5, 0, 0)
    dir   = doc.addVector (0, 0, 1);
    ier   = doc.performSymmetryLine (grid, orig, dir)

    doc .saveVtk ("transfo5.vtk")
    return doc

# ======================================================= test_sym_plan
def test_sym_plan () :

    doc  = hexablock.addDocument ("default")
    grid = make_grid (doc)

    orig  = doc.addVertex (5, 0, 0)
    dir   = doc.addVector (1, 0, 0);
    ier   = doc.performSymmetryPlane (grid, orig, dir)

    doc .saveVtk ("transfo6.vtk")
    return doc

# ================================================================= Begin

### doc = test_translation  ()
doc = test_scale  ()
### doc = test_sym_point  ()
### doc = test_sym_line  ()
### doc = test_sym_plan  ()

law = doc.addLaw("Uniform", 4)

for j in range(doc.countPropagation()):
    propa = doc.getPropagation(j)
    propa.setLaw(law) 

mesh_hexas = hexablock.mesh(doc, "maillage:hexas")

