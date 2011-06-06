# -*- coding: latin-1 -*-
#  Copyright (C) 2009-2011  CEA/DEN, EDF R&D
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  See http:#www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

#===============================================
# By Karima DEBCHI YATAGHENE Nov 2009 at CS 
#===============================================
import os
import geompy
import smesh
import hexablock
import math

# chemin du fichier BREP contenant la CAO
BREP_PATH = os.path.expandvars("$HEXABLOCK_ROOT_DIR/bin/salome/tuyau.brep")




#===================================================================
#               LE MODELE:
#   (Pour le tuyau on doit creer une grille cylindrique)
#===================================================================


#=============================
# PARAMETRES
#=============================
R = 4.5
r = 4.5
r_t = 3.6
h = 75.0


#=================================================
# Creation du document
#=================================================
doc = hexablock.addDocument()

#=================================================
# Creation du tuyau (grille cylindrique) dans le document
#=================================================
# centre de la grille cylindrique
c = doc.addVertex(0, 0, 0) 

# vecteurs de la grille cylindrique
dx = doc.addVector(h, 0, 0)
dy = doc.addVector(0, h, 0)
dz = doc.addVector(0, 0, h)

# taille du cylindre
dr = R
da = 360
dl = h

nr = 1
na = 4
nl = 1

model_tuyau_fin = doc.makeCylindrical(c, dx, dz, dr, da, dl, nr, na, nl, False)
# note: on obtient une liste qui contient 4 hexaedres eguaux


#===================================================================
# Recuperation des vertex,edges et quads du model pour l'association
#
# h = haut
# b = bas
# g = grand
# p = petit
# t = trou
#===================================================================

#=========================
#       VERTEX
#=========================

# Face du haut 
#   grand rayon
x_mod_h = doc.findVertex( 2*R , 0 , h )
y_mod_h = doc.findVertex( 0 , 2*R , h )
z_mod_h = doc.findVertex( -2*R , 0 , h )
u_mod_h = doc.findVertex( 0 , -2*R , h )

#   petit rayon
x_mod_h_t = doc.findVertex( R , 0 , h )
y_mod_h_t = doc.findVertex( 0 , R , h )
z_mod_h_t = doc.findVertex( -R , 0 , h )
u_mod_h_t = doc.findVertex( 0 , -R , h )

# Face du bas
#   grand rayon
x_mod_b = doc.findVertex( 2*R , 0 , 0 )
y_mod_b = doc.findVertex( 0 , 2*R , 0 )
z_mod_b = doc.findVertex( -2*R , 0 , 0 )
u_mod_b = doc.findVertex( 0 , -2*R , 0 )

#   petit rayon
x_mod_b_t = doc.findVertex( R , 0 , 0 )
y_mod_b_t = doc.findVertex( 0 , R , 0 )
z_mod_b_t = doc.findVertex( -R , 0 , 0 )
u_mod_b_t = doc.findVertex( 0 , -R , 0 )


# v�rifications
assert x_mod_h
assert y_mod_h
assert z_mod_h
assert u_mod_h

assert x_mod_h_t
assert y_mod_h_t
assert z_mod_h_t
assert u_mod_h_t

assert x_mod_b
assert y_mod_b
assert z_mod_b
assert u_mod_b

assert x_mod_b_t
assert y_mod_b_t
assert z_mod_b_t
assert u_mod_b_t


#=========================
#       EDGES
#=========================

# Face du haut

#   grand rayon
edge_mod_face_h_1_1 = doc.findEdge( y_mod_h , z_mod_h )
edge_mod_face_h_1_2 = doc.findEdge( z_mod_h , u_mod_h )
edge_mod_face_h_2_1 = doc.findEdge( y_mod_h,  x_mod_h )
edge_mod_face_h_2_2 = doc.findEdge( x_mod_h,  u_mod_h )

#   petit rayon
edge_mod_t_face_h_1_1 = doc.findEdge( y_mod_h_t , z_mod_h_t )
edge_mod_t_face_h_1_2 = doc.findEdge( z_mod_h_t , u_mod_h_t )
edge_mod_t_face_h_2_1 = doc.findEdge( y_mod_h_t,  x_mod_h_t )
edge_mod_t_face_h_2_2 = doc.findEdge( x_mod_h_t,  u_mod_h_t )


# Face du bas

