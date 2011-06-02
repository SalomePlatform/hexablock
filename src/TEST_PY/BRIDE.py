# -*- coding: latin-1 -*-
# Copyright (C) 2009-2011  CEA/DEN, EDF R&D
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


#===============================================
# By Karima DEBCHI YATAGHENE Dec 2009 in CS 
#===============================================


import geompy
import smesh
import hexablock
import math



#=============================
# CREATION DOCUMENT
#=============================

doc = hexablock.addDocument()

#=============================
# CREATION DU MODELE
#=============================

# Pour la bride on doit creer plusieurs grilles cylindriques

#=============================
# PARAMETRES
#=============================

h = 70
nl = 7

# Taille de la 1ere grille cylindrique
r1 = 12
r1_t = 7.88
R = r1-r1_t
a1 = 45
l1 = h

nr1 = 4.0
na1 = 1.0
nl1 = nl = 1.0

# Taille de la 2eme grille cylindrique
r2 = 20
r2_t = 2
r = r2-r2_t
a2 = 180
l2 = h

nr2 = 5.0
na2 = 5.0
nl2 = nl = 1.0


#=============================
# Creation des vecteurs
#=============================

dx = doc.addVector( h , 0 , 0 )
dy = doc.addVector( 0 , h , 0 )
dz = doc.addVector( 0 , 0 , h )


#=================================================
# Creation des centres des grilles cylindriques
#=================================================

P = 40
c1 = doc.addVertex( 0 , 0 , h )
c2 = doc.addVertex( P , 0 , h )


#=================================================
# Creation de la 1ere grille cylindrique
#=================================================

grille_cyl_1 = doc.makeCylindrical( c1 , dx , dz , R , a1 , l1 , nr1 , na1 , nl1 , False )


ni = nr1 = 4
nj = na1 = 1
nk = nl1 = 1

0 <= i < ni
0 <= j < nj
0 <= k < nk

# reperer les vertex

ni = nr1 = 4
nj = na1 = 1
nk = nl1 = 1

0 <= i < ni + 1 = 5
0 <= j < nj + 1 = 2
0 <= k < nk + 1 = 2

vtx_B_g1 = grille_cyl_1.getVertexXYZ( 2 , 0 , 0 )
vtx_B_g1 = grille_cyl_1.getVertexXYZ( 2 , 0 , 0 )


# reperer les quads

ni = nr1 = 4
nj = na1 = 1
nk = nl1 = 1

0 <= i < ni + 1 = 5
0 <= j < nj = 1
0 <= k < nk = 1

quad_A_g1 = grille_cyl_1.getQuadYZ( 5 , 0 , 0 )

0 <= i < ni = 4
0 <= j < nj + 1 = 2
0 <= k < nk = 2

quad_B_g1 = grille_cyl_1.getQuadXZ( 3 , 0 , 0 )
quad_C_g1 = grille_cyl_1.getQuadXZ( 2 , 0 , 0 )


# reperer les hexas

ni = nr1 = 4
nj = na1 = 1
nk = nl1 = 1

0 <= i < ni = 4
0 <= j < nj = 1
0 <= k < nk = 1

hexa_A_g1 = grille_cyl_1.getHexaXYZ( 3 , 0 , 0 )
hexa_B_g1 = grille_cyl_1.getHexaXYZ( 2 , 0 , 0 )




#=================================================
# Creation de la 2eme grille cylindrique
#=================================================

grille_cyl_2 = doc.makeCylindrical( c2 , dx , dz , r , a2 , l2 , nr2 , na2 , nl2 , False )

ni = nr2 = 5
nj = na2 = 5
nk = nl2 = 1

0 <= i < ni
0 <= j < nj
0 <= k < nk

# reperer les vertex

ni = nr2 = 5
nj = na2 = 5
nk = nl2 = 1

0 <= i < ni + 1 = 5
0 <= j < nj + 1 = 2
0 <= k < nk + 1 = 2

vtx_B_g2 = grille_cyl_2.getVertexXYZ( 2 , 0 , 0 )
vtx_B_g2 = grille_cyl_2.getVertexXYZ( 2 , 0 , 0 )


# reperer les quads

ni = nr2 = 5
nj = na2 = 5
nk = nl2 = 1

0 <= i < ni = 4
0 <= j < nj + 1 = 2
0 <= k < nk = 2

quad_A_g2 = grille_cyl_2.getQuadXZ( 2 , 0 , 0 )
quad_B_g2 = grille_cyl_2.getQuadXZ( 3 , 0 , 0 )

0 <= i < ni + 1 = 5
0 <= j < nj = 1
0 <= k < nk = 1

quad_AB_g2 = grille_cyl_2.getQuadXZ( 4 , 3 , 0 )


# reperer les hexas

ni = nr2 = 5
nj = na2 = 5
nk = nl2 = 1

0 <= i < ni = 4
0 <= j < nj = 1
0 <= k < nk = 1

hexa_A_g2 = grille_cyl_2.getHexaXYZ( 4 , 2 , 0 )
hexa_B_g2 = grille_cyl_2.getHexaXYZ( 4 , 3 , 0 )
hexa_C_g2 = grille_cyl_2.getHexaXYZ( 4 , 4 , 0 )

# deconnexion des quads

quad_A_g2 = doc.disconnectQuad( hexa_A_g2 , quad_AB_g2 )
quad_B_g2 = doc.disconnectQuad( hexa_B_g2 , quad_AB_g2 )



# fusionner les quads

fusion_g1_g2_quad_A = doc.mergeQuads( quad_A_g2 , quad_A_g1 , vtx_A_1_g2 , vtx_A_1_g1 , vtx_A_2_g2 , vtx_A_2_g1 )
fusion_g1_g2_quad_B = doc.mergeQuads( quad_B_g2 , quad_B_g1 , vtx_B_1_g2 , vtx_B_1_g1 , vtx_B_2_g2 , vtx_B_2_g1 )

fusion_g1_g2 = doc.mergeEdges( edge_B_g2 , edge_C_g1 , vtx_B_h_g2 , vtx_C_h_g1 )



# rajouter l hexa qui manque entre les deux grilles

edge_h = doc.addEdge( vtx_C_h_g2 , vtx_C_h_g1 )
edge_b = doc.addEdge( vtx_C_b_g2 , vtx_C_b_g1 )

quad_h = doc.addQuad( edge_B_h_g2 , edge_C_h_g2 , edge_h , edge_C_h_g1 )
quad_b = doc.addQuad( edge_B_b_g2 , edge_C_b_g2 , edge_b , edge_C_b_g1 )
quad_f = doc.addQuad( edge_h , edge_C_g2 , edge_C_g1 , edge_b )

hexa_g2_g1 = doc.addHexa( quad_B_g2 , quad_C_g2 , quad_f , quad_C_g1 , quad_h , quad_b )


#========================================================================================
# rajouter trois hexa qui servent au remplissage de la partie centrale de la 2eme grille
#========================================================================================


# reperer les vertex

ni = nr2 = 5
nj = na2 = 5
nk = nl2 = 1

0 <= i < ni + 1 = 6
0 <= j < nj + 1 = 6
0 <= k < nk + 1 = 2

vtx_1_h_g2 = grille_cyl_2.getVertexXYZ( 0 , 0 , 1 )
vtx_2_h_g2 = grille_cyl_2.getVertexXYZ( 0 , 1 , 1 )
vtx_3_h_g2 = grille_cyl_2.getVertexXYZ( 0 , 2 , 1 )
vtx_4_h_g2 = grille_cyl_2.getVertexXYZ( 0 , 3 , 1 )
vtx_5_h_g2 = grille_cyl_2.getVertexXYZ( 0 , 4 , 1 )
vtx_6_h_g2 = grille_cyl_2.getVertexXYZ( 0 , 5 , 1 )

vtx_1_b_g2 = grille_cyl_2.getVertexXYZ( 0 , 0 , 0 )
vtx_2_b_g2 = grille_cyl_2.getVertexXYZ( 0 , 1 , 0 )
vtx_3_b_g2 = grille_cyl_2.getVertexXYZ( 0 , 2 , 0 )
vtx_4_b_g2 = grille_cyl_2.getVertexXYZ( 0 , 3 , 0 )
vtx_5_b_g2 = grille_cyl_2.getVertexXYZ( 0 , 4 , 0 )
vtx_6_b_g2 = grille_cyl_2.getVertexXYZ( 0 , 5 , 0 )


vtx_7_h_g2 = doc.addVertex( P + 0.5 , 0 , h )
vtx_8_h_g2 = doc.addVertex( P - 0.5 , 0 , h )

Pt_dtr_1 = doc.makeVertex( P , 0 , h )
Pt_dtr_2 = doc.makeVertex( P , 0 , 0 )

dtr = doc.makeVectorVertices( Pt_dtr_2 , Pt_dtr_1 )

vtx_7_b_g2 = doc.makeTranslation( vtx_1_b_g2 , dtr )
vtx_8_b_g2 = doc.makeTranslation( vtx_6_b_g2 , dtr )



# reperer les edges

ni = nr2 = 5
nj = na2 = 5
nk = nl2 = 1

# edge Y

0 <= i < ni + 1 = 6
0 <= j < nj = 5
0 <= k < nk + 1 = 2

edge_1_2_h_g2 = grille_cyl_2.getEdgeY( 0 , 0 , 1 )
edge_2_3_h_g2 = grille_cyl_2.getEdgeY( 1 , 0 , 1 )
edge_3_4_h_g2 = grille_cyl_2.getEdgeY( 2 , 0 , 1 )
edge_4_5_h_g2 = grille_cyl_2.getEdgeY( 3 , 0 , 1 )
edge_5_6_h_g2 = grille_cyl_2.getEdgeY( 4 , 0 , 1 )

edge_1_2_b_g2 = grille_cyl_2.getEdgeY( 0 , 0 , 0 )
edge_2_3_b_g2 = grille_cyl_2.getEdgeY( 1 , 0 , 0 )
edge_3_4_b_g2 = grille_cyl_2.getEdgeY( 2 , 0 , 0 )
edge_4_5_b_g2 = grille_cyl_2.getEdgeY( 3 , 0 , 0 )
edge_5_6_b_g2 = grille_cyl_2.getEdgeY( 4 , 0 , 0 )

# edge Z

0 <= i < ni + 1 = 6
0 <= j < nj + 1 = 6
0 <= k < nk = 1

edge_1_z_g2 = grille_cyl_2.getEdgeZ( 0 , 0 , 0 )
edge_2_z_g2 = grille_cyl_2.getEdgeZ( 0 , 1 , 0 )
edge_3_z_g2 = grille_cyl_2.getEdgeZ( 0 , 2 , 0 )
edge_4_z_g2 = grille_cyl_2.getEdgeZ( 0 , 3 , 0 )
edge_5_z_g2 = grille_cyl_2.getEdgeZ( 0 , 4 , 0 )
edge_6_z_g2 = grille_cyl_2.getEdgeZ( 0 , 5 , 0 )


# creer les edges manquants

edge_1_7_h_g2 = doc.addEdge( vtx_1_h_g2 , vtx_7_h_g2 )
edge_3_7_h_g2 = doc.addEdge( vtx_7_h_g2 , vtx_3_h_g2 )
edge_7_8_h_g2 = doc.addEdge( vtx_7_h_g2 , vtx_8_h_g2 )
edge_4_8_h_g2 = doc.addEdge( vtx_4_h_g2 , vtx_8_h_g2 )
edge_6_8_h_g2 = doc.addEdge( vtx_6_h_g2 , vtx_8_h_g2 )

edge_1_7_b_g2 = doc.addEdge( vtx_1_b_g2 , vtx_7_b_g2 )
edge_3_7_b_g2 = doc.addEdge( vtx_7_b_g2 , vtx_3_b_g2 )
edge_7_8_b_g2 = doc.addEdge( vtx_7_b_g2 , vtx_8_b_g2 )
edge_4_8_b_g2 = doc.addEdge( vtx_4_b_g2 , vtx_8_b_g2 )
edge_6_8_b_g2 = doc.addEdge( vtx_6_b_g2 , vtx_8_b_g2 )

edge_7_h_b_g2 = doc.addEdge( vtx_7_h_g2 , vtx_7_b_g2 )
edge_8_h_b_g2 = doc.addEdge( vtx_8_h_g2 , vtx_8_b_g2 )


# creer les quads manquants

quad_7_h_g2 = doc.addQuad( edge_1_2_h_g2 , edge_2_3_h_g2 , edge_3_7_h_g2 , edge_1_7_h_g2 )
quad_7_8_h_g2 = doc.addQuad( edge_3_4_h_g2 , edge_4_8_h_g2 , edge_7_8_h_g2 , edge_3_7_h_g2 )
quad_8_h_g2 = doc.addQuad( edge_4_5_h_g2 , edge_5_6_h_g2 , edge_6_8_h_g2 , edge_4_8_h_g2 )

quad_7_b_g2 = doc.addQuad( edge_1_2_b_g2 , edge_2_3_b_g2 , edge_3_7_b_g2 , edge_1_7_b_g2 )
quad_7_8_b_g2 = doc.addQuad( edge_3_4_b_g2 , edge_4_8_b_g2 , edge_7_8_b_g2 , edge_3_7_b_g2 )
quad_8_b_g2 = doc.addQuad( edge_4_5_b_g2 , edge_5_6_b_g2 , edge_6_8_b_g2 , edge_4_8_b_g2 )

quad_7_f_g2 = doc.addQuad( edge_1_7_h_g2 , edge_1_z_g2 , edge_1_7_b_g2 , edge_7_h_b_g2 )
quad_7_8_f_g2 = doc.addQuad( edge_7_8_h_g2 , edge_7_h_b_g2 , edge_7_8_b_g2 , edge_8_h_b_g2 )
quad_8_f_g2 = doc.addQuad( edge_6_8_h_g2 , edge_6_z_g2 , edge_6_8_b_g2 , edge_8_h_b_g2 )

quad_3_7_f_g2 = doc.addQuad( edge_3_z_g2 , edge_3_7_h_g2 , edge_7_h_b_g2 , edge_3_7_b_g2 )
quad_4_8_f_g2 = doc.addQuad( edge_4_z_g2 , edge_4_8_h_g2 , edge_8_h_b_g2 , edge_4_8_b_g2 )



# reperer les quads

ni = nr2 = 5
nj = na2 = 5
nk = nl2 = 1

0 <= i < ni + 1 = 6
0 <= j < nj = 5
0 <= k < nk = 1

quad_1_2_z_g2 = grille_cyl_2.getQuadYZ( 0 , 0 , 0 )
quad_2_3_z_g2 = grille_cyl_2.getQuadYZ( 0 , 1 , 0 )
quad_3_4_z_g2 = grille_cyl_2.getQuadYZ( 0 , 2 , 0 )
quad_4_5_z_g2 = grille_cyl_2.getQuadYZ( 0 , 3 , 0 )
quad_5_6_z_g2 = grille_cyl_2.getQuadYZ( 0 , 4 , 0 )




# creer les hexas manquants

hexa_rmp_7_g2 = doc.addHexa( quad_1_2_z_g2 , quad_2_3_z_g2 , quad_3_7_f_g2 , quad_7_f_g2 , quad_7_h_g2 , quad_7_b_g2 )

hexa_rmp_7_8_g2 = doc.addHexa( quad_3_7_f_g2 , quad_3_4_z_g2 , quad_4_8_f_g2 , quad_7_8_f_g2 , quad_7_8_h_g2 , quad_7_8_b_g2 )

hexa_rmp_8_g2 = doc.addHexa( quad_4_8_f_g2 , quad_4_5_z_g2 , quad_5_6_z_g2 , quad_8_f_g2 , quad_8_h_g2 , quad_8_b_g2 )


#=======================================
# couper le modele pour avoir 7 couches
#=======================================

# repérer l'edge qui sert à l'opération cut

0 <= i < ni + 1 = 6
0 <= j < nj + 1 = 6
0 <= k < nk = 1

edge_cut = grille_cyl_2.getEdgeZ( 5 , 0 , 0 )

mod_7_couche = doc.cut( edge_cut , 7 )


#===============================================
# reperer les hexas en plus pour les supprimer
#===============================================

# reperer les hexas

ni = nr2 = 5
nj = na2 = 5
nk = nl2 = 1

0 <= i < ni = 4
0 <= j < nj = 1
0 <= k < nk = 1

# 1ere grille

