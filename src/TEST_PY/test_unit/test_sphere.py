# -*- coding: latin-1 -*-
# Hexa : Creation d'hexaedres 

import hexablock
import os
import geompy

#---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8


# ======================================================= test_sphere
def test_sphere () :

    doc  = hexablock.addDocument ("default")

    center   = doc.addVertex (0,0,0)
    radius   = 1
    ncouches = 1
    k        = 1

    doc.makeSpherical (center, radius, ncouches, k)
    doc.saveVtk ("Sphere.vtk")
    return doc

# ================================================================= Begin

doc = test_sphere  ()

law = doc.addLaw("Uniform", 4)

for j in range(doc.countPropagation()):
    propa = doc.getPropagation(j)
    propa.setLaw(law) 

mesh_hexas = hexablock.mesh("rind:hexas", doc)