#   grand rayon
edge_mod_face_b_1_1 = doc.findEdge( y_mod_b , z_mod_b )
edge_mod_face_b_1_2 = doc.findEdge( z_mod_b , u_mod_b )
edge_mod_face_b_2_1 = doc.findEdge( y_mod_b,  x_mod_b )
edge_mod_face_b_2_2 = doc.findEdge( x_mod_b,  u_mod_b )

#   petit rayon
edge_mod_t_face_b_1_1 = doc.findEdge( y_mod_b_t , z_mod_b_t )
edge_mod_t_face_b_1_2 = doc.findEdge( z_mod_b_t , u_mod_b_t )
edge_mod_t_face_b_2_1 = doc.findEdge( y_mod_b_t,  x_mod_b_t )
edge_mod_t_face_b_2_2 = doc.findEdge( x_mod_b_t,  u_mod_b_t )



# Joignant faces haut et du bas
Edge_Mod_ext_h = doc.findEdge( y_mod_h , y_mod_b )
Edge_Mod_int_h = doc.findEdge( y_mod_h_t , y_mod_b_t )

Edge_Mod_int_b = doc.findEdge( u_mod_h_t , u_mod_b_t )
Edge_Mod_ext_b = doc.findEdge( u_mod_h , u_mod_b )

Edge_Mod_ext_g = doc.findEdge( x_mod_h , x_mod_b )
Edge_Mod_int_g = doc.findEdge( x_mod_h_t , x_mod_b_t )

Edge_Mod_int_d = doc.findEdge( z_mod_h_t , z_mod_b_t )
Edge_Mod_ext_d = doc.findEdge( z_mod_h , z_mod_b )


# v�rifications
assert edge_mod_face_h_1_1
assert edge_mod_face_h_1_2
assert edge_mod_face_h_2_1
assert edge_mod_face_h_2_2

assert edge_mod_t_face_h_1_1
assert edge_mod_t_face_h_1_2
assert edge_mod_t_face_h_2_1
assert edge_mod_t_face_h_2_2

assert edge_mod_face_b_1_1
assert edge_mod_face_b_1_2
assert edge_mod_face_b_2_1
assert edge_mod_face_b_2_2

assert edge_mod_t_face_b_1_1
assert edge_mod_t_face_b_1_2
assert edge_mod_t_face_b_2_1
assert edge_mod_t_face_b_2_2


assert Edge_Mod_ext_h
assert Edge_Mod_int_h

assert Edge_Mod_int_b
assert Edge_Mod_ext_b

assert Edge_Mod_ext_g
assert Edge_Mod_int_g

assert Edge_Mod_int_d
assert Edge_Mod_ext_d



#=========================
#       QUADS
#=========================
Quad_mod_0 = model_tuyau_fin.getQuadJK( 1 , 0 , 0 )
Quad_mod_1 = model_tuyau_fin.getQuadJK( 1 , 1 , 0 )
Quad_mod_2 = model_tuyau_fin.getQuadJK( 1 , 2 , 0 )
Quad_mod_3 = model_tuyau_fin.getQuadJK( 1 , 3 , 0 )



#===================================================================
#               LA GEOMETRIE
#===================================================================

#===================================================================
# Recuperation des points de la geometrie tuyau pour l'association
#
#===================================================================
Tuyau_geom = geompy.Import(BREP_PATH, "BREP")


#=========================
#       SOMMETS
#=========================
Pt_A = geompy.MakeVertex( h , 20 , 4.5 )
Pt_B = geompy.MakeVertex( h , 20 , 3.6 )
Pt_C = geompy.MakeVertex( h , 20 , -4.5 )
Pt_D = geompy.MakeVertex( h , 20 , -3.6 )
Pt_E = geompy.MakeVertex( 0 , 0 , 2 )
Pt_F = geompy.MakeVertex( 0 , 0 , 1.6 )
Pt_G = geompy.MakeVertex( 0 , 0 , -2 )
Pt_H = geompy.MakeVertex( 0 , 0 , -1.6 )