ni = nr1 = 4
nj = na1 = 1
nk = nl1 = 7

0 <= i < ni = 4
0 <= j < nj = 1
0 <= k < nk = 1

hexa_1_x_3_z_g1 = grille_cyl_1.getHexaXYZ( 1 , 0 , 3 )
hexa_1_x_4_z_g1 = grille_cyl_1.getHexaXYZ( 1 , 0 , 4 )
hexa_1_x_5_z_g1 = grille_cyl_1.getHexaXYZ( 1 , 0 , 5 )
hexa_1_x_6_z_g1 = grille_cyl_1.getHexaXYZ( 1 , 0 , 6 )

hexa_2_x_3_z_g1 = grille_cyl_1.getHexaXYZ( 2 , 0 , 3 )
hexa_2_x_4_z_g1 = grille_cyl_1.getHexaXYZ( 2 , 0 , 4 )
hexa_2_x_5_z_g1 = grille_cyl_1.getHexaXYZ( 2 , 0 , 5 )
hexa_2_x_6_z_g1 = grille_cyl_1.getHexaXYZ( 2 , 0 , 6 )

hexa_3_x_3_z_g1 = grille_cyl_1.getHexaXYZ( 3 , 0 , 3 )
hexa_3_x_4_z_g1 = grille_cyl_1.getHexaXYZ( 3 , 0 , 4 )
hexa_3_x_5_z_g1 = grille_cyl_1.getHexaXYZ( 3 , 0 , 5 )
hexa_3_x_6_z_g1 = grille_cyl_1.getHexaXYZ( 3 , 0 , 6 )


hexa_list_g1 = [ hexa_1_x_3_z_g1 , hexa_1_x_4_z_g1 , hexa_1_x_5_z_g1 , hexa_1_x_6_z_g1 , hexa_2_x_3_z_g1 , hexa_2_x_4_z_g1 , hexa_2_x_5_z_g1 , hexa_2_x_6_z_g1 , hexa_3_x_3_z_g1 , hexa_3_x_4_z_g1 , hexa_3_x_5_z_g1 , hexa_3_x_6_z_g1 ]

for hexa_i in range(hexa_list_g1):
	doc.removeHexa(hexa_i)


# 2eme grille

ni = nr2 = 5
nj = na2 = 5
nk = nl2 = 7

0 <= i < ni = 5
0 <= j < nj = 5
0 <= k < nk = 7

# 1er range x = 0

hexa_0_x_0_y_6_z_g2 = grille_cyl_2.getHexaXYZ( 0 , 0 , 6 )
hexa_0_x_1_y_6_z_g2 = grille_cyl_2.getHexaXYZ( 0 , 1 , 6 )
hexa_0_x_2_y_6_z_g2 = grille_cyl_2.getHexaXYZ( 0 , 2 , 6 )
hexa_0_x_3_y_6_z_g2 = grille_cyl_2.getHexaXYZ( 0 , 3 , 6 )
hexa_0_x_4_y_6_z_g2 = grille_cyl_2.getHexaXYZ( 0 , 4 , 6 )


hexa_list_x_0_g2 = [ hexa_0_x_0_y_6_z_g2 , hexa_0_x_1_y_6_z_g2 , hexa_0_x_2_y_6_z_g2 , hexa_0_x_3_y_6_z_g2 , hexa_0_x_4_y_6_z_g2 ]

for hexa_i in range(hexa_list_x_0_g2):
	doc.removeHexa(hexa_i)

# 2eme range x = 1

hexa_1_x_0_y_0_z_g2 = grille_cyl_2.getHexaXYZ( 1 , 0 , 0 )
hexa_1_x_1_y_0_z_g2 = grille_cyl_2.getHexaXYZ( 1 , 1 , 0 )
hexa_1_x_2_y_0_z_g2 = grille_cyl_2.getHexaXYZ( 1 , 2 , 0 )
hexa_1_x_3_y_0_z_g2 = grille_cyl_2.getHexaXYZ( 1 , 3 , 0 )
hexa_1_x_4_y_0_z_g2 = grille_cyl_2.getHexaXYZ( 1 , 4 , 0 )

hexa_1_x_0_y_1_z_g2 = grille_cyl_2.getHexaXYZ( 1 , 0 , 1 )
hexa_1_x_1_y_1_z_g2 = grille_cyl_2.getHexaXYZ( 1 , 1 , 1 )
hexa_1_x_2_y_1_z_g2 = grille_cyl_2.getHexaXYZ( 1 , 2 , 1 )
hexa_1_x_3_y_1_z_g2 = grille_cyl_2.getHexaXYZ( 1 , 3 , 1 )
hexa_1_x_4_y_1_z_g2 = grille_cyl_2.getHexaXYZ( 1 , 4 , 1 )

hexa_1_x_0_y_2_z_g2 = grille_cyl_2.getHexaXYZ( 1 , 0 , 2 )
hexa_1_x_1_y_2_z_g2 = grille_cyl_2.getHexaXYZ( 1 , 1 , 2 )
hexa_1_x_2_y_2_z_g2 = grille_cyl_2.getHexaXYZ( 1 , 2 , 2 )
hexa_1_x_3_y_2_z_g2 = grille_cyl_2.getHexaXYZ( 1 , 3 , 2 )
hexa_1_x_4_y_2_z_g2 = grille_cyl_2.getHexaXYZ( 1 , 4 , 2 )

hexa_1_x_0_y_3_z_g2 = grille_cyl_2.getHexaXYZ( 1 , 0 , 3 )
hexa_1_x_1_y_3_z_g2 = grille_cyl_2.getHexaXYZ( 1 , 1 , 3 )
hexa_1_x_2_y_3_z_g2 = grille_cyl_2.getHexaXYZ( 1 , 2 , 3 )
hexa_1_x_3_y_3_z_g2 = grille_cyl_2.getHexaXYZ( 1 , 3 , 3 )
hexa_1_x_4_y_3_z_g2 = grille_cyl_2.getHexaXYZ( 1 , 4 , 3 )

hexa_1_x_0_y_5_z_g2 = grille_cyl_2.getHexaXYZ( 1 , 0 , 5 )
hexa_1_x_1_y_5_z_g2 = grille_cyl_2.getHexaXYZ( 1 , 1 , 5 )
hexa_1_x_2_y_5_z_g2 = grille_cyl_2.getHexaXYZ( 1 , 2 , 5 )
hexa_1_x_3_y_5_z_g2 = grille_cyl_2.getHexaXYZ( 1 , 3 , 5 )
hexa_1_x_4_y_5_z_g2 = grille_cyl_2.getHexaXYZ( 1 , 4 , 5 )

hexa_1_x_0_y_6_z_g2 = grille_cyl_2.getHexaXYZ( 1 , 0 , 6 )
hexa_1_x_1_y_6_z_g2 = grille_cyl_2.getHexaXYZ( 1 , 1 , 6 )
hexa_1_x_2_y_6_z_g2 = grille_cyl_2.getHexaXYZ( 1 , 2 , 6 )
hexa_1_x_3_y_6_z_g2 = grille_cyl_2.getHexaXYZ( 1 , 3 , 6 )
hexa_1_x_4_y_6_z_g2 = grille_cyl_2.getHexaXYZ( 1 , 4 , 6 )


hexa_list_x_1_g2 = [ hexa_1_x_0_y_0_z_g2 , hexa_1_x_1_y_0_z_g2 , hexa_1_x_2_y_0_z_g2 , hexa_1_x_3_y_0_z_g2 , hexa_1_x_4_y_0_z_g2 , hexa_1_x_0_y_1_z_g2 , hexa_1_x_1_y_1_z_g2 , hexa_1_x_2_y_1_z_g2 , hexa_1_x_3_y_1_z_g2 , hexa_1_x_4_y_1_z_g2 , hexa_1_x_0_y_2_z_g2 , hexa_1_x_1_y_2_z_g2 , hexa_1_x_2_y_2_z_g2 , hexa_1_x_3_y_2_z_g2 , hexa_1_x_4_y_2_z_g2 , hexa_1_x_0_y_3_z_g2 , hexa_1_x_1_y_3_z_g2 , hexa_1_x_2_y_3_z_g2 , hexa_1_x_3_y_3_z_g2 , hexa_1_x_4_y_3_z_g2 , hexa_1_x_0_y_5_z_g2 , hexa_1_x_1_y_5_z_g2 , hexa_1_x_2_y_5_z_g2 , hexa_1_x_3_y_5_z_g2 , hexa_1_x_4_y_5_z_g2 , hexa_1_x_0_y_6_z_g2 , hexa_1_x_1_y_6_z_g2 , hexa_1_x_2_y_6_z_g2 , hexa_1_x_3_y_6_z_g2 , hexa_1_x_4_y_6_z_g2 ]

for hexa_i in range(hexa_list_x_1_g2):
	doc.removeHexa(hexa_i)


# 3eme range x = 2

hexa_2_x_0_y_0_z_g2 = grille_cyl_2.getHexaXYZ( 2 , 0 , 0 )
hexa_2_x_1_y_0_z_g2 = grille_cyl_2.getHexaXYZ( 2 , 1 , 0 )
hexa_2_x_2_y_0_z_g2 = grille_cyl_2.getHexaXYZ( 2 , 2 , 0 )
hexa_2_x_3_y_0_z_g2 = grille_cyl_2.getHexaXYZ( 2 , 3 , 0 )
hexa_2_x_4_y_0_z_g2 = grille_cyl_2.getHexaXYZ( 2 , 4 , 0 )

hexa_2_x_0_y_1_z_g2 = grille_cyl_2.getHexaXYZ( 2 , 0 , 1 )
hexa_2_x_1_y_1_z_g2 = grille_cyl_2.getHexaXYZ( 2 , 1 , 1 )
hexa_2_x_2_y_1_z_g2 = grille_cyl_2.getHexaXYZ( 2 , 2 , 1 )
hexa_2_x_3_y_1_z_g2 = grille_cyl_2.getHexaXYZ( 2 , 3 , 1 )
hexa_2_x_4_y_1_z_g2 = grille_cyl_2.getHexaXYZ( 2 , 4 , 1 )

hexa_2_x_0_y_5_z_g2 = grille_cyl_2.getHexaXYZ( 2 , 0 , 5 )
hexa_2_x_1_y_5_z_g2 = grille_cyl_2.getHexaXYZ( 2 , 1 , 5 )
hexa_2_x_2_y_5_z_g2 = grille_cyl_2.getHexaXYZ( 2 , 2 , 5 )
hexa_2_x_3_y_5_z_g2 = grille_cyl_2.getHexaXYZ( 2 , 3 , 5 )
hexa_2_x_4_y_5_z_g2 = grille_cyl_2.getHexaXYZ( 2 , 4 , 5 )

hexa_2_x_0_y_6_z_g2 = grille_cyl_2.getHexaXYZ( 2 , 0 , 6 )
hexa_2_x_1_y_6_z_g2 = grille_cyl_2.getHexaXYZ( 2 , 1 , 6 )
hexa_2_x_2_y_6_z_g2 = grille_cyl_2.getHexaXYZ( 2 , 2 , 6 )
hexa_2_x_3_y_6_z_g2 = grille_cyl_2.getHexaXYZ( 2 , 3 , 6 )
hexa_2_x_4_y_6_z_g2 = grille_cyl_2.getHexaXYZ( 2 , 4 , 6 )


hexa_list_x_2_g2 = [ hexa_2_x_0_y_0_z_g2 , hexa_2_x_1_y_0_z_g2 , hexa_2_x_2_y_0_z_g2 , hexa_2_x_3_y_0_z_g2 , hexa_2_x_4_y_0_z_g2 , hexa_2_x_0_y_1_z_g2 , hexa_2_x_1_y_1_z_g2 , hexa_2_x_2_y_1_z_g2 , hexa_2_x_3_y_1_z_g2 , hexa_2_x_4_y_1_z_g2 , hexa_2_x_0_y_5_z_g2 , hexa_2_x_1_y_5_z_g2 , hexa_2_x_2_y_5_z_g2 , hexa_2_x_3_y_5_z_g2 , hexa_2_x_4_y_5_z_g2 , hexa_2_x_0_y_6_z_g2 , hexa_2_x_1_y_6_z_g2 , hexa_2_x_2_y_6_z_g2 , hexa_2_x_3_y_6_z_g2 , hexa_2_x_4_y_6_z_g2 ]

for hexa_i in range(hexa_list_x_2_g2):
	doc.removeHexa(hexa_i)



# 4eme range x = 3

hexa_3_x_0_y_0_z_g2 = grille_cyl_2.getHexaXYZ( 3 , 0 , 0 )
hexa_3_x_1_y_0_z_g2 = grille_cyl_2.getHexaXYZ( 3 , 1 , 0 )
hexa_3_x_2_y_0_z_g2 = grille_cyl_2.getHexaXYZ( 3 , 2 , 0 )
hexa_3_x_3_y_0_z_g2 = grille_cyl_2.getHexaXYZ( 3 , 3 , 0 )
hexa_3_x_4_y_0_z_g2 = grille_cyl_2.getHexaXYZ( 3 , 4 , 0 )

hexa_3_x_0_y_1_z_g2 = grille_cyl_2.getHexaXYZ( 3 , 0 , 1 )
hexa_3_x_1_y_1_z_g2 = grille_cyl_2.getHexaXYZ( 3 , 1 , 1 )
hexa_3_x_2_y_1_z_g2 = grille_cyl_2.getHexaXYZ( 3 , 2 , 1 )
hexa_3_x_3_y_1_z_g2 = grille_cyl_2.getHexaXYZ( 3 , 3 , 1 )
hexa_3_x_4_y_1_z_g2 = grille_cyl_2.getHexaXYZ( 3 , 4 , 1 )

hexa_3_x_0_y_4_z_g2 = grille_cyl_2.getHexaXYZ( 3 , 0 , 4 )
hexa_3_x_1_y_4_z_g2 = grille_cyl_2.getHexaXYZ( 3 , 1 , 4 )
hexa_3_x_2_y_4_z_g2 = grille_cyl_2.getHexaXYZ( 3 , 2 , 4 )
hexa_3_x_3_y_4_z_g2 = grille_cyl_2.getHexaXYZ( 3 , 3 , 4 )
hexa_3_x_4_y_4_z_g2 = grille_cyl_2.getHexaXYZ( 3 , 4 , 4 )

hexa_3_x_0_y_5_z_g2 = grille_cyl_2.getHexaXYZ( 3 , 0 , 5 )
hexa_3_x_1_y_5_z_g2 = grille_cyl_2.getHexaXYZ( 3 , 1 , 5 )
hexa_3_x_2_y_5_z_g2 = grille_cyl_2.getHexaXYZ( 3 , 2 , 5 )
hexa_3_x_3_y_5_z_g2 = grille_cyl_2.getHexaXYZ( 3 , 3 , 5 )
hexa_3_x_4_y_5_z_g2 = grille_cyl_2.getHexaXYZ( 3 , 4 , 5 )

hexa_3_x_0_y_6_z_g2 = grille_cyl_2.getHexaXYZ( 3 , 0 , 6 )
hexa_3_x_1_y_6_z_g2 = grille_cyl_2.getHexaXYZ( 3 , 1 , 6 )
hexa_3_x_2_y_6_z_g2 = grille_cyl_2.getHexaXYZ( 3 , 2 , 6 )
hexa_3_x_3_y_6_z_g2 = grille_cyl_2.getHexaXYZ( 3 , 3 , 6 )
hexa_3_x_4_y_6_z_g2 = grille_cyl_2.getHexaXYZ( 3 , 4 , 6 )


hexa_list_x_3_g2 = [ hexa_3_x_0_y_0_z_g2 , hexa_3_x_1_y_0_z_g2 , hexa_3_x_2_y_0_z_g2 , hexa_3_x_3_y_0_z_g2 , hexa_3_x_4_y_0_z_g2 , hexa_3_x_0_y_1_z_g2 , hexa_3_x_1_y_1_z_g2 , hexa_3_x_2_y_1_z_g2 , hexa_3_x_3_y_1_z_g2 , hexa_3_x_4_y_1_z_g2 , hexa_3_x_0_y_4_z_g2 , hexa_3_x_1_y_4_z_g2 , hexa_3_x_2_y_4_z_g2 , hexa_3_x_3_y_4_z_g2 , hexa_3_x_4_y_4_z_g2 , hexa_3_x_0_y_5_z_g2 , hexa_3_x_1_y_5_z_g2 , hexa_3_x_2_y_5_z_g2 , hexa_3_x_3_y_5_z_g2 , hexa_3_x_4_y_5_z_g2 , hexa_3_x_0_y_6_z_g2 , hexa_3_x_1_y_6_z_g2 , hexa_3_x_2_y_6_z_g2 , hexa_3_x_3_y_6_z_g2 , hexa_3_x_4_y_6_z_g2 ]

