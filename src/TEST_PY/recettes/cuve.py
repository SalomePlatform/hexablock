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

# Francis KLOSS - 2011-2013 - CEA-Saclay, DEN, DM2S, SFME, LGLS, F-91191 Gif-sur-Yvette, France
# =============================================================================================

import math

### import geompy
import hexablock
geompy = hexablock.geompy

# Charger la géométrie
# ====================

nom = "cuve"

cuve = geompy.ImportBREP(nom+".brep")

# Sélectionner des sommets de la géométrie
# -----------------------------------------

sommets = geompy.SubShapeAllSortedCentres(cuve, geompy.ShapeType["VERTEX"])

coin_a = sommets[ 4]
coin_b = sommets[43]
coin_c = sommets[62]
coin_d = sommets[15]

support_a_o = sommets[ 3]
support_a_x = sommets[ 8]
support_a_y = sommets[ 6]
support_a_d = sommets[10]

support_b_o = sommets[42]
support_b_x = sommets[36]
support_b_y = sommets[47]
support_b_d = sommets[38]

support_c_o = sommets[61]
support_c_x = sommets[51]
support_c_y = sommets[59]
support_c_d = sommets[49]

support_d_o = sommets[14]
support_d_x = sommets[19]
support_d_y = sommets[12]
support_d_d = sommets[17]

coin_x1, coin_y1, coin_z1 = geompy.PointCoordinates(sommets[ 2])
coin_x2, coin_y2, coin_z2 = geompy.PointCoordinates(support_a_d)
coin_x3, coin_y3, coin_z3 = geompy.PointCoordinates(support_a_o)
coin_x4, coin_y4, coin_z4 = geompy.PointCoordinates(coin_c)

base_x, base_y, base_z = geompy.PointCoordinates(sommets[52])
oppo_x, oppo_y, oppo_z = geompy.PointCoordinates(sommets[57])

# Sélectionner des arêtes de la géométrie
# ---------------------------------------

aretes = geompy.SubShapeAllSortedCentres(cuve, geompy.ShapeType["EDGE"])

interne0, interne_x, interne_y, interne_z,  interne_dx, interne_dy, interne_dz,  interne_rayon = geompy.KindOfShape(aretes[48])
externe0, externe_x, externe_y, externe_z,  externe_dx, externe_dy, externe_dz,  externe_rayon = geompy.KindOfShape(aretes[49])

cote_a = aretes[41]
cote_b = aretes[91]
cote_c = aretes[59]
cote_d = aretes[12]

piquage_1_int = aretes[  0]
piquage_1_ext = aretes[  1]
piquage_1_feb = aretes[ 36]
piquage_1_feh = aretes[ 38]
piquage_1_eeb = aretes[ 33]
piquage_1_eeh = aretes[ 37]
piquage_1_eib = aretes[ 34]
piquage_1_eih = aretes[ 35]

piquage_2_int = aretes[103]
piquage_2_ext = aretes[104]
piquage_2_feg = aretes[ 68]
piquage_2_fed = aretes[ 73]
piquage_2_eeg = aretes[ 79]
piquage_2_eed = aretes[ 88]
piquage_2_eig = aretes[ 80]
piquage_2_eid = aretes[ 89]

support_l, support_a, support_v = geompy.BasicProperties(aretes[43])
cote_lgr, cote_aire, cote_vol   = geompy.BasicProperties(cote_a)

# Sélectionner des faces de la géométrie
# --------------------------------------

faces = geompy.SubShapeAllSortedCentres(cuve, geompy.ShapeType["FACE"])

face_cylindrique_int = faces[21]
face_cylindrique_ext = faces[22]

# Construire le modèle de blocs
# =============================

# Créer le document
# -----------------

doc = hexablock.addDocument(nom)

# Calculer l'angle défini par le centre de la cuve et 2 points (projeté dans le plan XY)
# --------------------------------------------------------------------------------------

centre_cuve = geompy.MakeVertex(externe_x, externe_y, 0)

