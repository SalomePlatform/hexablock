# -*- coding: latin-1 -*-
# Hexa : Creation d'hexaedres 

import hexablock
import os
import geompy

#---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8


# ======================================================= test_sphere
def test_sphere () :

    doc  = hexablock.addDocument()

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

    doc  = hexablock.addDocument()

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

    doc  = hexablock.addDocument()

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

    doc  = hexablock.addDocument()

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

mesh_hexas = hexablock.mesh("rind:hexas", doc)