for hexa_i in range(hexa_list_x_3_g2):
	doc.removeHexa(hexa_i)


# 5eme range x = 4

hexa_4_x_0_y_0_z_g2 = grille_cyl_2.getHexaXYZ( 4 , 0 , 0 )
hexa_4_x_1_y_0_z_g2 = grille_cyl_2.getHexaXYZ( 4 , 1 , 0 )
hexa_4_x_2_y_0_z_g2 = grille_cyl_2.getHexaXYZ( 4 , 2 , 0 )
hexa_4_x_3_y_0_z_g2 = grille_cyl_2.getHexaXYZ( 4 , 3 , 0 )
hexa_4_x_4_y_0_z_g2 = grille_cyl_2.getHexaXYZ( 4 , 4 , 0 )

hexa_4_x_0_y_1_z_g2 = grille_cyl_2.getHexaXYZ( 4 , 0 , 1 )
hexa_4_x_1_y_1_z_g2 = grille_cyl_2.getHexaXYZ( 4 , 1 , 1 )
hexa_4_x_2_y_1_z_g2 = grille_cyl_2.getHexaXYZ( 4 , 2 , 1 )
hexa_4_x_3_y_1_z_g2 = grille_cyl_2.getHexaXYZ( 4 , 3 , 1 )
hexa_4_x_4_y_1_z_g2 = grille_cyl_2.getHexaXYZ( 4 , 4 , 1 )

hexa_4_x_0_y_3_z_g2 = grille_cyl_2.getHexaXYZ( 4 , 0 , 3 )
hexa_4_x_1_y_3_z_g2 = grille_cyl_2.getHexaXYZ( 4 , 1 , 3 )
hexa_4_x_2_y_3_z_g2 = grille_cyl_2.getHexaXYZ( 4 , 2 , 3 )
hexa_4_x_3_y_3_z_g2 = grille_cyl_2.getHexaXYZ( 4 , 3 , 3 )
hexa_4_x_4_y_3_z_g2 = grille_cyl_2.getHexaXYZ( 4 , 4 , 3 )

hexa_4_x_0_y_4_z_g2 = grille_cyl_2.getHexaXYZ( 4 , 0 , 4 )
hexa_4_x_1_y_4_z_g2 = grille_cyl_2.getHexaXYZ( 4 , 1 , 4 )
hexa_4_x_2_y_4_z_g2 = grille_cyl_2.getHexaXYZ( 4 , 2 , 4 )
hexa_4_x_3_y_4_z_g2 = grille_cyl_2.getHexaXYZ( 4 , 3 , 4 )
hexa_4_x_4_y_4_z_g2 = grille_cyl_2.getHexaXYZ( 4 , 4 , 4 )

hexa_4_x_0_y_5_z_g2 = grille_cyl_2.getHexaXYZ( 4 , 0 , 5 )
hexa_4_x_1_y_5_z_g2 = grille_cyl_2.getHexaXYZ( 4 , 1 , 5 )
hexa_4_x_2_y_5_z_g2 = grille_cyl_2.getHexaXYZ( 4 , 2 , 5 )
hexa_4_x_3_y_5_z_g2 = grille_cyl_2.getHexaXYZ( 4 , 3 , 5 )
hexa_4_x_4_y_5_z_g2 = grille_cyl_2.getHexaXYZ( 4 , 4 , 5 )

hexa_4_x_0_y_6_z_g2 = grille_cyl_2.getHexaXYZ( 4 , 0 , 6 )
hexa_4_x_1_y_6_z_g2 = grille_cyl_2.getHexaXYZ( 4 , 1 , 6 )
hexa_4_x_2_y_6_z_g2 = grille_cyl_2.getHexaXYZ( 4 , 2 , 6 )
hexa_4_x_3_y_6_z_g2 = grille_cyl_2.getHexaXYZ( 4 , 3 , 6 )
hexa_4_x_4_y_6_z_g2 = grille_cyl_2.getHexaXYZ( 4 , 4 , 6 )


hexa_list_x_4_g2 = [ hexa_4_x_0_y_0_z_g2 , hexa_4_x_1_y_0_z_g2 , hexa_4_x_2_y_0_z_g2 , hexa_4_x_3_y_0_z_g2 , hexa_4_x_4_y_0_z_g2 , hexa_4_x_0_y_1_z_g2 , hexa_4_x_1_y_1_z_g2 , hexa_4_x_2_y_1_z_g2 , hexa_4_x_3_y_1_z_g2 , hexa_4_x_4_y_1_z_g2 , hexa_4_x_0_y_3_z_g2 , hexa_4_x_1_y_3_z_g2 , hexa_4_x_2_y_3_z_g2 , hexa_4_x_3_y_3_z_g2 , hexa_4_x_4_y_3_z_g2 , hexa_4_x_0_y_4_z_g2 , hexa_4_x_1_y_4_z_g2 , hexa_4_x_2_y_4_z_g2 , hexa_4_x_3_y_4_z_g2 , hexa_4_x_4_y_4_z_g2 , hexa_4_x_0_y_5_z_g2 , hexa_4_x_1_y_5_z_g2 , hexa_4_x_2_y_5_z_g2 , hexa_4_x_3_y_5_z_g2 , hexa_4_x_4_y_5_z_g2 , hexa_4_x_0_y_6_z_g2 , hexa_4_x_1_y_6_z_g2 , hexa_4_x_2_y_6_z_g2 , hexa_4_x_3_y_6_z_g2 , hexa_4_x_4_y_6_z_g2 ]

for hexa_i in range(hexa_list_x_4_g2):
	doc.removeHexa(hexa_i)


# c est pas fini on doit supprimer encore qlqs couches des hexas deja crees

z0 = 0
z1 = 1*h
#z1 = 1*(h/7)
#z2 = 2*(h/7)
#z3 = 3*(h/7)
#z4 = 4*(h/7)
#z5 = 5*(h/7)
#z6 = 6*(h/7)
#z7 = 7*(h/7) = h

# reperer les vertex

ni = nr2 = 5
nj = na2 = 5
nk = nl2 = 7

0 <= i < ni + 1 = 6
0 <= j < nj + 1 = 6
0 <= k < nk + 1 = 8

vtx_2_h_ng2 = grille_cyl_2.getVertexXYZ( 0 , 1 , 7 )
vtx_4_h_ng2 = grille_cyl_2.getVertexXYZ( 0 , 3 , 7 )
vtx_6_h_ng2 = grille_cyl_2.getVertexXYZ( 0 , 5 , 6 )

vtx_7_h_ng2 = doc.addVertex( P + 0.5 , 0 , (6*h)/7 )


hexa_rempl_7 = doc.findHexa( vtx_2_h_ng2 , vtx_7_h_ng2 )
hexa_rempl_7_8 = doc.findHexa( vtx_7_h_ng2 , vtx_4_h_ng2 )
hexa_rempl_8 = doc.findHexa( vtx_4_h_ng2 , vtx_6_h_ng2 )


hexa_list_rempl_g2 = [ hexa_rempl_7 , hexa_rempl_7_8 , hexa_rempl_8 ]

for hexa_i in range(hexa_list_rempl_g2):
	doc.removeHexa(hexa_i)



# reperer les vertex

z0 = 0
z1 = 1*h
#z1 = 1*(h/7)
#z2 = 2*(h/7)
#z3 = 3*(h/7)
#z4 = 4*(h/7)
#z5 = 5*(h/7)
#z6 = 6*(h/7)
#z7 = 7*(h/7) = h

ni = nr1 = 4
nj = na1 = 1
nk = nl1 = 7

0 <= i < ni + 1 = 5
0 <= j < nj + 1 = 2
0 <= k < nk + 1 = 8

ni = nr2 = 5
nj = na2 = 5
nk = nl2 = 7

0 <= i < ni + 1 = 6
0 <= j < nj + 1 = 6
0 <= k < nk + 1 = 8

vtx_7_z_ng2 = grille_cyl_2.getVertexXYZ( 5 , 5 , 7 )
vtx_5_z_ng2 = grille_cyl_2.getVertexXYZ( 5 , 5 , 5 )

vtx_6_z_ng1 = grille_cyl_2.getVertexXYZ( 2 , 0 , 6 )
vtx_4_z_ng1 = grille_cyl_2.getVertexXYZ( 2 , 0 , 4 )


hexa_7_z_g1_g2 = doc.findHexa( vtx_7_z_ng2 , vtx_6_z_ng1 )
hexa_6_z_g1_g2 = doc.findHexa( vtx_6_z_ng1 , vtx_5_z_ng2 )
hexa_5_z_g1_g2 = doc.findHexa( vtx_5_z_ng2 , vtx_4_z_ng1 )


hexa_list_rempl_g1_g2 = [ hexa_7_z_g1_g2 , hexa_6_z_g1_g2 , hexa_5_z_g1_g2 ]

for hexa_i in range(hexa_list_rempl_g1_g2):
	doc.removeHexa(hexa_i)



# on doit compter le nombre des pts qui contient le document

nbr_vtx = doc.countVertex()


#==========================
# CREATION ASSOCIATION
#==========================

Bride_geom = geompy.Import("/export/home/guest/Bureau/Karima/CAO_CEA/bride.brep", "BREP")

geompy.addToStudy(Bride_geom, "Bride_geom")

Pt_A = geompy.MakeVertex( 0 , 0 , 0 )
DZ = geompy.MakeVector( 0 , 0 , 1 )


#==============================================
# Recuperation des pts pour la 1ere grille
#==============================================


z0 = -1
z1 = 0
z2 = 1.5
z3 = 21.5
z4 = 24
z5 = 36
z6 = 46.5
z7 = 66.5

#geom
vx_g1_1c_y0_z0 = geompy.MakeVertex( r1_t , 0 , z0 )
vx_g1_1c_y0_z1 = geompy.MakeVertex( r1_t , 0 , z1 )
vx_g1_1c_y0_z2 = geompy.MakeVertex( r1_t , 0 , z2 )
vx_g1_1c_y0_z3 = geompy.MakeVertex( r1_t , 0 , z3 )
vx_g1_1c_y0_z4 = geompy.MakeVertex( r1_t , 0 , z4 )
vx_g1_1c_y0_z5 = geompy.MakeVertex( r1_t , 0 , z5 )
vx_g1_1c_y0_z6 = geompy.MakeVertex( r1_t , 0 , z6 )
vx_g1_1c_y0_z7 = geompy.MakeVertex( r1_t , 0 , z7 )
#8

#modele
vx_g1_x0_y0_z0_mod = grille_cyl_1.getVertexXYZ( 0 , 0 , 0 )
vx_g1_x0_y0_z1_mod = grille_cyl_1.getVertexXYZ( 0 , 0 , 1 )
vx_g1_x0_y0_z2_mod = grille_cyl_1.getVertexXYZ( 0 , 0 , 2 )
vx_g1_x0_y0_z3_mod = grille_cyl_1.getVertexXYZ( 0 , 0 , 3 )
vx_g1_x0_y0_z4_mod = grille_cyl_1.getVertexXYZ( 0 , 0 , 4 )
vx_g1_x0_y0_z5_mod = grille_cyl_1.getVertexXYZ( 0 , 0 , 5 )
vx_g1_x0_y0_z6_mod = grille_cyl_1.getVertexXYZ( 0 , 0 , 6 )
vx_g1_x0_y0_z7_mod = grille_cyl_1.getVertexXYZ( 0 , 0 , 7 )

#association
vx_g1_x0_y0_z0_mod.setAssociation( vx_g1_1c_y0_z0 )
vx_g1_x0_y0_z1_mod.setAssociation( vx_g1_1c_y0_z1 )
vx_g1_x0_y0_z2_mod.setAssociation( vx_g1_1c_y0_z2 )
vx_g1_x0_y0_z3_mod.setAssociation( vx_g1_1c_y0_z3 )
vx_g1_x0_y0_z4_mod.setAssociation( vx_g1_1c_y0_z4 )
vx_g1_x0_y0_z5_mod.setAssociation( vx_g1_1c_y0_z5 )
vx_g1_x0_y0_z6_mod.setAssociation( vx_g1_1c_y0_z6 )
vx_g1_x0_y0_z7_mod.setAssociation( vx_g1_1c_y0_z7 )


#geom
vx_g1_1c_y1_z0 = geompy.MakeRotation( vx_g1_1c_y0_z0 , DZ , math.pi/4 )
vx_g1_1c_y1_z1 = geompy.MakeRotation( vx_g1_1c_y0_z1 , DZ , math.pi/4 )
vx_g1_1c_y1_z2 = geompy.MakeRotation( vx_g1_1c_y0_z2 , DZ , math.pi/4 )
vx_g1_1c_y1_z3 = geompy.MakeRotation( vx_g1_1c_y0_z3 , DZ , math.pi/4 )
vx_g1_1c_y1_z4 = geompy.MakeRotation( vx_g1_1c_y0_z4 , DZ , math.pi/4 )
vx_g1_1c_y1_z5 = geompy.MakeRotation( vx_g1_1c_y0_z5 , DZ , math.pi/4 )
vx_g1_1c_y1_z6 = geompy.MakeRotation( vx_g1_1c_y0_z6 , DZ , math.pi/4 )
vx_g1_1c_y1_z7 = geompy.MakeRotation( vx_g1_1c_y0_z7 , DZ , math.pi/4 )
#8

#modele
vx_g1_x0_y1_z0_mod = grille_cyl_1.getVertexXYZ( 0 , 1 , 0 )
vx_g1_x0_y1_z1_mod = grille_cyl_1.getVertexXYZ( 0 , 1 , 1 )
vx_g1_x0_y1_z2_mod = grille_cyl_1.getVertexXYZ( 0 , 1 , 2 )
vx_g1_x0_y1_z3_mod = grille_cyl_1.getVertexXYZ( 0 , 1 , 3 )
vx_g1_x0_y1_z4_mod = grille_cyl_1.getVertexXYZ( 0 , 1 , 4 )
vx_g1_x0_y1_z5_mod = grille_cyl_1.getVertexXYZ( 0 , 1 , 5 )
vx_g1_x0_y1_z6_mod = grille_cyl_1.getVertexXYZ( 0 , 1 , 6 )
vx_g1_x0_y1_z7_mod = grille_cyl_1.getVertexXYZ( 0 , 1 , 7 )

#association
vx_g1_x0_y1_z0_mod.setAssociation( vx_g1_1c_y1_z0 )
vx_g1_x0_y1_z1_mod.setAssociation( vx_g1_1c_y1_z1 )
vx_g1_x0_y1_z2_mod.setAssociation( vx_g1_1c_y1_z2 )
vx_g1_x0_y1_z3_mod.setAssociation( vx_g1_1c_y1_z3 )
vx_g1_x0_y1_z4_mod.setAssociation( vx_g1_1c_y1_z4 )
vx_g1_x0_y1_z5_mod.setAssociation( vx_g1_1c_y1_z5 )
vx_g1_x0_y1_z6_mod.setAssociation( vx_g1_1c_y1_z6 )
vx_g1_x0_y1_z7_mod.setAssociation( vx_g1_1c_y1_z7 )


#geom
vx_g1_2c_y0_z0 = geompy.MakeVertex( 19 , 0 , z0 )
vx_g1_2c_y0_z1 = geompy.MakeVertex( 19 , 0 , z1 )
vx_g1_2c_y0_z2 = geompy.MakeVertex( 19 , 0 , z2 )
vx_g1_2c_y0_z3 = geompy.MakeVertex( 19 , 0 , z3 )
vx_g1_2c_y0_z4 = geompy.MakeVertex( 18.165 , 0 , z4 )
vx_g1_2c_y0_z5 = geompy.MakeVertex( 14.157 , 0 , z5 )
vx_g1_2c_y0_z6 = geompy.MakeVertex( 10.65 , 0 , z6 )
vx_g1_2c_y0_z7 = geompy.MakeVertex( 10.65 , 0 , z7 )
#8

