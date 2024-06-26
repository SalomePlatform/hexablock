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

# Francis KLOSS : 2011-2013 : CEA-Saclay, DEN, DM2S, SFME, LGLS, F-91191 Gif-sur-Yvette, France
# =============================================================================================

import math

import geompy
import hexablock

# Construire le mod�le de bloc
# ============================

doc = hexablock.addDocument("bielle")

# Construire les 2 grilles cylindriques
# -------------------------------------

centre_pb = doc.addVertex(0, 0, 0)
centre_gb = doc.addVertex(7, 0, 0)

angle_px   = math.pi / 3
vecteur_px = doc.addVector(math.cos(angle_px), math.sin(angle_px), 0)
vecteur_gx = doc.addVector(1, 0, 0)

vecteur_z = doc.addVector(0, 0, 1)

grille_p = doc.makeCylindrical(centre_pb, vecteur_px, vecteur_z,  1, 360, 1,  1, 3, 1,  False)
grille_g = doc.makeCylindrical(centre_gb, vecteur_gx, vecteur_z,  1, 360, 1,  1, 3, 1,  False)

# Relier les 2 grilles
# --------------------

quad_p = grille_p.getQuadJK(1, 2, 0)
quad_g = grille_g.getQuadJK(1, 1, 0)

point_p1 = grille_p.getVertexIJK(1, 0, 0)
point_p2 = grille_p.getVertexIJK(1, 2, 0)

point_g1 = grille_g.getVertexIJK(1, 1, 0)
point_g2 = grille_g.getVertexIJK(1, 2, 0)

prisme = doc.joinQuad(quad_p, quad_g,  point_p1, point_g1,  point_p2, point_g2,  3)

# Charger la g�om�trie
# ====================

bielle = geompy.ImportSTEP("bielle.stp")

# S�lectionner des sous-parties de la g�om�trie
# ---------------------------------------------

sommets = geompy.SubShapeAllSortedCentres(bielle, geompy.ShapeType["VERTEX"])

sommets_petit = [  6,  8,  7,  9 ]
sommets_grand = [ 10, 12, 11, 13 ]

aretes = geompy.SubShapeAllSortedCentres(bielle, geompy.ShapeType["EDGE"])

aretes_petit = [  7,  9,  8, 10 ]
aretes_grand = [ 19, 21, 20, 22 ]

ga_pbcd = aretes[ 0]
ga_pbe  = aretes[ 2]

ga_phcd = aretes[ 1]
ga_phe  = aretes[ 3]

ga_gbcd = aretes[27]
ga_gbe  = aretes[25]

ga_ghcd = aretes[28]
ga_ghe  = aretes[26]

# Associer le mod�le de bloc avec la g�om�trie
# ============================================

doc.setShape(bielle)

# Netoyer les associations implicites du prisme
# ---------------------------------------------

for i in range( prisme.countEdge() ):
  em = prisme.getEdge(i)
  if em != None:
    em.clearAssociation()

# Associer les cercles ext�rieurs
# -------------------------------

def cercle(grille, k, ge, p):
    ms  = grille.getVertexIJK(0, 0, k)
    ma1 = grille.getEdgeJ(0, 2, k)
    ma2 = grille.getEdgeJ(0, 1, k)
    ma3 = grille.getEdgeJ(0, 0, k)
    doc.associateClosedLine(ms, ma1, [ ma2, ma3 ], ge, p, False, [])

cercle(grille_p, 0, ga_pbe, 5.0/6)
cercle(grille_p, 1, ga_phe, 5.0/6)

cercle(grille_g, 0, ga_gbe, 0)
cercle(grille_g, 1, ga_ghe, 0)

# Associer les arcs ext�rieurs (excentr�s)
# ----------------------------------------

def arc(grille, i1, i2, k, ge):
    ma1 = grille.getEdgeJ(1, i1, k)
    ma2 = grille.getEdgeJ(1, i2, k)
    doc.associateOpenedLine(ma1, [ ma2 ], ge, 0, [], 1)

arc(grille_p, 1, 0, 0, ga_pbcd)
arc(grille_p, 1, 0, 1, ga_phcd)

arc(grille_g, 0, 2, 0, ga_gbcd)
arc(grille_g, 0, 2, 1, ga_ghcd)

# Associer les arcs de cercle de raccord
# --------------------------------------

hm = prisme.getHexa(0)
for i in range(0, 4):
  em = hm.getEdge(i+8)
  ga = aretes[ aretes_petit[i] ]

  em.addAssociation(ga, 0, 1)

hm = prisme.getHexa(2)
for i in range(0, 4):
  em = hm.getEdge(i+8)
  ga = aretes[ aretes_grand[i] ]

