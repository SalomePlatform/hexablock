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

# Francis KLOSS : 2011-2013 : CEA-Saclay, DEN, DM2S, SFME, LGLS, F-91191 Gif-sur-Yvette, France
# =============================================================================================

import math
import hexablock

### import geompy
geompy = hexablock.geompy


# Construire le modele de bloc
# ============================

doc = hexablock.addDocument("bielle")

# Construire les 2 grilles cylindriques : petite = 1, grande = 2

letour  = 360
hauteur = 0.04
angle1  = math.pi / 3

center1 = doc.addVertex(-0.107, 0, -hauteur/2)
center2 = doc.addVertex( 0.119, 0, -hauteur/2)
rint1   = 0.012
rint2   = 0.025
rext1   = 2*rint1
rext2   = 2*rint2

vx1 = doc.addVector(math.cos(angle1), math.sin(angle1), 0)
vx2 = doc.addVector(1, 0, 0)
vz  = doc.addVector(0, 0, 1)

grille_p = doc.makeCylinderUni (center1, vx1, vz,  rint1, rext1, letour, hauteur, 1, 3, 1)
hexablock.what ()
grille_g = doc.makeCylinderUni (center2, vx2, vz,  rint2, rext2, letour, hauteur, 1, 3, 1)
hexablock.what ()

# Relier les 2 grilles
# --------------------

quad_p = grille_p.getQuadJK(1, 2, 0)
quad_g = grille_g.getQuadJK(1, 1, 0)

point_p1 = grille_p.getVertexIJK(1, 0, 0)
point_p2 = grille_p.getVertexIJK(1, 2, 0)

point_g1 = grille_g.getVertexIJK(1, 1, 0)
point_g2 = grille_g.getVertexIJK(1, 2, 0)

prisme = doc.joinQuadUni (quad_p, quad_g,  point_p1, point_g1,  point_p2, point_g2,  3)

# Charger la geometrie
# ====================

bielle = geompy.ImportSTEP("bielle.stp")

# Selectionner des sous-parties de la geometrie
# ---------------------------------------------

sommets  = geompy.SubShapeAllSortedCentres    (bielle, geompy.ShapeType["VERTEX"])
nodes_id = geompy.SubShapeAllSortedCentresIDs (bielle, geompy.ShapeType["VERTEX"])
     ##   0   1   2   3   4   5   6   7   8   9
     ##  23, 70, 15, 52, 17, 57, 13, 47, 19, 62,
     ##  11, 42,  6, 30,  9, 37,  7, 32, 26, 75]

sommets_petit     = [  6,  8,  7,  9 ]
sommets_grand     = [ 10, 12, 11, 13 ]
node_little_subid = [ 13, 19, 47, 62 ]
node_big_subid    = [ 11,  6, 42, 30 ]

aretes  = geompy.SubShapeAllSortedCentres    (bielle, geompy.ShapeType["EDGE"])
edge_id = geompy.SubShapeAllSortedCentresIDs (bielle, geompy.ShapeType["EDGE"])
     ##   0   1   2   3   4   5   6   7   8   9
     ##  16, 56, 22, 71, 69, 53, 58, 14, 51, 18,
     ##  61, 48, 63, 12, 46, 20, 66, 43, 29, 10,
     ##  41,  5, 31, 38, 33, 25, 76,  8, 36, 74


aretes_petit     = [  7,  9,  8, 10 ]
aretes_grand     = [ 19, 21, 20, 22 ]
arc_little_subid = [ 14, 18, 51, 61 ]
arc_big_subid    = [ 10,  5, 41, 31 ]

subid_pbcd = 16
subid_pbe  = 22

subid_phcd = 56
subid_phe  = 71

subid_gbcd = 8
subid_gbe  = 25

subid_ghcd = 36
subid_ghe  = 76

ga_pbcd = aretes[ 0]
ga_pbe  = aretes[ 2]

ga_phcd = aretes[ 1]
ga_phe  = aretes[ 3]

ga_gbcd = aretes[27]
ga_gbe  = aretes[25]

ga_ghcd = aretes[28]
ga_ghe  = aretes[26]

# Associer le modele de bloc avec la geometrie
# ============================================

geometry = hexablock.addShape (doc, bielle, "bielle")
doc.setLevel (748)

# Nettoyer les associations implicites du document
# ------------------------------------------------

doc.clearAssociation()

# Associer les cercles exterieurs
# -------------------------------

def cercle(grille, k, subid, p):
    ms  = grille.getVertexIJK(0, 0, k)

    ma1 = grille.getEdgeJ (0, 2, k)
    ma2 = grille.getEdgeJ (0, 1, k)
    ma3 = grille.getEdgeJ (0, 0, k)
    tedges = [ma1, ma2, ma3 ]
    tgeom  = [geometry]

    doc.associateClosedLine (ms, tedges, tgeom, [subid], p, False)

cercle (grille_p, 0, subid_pbe, 5.0/6)
cercle (grille_p, 1, subid_phe, 5.0/6)

cercle (grille_g, 0, subid_gbe, 0)
cercle (grille_g, 1, subid_ghe, 0)

# Associer les arcs exterieurs (excentres)
# ----------------------------------------

def arc(grille, i1, i2, k, subid):
    ma1 = grille.getEdgeJ(1, i1, k)
    ma2 = grille.getEdgeJ(1, i2, k)
    doc.associateOpenedLine ([ma1, ma2 ], [geometry], [subid], 0, 1)

