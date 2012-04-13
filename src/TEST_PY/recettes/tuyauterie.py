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

import math

import geompy
import hexablock

# Charger la géométrie
# ====================

nom = "tuyauterie"

geometrie = geompy.ImportBREP(nom+".brep")

# Sélectionner des sous-parties de la géométrie
# ---------------------------------------------

sommets = geompy.SubShapeAllSortedCentres(geometrie, geompy.ShapeType["VERTEX"])
aretes  = geompy.SubShapeAllSortedCentres(geometrie, geompy.ShapeType["EDGE"  ])
faces   = geompy.SubShapeAllSortedCentres(geometrie, geompy.ShapeType["FACE"  ])

petit_arri_v = sommets[0]
petit_arri_x, petit_arri_y, petit_arri_z = geompy.PointCoordinates(petit_arri_v)
petit_avan_x, petit_avan_y, petit_avan_z = geompy.PointCoordinates(sommets[1])

moyen_arri_v = sommets[3]
moyen_arri_x, moyen_arri_y, moyen_arri_z = geompy.PointCoordinates(moyen_arri_v)
moyen_avan_x, moyen_avan_y, moyen_avan_z = geompy.PointCoordinates(sommets[4])

petit0, petit_x, petit_y, petit_z,  petit1, petit2, petit3,  petit_rayon = geompy.KindOfShape(aretes[0])

moyen0, moyen_x, moyen_y, moyen_z,  moyen1, moyen2, moyen3,  moyen_rayon = geompy.KindOfShape(aretes[4])

grand0, grand_xb, grand_yb, grand_zb,  grand1, grand2, grand3,  grand_rayon = geompy.KindOfShape(aretes[7])
grand4, grand_xh, grand_yh, grand_zh,  grand5, grand6, grand7,  grand8      = geompy.KindOfShape(aretes[8])

grand_generat = aretes[11]

arc0,  arc_x, arc_y, arc_z,  arc_dx, arc_dy, arc_dz,  arc_rayon, arc_x1, arc_y1, arc_z1,  arc_x2, arc_y2, arc_z2 = geompy.KindOfShape(aretes[12])

face_cylindre = faces[5]

# Construire des géométries intermédiaires
# ----------------------------------------

arc_sc = geompy.MakeVertex(arc_x , arc_y , arc_z )
arc_s1 = geompy.MakeVertex(arc_x1, arc_y1, arc_z1)
arc_s2 = geompy.MakeVertex(arc_x2, arc_y2, arc_z2)

arc_a1 = geompy.MakeEdge(arc_sc, arc_s1)
arc_a2 = geompy.MakeEdge(arc_sc, arc_s2)

# Construire le modèle de bloc
# ============================

doc = hexablock.addDocument(nom)

# Construire le grand cylindre en 2 parties
# -----------------------------------------

grand_base = doc.addVertex(grand_xb, grand_yb, grand_zb)
grand_oppo = doc.addVertex(grand_xh, grand_yh, grand_zh)

grand_dir_a = doc.addVectorVertices(grand_base, grand_oppo)
grand_dir_b = doc.addVectorVertices(grand_oppo, grand_base)

grand_hauteur_a = grand_dir_a.getNorm()*0.20
grand_hauteur_b = grand_dir_b.getNorm()*0.60

grand_cylindre_a = doc.addCylinder(grand_base, grand_dir_a, grand_rayon, grand_hauteur_a)
grand_cylindre_b = doc.addCylinder(grand_oppo, grand_dir_b, grand_rayon, grand_hauteur_b)

# Construire le moyen cylindre
# ----------------------------

moyen_base = doc.addVertex(moyen_x, moyen_y, moyen_z)

moyen_arri = doc.addVertex(moyen_arri_x, moyen_arri_y, moyen_arri_z)
moyen_avan = doc.addVertex(moyen_avan_x, moyen_avan_y, moyen_avan_z)

