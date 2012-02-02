# -*- coding: latin-1 -*-
# Hexa : Creation d'hexaedres 

import hexablock
import os
import geompy

#---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8


# ======================================================= bug_quad2
def bug_quad2 () :

    doc  = hexablock.addDocument ("default")

    ori  = doc.addVertex ( 0, 0, 0)
    vz   = doc.addVector ( 0, 0, 1)
    vx   = doc.addVector ( 1 ,0, 0)

    dr = 1;
    dl = 1;
    phi = 360;
    nr = 1;
    na = 8;
    nl = 1;

    c1 = doc.makeCylindrical (ori, vx,vz,dr, phi,dl, nr,na,nl, False);
    nvtk = 0;
    nom = "bugs_hexas";
    doc.saveVtk ("bugs_hexa_ab1.vtk")

    qa = c1.getQuadJK (0, 0, 0);
    qb = c1.getQuadJK (0, 4, 0);
    hexa = doc.addHexa2Quads (qa, qb);

 
    doc.setFile (nom);
    doc.saveFile ();
    doc.saveVtk ("bugs_hexa_ab2.vtk")
    return doc

# ======================================================= bug_quad3
def bug_quad3 () :

    doc  = hexablock.addDocument("quad3")

    ori  = doc.addVertex ( 0, 0, 0)
    vz   = doc.addVector ( 0, 0, 1)
    vx   = doc.addVector ( 1 ,0, 0)

    dr = 1;
    dl = 1;
    phi = 360;
    nr = 1;
    na = 8;
    nl = 1;

    c1 = doc.makeCylindrical (ori, vx,vz,dr, phi,dl, nr,na,nl, False);
    nvtk = 0;
    nom = "bugs_hexas";
    doc.saveVtk ("bugs_hexa_acd1.vtk")

    qc = c1.getQuadJK (0, 0, 0);
    qa = c1.getQuadJK (0, 1, 0);
    qd = c1.getQuadJK (0, 2, 0);

    hexa = doc.addHexa3Quads (qa, qc, qd);

 
    doc.setFile (nom);
    doc.saveFile ();
    doc.saveVtk ("bugs_hexa_acd2.vtk")
    return doc


# ======================================================= bug_quad3
def bug_quad1 () :

    doc  = hexablock.addDocument("quad1")
    centre     = doc.addVertex(0, 0, 0)
    vecteur_px = doc.addVector(1, 0, 0)
    vecteur_pz = doc.addVector(0, 0, 1)

    grille = doc.makeCylindrical(centre, vecteur_px, vecteur_pz,  1, 360, 1,  3, 8, 1,  False)

    quad_1 = grille.getQuadIK(0, 1, 0)
    quad_5 = grille.getQuadIK(0, 5, 0)

    quad_1 = grille.getQuadJK(0, 1, 0)
    quad_5 = grille.getQuadJK(0, 4, 0)
    quad_1.setScalar (5)

    doc.saveVtk ("bugs_hexa_ab1.vtk")
    doc.addHexa2Quads(quad_1, quad_5)
    doc.saveVtk ("bugs_hexa_ab2.vtk")
 
    return doc

# ================================================================= Begin

doc = bug_quad1  ()

law = doc.addLaw("Uniform", 4)

for j in range(doc.countPropagation()):
    propa = doc.getPropagation(j)
    propa.setLaw(law) 

mesh_hexas = hexablock.mesh(doc, "maillage:hexas")
