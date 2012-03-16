# -*- coding: latin-1 -*-
# Hexa : Creation d'hexaedres 

import hexablock
import os
import geompy

#---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8

# ======================================================= test_disco_edges
def test_disco_edges () :

    nom  = "test_disco_edges"
    doc  = hexablock.addDocument(nom)
    doc.setLevel (0)

    ori = doc.addVertex ( 0, 0, 0)
    vz  = doc.addVector ( 0, 0, 1)
    vy  = doc.addVector ( 0, 1, 1)
    vx  = doc.addVector ( 1 ,0, 0)

    size_x = 2
    size_y = 2
    size_z = 5

    grid = doc.makeCartesian (ori, vx, vy, vz, size_x, size_y, size_z)

    t_hexas = []
    t_edges = []
    
    for nk in range (size_z) :
        t_hexas.append (grid.getHexaIJK (1,1,nk))
        t_edges.append (grid.getEdgeK   (1,2,nk))

    disco_edges =  doc.disconnectEdges (t_hexas, t_edges)
    return doc

# ================================================================= Begin

doc = test_disco_edges  ()

law = doc.addLaw("Uniform", 4)

for j in range(doc.countPropagation()):
    propa = doc.getPropagation(j)
    propa.setLaw(law) 

mesh_hexas = hexablock.mesh(doc)
muv, mue, muq, muh = hexablock.dump(doc, mesh_hexas)
