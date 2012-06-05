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

# Francis KLOSS - 2011 - CEA-Saclay, DEN, DM2S, SFME, LGLS, F-91191 Gif-sur-Yvette, France
# ========================================================================================

import geompy
import hexablock

# D�finir les param�tres
# ----------------------

nom = "bielle"

# Construire le mod�le de bloc
# ============================

doc = hexablock.addDocument(nom)

# Construire les 2 grilles cylindriques
# -------------------------------------

centre_a = doc.addVertex(0, 0, 0)
centre_b = doc.addVertex(6, 0, 0)

vecteur_a = doc.addVector(1, 1, 0)
vecteur_z = doc.addVector(0, 0, 1)
vecteur_b = doc.addVector(6, 0, 0)

grille_a = doc.makeCylindrical(centre_a, vecteur_a, vecteur_z,  1, 360, 1,  1, 4, 1,  False)

### grille_b = doc.makeTranslation(grille_a, vecteur_b)
grille_b = doc.makeCylindrical(centre_b, vecteur_a, vecteur_z,  2, 360, 1,  1, 4, 1,  False)

grilles = [ grille_a, grille_b ]

# Relier les 2 grilles
# --------------------

quad_a = grille_a.getQuadJK(1, 3, 0)
quad_b = grille_b.getQuadJK(1, 1, 0)

point_a1 = grille_a.getVertexIJK(1, 0, 0)
point_a2 = grille_a.getVertexIJK(1, 3, 0)

point_b1 = grille_b.getVertexIJK(1, 1, 0)
point_b2 = grille_b.getVertexIJK(1, 2, 0)

prisme = doc.joinQuad(quad_a, quad_b,  point_a1, point_b1,  point_a2, point_b2,  3)

# Associer le mod�le de bloc avec la g�om�trie
# ============================================

# Charger la g�om�trie � associer
# -------------------------------

bielle = geompy.ImportSTEP("crank.stp")

doc.setShape(bielle)

# Extraire les subshapes de la g�om�trie
# --------------------------------------

g_vertices = geompy.SubShapeAllSortedCentres(bielle, geompy.ShapeType["VERTEX"])
g_edges    = geompy.SubShapeAllSortedCentres(bielle, geompy.ShapeType["EDGE"  ])

sommets = [ [ 8, 6, 9, 7 ], [ 12, 10, 13, 11 ] ]

cercles = [ [2, 3], [25, 26] ]
arcs_gr = [ [0, 1], [27, 28] ]
arcs_pe = [ [ 9, 7,  10, 8 ], [ 21, 19,  22, 20 ] ]

# Associer les 4 cercles int�rieurs
# ---------------------------------

for k in xrange(0, 2):
    for g in xrange(0, 2):
        grille = grilles[g]
        ve = grille.getVertexIJK(0, 1, k)
        le = [ grille.getEdgeJ(0, j, k) for j in xrange(0, 4) ]
        cercle  = g_edges[ cercles[g][k] ]
        doc.associateClosedLine(ve, le[0], le[1:], cercle, 0.625, False, [])

# Associer les 4 grands arcs de cercle ext�rieurs
# -----------------------------------------------

for k in xrange(0, 2):
  g=0
  grille = grilles[g]
  le = [ grille.getEdgeJ(1, j, k) for j in [2, 1, 0] ]
  ed = g_edges[ arcs_gr[g][k] ]
  doc.associateOpenedLine(le[0], le[1:], ed, 0, [], 1)

for k in xrange(0, 2):
  g=1
  grille = grilles[g]
  le = [ grille.getEdgeJ(1, j, k) for j in [0, 3, 2] ]
  ed = g_edges[ arcs_gr[g][k] ]
  doc.associateOpenedLine(le[0], le[1:], ed, 0, [], 1)

# Associer les 8 sommets des petits arcs de cercle ext�rieurs
# -----------------------------------------------------------

for g, h in [ [0, 1], [1, 2] ]:
  hexa = prisme.getHexa(h)
  for vi in xrange(0, 4):
    nv = 2*(vi/2) + 1 - vi % 2
    vm = hexa.getVertex(nv)
    vg = g_vertices[ sommets[g][vi] ]
    x, y, z = geompy.PointCoordinates(vg)
    vm.setAssociation(vg)

