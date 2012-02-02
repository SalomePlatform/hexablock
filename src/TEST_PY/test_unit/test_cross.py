# -*- coding: latin-1 -*-
# Hexa : Creation d'hexaedres 

import hexablock
import os
import geompy

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
    nr2 = 1
    nl2 = 10

    cyl1 = doc.addCylinder (ori1, vz, nr1, nl1)
    cyl2 = doc.addCylinder (ori2, vx, nr2, nl2)
    grid = doc.makeCylinders (cyl1, cyl2)
    ###  grid = doc.makePipes (cyl1, cyl2)

    doc.saveVtk ("croix.vtk")
    return doc

# ================================================================= Begin

doc = test_pipes  ()

law = doc.addLaw("Uniform", 4)

for j in range(doc.countPropagation()):
    propa = doc.getPropagation(j)
    propa.setLaw(law) 

mesh_hexas = hexablock.mesh(doc, "maillage:hexas")