moyen_dir = doc.addVectorVertices(moyen_arri, moyen_avan)

moyen_hauteur = geompy.MinDistance(moyen_arri_v, grand_generat) - grand_rayon

moyen_cylindre = doc.addCylinder(moyen_base, moyen_dir, moyen_rayon, moyen_hauteur)

# Construire le petit cylindre
# ----------------------------

petit_base = doc.addVertex(petit_x, petit_y, petit_z)

petit_arri = doc.addVertex(petit_arri_x, petit_arri_y, petit_arri_z)
petit_avan = doc.addVertex(petit_avan_x, petit_avan_y, petit_avan_z)

petit_dir = doc.addVectorVertices(petit_arri, petit_avan)

petit_hauteur = geompy.MinDistance(petit_arri_v, grand_generat)

petit_cylindre = doc.addCylinder(petit_base, petit_dir, petit_rayon, petit_hauteur)

# Construire les 2 cylindres qui s'intersectent
# ---------------------------------------------

cylindres_gm = doc.makeCylinders(grand_cylindre_a, moyen_cylindre)
cylindres_gp = doc.makeCylinders(grand_cylindre_b, petit_cylindre)

# Joindre les 2 croix
# -------------------

gm_quads = []

for i in xrange(1, -1, -1):
    for j in xrange( [hexablock.CV_MAXI_INT, hexablock.CV_MAXI_EXT][i] ):
        quad = cylindres_gm.getQuadIJ(hexablock.CYL_BIG, i, j, hexablock.CYL_BIG_SLICES)
        gm_quads.append(quad)

gp_q = cylindres_gp.getQuadIJ(hexablock.CYL_BIG, 1, hexablock.V_SW, hexablock.CYL_BIG_SLICES)

gp_v0 = cylindres_gp.getVertexIJK(hexablock.CYL_BIG, 2, hexablock.V_S , hexablock.CYL_BIG_SLICES)
gp_v1 = cylindres_gp.getVertexIJK(hexablock.CYL_BIG, 2, hexablock.V_SW, hexablock.CYL_BIG_SLICES)

gm_v0 = cylindres_gm.getVertexIJK(hexablock.CYL_BIG, 2, hexablock.V_E , hexablock.CYL_BIG_SLICES)
gm_v1 = cylindres_gm.getVertexIJK(hexablock.CYL_BIG, 2, hexablock.V_NE, hexablock.CYL_BIG_SLICES)

prisme = doc.joinQuads(gm_quads,  gp_q, gm_v0, gp_v0, gm_v1, gp_v1,  1)

# Ajouter le coude au grand cylindre
# ----------------------------------

coude_quads = []

for i in xrange(1, -1, -1):
    for j in xrange( [hexablock.CV_MAXI_INT, hexablock.CV_MAXI_EXT][i] ):
        quad = cylindres_gp.getQuadIJ(hexablock.CYL_BIG, i, j, 0)
        coude_quads.append(quad)

coude_centre = doc.addVertex(arc_x , arc_y , arc_z )
coude_dir    = doc.addVector(arc_dx, arc_dy, arc_dz)
coude_angle  = geompy.GetAngle(arc_a1, arc_a2)

coude = doc.revolutionQuads(coude_quads, coude_centre, coude_dir, [coude_angle])

# Associer la géométrie au modèle de bloc
# =======================================

doc.setShape(geometrie)

# Associer les arêtes du modèle issues du joinQuads
# -------------------------------------------------

