# -*- coding: latin-1 -*-
# Copyright (C) 2009-2024  CEA, EDF
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
geompy = hexablock.geompy

# Charger la geometrie
# ====================

nom = "troisCylindres"

piece = geompy.ImportBREP(nom+".brep")

# Selectionner des sous-parties de la geometrie
# ---------------------------------------------

sommets = geompy.SubShapeAllSortedCentres(piece, geompy.ShapeType["VERTEX"])
aretes  = geompy.SubShapeAllSortedCentres(piece, geompy.ShapeType["EDGE"  ])

moy_b_x, moy_b_y, moy_b_z = geompy.PointCoordinates(sommets[14])
moy_p_x, moy_p_y, moy_p_z = geompy.PointCoordinates(sommets[ 6])
moy_g_x, moy_g_y, moy_g_z = geompy.PointCoordinates(sommets[ 3])

grand0, grand_xb, grand_yb, grand_zb,  grand1, grand2, grand3,  grand_rayon = geompy.KindOfShape(aretes[ 0])
grand4, grand_xh, grand_yh, grand_zh,  grand5, grand6, grand7,  grand8      = geompy.KindOfShape(aretes[ 1])

moyen0, moyen_x , moyen_y , moyen_z ,  moyen1, moyen2, moyen3,  moyen_rayon = geompy.KindOfShape(aretes[20])

petit0, petit_xb, petit_yb, petit_zb,  petit1, petit2, petit3,  petit_rayon = geompy.KindOfShape(aretes[12])
petit4, petit_xh, petit_yh, petit_zh,  petit5, petit6, petit7,  petit8      = geompy.KindOfShape(aretes[13])

# Construire le modele de bloc
# ============================

doc = hexablock.addDocument(nom)

doc.addShape(piece,nom)

# Construire le grand cylindre
# ----------------------------

grand_base = doc.addVertex(grand_xb, grand_yb, grand_zb)
grand_oppo = doc.addVertex(grand_xh, grand_yh, grand_zh)

grand_dir = doc.addVectorVertices(grand_base, grand_oppo)

grand_hauteur = grand_dir.getNorm()

grand_cylindre = doc.addCylinder(grand_base, grand_dir, grand_rayon, grand_hauteur)

# Construire le moyen cylindre cote grand
# ---------------------------------------

moy_g_r1 = 0.8
moy_g_r2 = 0.5

moy_g_base_x = moy_p_x + (moy_g_x-moy_p_x)*moy_g_r1 + moyen_x - moy_b_x
moy_g_base_y = moy_p_y + (moy_g_y-moy_p_y)*moy_g_r1 + moyen_y - moy_b_y
moy_g_base_z = moy_p_z + (moy_g_z-moy_p_z)*moy_g_r1 + moyen_z - moy_b_z

moy_g_base = doc.addVertex(moy_g_base_x, moy_g_base_y, moy_g_base_z)
moy_g_oppo = doc.addVertex( (grand_xh+grand_xb) * moy_g_r2, (grand_yh+grand_yb) * moy_g_r2, (grand_zh+grand_zb) * moy_g_r2 )

moy_g_dir = doc.addVectorVertices(moy_g_base, moy_g_oppo)

moy_g_hauteur = moy_g_dir.getNorm()

moy_g_cylindre = doc.addCylinder(moy_g_base, moy_g_dir, moyen_rayon, moy_g_hauteur)

# Construire le moyen cylindre cote petit
# ---------------------------------------

moy_p_r = 0.18

moy_p_base = doc.addVertex(moyen_x, moyen_y, moyen_z)
moy_b_base = doc.addVertex(moy_b_x, moy_b_y, moy_b_z)
moy_p_oppo = doc.addVertex( moy_p_x+(moy_g_x-moy_p_x) * moy_p_r, moy_p_y+(moy_g_y-moy_p_y) * moy_p_r, moy_p_z+(moy_g_z-moy_p_z) * moy_p_r )

moy_p_dir = doc.addVectorVertices(moy_b_base, moy_p_oppo)

moy_p_hauteur = moy_p_dir.getNorm()

moy_p_cylindre = doc.addCylinder(moy_p_base, moy_p_dir, moyen_rayon, moy_p_hauteur)

# Construire le petit cylindre
# ----------------------------

petit_base = doc.addVertex(petit_xb, petit_yb, petit_zb)
petit_oppo = doc.addVertex(petit_xh, petit_yh, petit_zh)

petit_dir = doc.addVectorVertices(petit_base, petit_oppo)

petit_hauteur = petit_dir.getNorm()