# Face du bas
Edge_bas_droite_grd = geompy.GetEdgeNearPoint(Tuyau_geom, Pt_E)
Edge_bas_droite_pte = geompy.GetEdgeNearPoint(Tuyau_geom, Pt_F)
Edge_bas_gauche_grd = geompy.GetEdgeNearPoint(Tuyau_geom, Pt_G)
Edge_bas_gauche_pte = geompy.GetEdgeNearPoint(Tuyau_geom, Pt_H)
## Grand rayon
x_b = geompy.MakeVertexOnCurve(Edge_bas_droite_grd, 0.5)
y_b = geompy.MakeVertexOnCurve(Edge_bas_droite_grd, 1)
z_b = geompy.MakeVertexOnCurve(Edge_bas_gauche_grd, 0.5)
u_b = geompy.MakeVertexOnCurve(Edge_bas_gauche_grd, 0)

## Petit rayon
x_b_t = geompy.MakeVertexOnCurve(Edge_bas_droite_pte, 0.5)
y_b_t = geompy.MakeVertexOnCurve(Edge_bas_droite_pte, 1)
z_b_t = geompy.MakeVertexOnCurve(Edge_bas_gauche_pte, 0.5)
u_b_t = geompy.MakeVertexOnCurve(Edge_bas_gauche_pte, 0)


# Face du haut
Edge_haut_droite_grd = geompy.GetEdgeNearPoint(Tuyau_geom, Pt_A)
Edge_haut_droite_pte = geompy.GetEdgeNearPoint(Tuyau_geom, Pt_B)
Edge_haut_gauche_grd = geompy.GetEdgeNearPoint(Tuyau_geom, Pt_C)
Edge_haut_gauche_pte = geompy.GetEdgeNearPoint(Tuyau_geom, Pt_D)
## Grand rayon
x_h = geompy.MakeVertexOnCurve(Edge_haut_droite_grd, 0.5)
y_h = geompy.MakeVertexOnCurve(Edge_haut_droite_grd, 1)
z_h = geompy.MakeVertexOnCurve(Edge_haut_gauche_grd, 0.5)
u_h = geompy.MakeVertexOnCurve(Edge_haut_gauche_grd, 0)

## Petit rayon
x_h_t = geompy.MakeVertexOnCurve(Edge_haut_droite_pte, 0.5)
y_h_t = geompy.MakeVertexOnCurve(Edge_haut_droite_pte, 1)
z_h_t = geompy.MakeVertexOnCurve(Edge_haut_gauche_pte, 0.5)
u_h_t = geompy.MakeVertexOnCurve(Edge_haut_gauche_pte, 0)



#=========================
#       EDGES
#=========================
All_Edges_Tuy_1 = geompy.SubShapeAllSorted( Tuyau_geom , geompy.ShapeType["EDGE"] )

# Face du haut
## Grand rayon
edge_face_h_1   = All_Edges_Tuy_1[11]
edge_face_h_2   = All_Edges_Tuy_1[14]
## Petit rayon
edge_t_face_h_1 = All_Edges_Tuy_1[12]
edge_t_face_h_2 = All_Edges_Tuy_1[13]


# Face du bas
## Grand rayon
edge_face_b_1   = All_Edges_Tuy_1[1]
edge_face_b_2   = All_Edges_Tuy_1[4]
##  Petit rayon
edge_t_face_b_1 = All_Edges_Tuy_1[2]
edge_t_face_b_2 = All_Edges_Tuy_1[3]


# Joignant les 2 faces

## edges compl�tes
Edge_Tuy_ext_h = All_Edges_Tuy_1[6]
Edge_Tuy_int_h = All_Edges_Tuy_1[7]
Edge_Tuy_int_b = All_Edges_Tuy_1[8]
Edge_Tuy_ext_b = All_Edges_Tuy_1[9]

Pt_Z = geompy.MakeVertex( 75 , 20 , 0 )
Pt_Z_1 = geompy.MakeVertexWithRef( Pt_Z , 0 , 0 , 10 )
Pt_Z_2 = geompy.MakeVertexWithRef( Pt_Z , 0 , 0 , -10 )
Line_Z1_Z2 = geompy.MakeLineTwoPnt( Pt_Z_1 , Pt_Z_2 )


All_Edge_Tuy_ext_h = geompy.SubShapeAllSorted( Edge_Tuy_ext_h , geompy.ShapeType["VERTEX"] )

Vx_Edg_Tuy_ext_h_1 = All_Edge_Tuy_ext_h[0]
Vx_Edg_Tuy_ext_h_2 = All_Edge_Tuy_ext_h[1]

Vx_Edg_Tuy_ext_h_1_trans = geompy.MakeVertexWithRef( Vx_Edg_Tuy_ext_h_1 , -10 , -10 , 0 )
Vx_Edg_Tuy_ext_h_2_trans = geompy.MakeVertexWithRef( Vx_Edg_Tuy_ext_h_2 , 10 , 0 , 0 )