#modele
vx_g1_x1_y0_z0_mod = grille_cyl_1.getVertexXYZ( 1 , 0 , 0 )
vx_g1_x1_y0_z1_mod = grille_cyl_1.getVertexXYZ( 1 , 0 , 1 )
vx_g1_x1_y0_z2_mod = grille_cyl_1.getVertexXYZ( 1 , 0 , 2 )
vx_g1_x1_y0_z3_mod = grille_cyl_1.getVertexXYZ( 1 , 0 , 3 )
vx_g1_x1_y0_z4_mod = grille_cyl_1.getVertexXYZ( 1 , 0 , 4 )
vx_g1_x1_y0_z5_mod = grille_cyl_1.getVertexXYZ( 1 , 0 , 5 )
vx_g1_x1_y0_z6_mod = grille_cyl_1.getVertexXYZ( 1 , 0 , 6 )
vx_g1_x1_y0_z7_mod = grille_cyl_1.getVertexXYZ( 1 , 0 , 7 )

#association
vx_g1_x1_y0_z0_mod.setAssociation( vx_g1_2c_y0_z0 )
vx_g1_x1_y0_z1_mod.setAssociation( vx_g1_2c_y0_z1 )
vx_g1_x1_y0_z2_mod.setAssociation( vx_g1_2c_y0_z2 )
vx_g1_x1_y0_z3_mod.setAssociation( vx_g1_2c_y0_z3 )
vx_g1_x1_y0_z4_mod.setAssociation( vx_g1_2c_y0_z4 )
vx_g1_x1_y0_z5_mod.setAssociation( vx_g1_2c_y0_z5 )
vx_g1_x1_y0_z6_mod.setAssociation( vx_g1_2c_y0_z6 )
vx_g1_x1_y0_z7_mod.setAssociation( vx_g1_2c_y0_z7 )



#geom
vx_g1_2c_y1_z0 = geompy.MakeRotation( vx_g1_2c_y0_z0 , DZ , math.pi/4 )
vx_g1_2c_y1_z1 = geompy.MakeRotation( vx_g1_2c_y0_z1 , DZ , math.pi/4 )
vx_g1_2c_y1_z2 = geompy.MakeRotation( vx_g1_2c_y0_z2 , DZ , math.pi/4 )
vx_g1_2c_y1_z3 = geompy.MakeRotation( vx_g1_2c_y0_z3 , DZ , math.pi/4 )
vx_g1_2c_y1_z4 = geompy.MakeRotation( vx_g1_2c_y0_z4 , DZ , math.pi/4 )
vx_g1_2c_y1_z5 = geompy.MakeRotation( vx_g1_2c_y0_z5 , DZ , math.pi/4 )
vx_g1_2c_y1_z6 = geompy.MakeRotation( vx_g1_2c_y0_z6 , DZ , math.pi/4 )
vx_g1_2c_y1_z7 = geompy.MakeRotation( vx_g1_2c_y0_z7 , DZ , math.pi/4 )
#8

#modele
vx_g1_x1_y1_z0_mod = grille_cyl_1.getVertexXYZ( 1 , 1 , 0 )
vx_g1_x1_y1_z1_mod = grille_cyl_1.getVertexXYZ( 1 , 1 , 1 )
vx_g1_x1_y1_z2_mod = grille_cyl_1.getVertexXYZ( 1 , 1 , 2 )
vx_g1_x1_y1_z3_mod = grille_cyl_1.getVertexXYZ( 1 , 1 , 3 )
vx_g1_x1_y1_z4_mod = grille_cyl_1.getVertexXYZ( 1 , 1 , 4 )
vx_g1_x1_y1_z5_mod = grille_cyl_1.getVertexXYZ( 1 , 1 , 5 )
vx_g1_x1_y1_z6_mod = grille_cyl_1.getVertexXYZ( 1 , 1 , 6 )
vx_g1_x1_y1_z7_mod = grille_cyl_1.getVertexXYZ( 1 , 1 , 7 )

#association
vx_g1_x1_y1_z0_mod.setAssociation( vx_g1_2c_y1_z0 )
vx_g1_x1_y1_z1_mod.setAssociation( vx_g1_2c_y1_z1 )
vx_g1_x1_y1_z2_mod.setAssociation( vx_g1_2c_y1_z2 )
vx_g1_x1_y1_z3_mod.setAssociation( vx_g1_2c_y1_z3 )
vx_g1_x1_y1_z4_mod.setAssociation( vx_g1_2c_y1_z4 )
vx_g1_x1_y1_z5_mod.setAssociation( vx_g1_2c_y1_z5 )
vx_g1_x1_y1_z6_mod.setAssociation( vx_g1_2c_y1_z6 )
vx_g1_x1_y1_z7_mod.setAssociation( vx_g1_2c_y1_z7 )





#geom
vx_g1_3c_y0_z0 = geompy.MakeVertex( 21 , 0 , z0 )
vx_g1_3c_y0_z1 = geompy.MakeVertex( 21 , 0 , z1 )
vx_g1_3c_y0_z2 = geompy.MakeVertex( 21 , 0 , z2 )
vx_g1_3c_y0_z3 = geompy.MakeVertex( 21 , 0 , z3 )
vx_g1_3c_y0_z4 = geompy.MakeVertex( 21 , 0 , z4 )
#5

#modele
vx_g1_x2_y0_z0_mod = grille_cyl_1.getVertexXYZ( 2 , 0 , 0 )
vx_g1_x2_y0_z1_mod = grille_cyl_1.getVertexXYZ( 2 , 0 , 1 )
vx_g1_x2_y0_z2_mod = grille_cyl_1.getVertexXYZ( 2 , 0 , 2 )
vx_g1_x2_y0_z3_mod = grille_cyl_1.getVertexXYZ( 2 , 0 , 3 )
vx_g1_x2_y0_z4_mod = grille_cyl_1.getVertexXYZ( 2 , 0 , 4 )

#association
vx_g1_x2_y0_z0_mod.setAssociation( vx_g1_3c_y0_z0 )
vx_g1_x2_y0_z1_mod.setAssociation( vx_g1_3c_y0_z1 )
vx_g1_x2_y0_z2_mod.setAssociation( vx_g1_3c_y0_z2 )
vx_g1_x2_y0_z3_mod.setAssociation( vx_g1_3c_y0_z3 )
vx_g1_x2_y0_z4_mod.setAssociation( vx_g1_3c_y0_z4 )




#geom
vx_g1_3c_y1_z0 = geompy.MakeRotation( vx_g1_3c_y0_z0 , DZ , math.pi/4 )
vx_g1_3c_y1_z1 = geompy.MakeRotation( vx_g1_3c_y0_z1 , DZ , math.pi/4 )
vx_g1_3c_y1_z2 = geompy.MakeRotation( vx_g1_3c_y0_z2 , DZ , math.pi/4 )
vx_g1_3c_y1_z3 = geompy.MakeRotation( vx_g1_3c_y0_z3 , DZ , math.pi/4 )
#4

#modele
vx_g1_x2_y1_z0_mod = grille_cyl_1.getVertexXYZ( 2 , 1 , 0 )
vx_g1_x2_y1_z1_mod = grille_cyl_1.getVertexXYZ( 2 , 1 , 1 )
vx_g1_x2_y1_z2_mod = grille_cyl_1.getVertexXYZ( 2 , 1 , 2 )
vx_g1_x2_y1_z3_mod = grille_cyl_1.getVertexXYZ( 2 , 1 , 3 )

#association
vx_g1_x2_y1_z0_mod.setAssociation( vx_g1_3c_y1_z0 )
vx_g1_x2_y1_z1_mod.setAssociation( vx_g1_3c_y1_z1 )
vx_g1_x2_y1_z2_mod.setAssociation( vx_g1_3c_y1_z2 )
vx_g1_x2_y1_z3_mod.setAssociation( vx_g1_3c_y1_z3 )





Circle_1_g1 = geompy.MakeCircle( Pt_A , DZ , 22.5 )
Circle_2_g1 = geompy.MakeCircle( Pt_A , DZ , 24 )
Circle_1_g2 = geompy.MakeCircle( Pt_A , DZ , 12.5 )


#geom
vx_g1_4c_y0_z1 = geompy.MakeVertexOnLinesIntersection( Circle_1_g1 , Circle_1_g2 )
vx_g1_4c_y0_z0 = geompy.MakeTranslation( vx_g1_4c_y0_z1 , 0 , 0 , z0 )
vx_g1_4c_y0_z2 = geompy.MakeTranslation( vx_g1_4c_y0_z1 , 0 , 0 , z2 )
vx_g1_4c_y0_z3 = geompy.MakeTranslation( vx_g1_4c_y0_z1 , 0 , 0 , z3 )
vx_g1_4c_y0_z4 = geompy.MakeTranslation( vx_g1_4c_y0_z1 , 0 , 0 , z4 )
#5

#modele
vx_g1_x3_y0_z0_mod = grille_cyl_1.getVertexXYZ( 3 , 0 , 0 )
vx_g1_x3_y0_z1_mod = grille_cyl_1.getVertexXYZ( 3 , 0 , 1 )
vx_g1_x3_y0_z2_mod = grille_cyl_1.getVertexXYZ( 3 , 0 , 2 )
vx_g1_x3_y0_z3_mod = grille_cyl_1.getVertexXYZ( 3 , 0 , 3 )
vx_g1_x3_y0_z4_mod = grille_cyl_1.getVertexXYZ( 3 , 0 , 4 )

#association
vx_g1_x3_y0_z0_mod.setAssociation( vx_g1_4c_y0_z0 )
vx_g1_x3_y0_z1_mod.setAssociation( vx_g1_4c_y0_z1 )
vx_g1_x3_y0_z2_mod.setAssociation( vx_g1_4c_y0_z2 )
vx_g1_x3_y0_z3_mod.setAssociation( vx_g1_4c_y0_z3 )
vx_g1_x3_y0_z4_mod.setAssociation( vx_g1_4c_y0_z4 )



#geom
vx_g1_4c_y1_z0 = geompy.MakeRotation( vx_g1_4c_y0_z0 , DZ , (math.pi*32.5852370)/180 )
vx_g1_4c_y1_z1 = geompy.MakeRotation( vx_g1_4c_y0_z1 , DZ , (math.pi*32.5852370)/180 )
vx_g1_4c_y1_z2 = geompy.MakeRotation( vx_g1_4c_y0_z2 , DZ , (math.pi*32.5852370)/180 )
vx_g1_4c_y1_z3 = geompy.MakeRotation( vx_g1_4c_y0_z3 , DZ , (math.pi*32.5852370)/180 )
#4

#modele
vx_g1_x3_y1_z0_mod = grille_cyl_1.getVertexXYZ( 3 , 1 , 0 )
vx_g1_x3_y1_z1_mod = grille_cyl_1.getVertexXYZ( 3 , 1 , 1 )
vx_g1_x3_y1_z2_mod = grille_cyl_1.getVertexXYZ( 3 , 1 , 2 )
vx_g1_x3_y1_z3_mod = grille_cyl_1.getVertexXYZ( 3 , 1 , 3 )

#association
vx_g1_x3_y1_z0_mod.setAssociation( vx_g1_4c_y1_z0 )
vx_g1_x3_y1_z1_mod.setAssociation( vx_g1_4c_y1_z1 )
vx_g1_x3_y1_z2_mod.setAssociation( vx_g1_4c_y1_z2 )
vx_g1_x3_y1_z3_mod.setAssociation( vx_g1_4c_y1_z3 )




#geom
vx_g1_5c_y0_z1 = geompy.MakeVertexOnLinesIntersection( Circle_2_g1 , Circle_1_g2 )
vx_g1_5c_y0_z0 = geompy.MakeTranslation( vx_g1_5c_y0_z1 , 0 , 0 , z0 )
vx_g1_5c_y0_z2 = geompy.MakeTranslation( vx_g1_5c_y0_z1 , 0 , 0 , z2 )
vx_g1_5c_y0_z3 = geompy.MakeTranslation( vx_g1_5c_y0_z1 , 0 , 0 , z3 )
vx_g1_5c_y0_z4 = geompy.MakeTranslation( vx_g1_5c_y0_z1 , 0 , 0 , z4 )
#5

#modele
vx_g1_x4_y0_z0_mod = grille_cyl_1.getVertexXYZ( 4 , 0 , 0 )
vx_g1_x4_y0_z1_mod = grille_cyl_1.getVertexXYZ( 4 , 0 , 1 )
vx_g1_x4_y0_z2_mod = grille_cyl_1.getVertexXYZ( 4 , 0 , 2 )
vx_g1_x4_y0_z3_mod = grille_cyl_1.getVertexXYZ( 4 , 0 , 3 )
vx_g1_x4_y0_z4_mod = grille_cyl_1.getVertexXYZ( 4 , 0 , 4 )

#association
vx_g1_x4_y0_z0_mod.setAssociation( vx_g1_5c_y0_z0 )
vx_g1_x4_y0_z1_mod.setAssociation( vx_g1_5c_y0_z1 )
vx_g1_x4_y0_z2_mod.setAssociation( vx_g1_5c_y0_z2 )
vx_g1_x4_y0_z3_mod.setAssociation( vx_g1_5c_y0_z3 )
vx_g1_x4_y0_z4_mod.setAssociation( vx_g1_5c_y0_z4 )



#geom
vx_g1_5c_y1_z0 = geompy.MakeRotation( vx_g1_5c_y0_z0 , DZ , (math.pi*32.5852370)/180 )
vx_g1_5c_y1_z1 = geompy.MakeRotation( vx_g1_5c_y0_z1 , DZ , (math.pi*32.5852370)/180 )
vx_g1_5c_y1_z2 = geompy.MakeRotation( vx_g1_5c_y0_z2 , DZ , (math.pi*32.5852370)/180 )
vx_g1_5c_y1_z3 = geompy.MakeRotation( vx_g1_5c_y0_z3 , DZ , (math.pi*32.5852370)/180 )
#4

#modele
vx_g1_x4_y1_z0_mod = grille_cyl_1.getVertexXYZ( 4 , 1 , 0 )
vx_g1_x4_y1_z1_mod = grille_cyl_1.getVertexXYZ( 4 , 1 , 1 )
vx_g1_x4_y1_z2_mod = grille_cyl_1.getVertexXYZ( 4 , 1 , 2 )
vx_g1_x4_y1_z3_mod = grille_cyl_1.getVertexXYZ( 4 , 1 , 3 )

#association
vx_g1_x4_y1_z0_mod.setAssociation( vx_g1_5c_y1_z0 )
vx_g1_x4_y1_z1_mod.setAssociation( vx_g1_5c_y1_z1 )
vx_g1_x4_y1_z2_mod.setAssociation( vx_g1_5c_y1_z2 )
vx_g1_x4_y1_z3_mod.setAssociation( vx_g1_5c_y1_z3 )





#==============================================
# Recuperation des pts pour la 2eme grille
#==============================================

z0 = -1
z1 = 0
z2 = 1.5
z3 = 21.5
z4 = 24
z5 = 36
z6 = 41.5


f0 = 0
f1 = 0.2
f2 = 0.4
f3 = 0.6
f4 = 0.8
f5 = 1


Pt_B_1 = geompy.MakeVertex( 33.5 , 6 , 0 )
Edge_B_1 = geompy.MakeEdgeNearPoint( Bride_geom , Pt_B_1 )

#geom
vx_g2_2c_f0_z1 = geompy.MakeVertexOnCurve( Edge_B_1 , f0 )
vx_g2_2c_f1_z1 = geompy.MakeVertexOnCurve( Edge_B_1 , f1 )
vx_g2_2c_f2_z1 = geompy.MakeVertexOnCurve( Edge_B_1 , f2 )
vx_g2_2c_f3_z1 = geompy.MakeVertexOnCurve( Edge_B_1 , f3 )
vx_g2_2c_f4_z1 = geompy.MakeVertexOnCurve( Edge_B_1 , f4 )
vx_g2_2c_f5_z1 = geompy.MakeVertexOnCurve( Edge_B_1 , f5 )

#modele
vx_g2_x1_y0_z1_mod = grille_cyl_2.getVertexXYZ( 1 , 0 , 1 )
vx_g2_x1_y1_z1_mod = grille_cyl_2.getVertexXYZ( 1 , 1 , 1 )
vx_g2_x1_y2_z1_mod = grille_cyl_2.getVertexXYZ( 1 , 2 , 1 )
vx_g2_x1_y3_z1_mod = grille_cyl_2.getVertexXYZ( 1 , 3 , 1 )
vx_g2_x1_y4_z1_mod = grille_cyl_2.getVertexXYZ( 1 , 4 , 1 )
vx_g2_x1_y5_z1_mod = grille_cyl_2.getVertexXYZ( 1 , 5 , 1 )

