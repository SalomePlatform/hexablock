# -*- coding: latin-1 -*-
# Hexa : Creation d'hexaedres 

import hexablock
import os
import geompy

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