Line_1 = geompy.MakeLineTwoPnt(Vx_Edg_Tuy_ext_h_1, Vx_Edg_Tuy_ext_h_1_trans)
Line_2 = geompy.MakeLineTwoPnt(Vx_Edg_Tuy_ext_h_2, Vx_Edg_Tuy_ext_h_2_trans)

Wire = geompy.MakeWire([Line_1, Line_2, Edge_Tuy_ext_h])
Pipe_Z = geompy.MakePipe( Line_Z1_Z2 , Wire )
Pipe_Ztt = geompy.MakePipe( Line_Z1_Z2 , Edge_Tuy_ext_h )
Partition_Ztt = geompy.MakePartition([Tuyau_geom], [Pipe_Ztt], [], [], geompy.ShapeType["SOLID"], 0, [], 0)
All_Edges_Tuy_2tt = geompy.SubShapeAllSorted( Partition_Ztt , geompy.ShapeType["EDGE"] )

Edge_Tuy_ext_g = All_Edges_Tuy_2tt[12]
Edge_Tuy_int_g = All_Edges_Tuy_2tt[13]
Edge_Tuy_ext_d = All_Edges_Tuy_2tt[10]
Edge_Tuy_int_d = All_Edges_Tuy_2tt[11]

## une edge en plusieurs(4) morceaux : Edge_Tuy_ext_h
X_pln1 = geompy.MakeVertexWithRef( Pt_Z , -20 , 0 , 0 )
Vec_X = geompy.MakeVectorDXDYDZ( 1 , 0 , 0 )

Plan1 = geompy.MakePlane( X_pln1 , Vec_X , 200 )
Plan2 = geompy.MakeTranslation( Plan1 , -20 , 0 , 0 )
Plan3 = geompy.MakeTranslation( Plan1 , -40 , 0 , 0 )

Partition_Edge_g_d_h_b = geompy.MakePartition([
    Edge_Tuy_ext_h,
    Edge_Tuy_int_h,
    Edge_Tuy_ext_b,
    Edge_Tuy_int_b,
    Edge_Tuy_ext_g,
    Edge_Tuy_int_g,
    Edge_Tuy_ext_d,
    Edge_Tuy_int_d], 
    [Plan1, Plan2, Plan3], [], [], geompy.ShapeType["EDGE"], 0, [], 0)

All_Edge_g_d_h_b = geompy.SubShapeAllSorted( Partition_Edge_g_d_h_b , geompy.ShapeType["EDGE"] )
Edge_Tuy_ext_h_1 = All_Edge_g_d_h_b[0]
Edge_Tuy_ext_h_2 = All_Edge_g_d_h_b[15]
Edge_Tuy_ext_h_3 = All_Edge_g_d_h_b[23]
Edge_Tuy_ext_h_4 = All_Edge_g_d_h_b[31]



#=========================
#       FACES
#=========================
All_Faces_Tuy_1 = geompy.SubShapeAllSorted( Tuyau_geom , geompy.ShapeType["FACE"] )

Face_Tuy_ext_1 = All_Faces_Tuy_1[4]
Face_Tuy_ext_2 = All_Faces_Tuy_1[5]

Partition_Face = geompy.MakePartition([ Face_Tuy_ext_1 , Face_Tuy_ext_2 ], [ Plan1 , Plan2 , Plan3 , Pipe_Z ], [], [], geompy.ShapeType["FACE"], 0, [], 0)


All_Faces_Partition_Face = geompy.SubShapeAllSorted( Partition_Face , geompy.ShapeType["FACE"] )

## -------------------------------------
Face_0 = All_Faces_Partition_Face[0]
Face_6 = All_Faces_Partition_Face[6]
Face_10 = All_Faces_Partition_Face[10]
Face_14 = All_Faces_Partition_Face[14]

## -------------------------------------
Face_1 = All_Faces_Partition_Face[1]
Face_7 = All_Faces_Partition_Face[7]
Face_11 = All_Faces_Partition_Face[11]
Face_15 = All_Faces_Partition_Face[15]

## -------------------------------------
Face_2 = All_Faces_Partition_Face[2]
Face_4 = All_Faces_Partition_Face[4]
Face_8 = All_Faces_Partition_Face[8]
Face_12 = All_Faces_Partition_Face[12]