#association
vx_g2_x1_y0_z1_mod.setAssociation( vx_g2_2c_f0_z1 )
vx_g2_x1_y1_z1_mod.setAssociation( vx_g2_2c_f1_z1 )
vx_g2_x1_y2_z1_mod.setAssociation( vx_g2_2c_f2_z1 )
vx_g2_x1_y3_z1_mod.setAssociation( vx_g2_2c_f3_z1 )
vx_g2_x1_y4_z1_mod.setAssociation( vx_g2_2c_f4_z1 )
vx_g2_x1_y5_z1_mod.setAssociation( vx_g2_2c_f5_z1 )




#geom
vx_g2_2c_f0_z0 = geompy.MakeTranslation( vx_g2_2c_f0_z1 , 0 , 0 , z0  )
vx_g2_2c_f1_z0 = geompy.MakeTranslation( vx_g2_2c_f1_z1 , 0 , 0 , z0  )
vx_g2_2c_f2_z0 = geompy.MakeTranslation( vx_g2_2c_f2_z1 , 0 , 0 , z0  )
vx_g2_2c_f3_z0 = geompy.MakeTranslation( vx_g2_2c_f3_z1 , 0 , 0 , z0  )
vx_g2_2c_f4_z0 = geompy.MakeTranslation( vx_g2_2c_f4_z1 , 0 , 0 , z0  )
vx_g2_2c_f5_z0 = geompy.MakeTranslation( vx_g2_2c_f5_z1 , 0 , 0 , z0  )

#modele
vx_g2_x1_y0_z0_mod = grille_cyl_2.getVertexXYZ( 1 , 0 , 0 )
vx_g2_x1_y1_z0_mod = grille_cyl_2.getVertexXYZ( 1 , 1 , 0 )
vx_g2_x1_y2_z0_mod = grille_cyl_2.getVertexXYZ( 1 , 2 , 0 )
vx_g2_x1_y3_z0_mod = grille_cyl_2.getVertexXYZ( 1 , 3 , 0 )
vx_g2_x1_y4_z0_mod = grille_cyl_2.getVertexXYZ( 1 , 4 , 0 )
vx_g2_x1_y5_z0_mod = grille_cyl_2.getVertexXYZ( 1 , 5 , 0 )

#association
vx_g2_x1_y0_z0_mod.setAssociation( vx_g2_2c_f0_z0 )
vx_g2_x1_y1_z0_mod.setAssociation( vx_g2_2c_f1_z0 )
vx_g2_x1_y2_z0_mod.setAssociation( vx_g2_2c_f2_z0 )
vx_g2_x1_y3_z0_mod.setAssociation( vx_g2_2c_f3_z0 )
vx_g2_x1_y4_z0_mod.setAssociation( vx_g2_2c_f4_z0 )
vx_g2_x1_y5_z0_mod.setAssociation( vx_g2_2c_f5_z0 )


#geom
vx_g2_2c_f0_z2 = geompy.MakeTranslation( vx_g2_2c_f0_z1 , 0 , 0 , z2  )
vx_g2_2c_f1_z2 = geompy.MakeTranslation( vx_g2_2c_f1_z1 , 0 , 0 , z2  )
vx_g2_2c_f2_z2 = geompy.MakeTranslation( vx_g2_2c_f2_z1 , 0 , 0 , z2  )
vx_g2_2c_f3_z2 = geompy.MakeTranslation( vx_g2_2c_f3_z1 , 0 , 0 , z2  )
vx_g2_2c_f4_z2 = geompy.MakeTranslation( vx_g2_2c_f4_z1 , 0 , 0 , z2  )
vx_g2_2c_f5_z2 = geompy.MakeTranslation( vx_g2_2c_f5_z1 , 0 , 0 , z2  )

#modele
vx_g2_x1_y0_z2_mod = grille_cyl_2.getVertexXYZ( 1 , 0 , 2 )
vx_g2_x1_y1_z2_mod = grille_cyl_2.getVertexXYZ( 1 , 1 , 2 )
vx_g2_x1_y2_z2_mod = grille_cyl_2.getVertexXYZ( 1 , 2 , 2 )
vx_g2_x1_y3_z2_mod = grille_cyl_2.getVertexXYZ( 1 , 3 , 2 )
vx_g2_x1_y4_z2_mod = grille_cyl_2.getVertexXYZ( 1 , 4 , 2 )
vx_g2_x1_y5_z2_mod = grille_cyl_2.getVertexXYZ( 1 , 5 , 2 )

#association
vx_g2_x1_y0_z2_mod.setAssociation( vx_g2_2c_f0_z2 )
vx_g2_x1_y1_z2_mod.setAssociation( vx_g2_2c_f1_z2 )
vx_g2_x1_y2_z2_mod.setAssociation( vx_g2_2c_f2_z2 )
vx_g2_x1_y3_z2_mod.setAssociation( vx_g2_2c_f3_z2 )
vx_g2_x1_y4_z2_mod.setAssociation( vx_g2_2c_f4_z2 )
vx_g2_x1_y5_z2_mod.setAssociation( vx_g2_2c_f5_z2 )


#geom
vx_g2_2c_f0_z3 = geompy.MakeTranslation( vx_g2_2c_f0_z1 , 0 , 0 , z3  )
vx_g2_2c_f1_z3 = geompy.MakeTranslation( vx_g2_2c_f1_z1 , 0 , 0 , z3  )
vx_g2_2c_f2_z3 = geompy.MakeTranslation( vx_g2_2c_f2_z1 , 0 , 0 , z3  )
vx_g2_2c_f3_z3 = geompy.MakeTranslation( vx_g2_2c_f3_z1 , 0 , 0 , z3  )
vx_g2_2c_f4_z3 = geompy.MakeTranslation( vx_g2_2c_f4_z1 , 0 , 0 , z3  )
vx_g2_2c_f5_z3 = geompy.MakeTranslation( vx_g2_2c_f5_z1 , 0 , 0 , z3  )

#modele
vx_g2_x1_y0_z3_mod = grille_cyl_2.getVertexXYZ( 1 , 0 , 3 )
vx_g2_x1_y1_z3_mod = grille_cyl_2.getVertexXYZ( 1 , 1 , 3 )
vx_g2_x1_y2_z3_mod = grille_cyl_2.getVertexXYZ( 1 , 2 , 3 )
vx_g2_x1_y3_z3_mod = grille_cyl_2.getVertexXYZ( 1 , 3 , 3 )
vx_g2_x1_y4_z3_mod = grille_cyl_2.getVertexXYZ( 1 , 4 , 3 )
vx_g2_x1_y5_z3_mod = grille_cyl_2.getVertexXYZ( 1 , 5 , 3 )

#association
vx_g2_x1_y0_z3_mod.setAssociation( vx_g2_2c_f0_z3 )
vx_g2_x1_y1_z3_mod.setAssociation( vx_g2_2c_f1_z3 )
vx_g2_x1_y2_z3_mod.setAssociation( vx_g2_2c_f2_z3 )
vx_g2_x1_y3_z3_mod.setAssociation( vx_g2_2c_f3_z3 )
vx_g2_x1_y4_z3_mod.setAssociation( vx_g2_2c_f4_z3 )
vx_g2_x1_y5_z3_mod.setAssociation( vx_g2_2c_f5_z3 )



#geom
vx_g2_2c_f0_z4 = geompy.MakeTranslation( vx_g2_2c_f0_z1 , 0 , 0 , z4  )
vx_g2_2c_f1_z4 = geompy.MakeTranslation( vx_g2_2c_f1_z1 , 0 , 0 , z4  )
vx_g2_2c_f2_z4 = geompy.MakeTranslation( vx_g2_2c_f2_z1 , 0 , 0 , z4  )
vx_g2_2c_f3_z4 = geompy.MakeTranslation( vx_g2_2c_f3_z1 , 0 , 0 , z4  )
vx_g2_2c_f4_z4 = geompy.MakeTranslation( vx_g2_2c_f4_z1 , 0 , 0 , z4  )
vx_g2_2c_f5_z4 = geompy.MakeTranslation( vx_g2_2c_f5_z1 , 0 , 0 , z4  )

#modele
vx_g2_x1_y0_z4_mod = grille_cyl_2.getVertexXYZ( 1 , 0 , 4 )
vx_g2_x1_y1_z4_mod = grille_cyl_2.getVertexXYZ( 1 , 1 , 4 )
vx_g2_x1_y2_z4_mod = grille_cyl_2.getVertexXYZ( 1 , 2 , 4 )
vx_g2_x1_y3_z4_mod = grille_cyl_2.getVertexXYZ( 1 , 3 , 4 )
vx_g2_x1_y4_z4_mod = grille_cyl_2.getVertexXYZ( 1 , 4 , 4 )
vx_g2_x1_y5_z4_mod = grille_cyl_2.getVertexXYZ( 1 , 5 , 4 )

#association
vx_g2_x1_y0_z4_mod.setAssociation( vx_g2_2c_f0_z4 )
vx_g2_x1_y1_z4_mod.setAssociation( vx_g2_2c_f1_z4 )
vx_g2_x1_y2_z4_mod.setAssociation( vx_g2_2c_f2_z4 )
vx_g2_x1_y3_z4_mod.setAssociation( vx_g2_2c_f3_z4 )
vx_g2_x1_y4_z4_mod.setAssociation( vx_g2_2c_f4_z4 )
vx_g2_x1_y5_z4_mod.setAssociation( vx_g2_2c_f5_z4 )


#geom
vx_g2_2c_f0_z5 = geompy.MakeTranslation( vx_g2_2c_f0_z1 , 0 , 0 , z5  )
vx_g2_2c_f1_z5 = geompy.MakeTranslation( vx_g2_2c_f1_z1 , 0 , 0 , z5  )
vx_g2_2c_f2_z5 = geompy.MakeTranslation( vx_g2_2c_f2_z1 , 0 , 0 , z5  )
vx_g2_2c_f3_z5 = geompy.MakeTranslation( vx_g2_2c_f3_z1 , 0 , 0 , z5  )
vx_g2_2c_f4_z5 = geompy.MakeTranslation( vx_g2_2c_f4_z1 , 0 , 0 , z5  )
vx_g2_2c_f5_z5 = geompy.MakeTranslation( vx_g2_2c_f5_z1 , 0 , 0 , z5  )

#modele
vx_g2_x1_y0_z5_mod = grille_cyl_2.getVertexXYZ( 1 , 0 , 5 )
vx_g2_x1_y1_z5_mod = grille_cyl_2.getVertexXYZ( 1 , 1 , 5 )
vx_g2_x1_y2_z5_mod = grille_cyl_2.getVertexXYZ( 1 , 2 , 5 )
vx_g2_x1_y3_z5_mod = grille_cyl_2.getVertexXYZ( 1 , 3 , 5 )
vx_g2_x1_y4_z5_mod = grille_cyl_2.getVertexXYZ( 1 , 4 , 5 )
vx_g2_x1_y5_z5_mod = grille_cyl_2.getVertexXYZ( 1 , 5 , 5 )

#association
vx_g2_x1_y0_z5_mod.setAssociation( vx_g2_2c_f0_z5 )
vx_g2_x1_y1_z5_mod.setAssociation( vx_g2_2c_f1_z5 )
vx_g2_x1_y2_z5_mod.setAssociation( vx_g2_2c_f2_z5 )
vx_g2_x1_y3_z5_mod.setAssociation( vx_g2_2c_f3_z5 )
vx_g2_x1_y4_z5_mod.setAssociation( vx_g2_2c_f4_z5 )
vx_g2_x1_y5_z5_mod.setAssociation( vx_g2_2c_f5_z5 )


#geom
vx_g2_2c_f0_z6 = geompy.MakeTranslation( vx_g2_2c_f0_z1 , 0 , 0 , z6  )
vx_g2_2c_f1_z6 = geompy.MakeTranslation( vx_g2_2c_f1_z1 , 0 , 0 , z6  )
vx_g2_2c_f2_z6 = geompy.MakeTranslation( vx_g2_2c_f2_z1 , 0 , 0 , z6  )
vx_g2_2c_f3_z6 = geompy.MakeTranslation( vx_g2_2c_f3_z1 , 0 , 0 , z6  )
vx_g2_2c_f4_z6 = geompy.MakeTranslation( vx_g2_2c_f4_z1 , 0 , 0 , z6  )
vx_g2_2c_f5_z6 = geompy.MakeTranslation( vx_g2_2c_f5_z1 , 0 , 0 , z6  )

#modele
vx_g2_x1_y0_z6_mod = grille_cyl_2.getVertexXYZ( 1 , 0 , 6 )
vx_g2_x1_y1_z6_mod = grille_cyl_2.getVertexXYZ( 1 , 1 , 6 )
vx_g2_x1_y2_z6_mod = grille_cyl_2.getVertexXYZ( 1 , 2 , 6 )
vx_g2_x1_y3_z6_mod = grille_cyl_2.getVertexXYZ( 1 , 3 , 6 )
vx_g2_x1_y4_z6_mod = grille_cyl_2.getVertexXYZ( 1 , 4 , 6 )
vx_g2_x1_y5_z6_mod = grille_cyl_2.getVertexXYZ( 1 , 5 , 6 )

#association
vx_g2_x1_y0_z6_mod.setAssociation( vx_g2_2c_f0_z6 )
vx_g2_x1_y1_z6_mod.setAssociation( vx_g2_2c_f1_z6 )
vx_g2_x1_y2_z6_mod.setAssociation( vx_g2_2c_f2_z6 )
vx_g2_x1_y3_z6_mod.setAssociation( vx_g2_2c_f3_z6 )
vx_g2_x1_y4_z6_mod.setAssociation( vx_g2_2c_f4_z6 )
vx_g2_x1_y5_z6_mod.setAssociation( vx_g2_2c_f5_z6 )




Pt_B_2 = geompy.MakeVertex( 33.5 , 7 , 1.5 )
Edge_B_2 = geompy.MakeEdgeNearPoint( Bride_geom , Pt_B_2 )

#geom
vx_g2_3c_f0_z2 = geompy.MakeVertexOnCurve( Edge_B_2 , f0 )
vx_g2_3c_f1_z2 = geompy.MakeVertexOnCurve( Edge_B_2 , f1 )
vx_g2_3c_f2_z2 = geompy.MakeVertexOnCurve( Edge_B_2 , f2 )
vx_g2_3c_f3_z2 = geompy.MakeVertexOnCurve( Edge_B_2 , f3 )
vx_g2_3c_f4_z2 = geompy.MakeVertexOnCurve( Edge_B_2 , f4 )
vx_g2_3c_f5_z2 = geompy.MakeVertexOnCurve( Edge_B_2 , f5 )

#modele
vx_g2_x2_y0_z2_mod = grille_cyl_2.getVertexXYZ( 2 , 0 , 2 )
vx_g2_x2_y1_z2_mod = grille_cyl_2.getVertexXYZ( 2 , 1 , 2 )
vx_g2_x2_y2_z2_mod = grille_cyl_2.getVertexXYZ( 2 , 2 , 2 )
vx_g2_x2_y3_z2_mod = grille_cyl_2.getVertexXYZ( 2 , 3 , 2 )
vx_g2_x2_y4_z2_mod = grille_cyl_2.getVertexXYZ( 2 , 4 , 2 )
vx_g2_x2_y5_z2_mod = grille_cyl_2.getVertexXYZ( 2 , 5 , 2 )

#association
vx_g2_x2_y0_z2_mod.setAssociation( vx_g2_3c_f0_z2 )
vx_g2_x2_y1_z2_mod.setAssociation( vx_g2_3c_f1_z2 )
vx_g2_x2_y2_z2_mod.setAssociation( vx_g2_3c_f2_z2 )
vx_g2_x2_y3_z2_mod.setAssociation( vx_g2_3c_f3_z2 )
vx_g2_x2_y4_z2_mod.setAssociation( vx_g2_3c_f4_z2 )
vx_g2_x2_y5_z2_mod.setAssociation( vx_g2_3c_f5_z2 )