def calculer_angle(point1, point2):
  x1, y1, z1 = geompy.PointCoordinates(point1)
  x2, y2, z2 = geompy.PointCoordinates(point2)

  p1 = geompy.MakeVertex(x1, y1, 0)
  p2 = geompy.MakeVertex(x2, y2, 0)

  a1 = geompy.MakeEdge(centre_cuve, p1)
  a2 = geompy.MakeEdge(centre_cuve, p2)

  return geompy.GetAngle(a1, a2)

# Construire les 8 points de la grille
# ------------------------------------

p1_int0, p1_int_x, p1_int_y, p1_int_z,  p1_int_dx, p1_int_dy, p1_int_dz,  p1_int_rayon = geompy.KindOfShape(piquage_1_int)
p2_int0, p2_int_x, p2_int_y, p2_int_z,  p2_int_dx, p2_int_dy, p2_int_dz,  p2_int_rayon = geompy.KindOfShape(piquage_2_int)

p1_int_c = geompy.MakeVertex(p1_int_x, p1_int_y, p1_int_z)
p2_int_c = geompy.MakeVertex(p2_int_x, p2_int_y, p2_int_z)

p1_int_e = geompy.MakePrismDXDYDZ2Ways(p1_int_c, p1_int_dx*cote_lgr, p1_int_dy*cote_lgr, p1_int_dz*cote_lgr)
p2_int_e = geompy.MakePrismDXDYDZ2Ways(p2_int_c, p2_int_dx*cote_lgr, p2_int_dy*cote_lgr, p2_int_dz*cote_lgr)

plan_2 = geompy.MakePrismDXDYDZ(p1_int_e, +cote_lgr, +cote_lgr, -cote_lgr)
plan_3 = geompy.MakePrismDXDYDZ(p1_int_e, -cote_lgr, -cote_lgr, -cote_lgr)

plan_7 = geompy.MakePrismDXDYDZ(p2_int_e, 0, -cote_lgr, -cote_lgr)
plan_8 = geompy.MakePrismDXDYDZ(p2_int_e, 0, +cote_lgr, -cote_lgr)

part_2 = geompy.MakePartition([piquage_1_eeb], [plan_2], [], [], geompy.ShapeType["VERTEX"])
part_3 = geompy.MakePartition([piquage_1_eeb], [plan_3], [], [], geompy.ShapeType["VERTEX"])

part_7 = geompy.MakePartition([piquage_2_feg], [plan_7], [], [], geompy.ShapeType["VERTEX"])
part_8 = geompy.MakePartition([piquage_2_fed], [plan_8], [], [], geompy.ShapeType["VERTEX"])

point_1 = coin_c
point_2 = geompy.SubShapeAllSortedCentres(part_2, geompy.ShapeType["VERTEX"])[1]
point_3 = geompy.SubShapeAllSortedCentres(part_3, geompy.ShapeType["VERTEX"])[1]
point_4 = geompy.MakeVertexOnCurve(cote_d, 0.55)
point_5 = coin_a
point_6 = coin_b
point_7 = geompy.SubShapeAllSortedCentres(part_7, geompy.ShapeType["VERTEX"])[0]
point_8 = geompy.SubShapeAllSortedCentres(part_8, geompy.ShapeType["VERTEX"])[2]

# Construire la grille cylindrique
# --------------------------------

grille_hauteur = coin_z4-coin_z2

centre = doc.addVertex(externe_x, externe_y, externe_z-grille_hauteur)

grille_x = doc.addVector(1, 1, 0)
grille_z = doc.addVector(0, 0, 1)

petit_rayon = interne_rayon * 0.2
grand_rayon = math.sqrt( (coin_x1-externe_x)**2 + (coin_y1-externe_y)**2 )

rayons = [ petit_rayon, interne_rayon-petit_rayon, externe_rayon-interne_rayon, grand_rayon-externe_rayon ]

points = [ point_1, point_2, point_3, point_4, point_5, point_6, point_7, point_8, point_1 ]

angles = []
for i in xrange(8):
  angle = calculer_angle(points[i], points[i+1])
  angles.append(angle)

