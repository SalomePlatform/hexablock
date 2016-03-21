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

# Francis KLOSS : 2012 : CEA-Saclay, DEN, DM2S, SFME, LGLS, F-91191 Gif-sur-Yvette, France
# ========================================================================================

import hexablock
### import geompy
geompy = hexablock.geompy

# Charger la geometrie
# ====================

nom = "tuyau"

geometrie = geompy.ImportBREP(nom+".brep")

# Selectionner des sous-parties de la geometrie
# ---------------------------------------------

aretes  = geompy.SubShapeAllSortedCentres    (geometrie, geompy.ShapeType["EDGE"])
edge_id = geompy.SubShapeAllSortedCentresIDs (geometrie, geompy.ShapeType["EDGE"])
faces   = geompy.SubShapeAllSortedCentres    (geometrie, geompy.ShapeType["FACE"])
face_id = geompy.SubShapeAllSortedCentresIDs (geometrie, geompy.ShapeType["FACE"])

subid_arc_int_bas  = 30
subid_arc_int_haut = 34
subid_arc_ext_bas  = 12
subid_arc_ext_haut = 16

arc_int_bas  = aretes[12]
arc_int_haut = aretes[13]
arc_ext_bas  = aretes[11]
arc_ext_haut = aretes[14]

subid_ell_int_bas  = 21
subid_ell_int_haut = 33
subid_ell_ext_bas  =  8
subid_ell_ext_haut = 15

ell_int_bas  = aretes[2]
ell_int_haut = aretes[3]
ell_ext_bas  = aretes[1]
ell_ext_haut = aretes[4]

subid_gen_int_avan = 26
subid_gen_int_arri = 28
subid_gen_ext_avan =  5
subid_gen_ext_arri = 10

gen_int_avan = aretes[8]
gen_int_arri = aretes[7]
gen_ext_avan = aretes[9]
gen_ext_arri = aretes[6]

subid_face_int_bas  = 24
subid_face_int_haut = 31
subid_face_ext_bas  =  3
subid_face_ext_haut = 13

face_int_bas  = faces[4]
face_int_haut = faces[5]
face_ext_bas  = faces[2]
face_ext_haut = faces[3]

# Construire le modele de bloc
# ============================

doc = hexablock.addDocument(nom)

# Definir le tuyau
# ----------------

origin = doc.addVertex(0, 0, 0)

direction = doc.addVector(1, 0, 0)

# Construire le modele de blocs du tuyau
# --------------------------------------

base = doc.addVector(0, 1, 0)

###  tuyau = doc.addPipe(origin, direction, 1, 2, 10)
###  modele = doc.makePipe(tuyau, base, 1, 4, 1)

modele = doc.makePipeUni (origin, base, direction, 1.0, 2.0, 360.0, 10.0, 1, 4, 1)

# Associer le modele de bloc a la geometrie
# =========================================

sh_tuyau = hexablock.addShape (doc, geometrie,nom)
doc.clearAssociation()

# Associer les cercles
# --------------------

cer_int_dep = modele.getVertexIJK(0, 0, 1)
cer_ext_dep = modele.getVertexIJK(1, 0, 1)

cer_int = []
cer_ext = []
for j in xrange(4):
    a = modele.getEdgeJ(0, j, 1)
    cer_int.append(a)

    a = modele.getEdgeJ(1, j, 1)
    cer_ext.append(a)

doc.associateClosedLine(cer_int_dep, cer_int, [sh_tuyau], [34,30], 1, True)
doc.associateClosedLine(cer_ext_dep, cer_ext, [sh_tuyau], [16,12], 1, True)

# Associer les ellipses
# ---------------------

ell_int_dep = modele.getVertexIJK(0, 0, 0)
ell_ext_dep = modele.getVertexIJK(1, 0, 0)

ell_int = []
ell_ext = []
for j in xrange(4):
    a = modele.getEdgeJ(0, j, 0)
    ell_int.append(a)

    a = modele.getEdgeJ(1, j, 0)
    ell_ext.append(a)

doc.associateClosedLine(ell_int_dep, ell_int, [sh_tuyau], [33,21], 1, True)
doc.associateClosedLine(ell_ext_dep, ell_ext, [sh_tuyau], [15, 8], 1, True)

# Associer les generatrices
# -------------------------