#geom
vx_g2_3c_f0_z3 = geompy.MakeTranslation( vx_g2_3c_f0_z2 , 0 , 0 , z3  )
vx_g2_3c_f1_z3 = geompy.MakeTranslation( vx_g2_3c_f1_z2 , 0 , 0 , z3  )
vx_g2_3c_f2_z3 = geompy.MakeTranslation( vx_g2_3c_f2_z2 , 0 , 0 , z3  )
vx_g2_3c_f3_z3 = geompy.MakeTranslation( vx_g2_3c_f3_z2 , 0 , 0 , z3  )
vx_g2_3c_f4_z3 = geompy.MakeTranslation( vx_g2_3c_f4_z2 , 0 , 0 , z3  )
vx_g2_3c_f5_z3 = geompy.MakeTranslation( vx_g2_3c_f5_z2 , 0 , 0 , z3  )

#modele
vx_g2_x2_y0_z3_mod = grille_cyl_2.getVertexXYZ( 2 , 0 , 3 )
vx_g2_x2_y1_z3_mod = grille_cyl_2.getVertexXYZ( 2 , 1 , 3 )
vx_g2_x2_y2_z3_mod = grille_cyl_2.getVertexXYZ( 2 , 2 , 3 )
vx_g2_x2_y3_z3_mod = grille_cyl_2.getVertexXYZ( 2 , 3 , 3 )
vx_g2_x2_y4_z3_mod = grille_cyl_2.getVertexXYZ( 2 , 4 , 3 )
vx_g2_x2_y5_z3_mod = grille_cyl_2.getVertexXYZ( 2 , 5 , 3 )

#association
vx_g2_x2_y0_z3_mod.setAssociation( vx_g2_3c_f0_z3 )
vx_g2_x2_y1_z3_mod.setAssociation( vx_g2_3c_f1_z3 )
vx_g2_x2_y2_z3_mod.setAssociation( vx_g2_3c_f2_z3 )
vx_g2_x2_y3_z3_mod.setAssociation( vx_g2_3c_f3_z3 )
vx_g2_x2_y4_z3_mod.setAssociation( vx_g2_3c_f4_z3 )
vx_g2_x2_y5_z3_mod.setAssociation( vx_g2_3c_f5_z3 )


#geom
vx_g2_3c_f0_z4 = geompy.MakeTranslation( vx_g2_3c_f0_z2 , 0 , 0 , z4  )
vx_g2_3c_f1_z4 = geompy.MakeTranslation( vx_g2_3c_f1_z2 , 0 , 0 , z4  )
vx_g2_3c_f2_z4 = geompy.MakeTranslation( vx_g2_3c_f2_z2 , 0 , 0 , z4  )
vx_g2_3c_f3_z4 = geompy.MakeTranslation( vx_g2_3c_f3_z2 , 0 , 0 , z4  )
vx_g2_3c_f4_z4 = geompy.MakeTranslation( vx_g2_3c_f4_z2 , 0 , 0 , z4  )
vx_g2_3c_f5_z4 = geompy.MakeTranslation( vx_g2_3c_f5_z2 , 0 , 0 , z4  )

#modele
vx_g2_x2_y0_z4_mod = grille_cyl_2.getVertexXYZ( 2 , 0 , 4 )
vx_g2_x2_y1_z4_mod = grille_cyl_2.getVertexXYZ( 2 , 1 , 4 )
vx_g2_x2_y2_z4_mod = grille_cyl_2.getVertexXYZ( 2 , 2 , 4 )
vx_g2_x2_y3_z4_mod = grille_cyl_2.getVertexXYZ( 2 , 3 , 4 )
vx_g2_x2_y4_z4_mod = grille_cyl_2.getVertexXYZ( 2 , 4 , 4 )
vx_g2_x2_y5_z4_mod = grille_cyl_2.getVertexXYZ( 2 , 5 , 4 )

#association
vx_g2_x2_y0_z4_mod.setAssociation( vx_g2_3c_f0_z4 )
vx_g2_x2_y1_z4_mod.setAssociation( vx_g2_3c_f1_z4 )
vx_g2_x2_y2_z4_mod.setAssociation( vx_g2_3c_f2_z4 )
vx_g2_x2_y3_z4_mod.setAssociation( vx_g2_3c_f3_z4 )
vx_g2_x2_y4_z4_mod.setAssociation( vx_g2_3c_f4_z4 )
vx_g2_x2_y5_z4_mod.setAssociation( vx_g2_3c_f5_z4 )


#geom
vx_g2_3c_f0_z5 = geompy.MakeTranslation( vx_g2_3c_f0_z2 , 0 , 0 , z5  )
vx_g2_3c_f1_z5 = geompy.MakeTranslation( vx_g2_3c_f1_z2 , 0 , 0 , z5  )
vx_g2_3c_f2_z5 = geompy.MakeTranslation( vx_g2_3c_f2_z2 , 0 , 0 , z5  )
vx_g2_3c_f3_z5 = geompy.MakeTranslation( vx_g2_3c_f3_z2 , 0 , 0 , z5  )
vx_g2_3c_f4_z5 = geompy.MakeTranslation( vx_g2_3c_f4_z2 , 0 , 0 , z5  )
vx_g2_3c_f5_z5 = geompy.MakeTranslation( vx_g2_3c_f5_z2 , 0 , 0 , z5  )

#modele
vx_g2_x2_y0_z5_mod = grille_cyl_2.getVertexXYZ( 2 , 0 , 5 )
vx_g2_x2_y1_z5_mod = grille_cyl_2.getVertexXYZ( 2 , 1 , 5 )
vx_g2_x2_y2_z5_mod = grille_cyl_2.getVertexXYZ( 2 , 2 , 5 )
vx_g2_x2_y3_z5_mod = grille_cyl_2.getVertexXYZ( 2 , 3 , 5 )
vx_g2_x2_y4_z5_mod = grille_cyl_2.getVertexXYZ( 2 , 4 , 5 )
vx_g2_x2_y5_z5_mod = grille_cyl_2.getVertexXYZ( 2 , 5 , 5 )

#association
vx_g2_x2_y0_z5_mod.setAssociation( vx_g2_3c_f0_z5 )
vx_g2_x2_y1_z5_mod.setAssociation( vx_g2_3c_f1_z5 )
vx_g2_x2_y2_z5_mod.setAssociation( vx_g2_3c_f2_z5 )
vx_g2_x2_y3_z5_mod.setAssociation( vx_g2_3c_f3_z5 )
vx_g2_x2_y4_z5_mod.setAssociation( vx_g2_3c_f4_z5 )
vx_g2_x2_y5_z5_mod.setAssociation( vx_g2_3c_f5_z5 )




Pt_B_3 = geompy.MakeVertex( 33.5 , 9 , 1.5 )
Edge_B_3 = geompy.MakeEdgeNearPoint( Bride_geom , Pt_B_3 )

#geom
vx_g2_4c_f0_z2 = geompy.MakeVertexOnCurve( Edge_B_3 , f0 )
vx_g2_4c_f1_z2 = geompy.MakeVertexOnCurve( Edge_B_3 , f1 )
vx_g2_4c_f2_z2 = geompy.MakeVertexOnCurve( Edge_B_3 , f2 )
vx_g2_4c_f3_z2 = geompy.MakeVertexOnCurve( Edge_B_3 , f3 )
vx_g2_4c_f4_z2 = geompy.MakeVertexOnCurve( Edge_B_3 , f4 )
vx_g2_4c_f5_z2 = geompy.MakeVertexOnCurve( Edge_B_3 , f5 )

#modele
vx_g2_x3_y0_z2_mod = grille_cyl_2.getVertexXYZ( 3 , 0 , 2 )
vx_g2_x3_y1_z2_mod = grille_cyl_2.getVertexXYZ( 3 , 1 , 2 )
vx_g2_x3_y2_z2_mod = grille_cyl_2.getVertexXYZ( 3 , 2 , 2 )
vx_g2_x3_y3_z2_mod = grille_cyl_2.getVertexXYZ( 3 , 3 , 2 )
vx_g2_x3_y4_z2_mod = grille_cyl_2.getVertexXYZ( 3 , 4 , 2 )
vx_g2_x3_y5_z2_mod = grille_cyl_2.getVertexXYZ( 3 , 5 , 2 )

#association
vx_g2_x3_y0_z2_mod.setAssociation( vx_g2_4c_f0_z2 )
vx_g2_x3_y1_z2_mod.setAssociation( vx_g2_4c_f1_z2 )
vx_g2_x3_y2_z2_mod.setAssociation( vx_g2_4c_f2_z2 )
vx_g2_x3_y3_z2_mod.setAssociation( vx_g2_4c_f3_z2 )
vx_g2_x3_y4_z2_mod.setAssociation( vx_g2_4c_f4_z2 )
vx_g2_x3_y5_z2_mod.setAssociation( vx_g2_4c_f5_z2 )


#geom
vx_g2_4c_f0_z3 = geompy.MakeTranslation( vx_g2_4c_f0_z2 , 0 , 0 , z3  )
vx_g2_4c_f1_z3 = geompy.MakeTranslation( vx_g2_4c_f1_z2 , 0 , 0 , z3  )
vx_g2_4c_f2_z3 = geompy.MakeTranslation( vx_g2_4c_f2_z2 , 0 , 0 , z3  )
vx_g2_4c_f3_z3 = geompy.MakeTranslation( vx_g2_4c_f3_z2 , 0 , 0 , z3  )
vx_g2_4c_f4_z3 = geompy.MakeTranslation( vx_g2_4c_f4_z2 , 0 , 0 , z3  )
vx_g2_4c_f5_z3 = geompy.MakeTranslation( vx_g2_4c_f5_z2 , 0 , 0 , z3  )

#modele
vx_g2_x3_y0_z3_mod = grille_cyl_2.getVertexXYZ( 3 , 0 , 3 )
vx_g2_x3_y1_z3_mod = grille_cyl_2.getVertexXYZ( 3 , 1 , 3 )
vx_g2_x3_y2_z3_mod = grille_cyl_2.getVertexXYZ( 3 , 2 , 3 )
vx_g2_x3_y3_z3_mod = grille_cyl_2.getVertexXYZ( 3 , 3 , 3 )
vx_g2_x3_y4_z3_mod = grille_cyl_2.getVertexXYZ( 3 , 4 , 3 )
vx_g2_x3_y5_z3_mod = grille_cyl_2.getVertexXYZ( 3 , 5 , 3 )

#association
vx_g2_x3_y0_z3_mod.setAssociation( vx_g2_4c_f0_z3 )
vx_g2_x3_y1_z3_mod.setAssociation( vx_g2_4c_f1_z3 )
vx_g2_x3_y2_z3_mod.setAssociation( vx_g2_4c_f2_z3 )
vx_g2_x3_y3_z3_mod.setAssociation( vx_g2_4c_f3_z3 )
vx_g2_x3_y4_z3_mod.setAssociation( vx_g2_4c_f4_z3 )
vx_g2_x3_y5_z3_mod.setAssociation( vx_g2_4c_f5_z3 )


#geom
vx_g2_4c_f0_z4 = geompy.MakeTranslation( vx_g2_4c_f0_z2 , 0 , 0 , z4  )
vx_g2_4c_f1_z4 = geompy.MakeTranslation( vx_g2_4c_f1_z2 , 0 , 0 , z4  )
vx_g2_4c_f2_z4 = geompy.MakeTranslation( vx_g2_4c_f2_z2 , 0 , 0 , z4  )
vx_g2_4c_f3_z4 = geompy.MakeTranslation( vx_g2_4c_f3_z2 , 0 , 0 , z4  )
vx_g2_4c_f4_z4 = geompy.MakeTranslation( vx_g2_4c_f4_z2 , 0 , 0 , z4  )
vx_g2_4c_f5_z4 = geompy.MakeTranslation( vx_g2_4c_f5_z2 , 0 , 0 , z4  )

#modele
vx_g2_x3_y0_z4_mod = grille_cyl_2.getVertexXYZ( 3 , 0 , 4 )
vx_g2_x3_y1_z4_mod = grille_cyl_2.getVertexXYZ( 3 , 1 , 4 )
vx_g2_x3_y2_z4_mod = grille_cyl_2.getVertexXYZ( 3 , 2 , 4 )
vx_g2_x3_y3_z4_mod = grille_cyl_2.getVertexXYZ( 3 , 3 , 4 )
vx_g2_x3_y4_z4_mod = grille_cyl_2.getVertexXYZ( 3 , 4 , 4 )
vx_g2_x3_y5_z4_mod = grille_cyl_2.getVertexXYZ( 3 , 5 , 4 )

#association
vx_g2_x3_y0_z4_mod.setAssociation( vx_g2_4c_f0_z4 )
vx_g2_x3_y1_z4_mod.setAssociation( vx_g2_4c_f1_z4 )
vx_g2_x3_y2_z4_mod.setAssociation( vx_g2_4c_f2_z4 )
vx_g2_x3_y3_z4_mod.setAssociation( vx_g2_4c_f3_z4 )
vx_g2_x3_y4_z4_mod.setAssociation( vx_g2_4c_f4_z4 )
vx_g2_x3_y5_z4_mod.setAssociation( vx_g2_4c_f5_z4 )


#geom
vx_g2_4c_f0_z5 = geompy.MakeTranslation( vx_g2_4c_f0_z2 , 0 , 0 , z5  )
vx_g2_4c_f1_z5 = geompy.MakeTranslation( vx_g2_4c_f1_z2 , 0 , 0 , z5  )
vx_g2_4c_f2_z5 = geompy.MakeTranslation( vx_g2_4c_f2_z2 , 0 , 0 , z5  )
vx_g2_4c_f3_z5 = geompy.MakeTranslation( vx_g2_4c_f3_z2 , 0 , 0 , z5  )
vx_g2_4c_f4_z5 = geompy.MakeTranslation( vx_g2_4c_f4_z2 , 0 , 0 , z5  )
vx_g2_4c_f5_z5 = geompy.MakeTranslation( vx_g2_4c_f5_z2 , 0 , 0 , z5  )

#modele
vx_g2_x3_y0_z5_mod = grille_cyl_2.getVertexXYZ( 3 , 0 , 5 )
vx_g2_x3_y1_z5_mod = grille_cyl_2.getVertexXYZ( 3 , 1 , 5 )
vx_g2_x3_y2_z5_mod = grille_cyl_2.getVertexXYZ( 3 , 2 , 5 )
vx_g2_x3_y3_z5_mod = grille_cyl_2.getVertexXYZ( 3 , 3 , 5 )
vx_g2_x3_y4_z5_mod = grille_cyl_2.getVertexXYZ( 3 , 4 , 5 )
vx_g2_x3_y5_z5_mod = grille_cyl_2.getVertexXYZ( 3 , 5 , 5 )

#association
vx_g2_x3_y0_z5_mod.setAssociation( vx_g2_4c_f0_z5 )
vx_g2_x3_y1_z5_mod.setAssociation( vx_g2_4c_f1_z5 )
vx_g2_x3_y2_z5_mod.setAssociation( vx_g2_4c_f2_z5 )
vx_g2_x3_y3_z5_mod.setAssociation( vx_g2_4c_f3_z5 )
vx_g2_x3_y4_z5_mod.setAssociation( vx_g2_4c_f4_z5 )
vx_g2_x3_y5_z5_mod.setAssociation( vx_g2_4c_f5_z5 )




Pt_B_4 = geompy.MakeVertex( 33.5 , 12.5 , 1.5 )
Edge_B_4 = geompy.MakeEdgeNearPoint( Bride_geom , Pt_B_4 )

#geom
vx_g2_5c_f0_z2 = geompy.MakeVertexOnCurve( Edge_B_4 , f0 )
vx_g2_5c_f1_z2 = geompy.MakeVertexOnCurve( Edge_B_4 , f1 )
vx_g2_5c_f2_z2 = geompy.MakeVertexOnCurve( Edge_B_4 , f2 )

#modele
vx_g2_x4_y0_z2_mod = grille_cyl_2.getVertexXYZ( 4 , 0 , 2 )
vx_g2_x4_y1_z2_mod = grille_cyl_2.getVertexXYZ( 4 , 1 , 2 )
vx_g2_x4_y2_z2_mod = grille_cyl_2.getVertexXYZ( 4 , 2 , 2 )

#association
vx_g2_x4_y0_z2_mod.setAssociation( vx_g2_5c_f0_z2 )
vx_g2_x4_y1_z2_mod.setAssociation( vx_g2_5c_f1_z2 )
vx_g2_x4_y2_z2_mod.setAssociation( vx_g2_5c_f2_z2 )