hauteurs = [ grille_hauteur ]

grille = doc.makeCylindricals(centre, grille_x, grille_z, rayons, angles, hauteurs, False)

# Ajouter le centre
# -----------------

quad_0 = grille.getQuadJK(0, 7, 0)
quad_6 = grille.getQuadJK(0, 0, 0)
quad_7 = grille.getQuadJK(0, 1, 0)

centre_a = doc.addHexa3Quads(quad_0, quad_6, quad_7)

quad_2 = grille.getQuadJK(0, 3, 0)
quad_3 = grille.getQuadJK(0, 4, 0)
quad_4 = grille.getQuadJK(0, 5, 0)

centre_b = doc.addHexa3Quads(quad_2, quad_3, quad_4)

quad_1 = grille.getQuadJK(0, 2, 0)
quad_5 = grille.getQuadJK(0, 6, 0)

quad_a = centre_a.getQuad(1)
quad_b = centre_b.getQuad(1)

centre_c = doc.addHexa4Quads(quad_1, quad_a, quad_5, quad_b)

# Ajouter l'enceinte
# ------------------

plan_0  = geompy.MakePrismDXDYDZ(p1_int_e, +cote_lgr, +cote_lgr, +cote_lgr)
part_0  = geompy.MakePartition([piquage_1_feh], [plan_0], [], [], geompy.ShapeType["VERTEX"])
point_0 = geompy.SubShapeAllSortedCentres(part_0, geompy.ShapeType["VERTEX"])[1]

enceinte_quads = []
for j in xrange(8):
  q = grille.getQuadIJ(1, j, 1)
  enceinte_quads.append(q)

point_7x, point_7y, point_7z = geompy.PointCoordinates(point_7)
point_2x, point_2y, point_2z = geompy.PointCoordinates(point_2)
point_0x, point_0y, point_0z = geompy.PointCoordinates(point_0)

enceinte_h1 = point_7z - base_z
enceinte_h2 = point_2z - base_z
enceinte_h3 = point_0z - base_z
enceinte_h4 = oppo_z   - base_z

enceinte_hauteurs = [ enceinte_h1, enceinte_h2, enceinte_h3, enceinte_h4 ]

enceinte_pz = doc.addVector(oppo_x - base_x, oppo_y - base_y, enceinte_h4)

enceinte = doc.prismQuadsVec(enceinte_quads, enceinte_pz, enceinte_hauteurs, 0)

# Déconnecter via des arêtes, puis prismer, puis compléter (3) ou coller (1)
# --------------------------------------------------------------------------

periph_hexas     = []
supports_quads_b = []
supports_quads_h = []

v1 = doc.addVector(   0, +0.1, 0)
v2 = doc.addVector(   0, -0.1, 0)
v3 = doc.addVector(+0.2, -0.1, 0)

for j, v in [ [0, v1], [1, v1], [4, v2], [5, v3] ]:
  h = grille.getHexaIJK(2, j, 0)
  a = grille.getEdgeK(3, j, 0)
  d = doc.disconnectEdge(h, a)

  q1 = d.getQuad(0)
  q2 = grille.getQuadIK(2, j, 0)
  if j==1:
    h0 = doc.addHexa2Quads(q1, q2)
    q3 = grille.getQuadJK(3, j, 0)
    q4 = h0.getQuad(3)
    h1 = doc.addHexa2Quads(q3, q4)
    qb = h1.getQuad(hexablock.Q_E)
    qh = h1.getQuad(hexablock.Q_F)
  else:
    p = doc.prismQuad(q1, v, 2)

    h0 = p.getHexa(0)
    q3 = h0.getQuad(5)
    h1 = p.getHexa(1)
    q4 = h1.getQuad(5)
    doc.addHexa3Quads(q2, q3, q4)
    periph_hexas.append(h0)
    qb = h1.getQuad(hexablock.Q_C)
    qh = h1.getQuad(hexablock.Q_D)

  supports_quads_b.append(qb)
  supports_quads_h.append(qh)

# Piquer les 2 tuyaux sur l'enceinte
# ----------------------------------