petit_cylindre = doc.addCylinder(petit_base, petit_dir, petit_rayon, petit_hauteur)

# Construire le T et la X
# -----------------------

en_te = doc.makeCylinders(grand_cylindre, moy_g_cylindre)
croix = doc.makeCylinders(moy_p_cylindre, petit_cylindre)

# Relier le T et la X
# -------------------

croix_quads = []

for i in range(1, -1, -1):
    for j in range( [hexablock.CV_MAXI_INT, hexablock.CV_MAXI_EXT][i] ):
        quad = croix.getQuadIJ(hexablock.CYL_BIG, i, j, hexablock.CYL_BIG_SLICES)
        croix_quads.append(quad)

en_te_q = en_te.getQuadIJ(hexablock.CYL_SMALL, 1, hexablock.V_N, 0)

en_te_v0 = en_te.getVertexIJK(hexablock.CYL_SMALL, 2, hexablock.V_N , 0)
en_te_v1 = en_te.getVertexIJK(hexablock.CYL_SMALL, 2, hexablock.V_NW, 0)

croix_v0 = croix.getVertexIJK(hexablock.CYL_BIG, 2, hexablock.V_E , hexablock.CYL_BIG_SLICES)
croix_v1 = croix.getVertexIJK(hexablock.CYL_BIG, 2, hexablock.V_NE, hexablock.CYL_BIG_SLICES)

prisme = doc.joinQuads(croix_quads, en_te_q,  croix_v0, en_te_v0,  croix_v1, en_te_v1,  1)

# Associer
# ========

# fkl: comme pour la tuyauterie: associer pour le prisme (sauf si item 142 et 146 de Taches.ods)

# Mailler le modele de bloc
# =========================

# Definir 5 groupes de faces
# --------------------------

groupe_grand_a = doc.addQuadGroup("Grand_A")
groupe_grand_b = doc.addQuadGroup("Grand_B")
groupe_moyen_f = doc.addQuadGroup("Moyen")
groupe_petit_a = doc.addQuadGroup("Petit_A")
groupe_petit_b = doc.addQuadGroup("Petit_B")

for i in range(2):
    for j in range( [hexablock.CV_MAXI_INT, hexablock.CV_MAXI_EXT][i] ):
        quad = en_te.getQuadIJ(hexablock.CYL_BIG  , i, j, 0)
        groupe_grand_a.addElement(quad)

        quad = en_te.getQuadIJ(hexablock.CYL_BIG  , i, j, hexablock.CYL_BIG_SLICES)
        groupe_grand_b.addElement(quad)

        quad = croix.getQuadIJ(hexablock.CYL_BIG  , i, j, 0)
        groupe_moyen_f.addElement(quad)

        quad = croix.getQuadIJ(hexablock.CYL_SMALL, i, j, 0)
        groupe_petit_a.addElement(quad)

        quad = croix.getQuadIJ(hexablock.CYL_SMALL, i, j, hexablock.CYL_SMALL_SLICES)
        groupe_petit_b.addElement(quad)

# Definir 3 groupes de volumes
# ----------------------------

groupe_grand = doc.addHexaGroup("Grand")
groupe_moyen = doc.addHexaGroup("Moyen")
groupe_petit = doc.addHexaGroup("Petit")

for i in range( doc.countUsedHexa() ):
    h = doc.getUsedHexa(i)
    groupe_moyen.addElement(h)

for i in range(2):
    for j in range( [hexablock.CV_MAXI_INT, hexablock.CV_MAXI_EXT][i] ):
        for k in [ 0, hexablock.CYL_BIG_SLICES-1 ]:
            h = en_te.getHexaIJK(hexablock.CYL_BIG, i, j, k)
            groupe_grand.addElement(h)
            groupe_moyen.removeElement(h)

        for k in [0, hexablock.CYL_SMALL_SLICES-1 ]:
            h = croix.getHexaIJK(hexablock.CYL_SMALL, i, j, k)
            groupe_petit.addElement(h)
            groupe_moyen.removeElement(h)

# Mailler le modele de bloc avec association
# ------------------------------------------

########## hexablock.addLaws(doc, 10, True) Ne marche pas

law = doc.addLaw("Uniform", 4)
for j in range(doc.countPropagation()):
    propa = doc.getPropagation(j)
    propa.setLaw(law)

doc.save("troisCylindres_v5")
doc.setName("troisCylindres_v5")
hexablock.addToStudy(doc)

blocs = hexablock.mesh(doc, dim=3)

muv, mue, muq, muh = hexablock.dump(doc, blocs)
