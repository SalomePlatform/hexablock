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

import hexablock

doc = hexablock.addDocument("cuve")

# Construire la grille cylindrique
# --------------------------------

centre = doc.addVertex(0, 0, 0)

vecteur_px = doc.addVector(1, 0, 0)
vecteur_pz = doc.addVector(0, 0, 1)

grille = doc.makeCylindrical(centre, vecteur_px, vecteur_pz,  1, 360, 1,  3, 8, 1,  False)

# Ajouter le centre
# -----------------

quad_0 = grille.getQuadJK(0, 0, 0)
quad_6 = grille.getQuadJK(0, 6, 0)
quad_7 = grille.getQuadJK(0, 7, 0)

hexa_a = doc.addHexa3Quads(quad_0, quad_6, quad_7)

quad_2 = grille.getQuadJK(0, 2, 0)
quad_3 = grille.getQuadJK(0, 3, 0)
quad_4 = grille.getQuadJK(0, 4, 0)

hexa_b = doc.addHexa3Quads(quad_2, quad_3, quad_4)

quad_1 = grille.getQuadJK(0, 1, 0)
quad_5 = grille.getQuadJK(0, 5, 0)

quad_a = hexa_a.getQuad(1)
quad_b = hexa_b.getQuad(1)

hexa_c = doc.addHexa4Quads(quad_1, quad_a, quad_5, quad_b)


law = doc.addLaw("Uniform", 3)

for j in range(doc.countPropagation()):
    propa = doc.getPropagation(j)
    propa.setLaw(law) 

mesh_hexas = hexablock.mesh (doc)