## -------------------------------------
Face_3 = All_Faces_Partition_Face[3]
Face_5 = All_Faces_Partition_Face[5]
Face_9 = All_Faces_Partition_Face[9]
Face_13 = All_Faces_Partition_Face[13]





#==========================================================
#               LES ASSOCIATIONS
#==========================================================

##===========================================
##          VERTEX
##===========================================

# Face du haut

## Grand rayon
x_mod_h.setAssociation( x_h )
y_mod_h.setAssociation( y_h )
z_mod_h.setAssociation( z_h )
u_mod_h.setAssociation( u_h )

## Petit rayon
x_mod_h_t.setAssociation( x_h_t )
y_mod_h_t.setAssociation( y_h_t )
z_mod_h_t.setAssociation( z_h_t )
u_mod_h_t.setAssociation( u_h_t )


# Face du bas

## Grand rayon
x_mod_b.setAssociation( x_b )
y_mod_b.setAssociation( y_b )
z_mod_b.setAssociation( z_b )
u_mod_b.setAssociation( u_b )

## Petit rayon
x_mod_b_t.setAssociation( x_b_t )
y_mod_b_t.setAssociation( y_b_t )
z_mod_b_t.setAssociation( z_b_t )
u_mod_b_t.setAssociation( u_b_t )


##===========================================
##          EDGES
##===========================================

# Face du haut

## Grand rayon
edge_mod_face_h_1_1.addAssociation( edge_face_h_1, 0.5, 1. )
edge_mod_face_h_1_2.addAssociation( edge_face_h_1, 0., 0.5 )
edge_mod_face_h_2_1.addAssociation( edge_face_h_2, 0.5, 1. )
edge_mod_face_h_2_2.addAssociation( edge_face_h_2, 0., 0.5 )

## Petit rayon
edge_mod_t_face_h_1_1.addAssociation( edge_t_face_h_1, 0.5, 1. )
edge_mod_t_face_h_1_2.addAssociation( edge_t_face_h_1, 0., 0.5 )
edge_mod_t_face_h_2_1.addAssociation( edge_t_face_h_2, 0.5, 1. )
edge_mod_t_face_h_2_2.addAssociation( edge_t_face_h_2, 0., 0.5 )

# Face du bas

## Grand rayon
edge_mod_face_b_1_1.addAssociation( edge_face_b_1, 0.5, 1. )
edge_mod_face_b_1_2.addAssociation( edge_face_b_1, 0., 0.5 )
edge_mod_face_b_2_1.addAssociation( edge_face_b_2, 0.5, 1. )
edge_mod_face_b_2_2.addAssociation( edge_face_b_2, 0., 0.5 )

## Petit rayon
edge_mod_t_face_b_1_1.addAssociation( edge_t_face_b_1, 0.5, 1. )
edge_mod_t_face_b_1_2.addAssociation( edge_t_face_b_1, 0., 0.5 )
edge_mod_t_face_b_2_1.addAssociation( edge_t_face_b_2, 0.5, 1. )
edge_mod_t_face_b_2_2.addAssociation( edge_t_face_b_2, 0., 0.5 )



# Joignant les 2 faces
#Edge_Mod_ext_h.addAssociation( Edge_Tuy_ext_h, 0., 1. )
Edge_Mod_ext_h.addAssociation( Edge_Tuy_ext_h_1, 0., 1. )
Edge_Mod_ext_h.addAssociation( Edge_Tuy_ext_h_2, 0., 1. )
Edge_Mod_ext_h.addAssociation( Edge_Tuy_ext_h_3, 0., 1. )
Edge_Mod_ext_h.addAssociation( Edge_Tuy_ext_h_4, 0., 1. )
Edge_Mod_int_h.addAssociation( Edge_Tuy_int_h, 0., 1. )

Edge_Mod_int_b.addAssociation( Edge_Tuy_int_b, 0., 1. )
Edge_Mod_ext_b.addAssociation( Edge_Tuy_ext_b, 0., 1. )

Edge_Mod_ext_g.addAssociation( Edge_Tuy_ext_g, 0., 1. )
Edge_Mod_int_g.addAssociation( Edge_Tuy_int_g, 0., 1. )

Edge_Mod_int_d.addAssociation( Edge_Tuy_int_d, 0., 1. )
Edge_Mod_ext_d.addAssociation( Edge_Tuy_ext_d, 0., 1. )