def generatrice(face):
    n = 10
    l = []
    for i in xrange(0, n+1):
        v = float(i) / n
        s = geompy.MakeVertexOnSurface(face, 0.5, v)
        l.append(s)

    return geompy.MakeInterpol(l)

gen = []
gen_int_haut = generatrice(face_int_haut)
gen_ext_haut = generatrice(face_ext_haut)
gen_int_bas  = generatrice(face_int_bas )
gen_ext_bas  = generatrice(face_ext_bas )
gen.append(gen_int_haut)
gen.append(gen_ext_haut)
gen.append(gen_int_bas)
gen.append(gen_ext_bas)

generatrices = geompy.MakeCompound(gen)
sh_gen = hexablock.addShape (doc, generatrices, "generatrices")

modele.getEdgeK(0, 0, 0).addAssociation(sh_tuyau, 28, 0, 1)
modele.getEdgeK(1, 0, 0).addAssociation(sh_tuyau, 10, 0, 1)

modele.getEdgeK(0, 1, 0).addAssociation(sh_gen  ,  2, 0, 1)
modele.getEdgeK(1, 1, 0).addAssociation(sh_gen  ,  5, 0, 1)

modele.getEdgeK(0, 2, 0).addAssociation(sh_tuyau, 26, 0, 1)
modele.getEdgeK(1, 2, 0).addAssociation(sh_tuyau,  5, 0, 1)

modele.getEdgeK(0, 3, 0).addAssociation(sh_gen  ,  8, 0, 1)
modele.getEdgeK(1, 3, 0).addAssociation(sh_gen  , 11, 0, 1)

# Associer les faces courbees
# ---------------------------

modele.getQuadJK(0, 0, 0).addAssociation(sh_tuyau, 31)
modele.getQuadJK(0, 1, 0).addAssociation(sh_tuyau, 31)
modele.getQuadJK(0, 2, 0).addAssociation(sh_tuyau, 24)
modele.getQuadJK(0, 3, 0).addAssociation(sh_tuyau, 24)

modele.getQuadJK(1, 0, 0).addAssociation(sh_tuyau, 13)
modele.getQuadJK(1, 1, 0).addAssociation(sh_tuyau, 13)
modele.getQuadJK(1, 2, 0).addAssociation(sh_tuyau,  3)
modele.getQuadJK(1, 3, 0).addAssociation(sh_tuyau,  3)

# Mailler le modele de bloc
# =========================

# Definir 3 groupes d'aretes
# --------------------------

groupe_cercles      = doc.addEdgeGroup("Cercles")
groupe_ellipses     = doc.addEdgeGroup("Ellipses")
groupe_generatrices = doc.addEdgeGroup("Generatrices")

# Definir 4 groupes de faces
# --------------------------

groupe_couronne  = doc.addQuadGroup("Couronne")
groupe_ovale     = doc.addQuadGroup("Ovale")
groupe_interieur = doc.addQuadGroup("Interieur")
groupe_exterieur = doc.addQuadGroup("Exterieur")

# Constituer les groupes d'aretes
# -------------------------------

for i in xrange(2):
    for j in xrange(4):
        arete = modele.getEdgeJ(i, j, 1)
        groupe_cercles.addElement(arete)

        arete = modele.getEdgeJ(i, j, 0)
        groupe_ellipses.addElement(arete)

        arete = modele.getEdgeK(i, j, 0)
        groupe_generatrices.addElement(arete)

# Constituer les groupes de faces
# -------------------------------

for j in xrange(4):
    quad = modele.getQuadIJ(0, j, 1)
    groupe_couronne.addElement(quad)

    quad = modele.getQuadIJ(0, j, 0)
    groupe_ovale.addElement(quad)

    quad = modele.getQuadJK(0, j, 0)
    groupe_interieur.addElement(quad)

    quad = modele.getQuadJK(1, j, 0)
    groupe_exterieur.addElement(quad)

# Mailler le modele de bloc avec ses associations
# -----------------------------------------------

##########  hexablock.addLaws(doc, 0.7, True) Ne marche pas

law = doc.addLaw("Uniform", 4)
for j in range(doc.countPropagation()):
    propa = doc.getPropagation(j)
    propa.setLaw(law)

doc.save("tuyau_v5")
doc.setName("tuyau_v5")
hexablock.addToStudy(doc)

blocs = hexablock.mesh(doc)

muv, mue, muq, muh = hexablock.dump(doc, blocs)