# Associer les 8 petits arcs de cercle ext�rieurs
# -----------------------------------------------

for g, h in [ [0, 0], [1, 2] ]:
  hexa = prisme.getHexa(h)
  for ei in xrange(0, 4):
    nv = 2*(ei/2) + 1 - ei % 2
    em = hexa.getEdge(nv+8)
    eg = g_edges[ arcs_pe[g][ei] ]
    em.clearAssociation()
    em.addAssociation(eg, 0, 1)

# Associer 4 arcs nouveaux qui compl�tent les 4 grands arcs de cercle ext�rieurs
# ------------------------------------------------------------------------------

for h, ei, ech in [ [0, 0, 0.9], [0, 1, 0.9],  [2, 2, 0.85], [2, 3, 0.85] ]:
  hexa = prisme.getHexa(h)
  em = hexa.getEdge(ei)
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

# Mailler le mod�le de bloc
# =========================

# D�finir 5 groupes de faces
# --------------------------

groupe_trou_p  = doc.addQuadGroup("Trou_petit")
groupe_trou_g  = doc.addQuadGroup("Trou_grand")
groupe_bas     = doc.addQuadGroup("Bas")
groupe_haut    = doc.addQuadGroup("Haut")
groupe_contour = doc.addQuadGroup("Contour")

for i in xrange(4):
  groupe_trou_p.addElement(grille_a.getQuadJK(0, i, 0))
  groupe_trou_g.addElement(grille_b.getQuadJK(0, i, 0))

  groupe_bas.addElement( grille_a.getQuadIJ(0, i, 0))
  groupe_bas.addElement( grille_b.getQuadIJ(0, i, 0))
  groupe_haut.addElement(grille_a.getQuadIJ(0, i, 1))
  groupe_haut.addElement(grille_b.getQuadIJ(0, i, 1))

for i in xrange(3):
  groupe_contour.addElement(grille_a.getQuadJK(1, i, 0))

for i in [0, 2, 3]:
  groupe_contour.addElement(grille_b.getQuadJK(1, i, 0))

for i in xrange(0, 3):
  h = prisme.getHexa(i)

  q = h.getQuad(2)
  groupe_bas.addElement(q)

  q = h.getQuad(3)
  groupe_haut.addElement(q)

  q = h.getQuad(4)
  groupe_contour.addElement(q)
  q = h.getQuad(5)
  groupe_contour.addElement(q)

# D�finir 3 groupes de volumes
# ----------------------------

groupe_cyli_p = doc.addHexaGroup("Cylindre_petit")
groupe_cyli_g = doc.addHexaGroup("Cylindre_grand")
groupe_prisme = doc.addHexaGroup("Prisme")

for i in xrange(4):
  groupe_cyli_p.addElement(grille_a.getHexa(i))
  groupe_cyli_g.addElement(grille_b.getHexa(i))

for i in xrange(3):
  groupe_prisme.addElement(prisme.getHexa(i))

# Mailler le mod�le de bloc avec association
# ------------------------------------------

l = doc.addLaw("Uniform1", 17)
n = doc.countPropagation()

for i in xrange(n):
  p = doc.getPropagation(i)
  p.setLaw(l)

l = doc.addLaw("Uniform2", 40)
p = doc.getPropagation(11)
p.setLaw(l)

blocs = hexablock.mesh (doc)

print "nombre de sommets     du mod�le de bloc: ", doc.countUsedVertex()
print "nombre d'ar�tes       du mod�le de bloc: ", doc.countUsedEdge()
print "nombre de quadrangles du mod�le de bloc: ", doc.countUsedQuad()
print "nombre de blocs       du mod�le de bloc: ", doc.countUsedHexa()

print "Nombre de noeuds      du maillage: ", blocs.NbNodes()
print "Nombre de segments    du maillage: ", blocs.NbEdges()
print "Nombre de quadrangles du maillage: ", blocs.NbQuadrangles()
print "Nombre d'hexa�dres    du maillage: ", blocs.NbHexas()