arc(grille_p, 1, 0, 0, subid_pbcd)
arc(grille_p, 1, 0, 1, subid_phcd)

arc(grille_g, 0, 2, 0, subid_gbcd)
arc(grille_g, 0, 2, 1, subid_ghcd)

# Associer les sommets des arcs de cercle de raccord
# --------------------------------------------------

hm = prisme.getHexa(1)
for i in xrange(0, 4):
  vm = hm.getVertex(i)
  subid = node_little_subid [i]
  vm.setAssociation (geometry, subid)

hm = prisme.getHexa(2)
for i in xrange(0, 4):
  vm = hm.getVertex(i)
  subid = node_big_subid [i]
  vm.setAssociation (geometry, subid)

# Associer les arcs de cercle de raccord
# --------------------------------------

hm = prisme.getHexa(0)
for i in xrange(0, 4):
  em = hm.getEdge(i+8)
  subid = arc_little_subid [i]
  em.addAssociation (geometry, subid, 0, 1)

hm = prisme.getHexa(2)
for i in xrange(0, 4):
  em = hm.getEdge(i+8)
  subid = arc_big_subid [i]
  em.addAssociation (geometry, subid, 0, 1)

# Arrondir des associations implicites cylindriques
# -------------------------------------------------

larc = []
for h, i, ech in [ [0, 0, 0.95], [0, 1, 0.95],  [2, 2, 0.85], [2, 3, 0.85] ]:
  hm = prisme.getHexa(h)
  em = hm.getEdge(i)

  v1 = em.getVertex(0)
  v2 = em.getVertex(1)

  vax = v1.getAssoX ();
  vay = v1.getAssoY ();
  vaz = v1.getAssoZ ();

  vbx = v2.getAssoX ();
  vby = v2.getAssoY ();
  vbz = v2.getAssoZ ();

  vmx = ( vax + vbx ) / 2.0 * ech
  vmy = ( vay + vby ) / 2.0
  vmz = ( vaz + vbz ) / 2.0

  va = geompy.MakeVertex(vax, vay, vaz)
  vb = geompy.MakeVertex(vbx, vby, vbz)
  vm = geompy.MakeVertex(vmx, vmy, vmz)
  eg = geompy.MakeArc(va, vm, vb)
  larc.append(eg)

arc   = geompy.MakeCompound(larc)
shape = hexablock.addShape (doc, arc, "arc")

# Mailler le modele de bloc
for h, i in [ [0, 0], [0, 1],  [2, 2], [2, 3] ]:
  hm = prisme.getHexa(h)
  em = hm.getEdge(i)

  em.clearAssociation()
  shape_subid = 2+3*i
  em.addAssociation (shape, shape_subid, 0.0, 1.0)

# Mailler le modele de bloc
# =========================

# Definir 5 groupes de faces
# --------------------------

groupe_petit   = doc.addQuadGroup("Petit")
groupe_grand   = doc.addQuadGroup("Grand")
groupe_bas     = doc.addQuadGroup("Bas")
groupe_haut    = doc.addQuadGroup("Haut")
groupe_contour = doc.addQuadGroup("Contour")

# Constituer les groupes petit et grand
# -------------------------------------

for i in xrange(3):
  groupe_petit.addElement( grille_p.getQuadJK(0, i, 0) )
  groupe_grand.addElement( grille_g.getQuadJK(0, i, 0) )

# Constituer les groupes bas et haut
# ----------------------------------

for i in xrange(3):
  groupe_bas.addElement(  grille_p.getQuadIJ(0, i, 0) )
  groupe_bas.addElement(  grille_g.getQuadIJ(0, i, 0) )

  groupe_haut.addElement( grille_p.getQuadIJ(0, i, 1) )
  groupe_haut.addElement( grille_g.getQuadIJ(0, i, 1) )

for i in xrange(3):
  h = prisme.getHexa(i)

  groupe_bas.addElement(  h.getQuad(2) )
  groupe_haut.addElement( h.getQuad(3) )

# Constituer le groupe contour
# ----------------------------

for i in xrange(2):
  groupe_contour.addElement( grille_p.getQuadJK(1, i, 0) )

for i in [0, 2]:
  groupe_contour.addElement( grille_g.getQuadJK(1, i, 0) )

for i in xrange(3):
  h = prisme.getHexa(i)

  groupe_contour.addElement( h.getQuad(4) )
  groupe_contour.addElement( h.getQuad(5) )

# Definir 3 groupes de volumes
# ----------------------------

groupe_petit  = doc.addHexaGroup("Petit")
groupe_grand  = doc.addHexaGroup("Grand")
groupe_prisme = doc.addHexaGroup("Prisme")

for i in xrange(3):
  groupe_petit.addElement( grille_p.getHexa(i) )
  groupe_grand.addElement( grille_g.getHexa(i) )

for i in xrange(3):
  groupe_prisme.addElement( prisme.getHexa(i) )

# Mailler le modele de bloc avec association
# ------------------------------------------

doc.addLaws (0.003, True)

law = doc.addLaw("Uniform", 4)

for j in range(doc.countPropagation()):
    propa = doc.getPropagation(j)
    propa.setLaw(law)

doc.save("bielle_v5")
doc.setName("bielle_v5")
hexablock.addToStudy(doc)

blocs = hexablock.mesh(doc)

muv, mue, muq, muh = hexablock.dump(doc, blocs)