# Associer les sommets des arcs de cercle de raccord
# --------------------------------------------------

hm = prisme.getHexa(1)
for i in range(0, 4):
  vm = hm.getVertex(i)
  ga = sommets[ sommets_petit[i] ]
  vm.setAssociation(ga)

hm = prisme.getHexa(2)
for i in range(0, 4):
  vm = hm.getVertex(i)
  ga = sommets[ sommets_grand[i] ]
  vm.setAssociation(ga)

# Associer les arcs de cercle de raccord
# --------------------------------------

hm = prisme.getHexa(0)
for i in range(0, 4):
  em = hm.getEdge(i+8)
  ga = aretes[ aretes_petit[i] ]
  em.clearAssociation()
  em.addAssociation(ga, 0, 1)

hm = prisme.getHexa(2)
for i in range(0, 4):
  em = hm.getEdge(i+8)
  ga = aretes[ aretes_grand[i] ]
  em.clearAssociation()
  em.addAssociation(ga, 0, 1)

# Arrondir des associations implicites cylindriques
# -------------------------------------------------

for h, i, ech in [ [0, 0, 0.95], [0, 1, 0.95],  [2, 2, 0.85], [2, 3, 0.85] ]:
  hm = prisme.getHexa(h)
  em = hm.getEdge(i)
  va = em.getVertex(0).getAssociation()
  vb = em.getVertex(1).getAssociation()
  vax, vay, vaz = geompy.PointCoordinates(va)
  vbx, vby, vbz = geompy.PointCoordinates(vb)
  vmx = ( vax + vbx ) / 2.0 * ech
  vmy = ( vay + vby ) / 2.0
  vmz = ( vaz + vbz ) / 2.0
  vm = geompy.MakeVertex(vmx, vmy, vmz)
  eg = geompy.MakeArc(va, vm, vb)
  em.clearAssociation()
  em.addAssociation(eg, 0, 1)

# Test getAsso sur les edges
nbedges = doc.countEdge () 
for nro in range (nbedges) :
    edge  = doc.getEdge (nro)
    assos = edge.getAssociations()
    nbass = len (assos)
    print(" ... %d Edge = %s, nb asso = %d" % (nro, edge.getName(), nbass))
    for na in range (nbass) :
        print(" ... Asso(", na, ") = ", assos[na])


# Mailler le mod�le de bloc
# =========================

# D�finir 5 groupes de faces
# --------------------------

groupe_petit   = doc.addQuadGroup("Petit")
groupe_grand   = doc.addQuadGroup("Grand")
groupe_bas     = doc.addQuadGroup("Bas")
groupe_haut    = doc.addQuadGroup("Haut")
groupe_contour = doc.addQuadGroup("Contour")

# Constituer les groupes petit et grand
# -------------------------------------

for i in range(3):
  groupe_petit.addElement( grille_p.getQuadJK(0, i, 0) )
  groupe_grand.addElement( grille_g.getQuadJK(0, i, 0) )

# Constituer les groupes bas et haut
# ----------------------------------

for i in range(3):
  groupe_bas.addElement(  grille_p.getQuadIJ(0, i, 0) )
  groupe_bas.addElement(  grille_g.getQuadIJ(0, i, 0) )

  groupe_haut.addElement( grille_p.getQuadIJ(0, i, 1) )
  groupe_haut.addElement( grille_g.getQuadIJ(0, i, 1) )

for i in range(3):
  h = prisme.getHexa(i)
  groupe_bas.addElement(  h.getQuad(2) )
  groupe_haut.addElement( h.getQuad(3) )

# Constituer le groupe contour
# ----------------------------

for i in range(2):
  groupe_contour.addElement( grille_p.getQuadJK(1, i, 0) )

for i in [0, 2]:
  groupe_contour.addElement( grille_g.getQuadJK(1, i, 0) )

for i in range(3):
  h = prisme.getHexa(i)

  groupe_contour.addElement( h.getQuad(4) )
  groupe_contour.addElement( h.getQuad(5) )

# D�finir 3 groupes de volumes
# ----------------------------

groupe_petit  = doc.addHexaGroup("Petit")
groupe_grand  = doc.addHexaGroup("Grand")
groupe_prisme = doc.addHexaGroup("Prisme")

for i in range(3):
  groupe_petit.addElement( grille_p.getHexa(i) )
  groupe_grand.addElement( grille_g.getHexa(i) )

for i in range(3):
  groupe_prisme.addElement( prisme.getHexa(i) )

# Mailler le mod�le de bloc avec association
# ------------------------------------------

hexablock.addLaws(doc, 0.003, True)

blocs = hexablock.mesh(doc)

muv, mue, muq, muh = hexablock.dump(doc, blocs)