piquage_centre = doc.addVertex(10, 0, 0)

piquage_vx = doc.addVector(1, 0, 0)
piquage_vz = doc.addVector(0, 0, 1)

piquage = doc.makeCylindrical(piquage_centre, piquage_vx, piquage_vz,  1, 360, 1,  1, 4, 1,  False)

piquage_quads = [ piquage.getQuadIJ(0, j, 0) for j in xrange(4) ]

piquage_s1 = piquage.getVertexIJK(1, 0, 0)
piquage_s2 = piquage.getVertexIJK(1, 1, 0)
piquage_s3 = piquage.getVertexIJK(1, 2, 0)

enceinte_ha = enceinte.getHexa( 6)
enceinte_hb = enceinte.getHexa(25)

enceinte_s11 = enceinte_ha.getVertex(2)
enceinte_s12 = enceinte_ha.getVertex(3)
enceinte_s13 = enceinte_ha.getVertex(7)

enceinte_s21 = enceinte_hb.getVertex(2)
enceinte_s22 = enceinte_hb.getVertex(3)
enceinte_s23 = enceinte_hb.getVertex(7)

piquage_1 = doc.replace(piquage_quads, piquage_s1, enceinte_s11, piquage_s2, enceinte_s12, piquage_s3, enceinte_s13)
piquage_2 = doc.replace(piquage_quads, piquage_s1, enceinte_s21, piquage_s2, enceinte_s22, piquage_s3, enceinte_s23)

# Nettoyer le template du piquage
# -------------------------------

doc.removeElements(piquage)

# Associer le modèle de blocs avec la géométrie
# =============================================

doc.setShape(cuve)

# Nettoyer les associations implicites du centre et de la périphérie
# ------------------------------------------------------------------

for i in xrange(0, 4, 3):
  for j in xrange(8):
    for k in xrange(2):
      e = grille.getEdgeJ(i, j, k)
      e.clearAssociation()

# Associer le centre de l'enceinte
# --------------------------------

for j, ig in [ [2, 30], [3, 28], [6, 32], [7, 34] ]:
  sm0 = grille.getVertexIJK(0, j, 0)
  sg0 = sommets[ig]
  sm0.setAssociation(sg0)

  sm1 = grille.getVertexIJK(0, j, 1)
  sg1 = geompy.MakeTranslation(sg0, 0, 0, grille_hauteur)
  sm1.setAssociation(sg1)

for j, ig, sens in [ [0, 34, +1], [1, 30, +1], [4, 28, -1], [5, 32, -1] ]:
  sm0 = grille.getVertexIJK(0, j, 0)
  sg0 = geompy.MakeTranslation(sommets[ig], 0, sens*support_l, 0)
  sm0.setAssociation(sg0)

  sm1 = grille.getVertexIJK(0, j, 1)
  sg1 = geompy.MakeTranslation(sg0, 0, 0, grille_hauteur)
  sm1.setAssociation(sg1)

# Associer les 16 sommets des 4 supports externes
# -----------------------------------------------

supports_points = [
  [ support_c_o, support_c_y, support_c_d, support_c_x ],
  [ support_d_d, support_d_y, support_d_o, support_d_x ],
  [ support_a_o, support_a_y, support_a_d, support_a_x ],
  [ support_b_o, support_b_x, support_b_d, support_b_y ]
]

for s in xrange(4):
  qb = supports_quads_b[s]
  qh = supports_quads_h[s]
  cs = supports_points[s]
  for i in xrange(4):
    smb = qb.getVertex(i)
    sgb = cs[i]
    smb.setAssociation(sgb)

    if s==1:
      ind = i
    else:
      ind = [1, 0, 3, 2][i]
    smh = qh.getVertex(ind)
    sgh = geompy.MakeTranslation(sgb, 0, 0, grille_hauteur)
    smh.setAssociation(sgh)

# Associer les 7 sommets périphériques de la grille
# -------------------------------------------------

par_1 = 0.17
par_2 = 1 - par_1