#Quad_mod_1.addAssociation( Face_Tuy_ext_1 )
Quad_mod_1.addAssociation( Face_0 )
Quad_mod_1.addAssociation( Face_6 )
Quad_mod_1.addAssociation( Face_10 )
Quad_mod_1.addAssociation( Face_14 )

#Quad_mod_0.addAssociation( Face_Tuy_ext_2 )
Quad_mod_0.addAssociation( Face_1 )
Quad_mod_0.addAssociation( Face_7 )
Quad_mod_0.addAssociation( Face_11 )
Quad_mod_0.addAssociation( Face_15 )

#Quad_mod_2.addAssociation( Face_Tuy_ext_1 )
Quad_mod_2.addAssociation( Face_2 )
Quad_mod_2.addAssociation( Face_4 )
Quad_mod_2.addAssociation( Face_8 )
Quad_mod_2.addAssociation( Face_12 )

#Quad_mod_3.addAssociation( Face_Tuy_ext_2 )
Quad_mod_3.addAssociation( Face_3 )
Quad_mod_3.addAssociation( Face_5 )
Quad_mod_3.addAssociation( Face_9 )
Quad_mod_3.addAssociation( Face_13 )



#==========================================================
#       LA GEOMETRIE DANS L'ARBRE D'ETUDE SALOME
#==========================================================
#for i,e in enumerate(All_Edges_Tuy_1): geompy.addToStudy( e, "edge_"+str(i) )
geompy.addToStudy(Tuyau_geom, "Tuyau_geom")
geompy.addToStudy( x_h, "x_h" )
geompy.addToStudy( y_h, "y_h" )
geompy.addToStudy( z_h, "z_h" )
geompy.addToStudy( u_h, "u_h" )
geompy.addToStudy( x_h_t, "x_h_t")
geompy.addToStudy( y_h_t, "y_h_t" )
geompy.addToStudy( z_h_t, "z_h_t" )
geompy.addToStudy( u_h_t, "u_h_t" )

geompy.addToStudy( x_b, "x_b")
geompy.addToStudy( y_b, "y_b" )
geompy.addToStudy( z_b, "z_b" )
geompy.addToStudy( u_b, "u_b")
geompy.addToStudy( x_b_t, "x_b_t" )
geompy.addToStudy( y_b_t, "y_b_t" )
geompy.addToStudy( z_b_t, "z_b_t" )
geompy.addToStudy( u_b_t, "u_b_t" )


#geompy.addToStudy(Line_Z1_Z2, "Line_Z1_Z2")
#geompy.addToStudy(Pipe_Ztt, "Pipe_Ztt")
#geompy.addToStudy( Partition_Ztt , "Partition_Ztt" )
geompy.addToStudy( Edge_Tuy_ext_h , "Edge_Tuy_ext_h" )
geompy.addToStudy( Edge_Tuy_int_h , "Edge_Tuy_int_h" )
geompy.addToStudy( Edge_Tuy_int_b , "Edge_Tuy_int_b" )
geompy.addToStudy( Edge_Tuy_ext_b , "Edge_Tuy_ext_b" )
geompy.addToStudy( Edge_Tuy_ext_g , "Edge_Tuy_ext_g" )
geompy.addToStudy( Edge_Tuy_int_g , "Edge_Tuy_int_g" )
geompy.addToStudy( Edge_Tuy_ext_d , "Edge_Tuy_ext_d" )
geompy.addToStudy( Edge_Tuy_int_d , "Edge_Tuy_int_d" )
#geompy.addToStudy( Partition_Edge_g_d_h_b , "Partition_Edge_g_d_h_b" )
geompy.addToStudy( Face_Tuy_ext_1 , "Face_Tuy_ext_1" )
geompy.addToStudy( Face_Tuy_ext_2 , "Face_Tuy_ext_2" )
#geompy.addToStudy( Partition_Face , "Partition_Face" )
geompy.addToStudy( Face_0 , "Face_0" )
geompy.addToStudy( Face_1 , "Face_1" )
geompy.addToStudy( Face_2 , "Face_2" )
geompy.addToStudy( Face_3 , "Face_3" )
geompy.addToStudy( Face_4 , "Face_4" )
geompy.addToStudy( Face_5 , "Face_5" )
geompy.addToStudy( Face_6 , "Face_6" )
geompy.addToStudy( Face_7 , "Face_7" )
geompy.addToStudy( Face_8 , "Face_8" )
geompy.addToStudy( Face_9 , "Face_9" )
geompy.addToStudy( Face_10 , "Face_10" )
geompy.addToStudy( Face_11 , "Face_11" )
geompy.addToStudy( Face_12 , "Face_12" )
geompy.addToStudy( Face_13 , "Face_13" )
geompy.addToStudy( Face_14 , "Face_14" )
geompy.addToStudy( Face_15 , "Face_15" )