#geom
vx_g2_5c_f0_z3 = geompy.MakeTranslation( vx_g2_5c_f0_z2 , 0 , 0 , z3  )
vx_g2_5c_f1_z3 = geompy.MakeTranslation( vx_g2_5c_f1_z2 , 0 , 0 , z3  )
vx_g2_5c_f2_z3 = geompy.MakeTranslation( vx_g2_5c_f2_z2 , 0 , 0 , z3  )

#modele
vx_g2_x4_y0_z3_mod = grille_cyl_2.getVertexXYZ( 4 , 0 , 3 )
vx_g2_x4_y1_z3_mod = grille_cyl_2.getVertexXYZ( 4 , 1 , 3 )
vx_g2_x4_y2_z3_mod = grille_cyl_2.getVertexXYZ( 4 , 2 , 3 )

#association
vx_g2_x4_y0_z3_mod.setAssociation( vx_g2_5c_f0_z3 )
vx_g2_x4_y1_z3_mod.setAssociation( vx_g2_5c_f1_z3 )
vx_g2_x4_y2_z3_mod.setAssociation( vx_g2_5c_f2_z3 )


#geom
vx_g2_5c_f0_z4 = geompy.MakeTranslation( vx_g2_5c_f0_z2 , 0 , 0 , z4  )
vx_g2_5c_f1_z4 = geompy.MakeTranslation( vx_g2_5c_f1_z2 , 0 , 0 , z4  )
vx_g2_5c_f2_z4 = geompy.MakeTranslation( vx_g2_5c_f2_z2 , 0 , 0 , z4  )

#modele
vx_g2_x4_y0_z4_mod = grille_cyl_2.getVertexXYZ( 4 , 0 , 4 )
vx_g2_x4_y1_z4_mod = grille_cyl_2.getVertexXYZ( 4 , 1 , 4 )
vx_g2_x4_y2_z4_mod = grille_cyl_2.getVertexXYZ( 4 , 2 , 4 )

#association
vx_g2_x4_y0_z4_mod.setAssociation( vx_g2_5c_f0_z4 )
vx_g2_x4_y1_z4_mod.setAssociation( vx_g2_5c_f1_z4 )
vx_g2_x4_y2_z4_mod.setAssociation( vx_g2_5c_f2_z4 )


#geom
vx_g2_5c_f5_z1 = geompy.MakeVertex( 22.5 , 0 , z1 )
vx_g2_5c_f5_z0 = geompy.MakeTranslation( vx_g2_5c_f5_z1 , 0 , 0 , z0  )
vx_g2_5c_f5_z2 = geompy.MakeTranslation( vx_g2_5c_f5_z1 , 0 , 0 , z2  )
vx_g2_5c_f5_z3 = geompy.MakeTranslation( vx_g2_5c_f5_z1 , 0 , 0 , z3  )
vx_g2_5c_f5_z4 = geompy.MakeTranslation( vx_g2_5c_f5_z1 , 0 , 0 , z4  )

#modele
vx_g2_x4_y5_z0_mod = grille_cyl_2.getVertexXYZ( 4 , 5 , 0 )
vx_g2_x4_y5_z1_mod = grille_cyl_2.getVertexXYZ( 4 , 5 , 1 )
vx_g2_x4_y5_z2_mod = grille_cyl_2.getVertexXYZ( 4 , 5 , 2 )
vx_g2_x4_y5_z3_mod = grille_cyl_2.getVertexXYZ( 4 , 5 , 3 )
vx_g2_x4_y5_z4_mod = grille_cyl_2.getVertexXYZ( 4 , 5 , 4 )

#association
vx_g2_x4_y5_z0_mod.setAssociation( vx_g2_5c_f5_z0 )
vx_g2_x4_y5_z1_mod.setAssociation( vx_g2_5c_f5_z1 )
vx_g2_x4_y5_z2_mod.setAssociation( vx_g2_5c_f5_z2 )
vx_g2_x4_y5_z3_mod.setAssociation( vx_g2_5c_f5_z3 )
vx_g2_x4_y5_z4_mod.setAssociation( vx_g2_5c_f5_z4 )


#geom
vx_g2_5c_f4_z0 = geompy.MakeRotation( vx_g2_5c_f5_z0 , DZ , (math.pi*6)/180 )
vx_g2_5c_f4_z1 = geompy.MakeRotation( vx_g2_5c_f5_z1 , DZ , (math.pi*6)/180 )
vx_g2_5c_f4_z2 = geompy.MakeRotation( vx_g2_5c_f5_z2 , DZ , (math.pi*6)/180 )
vx_g2_5c_f4_z3 = geompy.MakeRotation( vx_g2_5c_f5_z3 , DZ , (math.pi*6)/180 )
vx_g2_5c_f4_z4 = geompy.MakeRotation( vx_g2_5c_f5_z4 , DZ , (math.pi*6)/180 )

#modele
vx_g2_x4_y4_z0_mod = grille_cyl_2.getVertexXYZ( 4 , 4 , 0 )
vx_g2_x4_y4_z1_mod = grille_cyl_2.getVertexXYZ( 4 , 4 , 1 )
vx_g2_x4_y4_z2_mod = grille_cyl_2.getVertexXYZ( 4 , 4 , 2 )
vx_g2_x4_y4_z3_mod = grille_cyl_2.getVertexXYZ( 4 , 4 , 3 )
vx_g2_x4_y4_z4_mod = grille_cyl_2.getVertexXYZ( 4 , 4 , 4 )

#association
vx_g2_x4_y4_z0_mod.setAssociation( vx_g2_5c_f4_z0 )
vx_g2_x4_y4_z1_mod.setAssociation( vx_g2_5c_f4_z1 )
vx_g2_x4_y4_z2_mod.setAssociation( vx_g2_5c_f4_z2 )
vx_g2_x4_y4_z3_mod.setAssociation( vx_g2_5c_f4_z3 )
vx_g2_x4_y4_z4_mod.setAssociation( vx_g2_5c_f4_z4 )


#geom
vx_g2_6c_f0_z2 = geompy.MakeVertex( 47.5 , 0 , z2 )
vx_g2_6c_f0_z3 = geompy.MakeVertex( 47.5 , 0 , z3 )

#modele
vx_g2_x5_y0_z2_mod = grille_cyl_2.getVertexXYZ( 5 , 0 , 2 )
vx_g2_x5_y0_z3_mod = grille_cyl_2.getVertexXYZ( 5 , 0 , 3 )

#association
vx_g2_x5_y0_z2_mod.setAssociation( vx_g2_6c_f0_z2 )
vx_g2_x5_y0_z3_mod.setAssociation( vx_g2_6c_f0_z3 )


#geom
vx_g2_6c_f1_z2 = geompy.MakeRotation( vx_g2_6c_f0_z2 , DZ , math.pi/8 )
vx_g2_6c_f1_z3 = geompy.MakeRotation( vx_g2_6c_f0_z3 , DZ , math.pi/8 )

#modele
vx_g2_x5_y1_z2_mod = grille_cyl_2.getVertexXYZ( 5 , 1 , 2 )
vx_g2_x5_y1_z3_mod = grille_cyl_2.getVertexXYZ( 5 , 1 , 3 )

#association
vx_g2_x5_y1_z2_mod.setAssociation( vx_g2_6c_f1_z2 )
vx_g2_x5_y1_z3_mod.setAssociation( vx_g2_6c_f1_z3 )


#geom
vx_g2_6c_f2_z2 = geompy.MakeRotation( vx_g2_6c_f0_z2 , DZ , math.pi/4 )
vx_g2_6c_f2_z3 = geompy.MakeRotation( vx_g2_6c_f0_z3 , DZ , math.pi/4 )

#modele
vx_g2_x5_y2_z2_mod = grille_cyl_2.getVertexXYZ( 5 , 2 , 2 )
vx_g2_x5_y2_z3_mod = grille_cyl_2.getVertexXYZ( 5 , 2 , 3 )

#association
vx_g2_x5_y2_z2_mod.setAssociation( vx_g2_6c_f2_z2 )
vx_g2_x5_y2_z3_mod.setAssociation( vx_g2_6c_f2_z3 )


#geom
vx_g2_6c_f5_z1 = geompy.MakeVertex( 24 , 0 , z1 )
vx_g2_6c_f5_z0 = geompy.MakeTranslation( vx_g2_6c_f5_z1 , 0 , 0 , z0  )
vx_g2_6c_f5_z2 = geompy.MakeTranslation( vx_g2_6c_f5_z1 , 0 , 0 , z2  )
vx_g2_6c_f5_z3 = geompy.MakeTranslation( vx_g2_6c_f5_z1 , 0 , 0 , z3  )
vx_g2_6c_f5_z4 = geompy.MakeTranslation( vx_g2_6c_f5_z1 , 0 , 0 , z4  )

#modele
vx_g2_x5_y5_z0_mod = grille_cyl_2.getVertexXYZ( 5 , 5 , 0 )
vx_g2_x5_y5_z1_mod = grille_cyl_2.getVertexXYZ( 5 , 5 , 1 )
vx_g2_x5_y5_z2_mod = grille_cyl_2.getVertexXYZ( 5 , 5 , 2 )
vx_g2_x5_y5_z3_mod = grille_cyl_2.getVertexXYZ( 5 , 5 , 3 )
vx_g2_x5_y5_z4_mod = grille_cyl_2.getVertexXYZ( 5 , 5 , 4 )

#association
vx_g2_x5_y5_z0_mod.setAssociation( vx_g2_6c_f5_z0 )
vx_g2_x5_y5_z1_mod.setAssociation( vx_g2_6c_f5_z1 )
vx_g2_x5_y5_z2_mod.setAssociation( vx_g2_6c_f5_z2 )
vx_g2_x5_y5_z3_mod.setAssociation( vx_g2_6c_f5_z3 )
vx_g2_x5_y5_z4_mod.setAssociation( vx_g2_6c_f5_z4 )


#geom
vx_g2_6c_f4_z0 = geompy.MakeRotation( vx_g2_6c_f5_z0 , DZ , (math.pi*8)/180 )
vx_g2_6c_f4_z1 = geompy.MakeRotation( vx_g2_6c_f5_z1 , DZ , (math.pi*8)/180 )
vx_g2_6c_f4_z2 = geompy.MakeRotation( vx_g2_6c_f5_z2 , DZ , (math.pi*8)/180 )
vx_g2_6c_f4_z3 = geompy.MakeRotation( vx_g2_6c_f5_z3 , DZ , (math.pi*8)/180 )
vx_g2_6c_f4_z4 = geompy.MakeRotation( vx_g2_6c_f5_z4 , DZ , (math.pi*8)/180 )

#modele
vx_g2_x5_y4_z0_mod = grille_cyl_2.getVertexXYZ( 5 , 4 , 0 )
vx_g2_x5_y4_z1_mod = grille_cyl_2.getVertexXYZ( 5 , 4 , 1 )
vx_g2_x5_y4_z2_mod = grille_cyl_2.getVertexXYZ( 5 , 4 , 2 )
vx_g2_x5_y4_z3_mod = grille_cyl_2.getVertexXYZ( 5 , 4 , 3 )
vx_g2_x5_y4_z4_mod = grille_cyl_2.getVertexXYZ( 5 , 4 , 4 )

#association
vx_g2_x5_y4_z0_mod.setAssociation( vx_g2_6c_f4_z0 )
vx_g2_x5_y4_z1_mod.setAssociation( vx_g2_6c_f4_z1 )
vx_g2_x5_y4_z2_mod.setAssociation( vx_g2_6c_f4_z2 )
vx_g2_x5_y4_z3_mod.setAssociation( vx_g2_6c_f4_z3 )
vx_g2_x5_y4_z4_mod.setAssociation( vx_g2_6c_f4_z4 )



# les hexas de remplissage de la 2eme grille

#geom
vx_g2_1c_f0_z1 = geompy.MakeVertex( 35.5 , 0 , z1 )
vx_g2_1c_f1_z1 = geompy.MakeTranslation( vx_g2_1c_f0_z1 , 0 , 1.5 , 0  )
vx_g2_1c_f7_z1 = geompy.MakeVertex( 34.5 , 0 , z1 )
vx_g2_1c_f2_z1 = geompy.MakeTranslation( vx_g2_1c_f7_z1 , 0 , 1.5 , 0  )
vx_g2_1c_f8_z1 = geompy.MakeVertex( 33.5 , 0 , z1 )
vx_g2_1c_f3_z1 = geompy.MakeTranslation( vx_g2_1c_f8_z1 , 0 , 1.5 , 0  )
vx_g2_1c_f5_z1 = geompy.MakeVertex( 32.5 , 0 , z1 )
vx_g2_1c_f4_z1 = geompy.MakeTranslation( vx_g2_1c_f5_z1 , 0 , 1.5 , 0  )


#modele
vx_g2_x0_y0_z1_mod = grille_cyl_2.getVertexXYZ( 0 , 0 , 1 )
vx_g2_x0_y1_z1_mod = grille_cyl_2.getVertexXYZ( 0 , 1 , 1 )
vx_g2_x0_y2_z1_mod = grille_cyl_2.getVertexXYZ( 0 , 2 , 1 )
vx_g2_x0_y3_z1_mod = grille_cyl_2.getVertexXYZ( 0 , 3 , 1 )
vx_g2_x0_y4_z1_mod = grille_cyl_2.getVertexXYZ( 0 , 4 , 1 )
vx_g2_x0_y5_z1_mod = grille_cyl_2.getVertexXYZ( 0 , 5 , 1 )
vx_g2_x0_y7_z1_mod = doc.findVertex( P + 0.5 , 0 , h/7 )
vx_g2_x0_y8_z1_mod = doc.findVertex( P - 0.5 , 0 , h/7 )

#association
vx_g2_x0_y0_z1_mod.setAssociation( vx_g2_1c_f0_z1 )
vx_g2_x0_y1_z1_mod.setAssociation( vx_g2_1c_f1_z1 )
vx_g2_x0_y2_z1_mod.setAssociation( vx_g2_1c_f2_z1 )
vx_g2_x0_y3_z1_mod.setAssociation( vx_g2_1c_f3_z1 )
vx_g2_x0_y4_z1_mod.setAssociation( vx_g2_1c_f4_z1 )
vx_g2_x0_y5_z1_mod.setAssociation( vx_g2_1c_f5_z1 )
vx_g2_x0_y7_z1_mod.setAssociation( vx_g2_1c_f7_z1 )
vx_g2_x0_y8_z1_mod.setAssociation( vx_g2_1c_f8_z1 )




#geom
vx_g2_1c_f0_z0 = geompy.MakeTranslation( vx_g2_1c_f0_z1 , 0 , 0 , z0  )
vx_g2_1c_f1_z0 = geompy.MakeTranslation( vx_g2_1c_f1_z1 , 0 , 0 , z0  )
vx_g2_1c_f2_z0 = geompy.MakeTranslation( vx_g2_1c_f2_z1 , 0 , 0 , z0  )
vx_g2_1c_f3_z0 = geompy.MakeTranslation( vx_g2_1c_f3_z1 , 0 , 0 , z0  )
vx_g2_1c_f4_z0 = geompy.MakeTranslation( vx_g2_1c_f4_z1 , 0 , 0 , z0  )
vx_g2_1c_f5_z0 = geompy.MakeTranslation( vx_g2_1c_f5_z1 , 0 , 0 , z0  )
vx_g2_1c_f7_z0 = geompy.MakeTranslation( vx_g2_1c_f7_z1 , 0 , 0 , z0  )
vx_g2_1c_f8_z0 = geompy.MakeTranslation( vx_g2_1c_f8_z1 , 0 , 0 , z0  )

#modele
vx_g2_x0_y0_z0_mod = grille_cyl_2.getVertexXYZ( 0 , 0 , 0 )
vx_g2_x0_y1_z0_mod = grille_cyl_2.getVertexXYZ( 0 , 1 , 0 )
vx_g2_x0_y2_z0_mod = grille_cyl_2.getVertexXYZ( 0 , 2 , 0 )
vx_g2_x0_y3_z0_mod = grille_cyl_2.getVertexXYZ( 0 , 3 , 0 )
vx_g2_x0_y4_z0_mod = grille_cyl_2.getVertexXYZ( 0 , 4 , 0 )
vx_g2_x0_y5_z0_mod = grille_cyl_2.getVertexXYZ( 0 , 5 , 0 )
vx_g2_x0_y7_z0_mod = doc.findVertex( P + 0.5 , 0 , 0 )
vx_g2_x0_y8_z0_mod = doc.findVertex( P - 0.5 , 0 , 0 )

