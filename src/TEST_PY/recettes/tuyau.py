# -*- coding: latin-1 -*-
# Copyright (C) 2009-2012  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
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

import geompy
import hexablock

# Charger la g�om�trie
# ====================

nom = "tuyau"

geometrie = geompy.ImportBREP(nom+".brep")

# S�lectionner des sous-parties de la g�om�trie
# ---------------------------------------------

aretes = geompy.SubShapeAllSortedCentres(geometrie, geompy.ShapeType["EDGE"])
faces  = geompy.SubShapeAllSortedCentres(geometrie, geompy.ShapeType["FACE"])

arc_int_bas  = aretes[12]
arc_int_haut = aretes[13]
arc_ext_bas  = aretes[11]
arc_ext_haut = aretes[14]

ell_int_bas  = aretes[2]
ell_int_haut = aretes[3]
ell_ext_bas  = aretes[1]
ell_ext_haut = aretes[4]

gen_int_avan = aretes[8]
gen_int_arri = aretes[7]
gen_ext_avan = aretes[9]
gen_ext_arri = aretes[6]

face_int_bas  = faces[4]
face_int_haut = faces[5]
face_ext_bas  = faces[2]
face_ext_haut = faces[3]

# Construire le mod�le de bloc
# ============================

doc = hexablock.addDocument(nom)

# D�finir le tuyau
# ----------------

base = doc.addVertex(0, 0, 0)

direction = doc.addVector(1, 0, 0)

tuyau = doc.addPipe(base, direction, 1, 2, 10)

# Construire le modele de blocs du tuyau
# --------------------------------------

depart = doc.addVector(0, 1, 0)

modele = doc.makePipe(tuyau, depart, 1, 4, 1)

# Associer le mod�le de bloc � la g�om�trie
# =========================================

doc.setShape(geometrie)

# Associer les cercles
# --------------------

cer_int_dep = modele.getVertexIJK(0, 0, 0)
cer_ext_dep = modele.getVertexIJK(1, 0, 0)

cer_int = []
cer_ext = []
for j in xrange(4):
    a = modele.getEdgeJ(0, j, 0)
    cer_int.append(a)

    a = modele.getEdgeJ(1, j, 0)
    cer_ext.append(a)

doc.associateClosedLine(cer_int_dep, cer_int[0], cer_int[1:], arc_int_haut, 1, True, [arc_int_bas])
doc.associateClosedLine(cer_ext_dep, cer_ext[0], cer_ext[1:], arc_ext_haut, 1, True, [arc_ext_bas])

# Associer les ellipses
# ---------------------

ell_int_dep = modele.getVertexIJK(0, 0, 1)
ell_ext_dep = modele.getVertexIJK(1, 0, 1)

ell_int = []
ell_ext = []
for j in xrange(4):
    a = modele.getEdgeJ(0, j, 1)
    ell_int.append(a)

    a = modele.getEdgeJ(1, j, 1)
    ell_ext.append(a)

doc.associateClosedLine(ell_int_dep, ell_int[0], ell_int[1:], ell_int_haut, 1, True, [ell_int_bas])
doc.associateClosedLine(ell_ext_dep, ell_ext[0], ell_ext[1:], ell_ext_haut, 1, True, [ell_ext_bas])

# Associer les g�n�ratrices
# -------------------------

def generatrice(face):
    n = 10
    l = []
    for i in xrange(0, n+1):
        v = float(i) / n
        s = geompy.MakeVertexOnSurface(face, 0.5, v)
        l.append(s)

    return geompy.MakeInterpol(l)

gen_int_haut = generatrice(face_int_haut)
gen_ext_haut = generatrice(face_ext_haut)
gen_int_bas  = generatrice(face_int_bas )
gen_ext_bas  = generatrice(face_ext_bas )

modele.getEdgeK(0, 0, 0).addAssociation(gen_int_arri, 0, 1)
modele.getEdgeK(1, 0, 0).addAssociation(gen_ext_arri, 0, 1)

modele.getEdgeK(0, 1, 0).addAssociation(gen_int_haut, 0, 1)
modele.getEdgeK(1, 1, 0).addAssociation(gen_ext_haut, 0, 1)

modele.getEdgeK(0, 2, 0).addAssociation(gen_int_avan, 0, 1)
modele.getEdgeK(1, 2, 0).addAssociation(gen_ext_avan, 0, 1)

modele.getEdgeK(0, 3, 0).addAssociation(gen_int_bas , 0, 1)
modele.getEdgeK(1, 3, 0).addAssociation(gen_ext_bas , 0, 1)

# Associer les faces courb�es
# ---------------------------

modele.getQuadJK(0, 0, 0).addAssociation(face_int_haut)
modele.getQuadJK(0, 1, 0).addAssociation(face_int_haut)
modele.getQuadJK(0, 2, 0).addAssociation(face_int_bas )
modele.getQuadJK(0, 3, 0).addAssociation(face_int_bas )

modele.getQuadJK(1, 0, 0).addAssociation(face_ext_haut)
modele.getQuadJK(1, 1, 0).addAssociation(face_ext_haut)
modele.getQuadJK(1, 2, 0).addAssociation(face_ext_bas )
modele.getQuadJK(1, 3, 0).addAssociation(face_ext_bas )

# Mailler le mod�le de bloc
# =========================

# D�finir 3 groupes d'ar�tes
# --------------------------

groupe_cercles      = doc.addEdgeGroup("Cercles")
groupe_ellipses     = doc.addEdgeGroup("Ellipses")
groupe_generatrices = doc.addEdgeGroup("Generatrices")

# D�finir 4 groupes de faces
# --------------------------

groupe_couronne  = doc.addQuadGroup("Couronne")
groupe_ovale     = doc.addQuadGroup("Ovale")
groupe_interieur = doc.addQuadGroup("Interieur")
groupe_exterieur = doc.addQuadGroup("Exterieur")

# Constituer les groupes d'ar�tes
# -------------------------------

for i in xrange(2):
    for j in xrange(4):
        arete = modele.getEdgeJ(i, j, 0)
        groupe_cercles.addElement(arete)

        arete = modele.getEdgeJ(i, j, 1)
        groupe_ellipses.addElement(arete)

        arete = modele.getEdgeK(i, j, 0)
        groupe_generatrices.addElement(arete)

# Constituer les groupes de faces
# -------------------------------

for j in xrange(4):
    quad = modele.getQuadIJ(0, j, 0)
    groupe_couronne.addElement(quad)

    quad = modele.getQuadIJ(0, j, 1)
    groupe_ovale.addElement(quad)

    quad = modele.getQuadJK(0, j, 0)
    groupe_interieur.addElement(quad)

    quad = modele.getQuadJK(1, j, 0)
    groupe_exterieur.addElement(quad)

# Mailler le mod�le de bloc avec ses associations
# -----------------------------------------------

hexablock.addLaws(doc, 0.7, True)

blocs = hexablock.mesh(doc)

muv, mue, muq, muh = hexablock.dump(doc, blocs)