#====================================
# CREATION MAILLAGE
#====================================


##=================================================
## Definir les groupes d'elements pour le maillage
##=================================================

# groupe de Hexa(volumes) 
Hexa_grp = doc.addHexaGroup("Hexa_all_grp")
Nbr_Hex = doc.countHexa()
print "doc.countHexa()->",Nbr_Hex

for i in range(Nbr_Hex):
    Hexa_i = doc.getHexa(i)
    Hexa_grp.addElement(Hexa_i)

#Nbr_elm_hex = Hexa_grp.countElement()
#print "Hexa_all_grp  countElement() ->",Nbr_elm_hex

for i in range(Nbr_Hex):
    Hexa_grp = doc.addHexaGroup("Hexa_grp"+str(i))
    Hexa_i = doc.getHexa(i)
    Hexa_grp.addElement(Hexa_i)

# groupe de quads (faces)
Quad_grp_haut = doc.addQuadGroup("Quad_grp_haut")
Quad_h_1 = doc.findQuad( x_mod_h , y_mod_h_t )
Quad_h_2 = doc.findQuad( y_mod_h_t , z_mod_h )
Quad_h_3 = doc.findQuad( z_mod_h , u_mod_h_t )
Quad_h_4 = doc.findQuad( u_mod_h_t , x_mod_h )

assert Quad_h_1
assert Quad_h_2
assert Quad_h_3
assert Quad_h_4

for Quad_h_i in [ Quad_h_1 , Quad_h_2 , Quad_h_3 , Quad_h_4 ]:
    Quad_grp_haut.addElement(Quad_h_i)


Quad_grp_bas = doc.addQuadGroup("Quad_grp_bas")
Quad_b_1 = doc.findQuad( x_mod_b , y_mod_b_t )
Quad_b_2 = doc.findQuad( y_mod_b_t , z_mod_b )
Quad_b_3 = doc.findQuad( z_mod_b , u_mod_b_t )
Quad_b_4 = doc.findQuad( u_mod_b_t , x_mod_b )


assert Quad_b_1
assert Quad_b_2
assert Quad_b_3
assert Quad_b_4

for Quad_b_i in [ Quad_b_1 , Quad_b_2 , Quad_b_3 , Quad_b_4 ]:
    Quad_grp_bas.addElement(Quad_b_i)

#Nbr_elm_qad_bas = Quad_grp_bas.countElement()
#print "Nbr_elm_qad_bas->",Nbr_elm_qad_bas



# groupe de Edges
Edge_Grp_haut = doc.addEdgeGroup("Edge_Grp_haut")
Edge_Grp_haut.addElement(Edge_Mod_ext_h)
Edge_Grp_haut.addElement(Edge_Mod_int_h)

Edge_Grp_gauche = doc.addEdgeGroup("Edge_Grp_gauche")
Edge_Grp_gauche.addElement(Edge_Mod_ext_g)
Edge_Grp_gauche.addElement(Edge_Mod_int_g)

Edge_Grp_bas = doc.addEdgeGroup("Edge_Grp_bas")
Edge_Grp_bas.addElement(Edge_Mod_int_b)
Edge_Grp_bas.addElement(Edge_Mod_ext_b)

Edge_Grp_droit = doc.addEdgeGroup("Edge_Grp_droit")
Edge_Grp_droit.addElement(Edge_Mod_int_d)
Edge_Grp_droit.addElement(Edge_Mod_ext_d)


# groupe de noeuds de Hexa 
Hexa_Nod_Grp = doc.addHexaNodeGroup("Hexa_Nod_Grp")
for i in range(Nbr_Hex):
    Hexa_i = doc.getHexa(i)
    Hexa_Nod_Grp.addElement(Hexa_i)