for i in xrange(hexablock.CV_MAXI_EXT):
    asso_h  = prisme.getHexa(i)
    asso_a  = [ hexablock.E_CE, hexablock.E_CF, hexablock.E_CE, hexablock.E_CF, hexablock.E_CE, hexablock.E_CF, hexablock.E_CE, hexablock.E_CF ][i]
    asso_e  = asso_h.getEdge(asso_a)
    asso_s1 = asso_e.getVertex(0)
    asso_s2 = asso_e.getVertex(1)
    asso_v1 = asso_s1.getAssociation()
    asso_v2 = asso_s2.getAssociation()
    x1, y1, z1 = geompy.PointCoordinates(asso_v1)
    x2, y2, z2 = geompy.PointCoordinates(asso_v2)
    asso_pts = [ asso_v1 ]
    n = 9
    pas = math.sqrt( (x2-x1)**2 + (y2-y1)**2 + (z2-z1)**2 ) / n
    for i in xrange(1, n):
        x = x1 + (x2 - x1) * i * pas
        y = y1 + (y2 - y1) * i * pas
        z = z1 + (z2 - z1) * i * pas
        asso_v = geompy.MakeVertexOnSurfaceByCoord(face_cylindre, x, y, z)
        asso_pts.append(asso_v)
    asso_pts.append(asso_v2)
    asso_curve = geompy.MakeInterpol(asso_pts)
    asso_e.addAssociation(asso_curve, 0, 1)

# Associer les quadrangles du modèle issues du joinQuads
# ------------------------------------------------------

for i in xrange(hexablock.CV_MAXI_EXT):
    asso_h = prisme.getHexa(i)
    asso_f = [ hexablock.Q_C, hexablock.Q_F, hexablock.Q_E, hexablock.Q_F, hexablock.Q_E, hexablock.Q_F, hexablock.Q_E, hexablock.Q_F ][i]
    asso_q = asso_h.getQuad(asso_f)
    asso_q.addAssociation(face_cylindre)

# Mailler le modèle de bloc
# =========================

# Définir 4 groupes de faces
# --------------------------

groupe_d_grand = doc.addQuadGroup("grand:disque")
groupe_d_moyen = doc.addQuadGroup("moyen:disque")
groupe_d_petit = doc.addQuadGroup("petit:disque")
groupe_d_coude = doc.addQuadGroup("coude:disque")

for i in xrange(2):
    for j in xrange( [hexablock.CV_MAXI_INT, hexablock.CV_MAXI_EXT][i] ):
        quad = cylindres_gm.getQuadIJ(hexablock.CYL_BIG  , i, j, 0)
        groupe_d_grand.addElement(quad)

        quad = cylindres_gm.getQuadIJ(hexablock.CYL_SMALL, i, j, 0)
        groupe_d_moyen.addElement(quad)

        quad = cylindres_gp.getQuadIJ(hexablock.CYL_SMALL, i, j, 0)
        groupe_d_petit.addElement(quad)

for i in xrange( hexablock.CV_MAXI_INT + hexablock.CV_MAXI_EXT ):
    h = coude.getHexa(i)
    quad = h.getQuad(hexablock.Q_B)
    groupe_d_coude.addElement(quad)

# Définir 3 groupes de volumes
# ----------------------------

groupe_grand = doc.addHexaGroup("grand")
groupe_moyen = doc.addHexaGroup("moyen")
groupe_petit = doc.addHexaGroup("petit")

for i in xrange( doc.countUsedHexa() ):
    h = doc.getUsedHexa(i)
    groupe_grand.addElement(h)

for i in xrange(2):
    for j in xrange( [hexablock.CV_MAXI_INT, hexablock.CV_MAXI_EXT][i] ):
        h = cylindres_gm.getHexaIJK(hexablock.CYL_SMALL, i, j, 0)
        groupe_moyen.addElement(h)
        groupe_grand.removeElement(h)

        h = cylindres_gp.getHexaIJK(hexablock.CYL_SMALL, i, j, 0)
        groupe_petit.addElement(h)
        groupe_grand.removeElement(h)

# Mailler le modèle de bloc avec ses associations
# -----------------------------------------------

hexablock.addLaws(doc, 0.02, True)

blocs = hexablock.mesh(doc)

muv, mue, muq, muh = hexablock.dump(doc, blocs)