#association
vx_g2_x0_y0_z0_mod.setAssociation( vx_g2_1c_f0_z0 )
vx_g2_x0_y1_z0_mod.setAssociation( vx_g2_1c_f1_z0 )
vx_g2_x0_y2_z0_mod.setAssociation( vx_g2_1c_f2_z0 )
vx_g2_x0_y3_z0_mod.setAssociation( vx_g2_1c_f3_z0 )
vx_g2_x0_y4_z0_mod.setAssociation( vx_g2_1c_f4_z0 )
vx_g2_x0_y5_z0_mod.setAssociation( vx_g2_1c_f5_z0 )
vx_g2_x0_y7_z0_mod.setAssociation( vx_g2_1c_f7_z0 )
vx_g2_x0_y8_z0_mod.setAssociation( vx_g2_1c_f8_z0 )



#geom
vx_g2_1c_f0_z2 = geompy.MakeTranslation( vx_g2_1c_f0_z1 , 0 , 0 , z2  )
vx_g2_1c_f1_z2 = geompy.MakeTranslation( vx_g2_1c_f1_z1 , 0 , 0 , z2  )
vx_g2_1c_f2_z2 = geompy.MakeTranslation( vx_g2_1c_f2_z1 , 0 , 0 , z2  )
vx_g2_1c_f3_z2 = geompy.MakeTranslation( vx_g2_1c_f3_z1 , 0 , 0 , z2  )
vx_g2_1c_f4_z2 = geompy.MakeTranslation( vx_g2_1c_f4_z1 , 0 , 0 , z2  )
vx_g2_1c_f5_z2 = geompy.MakeTranslation( vx_g2_1c_f5_z1 , 0 , 0 , z2  )
vx_g2_1c_f7_z2 = geompy.MakeTranslation( vx_g2_1c_f7_z1 , 0 , 0 , z2  )
vx_g2_1c_f8_z2 = geompy.MakeTranslation( vx_g2_1c_f8_z1 , 0 , 0 , z2  )

#modele
vx_g2_x0_y0_z2_mod = grille_cyl_2.getVertexXYZ( 0 , 0 , 2 )
vx_g2_x0_y1_z2_mod = grille_cyl_2.getVertexXYZ( 0 , 1 , 2 )
vx_g2_x0_y2_z2_mod = grille_cyl_2.getVertexXYZ( 0 , 2 , 2 )
vx_g2_x0_y3_z2_mod = grille_cyl_2.getVertexXYZ( 0 , 3 , 2 )
vx_g2_x0_y4_z2_mod = grille_cyl_2.getVertexXYZ( 0 , 4 , 2 )
vx_g2_x0_y5_z2_mod = grille_cyl_2.getVertexXYZ( 0 , 5 , 2 )
vx_g2_x0_y7_z2_mod = doc.findVertex( P + 0.5 , 0 , 2*h/7 )
vx_g2_x0_y8_z2_mod = doc.findVertex( P - 0.5 , 0 , 2*h/7 )

#association
vx_g2_x0_y0_z2_mod.setAssociation( vx_g2_1c_f0_z2 )
vx_g2_x0_y1_z2_mod.setAssociation( vx_g2_1c_f1_z2 )
vx_g2_x0_y2_z2_mod.setAssociation( vx_g2_1c_f2_z2 )
vx_g2_x0_y3_z2_mod.setAssociation( vx_g2_1c_f3_z2 )
vx_g2_x0_y4_z2_mod.setAssociation( vx_g2_1c_f4_z2 )
vx_g2_x0_y5_z2_mod.setAssociation( vx_g2_1c_f5_z2 )
vx_g2_x0_y7_z2_mod.setAssociation( vx_g2_1c_f7_z2 )
vx_g2_x0_y8_z2_mod.setAssociation( vx_g2_1c_f8_z2 )



#geom
vx_g2_1c_f0_z3 = geompy.MakeTranslation( vx_g2_1c_f0_z1 , 0 , 0 , z3  )
vx_g2_1c_f1_z3 = geompy.MakeTranslation( vx_g2_1c_f1_z1 , 0 , 0 , z3  )
vx_g2_1c_f2_z3 = geompy.MakeTranslation( vx_g2_1c_f2_z1 , 0 , 0 , z3  )
vx_g2_1c_f3_z3 = geompy.MakeTranslation( vx_g2_1c_f3_z1 , 0 , 0 , z3  )
vx_g2_1c_f4_z3 = geompy.MakeTranslation( vx_g2_1c_f4_z1 , 0 , 0 , z3  )
vx_g2_1c_f5_z3 = geompy.MakeTranslation( vx_g2_1c_f5_z1 , 0 , 0 , z3  )
vx_g2_1c_f7_z3 = geompy.MakeTranslation( vx_g2_1c_f7_z1 , 0 , 0 , z3  )
vx_g2_1c_f8_z3 = geompy.MakeTranslation( vx_g2_1c_f8_z1 , 0 , 0 , z3  )

#modele
vx_g2_x0_y0_z3_mod = grille_cyl_2.getVertexXYZ( 0 , 0 , 3 )
vx_g2_x0_y1_z3_mod = grille_cyl_2.getVertexXYZ( 0 , 1 , 3 )
vx_g2_x0_y2_z3_mod = grille_cyl_2.getVertexXYZ( 0 , 2 , 3 )
vx_g2_x0_y3_z3_mod = grille_cyl_2.getVertexXYZ( 0 , 3 , 3 )
vx_g2_x0_y4_z3_mod = grille_cyl_2.getVertexXYZ( 0 , 4 , 3 )
vx_g2_x0_y5_z3_mod = grille_cyl_2.getVertexXYZ( 0 , 5 , 3 )
vx_g2_x0_y7_z3_mod = doc.findVertex( P + 0.5 , 0 , 3*h/7 )
vx_g2_x0_y8_z3_mod = doc.findVertex( P - 0.5 , 0 , 3*h/7 )

#association
vx_g2_x0_y0_z3_mod.setAssociation( vx_g2_1c_f0_z3 )
vx_g2_x0_y1_z3_mod.setAssociation( vx_g2_1c_f1_z3 )
vx_g2_x0_y2_z3_mod.setAssociation( vx_g2_1c_f2_z3 )
vx_g2_x0_y3_z3_mod.setAssociation( vx_g2_1c_f3_z3 )
vx_g2_x0_y4_z3_mod.setAssociation( vx_g2_1c_f4_z3 )
vx_g2_x0_y5_z3_mod.setAssociation( vx_g2_1c_f5_z3 )
vx_g2_x0_y7_z3_mod.setAssociation( vx_g2_1c_f7_z3 )
vx_g2_x0_y8_z3_mod.setAssociation( vx_g2_1c_f8_z3 )



#geom
vx_g2_1c_f0_z4 = geompy.MakeTranslation( vx_g2_1c_f0_z1 , 0 , 0 , z4  )
vx_g2_1c_f1_z4 = geompy.MakeTranslation( vx_g2_1c_f1_z1 , 0 , 0 , z4  )
vx_g2_1c_f2_z4 = geompy.MakeTranslation( vx_g2_1c_f2_z1 , 0 , 0 , z4  )
vx_g2_1c_f3_z4 = geompy.MakeTranslation( vx_g2_1c_f3_z1 , 0 , 0 , z4  )
vx_g2_1c_f4_z4 = geompy.MakeTranslation( vx_g2_1c_f4_z1 , 0 , 0 , z4  )
vx_g2_1c_f5_z4 = geompy.MakeTranslation( vx_g2_1c_f5_z1 , 0 , 0 , z4  )
vx_g2_1c_f7_z4 = geompy.MakeTranslation( vx_g2_1c_f7_z1 , 0 , 0 , z4  )
vx_g2_1c_f8_z4 = geompy.MakeTranslation( vx_g2_1c_f8_z1 , 0 , 0 , z4  )

#modele
vx_g2_x0_y0_z4_mod = grille_cyl_2.getVertexXYZ( 0 , 0 , 4 )
vx_g2_x0_y1_z4_mod = grille_cyl_2.getVertexXYZ( 0 , 1 , 4 )
vx_g2_x0_y2_z4_mod = grille_cyl_2.getVertexXYZ( 0 , 2 , 4 )
vx_g2_x0_y3_z4_mod = grille_cyl_2.getVertexXYZ( 0 , 3 , 4 )
vx_g2_x0_y4_z4_mod = grille_cyl_2.getVertexXYZ( 0 , 4 , 4 )
vx_g2_x0_y5_z4_mod = grille_cyl_2.getVertexXYZ( 0 , 5 , 4 )
vx_g2_x0_y7_z4_mod = doc.findVertex( P + 0.5 , 0 , 4*h/7 )
vx_g2_x0_y8_z4_mod = doc.findVertex( P - 0.5 , 0 , 4*h/7 )

#association
vx_g2_x0_y0_z4_mod.setAssociation( vx_g2_1c_f0_z4 )
vx_g2_x0_y1_z4_mod.setAssociation( vx_g2_1c_f1_z4 )
vx_g2_x0_y2_z4_mod.setAssociation( vx_g2_1c_f2_z4 )
vx_g2_x0_y3_z4_mod.setAssociation( vx_g2_1c_f3_z4 )
vx_g2_x0_y4_z4_mod.setAssociation( vx_g2_1c_f4_z4 )
vx_g2_x0_y5_z4_mod.setAssociation( vx_g2_1c_f5_z4 )
vx_g2_x0_y7_z4_mod.setAssociation( vx_g2_1c_f7_z4 )
vx_g2_x0_y8_z4_mod.setAssociation( vx_g2_1c_f8_z4 )



#geom
vx_g2_1c_f0_z5 = geompy.MakeTranslation( vx_g2_1c_f0_z1 , 0 , 0 , z5  )
vx_g2_1c_f1_z5 = geompy.MakeTranslation( vx_g2_1c_f1_z1 , 0 , 0 , z5  )
vx_g2_1c_f2_z5 = geompy.MakeTranslation( vx_g2_1c_f2_z1 , 0 , 0 , z5  )
vx_g2_1c_f3_z5 = geompy.MakeTranslation( vx_g2_1c_f3_z1 , 0 , 0 , z5  )
vx_g2_1c_f4_z5 = geompy.MakeTranslation( vx_g2_1c_f4_z1 , 0 , 0 , z5  )
vx_g2_1c_f5_z5 = geompy.MakeTranslation( vx_g2_1c_f5_z1 , 0 , 0 , z5  )
vx_g2_1c_f7_z5 = geompy.MakeTranslation( vx_g2_1c_f7_z1 , 0 , 0 , z5  )
vx_g2_1c_f8_z5 = geompy.MakeTranslation( vx_g2_1c_f8_z1 , 0 , 0 , z5  )

#modele
vx_g2_x0_y0_z5_mod = grille_cyl_2.getVertexXYZ( 0 , 0 , 5 )
vx_g2_x0_y1_z5_mod = grille_cyl_2.getVertexXYZ( 0 , 1 , 5 )
vx_g2_x0_y2_z5_mod = grille_cyl_2.getVertexXYZ( 0 , 2 , 5 )
vx_g2_x0_y3_z5_mod = grille_cyl_2.getVertexXYZ( 0 , 3 , 5 )
vx_g2_x0_y4_z5_mod = grille_cyl_2.getVertexXYZ( 0 , 4 , 5 )
vx_g2_x0_y5_z5_mod = grille_cyl_2.getVertexXYZ( 0 , 5 , 5 )
vx_g2_x0_y7_z5_mod = doc.findVertex( P + 0.5 , 0 , 5*h/7 )
vx_g2_x0_y8_z5_mod = doc.findVertex( P - 0.5 , 0 , 5*h/7 )

#association
vx_g2_x0_y0_z5_mod.setAssociation( vx_g2_1c_f0_z5 )
vx_g2_x0_y1_z5_mod.setAssociation( vx_g2_1c_f1_z5 )
vx_g2_x0_y2_z5_mod.setAssociation( vx_g2_1c_f2_z5 )
vx_g2_x0_y3_z5_mod.setAssociation( vx_g2_1c_f3_z5 )
vx_g2_x0_y4_z5_mod.setAssociation( vx_g2_1c_f4_z5 )
vx_g2_x0_y5_z5_mod.setAssociation( vx_g2_1c_f5_z5 )
vx_g2_x0_y7_z5_mod.setAssociation( vx_g2_1c_f7_z5 )
vx_g2_x0_y8_z5_mod.setAssociation( vx_g2_1c_f8_z5 )



#geom
vx_g2_1c_f0_z6 = geompy.MakeTranslation( vx_g2_1c_f0_z1 , 0 , 0 , z6  )
vx_g2_1c_f1_z6 = geompy.MakeTranslation( vx_g2_1c_f1_z1 , 0 , 0 , z6  )
vx_g2_1c_f2_z6 = geompy.MakeTranslation( vx_g2_1c_f2_z1 , 0 , 0 , z6  )
vx_g2_1c_f3_z6 = geompy.MakeTranslation( vx_g2_1c_f3_z1 , 0 , 0 , z6  )
vx_g2_1c_f4_z6 = geompy.MakeTranslation( vx_g2_1c_f4_z1 , 0 , 0 , z6  )
vx_g2_1c_f5_z6 = geompy.MakeTranslation( vx_g2_1c_f5_z1 , 0 , 0 , z6  )
vx_g2_1c_f7_z6 = geompy.MakeTranslation( vx_g2_1c_f7_z1 , 0 , 0 , z6  )
vx_g2_1c_f8_z6 = geompy.MakeTranslation( vx_g2_1c_f8_z1 , 0 , 0 , z6  )

#modele
vx_g2_x0_y0_z6_mod = grille_cyl_2.getVertexXYZ( 0 , 0 , 6 )
vx_g2_x0_y1_z6_mod = grille_cyl_2.getVertexXYZ( 0 , 1 , 6 )
vx_g2_x0_y2_z6_mod = grille_cyl_2.getVertexXYZ( 0 , 2 , 6 )
vx_g2_x0_y3_z6_mod = grille_cyl_2.getVertexXYZ( 0 , 3 , 6 )
vx_g2_x0_y4_z6_mod = grille_cyl_2.getVertexXYZ( 0 , 4 , 6 )
vx_g2_x0_y5_z6_mod = grille_cyl_2.getVertexXYZ( 0 , 5 , 6 )
vx_g2_x0_y7_z6_mod = doc.findVertex( P + 0.5 , 0 , 6*h/7 )
vx_g2_x0_y8_z6_mod = doc.findVertex( P - 0.5 , 0 , 6*h/7 )

#association
vx_g2_x0_y0_z6_mod.setAssociation( vx_g2_1c_f0_z6 )
vx_g2_x0_y1_z6_mod.setAssociation( vx_g2_1c_f1_z6 )
vx_g2_x0_y2_z6_mod.setAssociation( vx_g2_1c_f2_z6 )
vx_g2_x0_y3_z6_mod.setAssociation( vx_g2_1c_f3_z6 )
vx_g2_x0_y4_z6_mod.setAssociation( vx_g2_1c_f4_z6 )
vx_g2_x0_y5_z6_mod.setAssociation( vx_g2_1c_f5_z6 )
vx_g2_x0_y7_z6_mod.setAssociation( vx_g2_1c_f7_z6 )
vx_g2_x0_y8_z6_mod.setAssociation( vx_g2_1c_f8_z6 )






#====================================
# CREATION MAILLAGE
#====================================


#====================================
# Definir une loi de discretisation
#====================================


# définir une loi: le choix de la loi reste aux utilisateurs

Law = doc.addLaw( "Uniform" , 4 )

#petit test
n = doc.countLaw()
print n

# chercher les propagations du modele

N_Propa = doc.countPropagation()

for j in range(N_Propa):
	Propa = doc.getPropagation(j)
	Propa.setLaw( Law )# appliquer la loi de discrétisation sur tout le modele et generer le maillage

mesh = hexablock.mesh("BRIDE", doc)

print "Nombre d'hexaédres:"   , mesh.NbHexas()
print "Nombre de quadrangles:", mesh.NbQuadrangles()
print "Nombre de segments:"   , mesh.NbEdges()
print "Nombre de noeuds:"     , mesh.NbNodes()



