# groupe de noeuds de Quad 
Quad_Nod_Grp_h_1 = doc.addQuadNodeGroup("Quad_Nod_Grp_h_1")
Quad_Nod_Grp_h_1.addElement(Quad_h_1)

Quad_Nod_Grp_b_1 = doc.addQuadNodeGroup("Quad_Nod_Grp_b_1")
Quad_Nod_Grp_b_1.addElement(Quad_b_1)

# groupe de noeuds de Edge 
Edge_Nod_Grp_b = doc.addEdgeNodeGroup("Edge_Nod_Grp_b")
Edge_Nod_Grp_b.addElement(Edge_Mod_int_b)
Edge_Nod_Grp_b.addElement(Edge_Mod_ext_b)

Edge_Nod_Grp_h = doc.addEdgeNodeGroup("Edge_Nod_Grp_h")
Edge_Nod_Grp_h.addElement(Edge_Mod_int_d)
Edge_Nod_Grp_h.addElement(Edge_Mod_ext_d)


# groupe de noeuds de vertex pour tout le modele
Vertex_Nod_Grp = doc.addVertexNodeGroup("Vertex_Nod_Grp")
Nbr_Vx = doc.countVertex()
for i in range(Nbr_Vx):
    Vertex_i = doc.getVertex(i)
    Vertex_Nod_Grp.addElement(Vertex_i)


#====================================
# Definir une loi de discretisation
#====================================
bp_law = doc.getLaw(0)
bp_law.setNodes(4)

# definir une loi: le choix de la loi reste aux utilisateurs

#Law_X = doc.addLaw( "Uniform" , 4 )
#Law_Y = doc.addLaw( "Arithmetic" , 2 )
#Law_Z = doc.addLaw( "Geometric" , 3 )
Law_X = doc.addLaw( "Arithmetic" , 5 )
Law_X.setKind(hexablock.ARITHMETIC)
Law_X.setCoefficient(0.1)

Law_Y = doc.addLaw( "Arithmetic" , 3 )
Law_Y.setKind(hexablock.ARITHMETIC)
Law_Y.setCoefficient(0.1)

#Law_Z = doc.addLaw( "Geometric" , 4 )
Law_Z = doc.addLaw( "Arithmetic" , 5 )
Law_Z.setKind(hexablock.ARITHMETIC)
Law_Z.setCoefficient(0.05)


# chercher les propagations du modele
Edge_Law_X = doc.findEdge( x_mod_h , x_mod_h_t )
Edge_Law_Y = doc.findEdge( y_mod_h , y_mod_h_t )
Edge_Law_Z = doc.findEdge( y_mod_h , y_mod_b )

Propa_X = doc.findPropagation( Edge_Law_X )
Propa_Y = doc.findPropagation( Edge_Law_Y )
Propa_Z = doc.findPropagation( Edge_Law_Z )

# appliquer la loi de discretisation sur tout le modele et generer le maillage
Propa_X.setLaw( Law_X )
Propa_Y.setLaw( Law_Y )
Propa_Z.setLaw( Law_Z )

print  " --- MAILLAGE HEXAHEDRIQUE --- "
mesh = hexablock.mesh("TUYAU_COURBE", doc)

print "Nombre d hexaedres:"   , mesh.NbHexas()
print "Nombre de quadrangles:", mesh.NbQuadrangles()
print "Nombre de segments:"   , mesh.NbEdges()
print "Nombre de noeuds:"     , mesh.NbNodes()

salome.sg.updateObjBrowser(1)

#Nombre d hexaedres: 480
#Nombre de quadrangles: 496
#Nombre de segments: 160
#Nombre de noeuds: 700

allGroups = mesh.GetGroups()
print  " --- GROUPES --- "
for aGroup in allGroups:
    print "\nNOM:", aGroup.GetName()
    print "IDS:", aGroup.GetIDs()



## --------------------
## Maillage hexa�drique
## --------------------
#mesh = mesh.ExportMED( "tuyau.med", 1 )
#maillages = smesh.CreateMeshesFromMED( "tuyau.med" )

#ijk = maillages[0][0]

#ijk.Hexahedron()

#ijk.Compute()

#print  " --- MAILLAGE HEXAHEDRIQUE --- "
#print "Nombre d hexaedres:", ijk.NbHexas()
#print "Nombre de quadrangles:", ijk.NbQuadrangles()
#print "Nombre de segments:", ijk.NbEdges()
#print "Nombre de noeuds:", ijk.NbNodes()