periph_grille = [ [0, cote_b, par_2], [1, cote_c, 0.22], [3, cote_d, 0.548], [4, cote_d, par_1], [5, cote_a, par_2], [6, cote_b, 0.44], [7, cote_b, 0.555] ]

for j, ag, p in periph_grille:
  smb = grille.getVertexIJK(3, j, 1)
  sgb = geompy.MakeVertexOnCurve(ag, p)
  smb.setAssociation(sgb)

  smh = grille.getVertexIJK(3, j, 0)
  sgh = geompy.MakeTranslation(sgb, 0, 0, -grille_hauteur)
  smh.setAssociation(sgh)

# Associer les 3 sommets liés aux déconnections sur la grille
# -----------------------------------------------------------

periph_deco = [ [cote_c, par_2], [cote_a, par_1], [cote_b, par_1] ]

for i in xrange(3):
  hxa = periph_hexas[i]
  ag, p = periph_deco[i]
  smb = hxa.getVertex(2)
  sgb = geompy.MakeVertexOnCurve(ag, p)
  smb.setAssociation(sgb)

  smh = hxa.getVertex(0)
  sgh = geompy.MakeTranslation(sgb, 0, 0, -grille_hauteur)
  smh.setAssociation(sgh)

# Réparer les piquages
# --------------------

piquage_e_base  = geompy.MakeVertex(interne_x, interne_y, interne_z)
piquage_e_axe   = geompy.MakeVectorDXDYDZ(interne_dx, interne_dy, interne_dz)
piquage_e_cyl   = geompy.MakeCylinder(piquage_e_base, piquage_e_axe, interne_rayon, enceinte_h4)

piquage_1_axe   = geompy.MakeVectorDXDYDZ(p1_int_dx, p1_int_dy, p1_int_dz)
piquage_1_cyl   = geompy.MakeCylinder(p1_int_c, piquage_1_axe, p1_int_rayon, cote_lgr)

piquage_2_axe   = geompy.MakeVectorDXDYDZ(p2_int_dx, p2_int_dy, p2_int_dz)
piquage_2_cyl   = geompy.MakeCylinder(p2_int_c, piquage_2_axe, p2_int_rayon, cote_lgr)

piquage_1_inter = geompy.SubShapeAllSortedCentres(geompy.MakeSection(piquage_e_cyl, piquage_1_cyl), geompy.ShapeType["EDGE"])
piquage_2_inter = geompy.SubShapeAllSortedCentres(geompy.MakeSection(piquage_e_cyl, piquage_2_cyl), geompy.ShapeType["EDGE"])

piquage_1_fi  = piquage_1_inter[0]

piquage_2_fig = piquage_2_inter[0]
piquage_2_fid = piquage_2_inter[1]

# Associer les piquages
# ---------------------

piquages_devant = [
  [ piquage_1, piquage_1_int, 0.375, piquage_1_ext, 0.125 ],
  [ piquage_2, piquage_2_int, 0.125, piquage_2_ext, 0.875 ]
]

for m_piq, a_int, p_int, a_ext, p_ext in piquages_devant:

  i1 = m_piq.getEdge(2)
  i2 = m_piq.getEdge(5)
  i3 = m_piq.getEdge(7)
  i4 = m_piq.getEdge(10)

  doc.associateClosedLine(i1.getVertex(1), i1, [ i2, i3, i4 ], a_int, p_int, True, [])

  e1 = m_piq.getEdge(0)
  e2 = m_piq.getEdge(1)
  e3 = m_piq.getEdge(9)
  e4 = m_piq.getEdge(11)

  doc.associateClosedLine(e1.getVertex(0), e1, [ e2, e3, e4 ], a_ext, p_ext, False, [])

piquages_milieu = [
  [ piquage_1, piquage_1_eib, piquage_1_eih, 0.30, piquage_1_eeb, piquage_1_eeh, 0.30 ],
  [ piquage_2, piquage_2_eid, piquage_2_eig, 0.75, piquage_2_eed, piquage_2_eeg, 0.75 ]
]

