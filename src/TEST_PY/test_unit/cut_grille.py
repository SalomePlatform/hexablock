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

# Francis KLOSS - 2011 - CEA-Saclay, DEN, DM2S, SFME, LGLS, F-91191 Gif-sur-Yvette, France
# ========================================================================================

import hexablock
geompy = hexablock.geompy

# Construire le modèle de blocs
# =============================

nom = "exemple_cut"

# Créer le document
# -----------------

doc = hexablock.addDocument("default")

# Construire la grille sphérique
# ------------------------------

centre = doc.addVertex(0, 0, 0)

vecteur_x = doc.addVector(1, 0, 0)
vecteur_y = doc.addVector(0, 1, 0)
vecteur_z = doc.addVector(0, 0, 1)

grille = doc.makeCartesian(centre,  vecteur_x, vecteur_y, vecteur_z,  4, 3, 2)

# Associer une arête, puis couper cettee arête
# --------------------------------------------

arete = grille.getEdgeJ(0, 1, 0)

sommet_a = arete.getVertex(0)
sommet_b = arete.getVertex(1)

ax = sommet_a.getX()
ay = sommet_a.getY()
az = sommet_a.getZ()

bx = sommet_b.getX()
by = sommet_b.getY()
bz = sommet_b.getZ()

mx = (ax+bx)/2.0 - 0.25
my = (ay+by)/2.0
mz = (az+bz)/2.0

vertex_d = geompy.MakeVertex(ax, ay, az)
vertex_m = geompy.MakeVertex(mx, my, mz)
vertex_f = geompy.MakeVertex(bx, by, bz)

edge = geompy.MakeArc(vertex_d, vertex_m, vertex_f)
geompy.addToStudy(edge, "arc")
geompy.addToStudy(vertex_d, "debut")
geompy.addToStudy(vertex_m, "milieu")
geompy.addToStudy(vertex_f, "fin")

sommet_a.setAssociation(vertex_d)
sommet_b.setAssociation(vertex_f)

arete.addAssociation(edge, 0, 1)

elements = doc.cut(arete, 4)

# Définir la loi de maillage sur les propagations
# -----------------------------------------------

l = doc.addLaw("Uniform", 5)
n = doc.countPropagation()

for i in xrange(n):
  p = doc.getPropagation(i)
  p.setLaw(l)

# Générer le maillage
# -------------------
maillage = hexablock.mesh (doc)

# Afficher des informations
# -------------------------

print "Sur le document:"
print "nombre de sommets     du modèle de bloc: ", doc.countUsedVertex()
print "nombre d'arêtes       du modèle de bloc: ", doc.countUsedEdge()
print "nombre de quadrangles du modèle de bloc: ", doc.countUsedQuad()
print "nombre de blocs       du modèle de bloc: ", doc.countUsedHexa()

print "Sur le maillage:"
print "  - Nombre de noeuds     : ", maillage.NbNodes()
print "  - Nombre de segments   : ", maillage.NbEdges()
print "  - Nombre de quadrangles: ", maillage.NbQuadrangles()
print "  - Nombre d'hexaèdres   : ", maillage.NbHexas()
