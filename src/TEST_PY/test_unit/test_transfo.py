# -*- coding: latin-1 -*-
# Hexa : Creation d'hexaedres 

import hexablock
import os
import geompy

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
    grid2  = doc.makeTranslation (grid, devant)

    doc .saveVtk ("transfo2.vtk")
    return doc

# ======================================================= test_scale
def test_scale () :

    doc  = hexablock.addDocument ("default")
    grid = make_grid (doc)

    dest   = doc.addVertex (15, 0, 0)
    grid2  = doc.makeScale (grid, dest, 0.5)

    doc .saveVtk ("transfo3.vtk")
    return doc

# ======================================================= test_sym_point
def test_sym_point () :

    doc  = hexablock.addDocument ("default")
    grid = make_grid (doc)

    orig  = doc.addVertex (5, 0, 0)
    grid2 = doc.makeSymmetryPoint (grid, orig)

    doc .saveVtk ("transfo4.vtk")
    return doc

# ======================================================= test_sym_line
def test_sym_line () :

    doc  = hexablock.addDocument ("default")
    grid = make_grid (doc)

    orig  = doc.addVertex (5, 0, 0)
    dir   = doc.addVector (0, 0, 1);
    grid2 = doc.makeSymmetryLine (grid, orig, dir)

    doc .saveVtk ("transfo5.vtk")
    return doc

# ======================================================= test_sym_plan
def test_sym_plan () :

    doc  = hexablock.addDocument ("default")
    grid = make_grid (doc)

    orig  = doc.addVertex (5, 0, 0)
    dir   = doc.addVector (1, 0, 0);
    grid2 = doc.makeSymmetryPlane (grid, orig, dir)

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

mesh_hexas = hexablock.mesh("maillage:hexas", doc)