for m_piq, a_int1, a_int2, p_int, a_ext1, a_ext2, p_ext in piquages_milieu:

  i1 = m_piq.getEdge(22)
  i2 = m_piq.getEdge(25)
  i3 = m_piq.getEdge(27)
  i4 = m_piq.getEdge(30)

  doc.associateClosedLine(i1.getVertex(0), i1, [ i4, i3, i2 ], a_int1, p_int, False, [a_int2])

  e1 = m_piq.getEdge(20)
  e2 = m_piq.getEdge(21)
  e3 = m_piq.getEdge(29)
  e4 = m_piq.getEdge(31)

  doc.associateClosedLine(e1.getVertex(0), e1, [ e2, e3, e4 ], a_ext1, p_ext, False, [a_ext2])

piquages_fond = [
  [ piquage_1, piquage_1_fi , []             , 0.125, piquage_1_feb, piquage_1_feh, 0.30 ],
  [ piquage_2, piquage_2_fid, [piquage_2_fig], 0.75 , piquage_2_fed, piquage_2_feg, 0.75 ]
]

for m_piq, a_int1, a_int2, p_int, a_ext1, a_ext2, p_ext in piquages_fond:

  i1 = m_piq.getEdge(42)
  i2 = m_piq.getEdge(45)
  i3 = m_piq.getEdge(47)
  i4 = m_piq.getEdge(50)

  doc.associateClosedLine(i1.getVertex(0), i1, [ i4, i3, i2 ], a_int1, p_int, False, a_int2)

  e1 = m_piq.getEdge(40)
  e2 = m_piq.getEdge(41)
  e3 = m_piq.getEdge(49)
  e4 = m_piq.getEdge(51)

  doc.associateClosedLine(e1.getVertex(1), e1, [ e2, e3, e4 ], a_ext1, p_ext, False, [a_ext2])

# Associer quelques faces
# -----------------------

for h in [ 2, 5, 7, 10,  21, 24, 26, 29 ]:
  enceinte_hc = enceinte.getHexa(h)

  enceinte_qc = enceinte_hc.getQuad(hexablock.Q_C)
  enceinte_qc.addAssociation(face_cylindrique_int)

  enceinte_qc = enceinte_hc.getQuad(hexablock.Q_D)
  enceinte_qc.addAssociation(face_cylindrique_ext)

# Primer les 5 supports (finalisation du modèle de blocs)
# -------------------------------------------------------

supports_quads_b.append( centre_c.getQuad(hexablock.Q_E) )

supports_z = doc.addVector(coin_x1 - coin_x3, coin_y1 - coin_y3, coin_z1 - coin_z3)

supports = doc.prismQuads(supports_quads_b, supports_z, 1)

# Mailler le modèle de blocs
# ==========================

# Définir les groupes volumiques
# ------------------------------

groupe_fd = doc.addHexaGroup("fond")
groupe_en = doc.addHexaGroup("enceinte")
groupe_p1 = doc.addHexaGroup("piquage:1")
groupe_p2 = doc.addHexaGroup("piquage:2")
groupe_su = doc.addHexaGroup("supports")

for i in xrange( doc.countUsedHexa() ):
  h = doc.getUsedHexa(i)
  groupe_fd.addElement(h)

for i in xrange( enceinte.countHexa() ):
  h = enceinte.getHexa(i)
  if h != None:
    groupe_en.addElement(h)
    groupe_fd.removeElement(h)

for i in xrange( piquage_1.countHexa() ):
  h = piquage_1.getHexa(i)
  groupe_p1.addElement(h)
  groupe_fd.removeElement(h)

for i in xrange( piquage_2.countHexa() ):
  h = piquage_2.getHexa(i)
  groupe_p2.addElement(h)
  groupe_fd.removeElement(h)

for i in xrange( supports.countHexa() ):
  h = supports.getHexa(i)
  groupe_su.addElement(h)
  groupe_fd.removeElement(h)

# Générer le maillage
# -------------------

hexablock.addLaws(doc, 0.015, False)

maillage = hexablock.mesh(doc)

muv, mue, muq, muh = hexablock.dump(doc, maillage)

