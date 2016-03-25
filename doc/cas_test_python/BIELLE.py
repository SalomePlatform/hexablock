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

#===============================================
# By Karima DEBCHI YATAGHENE Nov 2009 at CS 
#===============================================


import os
import GEOM
import geompy
import smesh
import hexablock
import math
import SALOMEDS


STEP_PATH = os.path.expandvars("$HEXABLOCK_ROOT_DIR/bin/salome/crank.stp")


#=============================
# CREATION DOCUMENT
#=============================

doc = hexablock.addDocument("Bielle")

#=============================
# CREATION DU MODELE
#=============================

# Pour la bielle on doit creer deux grilles cylindriques et prismer les quadrangles entre ces deux dernieres

#=============================
# PARAMETRES
#=============================

R = 40.0

r_grd = R
r_grd_t = R/2

r_pte = R
r_pte_t = R/2

l = 200.0
h = 0.019999999553*2
#h = 40.

# Taille du grand cylindre
dr_grd = R
da_grd = 360
dl_grd = h

nr_grd = 1
na_grd = 6
nl_grd = 1

# Taille du petit cylindre
dr_pte = R
da_pte = 360
dl_pte = h

nr_pte = 1
na_pte = 6
nl_pte = 1


#=============================
# Creation des vecteurs
#=============================

dx = doc.addVector(l, 0, 0)
dy = doc.addVector(0, l, 0)
dz = doc.addVector(0, 0, l)

#=================================================
# Creation des centres des grilles cylindriques
#=================================================

c_grd = doc.addVertex(0, 0, 0)
c_pte = doc.addVertex(2*l, 0, 0)
dx_prime = doc.addVectorVertices( c_grd, c_pte )

#=================================================
# Creation de la grande grille cylindrique
#=================================================

grille_cyl_grd = doc.makeCylindrical(c_grd, dx, dz, dr_grd, da_grd, dl_grd, nr_grd, na_grd, nl_grd, False)

# on obtient une liste qui contient 6 hexaedres eguaux

# 30 = pi/6
# 60 = pi/3
# 45 = pi/4
# 90 = pi/2
# 180 = pi

alpha_x = 2*R*math.cos(math.pi/3)
alpha_y = 2*R*math.sin(math.pi/3)

x1 = doc.findVertex( 2*R, 0, h )
x2 = doc.findVertex( alpha_x, alpha_y, 0 )
x3 = doc.findVertex( alpha_x, -alpha_y, 0 )
x4 = doc.findVertex( 2*R, 0, 0 )

quad_11 = doc.findQuad(x1, x2)
quad_12 = doc.findQuad(x1, x3)

quad_list = [ quad_11 , quad_12 ]

#=================================================
# Creation de la petite grille cylindrique
#=================================================

grille_cyl_pte = doc.makeTranslation( grille_cyl_grd , dx_prime )

# on obtient une liste qui contient 6 hexaedres eguaux

y1 = doc.findVertex( 2*l - 2*R , 0 , h )
y2 = doc.findVertex( 2*l - alpha_x , alpha_y , 0 )
y3 = doc.findVertex( 2*l - alpha_x , -alpha_y , 0 )
y4 = doc.findVertex( 2*l - 2*R , 0 , 0 )

quad_21 = doc.findQuad(y1, y2)
quad_22 = doc.findQuad(y1, y3)

#=================================================
# Assemblage des deux grilles cylindriques
#=================================================
model_biell_fin = doc.joinQuads( quad_list , quad_21 , x1 , y1 , x4 , y4 , 1 )


#===================================================================
# Recuperation des vertex du model hexa bielle pour l association
#===================================================================

# NB:
# h = haut
# b = bas
# g = grand
# p = petit
# t = trou

alpha_x = 2*R*math.cos(math.pi/3)
alpha_y = 2*R*math.sin(math.pi/3)

# Face du haut grand trou

## RQ:On remplace la fonction findVertex par getVertexIJK

v_mod_h_g = grille_cyl_grd.getVertexIJK( 1 , 0 , 1 )
w_mod_h_g = grille_cyl_grd.getVertexIJK( 1 , 1 , 1 )
z_mod_h_g = grille_cyl_grd.getVertexIJK( 1 , 2 , 1 )

y_mod_h_g = grille_cyl_grd.getVertexIJK( 1 , 3 , 1 )
x_mod_h_g = grille_cyl_grd.getVertexIJK( 1 , 4 , 1 )
u_mod_h_g = grille_cyl_grd.getVertexIJK( 1 , 5 , 1 )

v_mod_h_g_t = grille_cyl_grd.getVertexIJK( 0 , 0 , 1 )
w_mod_h_g_t = grille_cyl_grd.getVertexIJK( 0 , 1 , 1 )
z_mod_h_g_t = grille_cyl_grd.getVertexIJK( 0 , 2 , 1 )

y_mod_h_g_t = grille_cyl_grd.getVertexIJK( 0 , 3 , 1 )
x_mod_h_g_t = grille_cyl_grd.getVertexIJK( 0 , 4 , 1 )
u_mod_h_g_t = grille_cyl_grd.getVertexIJK( 0 , 5 , 1 )


assert x_mod_h_g
assert y_mod_h_g
assert z_mod_h_g

assert u_mod_h_g
assert v_mod_h_g
assert w_mod_h_g

assert x_mod_h_g_t
assert y_mod_h_g_t
assert z_mod_h_g_t

assert u_mod_h_g_t
assert v_mod_h_g_t
assert w_mod_h_g_t #


# Face du haut petit trou

## RQ:On remplace la fonction findVertex par getVertexIJK
v_mod_h_p = grille_cyl_pte.getVertexIJK( 1 , 0 , 1 )
w_mod_h_p = grille_cyl_pte.getVertexIJK( 1 , 1 , 1 )
z_mod_h_p = grille_cyl_pte.getVertexIJK( 1 , 2 , 1 )

y_mod_h_p = grille_cyl_pte.getVertexIJK( 1 , 3 , 1 )
x_mod_h_p = grille_cyl_pte.getVertexIJK( 1 , 4 , 1 )
u_mod_h_p = grille_cyl_pte.getVertexIJK( 1 , 5 , 1 )

v_mod_h_p_t = grille_cyl_pte.getVertexIJK( 0 , 0 , 1 )
w_mod_h_p_t = grille_cyl_pte.getVertexIJK( 0 , 1 , 1 )
z_mod_h_p_t = grille_cyl_pte.getVertexIJK( 0 , 2 , 1 )

y_mod_h_p_t = grille_cyl_pte.getVertexIJK( 0 , 3 , 1 )
x_mod_h_p_t = grille_cyl_pte.getVertexIJK( 0 , 4 , 1 )
u_mod_h_p_t = grille_cyl_pte.getVertexIJK( 0 , 5 , 1 )


assert x_mod_h_p
assert y_mod_h_p
assert z_mod_h_p

assert u_mod_h_p
assert v_mod_h_p
assert w_mod_h_p

assert x_mod_h_p_t
assert y_mod_h_p_t
assert z_mod_h_p_t

assert u_mod_h_p_t
assert v_mod_h_p_t
assert w_mod_h_p_t
#-----------

# Face du bas grand trou

## RQ:On remplace la fonction findVertex par getVertexIJK

v_mod_b_g = grille_cyl_grd.getVertexIJK( 1 , 0 , 0 )
w_mod_b_g = grille_cyl_grd.getVertexIJK( 1 , 1 , 0 )
z_mod_b_g = grille_cyl_grd.getVertexIJK( 1 , 2 , 0 )

y_mod_b_g = grille_cyl_grd.getVertexIJK( 1 , 3 , 0 )
x_mod_b_g = grille_cyl_grd.getVertexIJK( 1 , 4 , 0 )
u_mod_b_g = grille_cyl_grd.getVertexIJK( 1 , 5 , 0 )

v_mod_b_g_t = grille_cyl_grd.getVertexIJK( 0 , 0 , 0 )
w_mod_b_g_t = grille_cyl_grd.getVertexIJK( 0 , 1 , 0 )
z_mod_b_g_t = grille_cyl_grd.getVertexIJK( 0 , 2 , 0 )

y_mod_b_g_t = grille_cyl_grd.getVertexIJK( 0 , 3 , 0 )
x_mod_b_g_t = grille_cyl_grd.getVertexIJK( 0 , 4 , 0 )
u_mod_b_g_t = grille_cyl_grd.getVertexIJK( 0 , 5 , 0 )

assert x_mod_b_g
assert y_mod_b_g
assert z_mod_b_g

assert u_mod_b_g
assert v_mod_b_g
assert w_mod_b_g

assert x_mod_b_g_t
assert y_mod_b_g_t
assert z_mod_b_g_t

assert u_mod_b_g_t
assert v_mod_b_g_t
assert w_mod_b_g_t


# Face du bas petit trou


## RQ:On remplace la fonction findVertex par getVertexIJK

v_mod_b_p = grille_cyl_pte.getVertexIJK( 1 , 0 , 0 )
w_mod_b_p = grille_cyl_pte.getVertexIJK( 1 , 1 , 0 )
z_mod_b_p = grille_cyl_pte.getVertexIJK( 1 , 2 , 0 )

y_mod_b_p = grille_cyl_pte.getVertexIJK( 1 , 3 , 0 )
x_mod_b_p = grille_cyl_pte.getVertexIJK( 1 , 4 , 0 )
u_mod_b_p = grille_cyl_pte.getVertexIJK( 1 , 5 , 0 )

v_mod_b_p_t = grille_cyl_pte.getVertexIJK( 0 , 0 , 0 )
w_mod_b_p_t = grille_cyl_pte.getVertexIJK( 0 , 1 , 0 )
z_mod_b_p_t = grille_cyl_pte.getVertexIJK( 0 , 2 , 0 )

y_mod_b_p_t = grille_cyl_pte.getVertexIJK( 0 , 3 , 0 )
x_mod_b_p_t = grille_cyl_pte.getVertexIJK( 0 , 4 , 0 )
u_mod_b_p_t = grille_cyl_pte.getVertexIJK( 0 , 5 , 0 )

assert x_mod_b_p
assert y_mod_b_p
assert z_mod_b_p

assert u_mod_b_p
assert v_mod_b_p
assert w_mod_b_p

assert x_mod_b_p_t
assert y_mod_b_p_t
assert z_mod_b_p_t

assert u_mod_b_p_t
assert v_mod_b_p_t
assert w_mod_b_p_t
#-----------



#===================================================================
# Recuperation des points de la geometrie bielle pour l association
#===================================================================
# NB:
# h = haut
# b = bas
# g = grand
# p = petit
# t = trou

Bielle_geom = geompy.Import(STEP_PATH, "STEP")

geompy.addToStudy(Bielle_geom, "Bielle_geom")

Pt_A = geompy.MakeVertex(0, 0, h/2.)
Pt_B = geompy.MakeVertex(0, 0, -h/2.)

Face_haut = geompy.GetFaceNearPoint(Bielle_geom, Pt_A)
Face_bas = geompy.GetFaceNearPoint(Bielle_geom, Pt_B)


# Paramètre pour:
# -récupération des points.
# -et associations.
u_1 = 0.75#0.8
u_2 = 0.5
u_3 = 0.25#0.2



y_h_g = geompy.MakeVertexOnSurface(Face_haut, 1, 0.5)
y_b_g = geompy.MakeVertexWithRef(y_h_g, 0, 0, -h)
v_h_p = geompy.MakeVertexOnSurface(Face_haut, 0, 0.5)
v_b_p = geompy.MakeVertexWithRef(v_h_p, 0, 0, -h)


Edge_haut_grd = geompy.GetEdgeNearPoint(Bielle_geom, y_h_g)
Edge_haut_pte = geompy.GetEdgeNearPoint(Bielle_geom, v_h_p)
Edge_haut_grd_trou = geompy.GetEdgesByLength(Face_haut, 0.147, 0.148)
Edge_haut_pte_trou = geompy.GetEdgesByLength(Face_haut, 0.094, 0.095)
Edge_haut_droite = geompy.GetEdgesByLength(Face_haut, 0.136, 0.137)
Edge_haut_gauche = geompy.GetEdgesByLength(Face_haut, 0.131, 0.132)


#x_h_g = geompy.MakeVertexOnCurve(Edge_haut_grd, 0)
x_h_g = geompy.MakeVertexOnCurve(Edge_haut_grd, u_3)
x_b_g = geompy.MakeVertexWithRef(x_h_g, 0, 0, -h)

#z_h_g = geompy.MakeVertexOnCurve(Edge_haut_grd, 1.)
z_h_g = geompy.MakeVertexOnCurve(Edge_haut_grd, u_1)
z_b_g = geompy.MakeVertexWithRef(z_h_g, 0, 0, -h)

#w_h_p = geompy.MakeVertexOnCurve(Edge_haut_pte, 0)
w_h_p = geompy.MakeVertexOnCurve(Edge_haut_pte, u_3)
w_b_p = geompy.MakeVertexWithRef(w_h_p, 0, 0, -h)

#u_h_p = geompy.MakeVertexOnCurve(Edge_haut_pte, 1)
u_h_p = geompy.MakeVertexOnCurve(Edge_haut_pte, u_1)
u_b_p = geompy.MakeVertexWithRef(u_h_p, 0, 0, -h)


y_h_g_t = geompy.MakeVertexOnCurve(Edge_haut_grd_trou, 0)
y_b_g_t = geompy.MakeVertexWithRef(y_h_g_t, 0, 0, -h)

z_h_g_t = geompy.MakeVertexOnCurve(Edge_haut_grd_trou, 0.2)
z_b_g_t = geompy.MakeVertexWithRef(z_h_g_t, 0, 0, -h)

#w_h_g_t = geompy.MakeVertexOnCurve(Edge_haut_grd_trou, 0.35)
w_h_g_t = geompy.MakeVertexOnCurve(Edge_haut_grd_trou, 0.40)
w_b_g_t = geompy.MakeVertexWithRef(w_h_g_t, 0, 0, -h)


v_h_g_t = geompy.MakeVertexOnCurve(Edge_haut_grd_trou, 0.5)
v_b_g_t = geompy.MakeVertexWithRef(v_h_g_t, 0, 0, -h)

#u_h_g_t = geompy.MakeVertexOnCurve(Edge_haut_grd_trou, 0.65)
u_h_g_t = geompy.MakeVertexOnCurve(Edge_haut_grd_trou, 0.6)
u_b_g_t = geompy.MakeVertexWithRef(u_h_g_t, 0, 0, -h)

x_h_g_t = geompy.MakeVertexOnCurve(Edge_haut_grd_trou, 0.8)
x_b_g_t = geompy.MakeVertexWithRef(x_h_g_t, 0, 0, -h)

y_h_p_t = geompy.MakeVertexOnCurve(Edge_haut_pte_trou, 0)
y_b_p_t = geompy.MakeVertexWithRef(y_h_p_t, 0, 0, -h)

#z_h_p_t = geompy.MakeVertexOnCurve(Edge_haut_pte_trou, 0.2)
z_h_p_t = geompy.MakeVertexOnCurve(Edge_haut_pte_trou, 0.1)
z_b_p_t = geompy.MakeVertexWithRef(z_h_p_t, 0, 0, -h)

w_h_p_t = geompy.MakeVertexOnCurve(Edge_haut_pte_trou, 0.35)
w_b_p_t = geompy.MakeVertexWithRef(w_h_p_t, 0, 0, -h)

v_h_p_t = geompy.MakeVertexOnCurve(Edge_haut_pte_trou, 0.5)
v_b_p_t = geompy.MakeVertexWithRef(v_h_p_t, 0, 0, -h)

u_h_p_t = geompy.MakeVertexOnCurve(Edge_haut_pte_trou, 0.65)
u_b_p_t = geompy.MakeVertexWithRef(u_h_p_t, 0, 0, -h)



#x_h_p_t = geompy.MakeVertexOnCurve(Edge_haut_pte_trou, 0.8)
x_h_p_t = geompy.MakeVertexOnCurve(Edge_haut_pte_trou, 0.9)
x_b_p_t = geompy.MakeVertexWithRef(x_h_p_t, 0, 0, -h)

u_h_g = geompy.MakeVertexOnCurve(Edge_haut_droite, 1)
u_b_g = geompy.MakeVertexWithRef(u_h_g, 0, 0, -h)

x_h_p = geompy.MakeVertexOnCurve(Edge_haut_droite, 0)
x_b_p = geompy.MakeVertexWithRef(x_h_p, 0, 0, -h)


w_h_g = geompy.MakeVertexOnCurve(Edge_haut_gauche, 0)
w_b_g = geompy.MakeVertexWithRef(w_h_g, 0, 0, -h)


z_h_p = geompy.MakeVertexOnCurve(Edge_haut_gauche, 1)
z_b_p = geompy.MakeVertexWithRef(z_h_p, 0, 0, -h)


Edge_v_grd = geompy.MakeLineTwoPnt(u_h_g, w_h_g)
v_h_g = geompy.MakeVertexOnCurve(Edge_v_grd, 0.5)
v_b_g = geompy.MakeVertexWithRef(v_h_g, 0, 0, -h)

Edge_v_pte = geompy.MakeLineTwoPnt(x_h_p, z_h_p)

y_h_p = geompy.MakeVertexOnCurve(Edge_v_pte, 0.5)
y_b_p = geompy.MakeVertexWithRef(y_h_p, 0, 0, -h)








#=======================
# CREATION ASSOCIATION
#=======================

#===========================================
# association des sommets de la face du haut
#===========================================

# grand trou

x_mod_h_g.setAssociation( x_h_g )
y_mod_h_g.setAssociation( y_h_g )
z_mod_h_g.setAssociation( z_h_g )
u_mod_h_g.setAssociation( u_h_g )
v_mod_h_g.setAssociation( v_h_g )
w_mod_h_g.setAssociation( w_h_g )

x_mod_h_g_t.setAssociation( x_h_g_t )
y_mod_h_g_t.setAssociation( y_h_g_t )
z_mod_h_g_t.setAssociation( z_h_g_t )
u_mod_h_g_t.setAssociation( u_h_g_t )
v_mod_h_g_t.setAssociation( v_h_g_t )
w_mod_h_g_t.setAssociation( w_h_g_t )

# petit trou

x_mod_h_p.setAssociation( x_h_p )
y_mod_h_p.setAssociation( y_h_p )
z_mod_h_p.setAssociation( z_h_p )
u_mod_h_p.setAssociation( u_h_p )
v_mod_h_p.setAssociation( v_h_p )
w_mod_h_p.setAssociation( w_h_p )

x_mod_h_p_t.setAssociation( x_h_p_t )
y_mod_h_p_t.setAssociation( y_h_p_t )
z_mod_h_p_t.setAssociation( z_h_p_t )
u_mod_h_p_t.setAssociation( u_h_p_t )
v_mod_h_p_t.setAssociation( v_h_p_t )
w_mod_h_p_t.setAssociation( w_h_p_t )


#===========================================
# association des sommets de la face du bas
#===========================================

# grand trou

x_mod_b_g.setAssociation( x_b_g )
y_mod_b_g.setAssociation( y_b_g )
z_mod_b_g.setAssociation( z_b_g )
u_mod_b_g.setAssociation( u_b_g )
v_mod_b_g.setAssociation( v_b_g )
w_mod_b_g.setAssociation( w_b_g )

x_mod_b_g_t.setAssociation( x_b_g_t )
y_mod_b_g_t.setAssociation( y_b_g_t )
z_mod_b_g_t.setAssociation( z_b_g_t )
u_mod_b_g_t.setAssociation( u_b_g_t )
v_mod_b_g_t.setAssociation( v_b_g_t )
w_mod_b_g_t.setAssociation( w_b_g_t )

# petit trou

x_mod_b_p.setAssociation( x_b_p )
y_mod_b_p.setAssociation( y_b_p )
z_mod_b_p.setAssociation( z_b_p )
u_mod_b_p.setAssociation( u_b_p )
v_mod_b_p.setAssociation( v_b_p )
w_mod_b_p.setAssociation( w_b_p )

x_mod_b_p_t.setAssociation( x_b_p_t )
y_mod_b_p_t.setAssociation( y_b_p_t )
z_mod_b_p_t.setAssociation( z_b_p_t )
u_mod_b_p_t.setAssociation( u_b_p_t )
v_mod_b_p_t.setAssociation( v_b_p_t )
w_mod_b_p_t.setAssociation( w_b_p_t )




#=================================
# Creation association des edges
#=================================


All_Edges_Bielle = geompy.SubShapeAllSorted(Bielle_geom, geompy.ShapeType["EDGE"])

##------------------
## Arrondies du haut
##------------------

#geometrie
Edge_Arr_grd_g_h = All_Edges_Bielle[20]
Edge_Arr_grd_d_h = All_Edges_Bielle[22]
Edge_Arr_pte_g_h = All_Edges_Bielle[8]
Edge_Arr_pte_d_h = All_Edges_Bielle[10]

#model
Edge_Arr_grd_g_h_mod = grille_cyl_grd.getEdgeJ( 1 , 1 , 1 )
Edge_Arr_grd_d_h_mod = grille_cyl_grd.getEdgeJ( 1 , 4 , 1 )
Edge_Arr_pte_g_h_mod = grille_cyl_pte.getEdgeJ( 1 , 1 , 1 )
Edge_Arr_pte_d_h_mod = grille_cyl_pte.getEdgeJ( 1 , 4 , 1 )



##------------------
## Arrondies du bas
##------------------
# geometrie
Edge_Arr_grd_g_b = All_Edges_Bielle[19]
Edge_Arr_grd_d_b = All_Edges_Bielle[21]
Edge_Arr_pte_g_b = All_Edges_Bielle[7]
Edge_Arr_pte_d_b = All_Edges_Bielle[9]


#model
Edge_Arr_grd_g_b_mod = grille_cyl_grd.getEdgeJ( 1 , 1 , 0 )
Edge_Arr_grd_d_b_mod = grille_cyl_grd.getEdgeJ( 1 , 4 , 0 )
Edge_Arr_pte_g_b_mod = grille_cyl_pte.getEdgeJ( 1 , 1 , 0 )
Edge_Arr_pte_d_b_mod = grille_cyl_pte.getEdgeJ( 1 , 4 , 0 )



##------------------
## Grands rayons
##------------------

# geometrie
Edge_Ray_grd_g_b = All_Edges_Bielle[27]
Edge_Ray_grd_g_h = All_Edges_Bielle[28]
Edge_Ray_grd_d_b = All_Edges_Bielle[0]
Edge_Ray_grd_d_h = All_Edges_Bielle[1]


#model
Edge_Grd_Ray_0_mod = grille_cyl_pte.getEdgeJ( 1 , 0 , 0 )
Edge_Grd_Ray_1_mod = grille_cyl_pte.getEdgeJ( 1 , 0 , 1 )
Edge_Grd_Ray_2_mod = grille_cyl_pte.getEdgeJ( 1 , 5 , 0 )
Edge_Grd_Ray_3_mod = grille_cyl_pte.getEdgeJ( 1 , 5 , 1 )
Edge_Grd_Ray_4_mod = grille_cyl_grd.getEdgeJ( 1 , 2 , 0 )
Edge_Grd_Ray_5_mod = grille_cyl_grd.getEdgeJ( 1 , 2 , 1 )
Edge_Grd_Ray_6_mod = grille_cyl_grd.getEdgeJ( 1 , 3 , 0 )
Edge_Grd_Ray_7_mod = grille_cyl_grd.getEdgeJ( 1 , 3 , 1 )




##------------------
## ASSOCIATIONS : arrondies du haut, arrondies du bas et grands rayons 
##------------------


#arrondies du haut
Edge_Arr_grd_g_h_mod.addAssociation( Edge_Arr_grd_g_h, 0., 1. )
Edge_Arr_grd_g_h_mod.addAssociation( Edge_Ray_grd_g_h, u_1, 1. )#Edge_haut_grd


Edge_Arr_grd_d_h_mod.addAssociation( Edge_Arr_grd_d_h, 0., 1. )
Edge_Arr_grd_d_h_mod.addAssociation( Edge_Ray_grd_g_h, 0., u_3 )


Edge_Arr_pte_g_h_mod.addAssociation( Edge_Arr_pte_g_h, 0., 1. )
Edge_Arr_pte_g_h_mod.addAssociation( Edge_Ray_grd_d_h, 0., u_3 )


Edge_Arr_pte_d_h_mod.addAssociation( Edge_Arr_pte_d_h, 0., 1. )
Edge_Arr_pte_d_h_mod.addAssociation( Edge_Ray_grd_d_h, u_1, 1. )


#arrondies du bas
Edge_Arr_grd_g_b_mod.addAssociation( Edge_Arr_grd_g_b, 0., 1. )
Edge_Arr_grd_g_b_mod.addAssociation( Edge_Ray_grd_g_b, u_1, 1. )


Edge_Arr_grd_d_b_mod.addAssociation( Edge_Arr_grd_d_b, 0., 1. )
Edge_Arr_grd_d_b_mod.addAssociation( Edge_Ray_grd_g_b, 0., u_3 )


Edge_Arr_pte_g_b_mod.addAssociation( Edge_Arr_pte_g_b, 0., 1. )
Edge_Arr_pte_g_b_mod.addAssociation( Edge_Ray_grd_d_b, 0., u_3 )


Edge_Arr_pte_d_b_mod.addAssociation( Edge_Arr_pte_d_b, 0., 1. )
Edge_Arr_pte_d_b_mod.addAssociation( Edge_Ray_grd_d_b, u_1, 1. )


#grands rayon
Edge_Grd_Ray_0_mod.addAssociation( Edge_Ray_grd_d_b, u_3, u_2 )
Edge_Grd_Ray_1_mod.addAssociation( Edge_Ray_grd_d_h, u_3, u_2 )
Edge_Grd_Ray_2_mod.addAssociation( Edge_Ray_grd_d_b, u_2, u_1 )
Edge_Grd_Ray_3_mod.addAssociation( Edge_Ray_grd_d_h, u_2, u_1 )

Edge_Grd_Ray_4_mod.addAssociation( Edge_Ray_grd_g_b, u_2, u_1 )
Edge_Grd_Ray_5_mod.addAssociation( Edge_Ray_grd_g_h, u_2, u_1 )
Edge_Grd_Ray_6_mod.addAssociation( Edge_Ray_grd_g_b, u_3, u_2 )
Edge_Grd_Ray_7_mod.addAssociation( Edge_Ray_grd_g_h, u_3, u_2 )




# recuperer les rayons des trous de la bielle de la geometrie
Edge_Trou_grd_h = All_Edges_Bielle[26]
Edge_Trou_grd_b = All_Edges_Bielle[25]
Edge_Trou_pte_h = All_Edges_Bielle[3]
Edge_Trou_pte_b = All_Edges_Bielle[2]


Partition_Trou_grd_haut = geompy.MakePartition([ Edge_Trou_grd_h ], [ z_h_g_t , w_h_g_t , v_h_g_t , u_h_g_t , x_h_g_t ], [], [], geompy.ShapeType["EDGE"], 0, [], 0)
All_Edge_Partition_Trou_grd_haut = geompy.SubShapeAllSorted(Partition_Trou_grd_haut, geompy.ShapeType["EDGE"])

Edge_Trou_grd_h_0 = All_Edge_Partition_Trou_grd_haut[0]
Edge_Trou_grd_h_1 = All_Edge_Partition_Trou_grd_haut[1]
Edge_Trou_grd_h_2 = All_Edge_Partition_Trou_grd_haut[2]
Edge_Trou_grd_h_3 = All_Edge_Partition_Trou_grd_haut[3]
Edge_Trou_grd_h_4 = All_Edge_Partition_Trou_grd_haut[4]
Edge_Trou_grd_h_5 = All_Edge_Partition_Trou_grd_haut[5]


Edge_Trou_grd_h_0_mod = grille_cyl_grd.getEdgeJ( 0 , 0 , 1 )
Edge_Trou_grd_h_1_mod = grille_cyl_grd.getEdgeJ( 0 , 5 , 1 )
Edge_Trou_grd_h_2_mod = grille_cyl_grd.getEdgeJ( 0 , 1 , 1 )
Edge_Trou_grd_h_3_mod = grille_cyl_grd.getEdgeJ( 0 , 4 , 1 )
Edge_Trou_grd_h_4_mod = grille_cyl_grd.getEdgeJ( 0 , 2 , 1 )
Edge_Trou_grd_h_5_mod = grille_cyl_grd.getEdgeJ( 0 , 3 , 1 )


Edge_Trou_grd_h_0_mod.addAssociation( Edge_Trou_grd_h_0, 0., 1. )
Edge_Trou_grd_h_1_mod.addAssociation( Edge_Trou_grd_h_1, 0., 1. )
Edge_Trou_grd_h_2_mod.addAssociation( Edge_Trou_grd_h_2, 0., 1. )
Edge_Trou_grd_h_4_mod.addAssociation( Edge_Trou_grd_h_4, 0., 1. )
Edge_Trou_grd_h_5_mod.addAssociation( Edge_Trou_grd_h_5, 0., 1. )
Edge_Trou_grd_h_3_mod.addAssociation( Edge_Trou_grd_h_3, 0., 1. )


Partition_Trou_grd_bas = geompy.MakePartition([ Edge_Trou_grd_b ], [ z_b_g_t , w_b_g_t , v_b_g_t , u_b_g_t , x_b_g_t ], [], [], geompy.ShapeType["EDGE"], 0, [], 0)
All_Edge_Partition_Trou_grd_bas = geompy.SubShapeAllSorted(Partition_Trou_grd_bas, geompy.ShapeType["EDGE"])

Edge_Trou_grd_b_0 = All_Edge_Partition_Trou_grd_bas[0]
Edge_Trou_grd_b_1 = All_Edge_Partition_Trou_grd_bas[1]
Edge_Trou_grd_b_2 = All_Edge_Partition_Trou_grd_bas[2]
Edge_Trou_grd_b_3 = All_Edge_Partition_Trou_grd_bas[3]
Edge_Trou_grd_b_4 = All_Edge_Partition_Trou_grd_bas[4]
Edge_Trou_grd_b_5 = All_Edge_Partition_Trou_grd_bas[5]


Edge_Trou_grd_b_0_mod = grille_cyl_grd.getEdgeJ( 0 , 0 , 0 )
Edge_Trou_grd_b_1_mod = grille_cyl_grd.getEdgeJ( 0 , 5 , 0 )
Edge_Trou_grd_b_2_mod = grille_cyl_grd.getEdgeJ( 0 , 1 , 0 )
Edge_Trou_grd_b_3_mod = grille_cyl_grd.getEdgeJ( 0 , 4 , 0 )
Edge_Trou_grd_b_4_mod = grille_cyl_grd.getEdgeJ( 0 , 2 , 0 )
Edge_Trou_grd_b_5_mod = grille_cyl_grd.getEdgeJ( 0 , 3 , 0 )


Edge_Trou_grd_b_0_mod.addAssociation( Edge_Trou_grd_b_0, 0., 1. )
Edge_Trou_grd_b_1_mod.addAssociation( Edge_Trou_grd_b_1, 0., 1. )
Edge_Trou_grd_b_2_mod.addAssociation( Edge_Trou_grd_b_2, 0., 1. )
Edge_Trou_grd_b_3_mod.addAssociation( Edge_Trou_grd_b_3, 0., 1. )
Edge_Trou_grd_b_4_mod.addAssociation( Edge_Trou_grd_b_4, 0., 1. )
Edge_Trou_grd_b_5_mod.addAssociation( Edge_Trou_grd_b_5, 0., 1. )



Partition_Trou_pte_haut = geompy.MakePartition([ Edge_Trou_pte_h ], [ z_h_p_t , w_h_p_t , v_h_p_t , u_h_p_t , x_h_p_t ], [], [], geompy.ShapeType["EDGE"], 0, [], 0)
All_Edge_Partition_Trou_pte_haut = geompy.SubShapeAllSorted(Partition_Trou_pte_haut, geompy.ShapeType["EDGE"])

Edge_Trou_pte_h_0 = All_Edge_Partition_Trou_pte_haut[0]
Edge_Trou_pte_h_1 = All_Edge_Partition_Trou_pte_haut[1]
Edge_Trou_pte_h_2 = All_Edge_Partition_Trou_pte_haut[2]
Edge_Trou_pte_h_3 = All_Edge_Partition_Trou_pte_haut[3]
Edge_Trou_pte_h_4 = All_Edge_Partition_Trou_pte_haut[4]
Edge_Trou_pte_h_5 = All_Edge_Partition_Trou_pte_haut[5]


Edge_Trou_pte_h_0_mod = grille_cyl_pte.getEdgeJ( 0 , 0 , 1 )
Edge_Trou_pte_h_1_mod = grille_cyl_pte.getEdgeJ( 0 , 5 , 1 )
Edge_Trou_pte_h_2_mod = grille_cyl_pte.getEdgeJ( 0 , 1 , 1 )
Edge_Trou_pte_h_3_mod = grille_cyl_pte.getEdgeJ( 0 , 4 , 1 )
Edge_Trou_pte_h_4_mod = grille_cyl_pte.getEdgeJ( 0 , 2 , 1 )
Edge_Trou_pte_h_5_mod = grille_cyl_pte.getEdgeJ( 0 , 3 , 1 )


Edge_Trou_pte_h_0_mod.addAssociation( Edge_Trou_pte_h_0, 0., 1. )
Edge_Trou_pte_h_1_mod.addAssociation( Edge_Trou_pte_h_1, 0., 1. )
Edge_Trou_pte_h_2_mod.addAssociation( Edge_Trou_pte_h_2, 0., 1. )
Edge_Trou_pte_h_3_mod.addAssociation( Edge_Trou_pte_h_3, 0., 1. )
Edge_Trou_pte_h_4_mod.addAssociation( Edge_Trou_pte_h_4, 0., 1. )
Edge_Trou_pte_h_5_mod.addAssociation( Edge_Trou_pte_h_5, 0., 1. )


Partition_Trou_pte_bas = geompy.MakePartition([ Edge_Trou_pte_b ], [ z_b_p_t , w_b_p_t , v_b_p_t , u_b_p_t , x_b_p_t ], [], [], geompy.ShapeType["EDGE"], 0, [], 0)
All_Edge_Partition_Trou_pte_bas = geompy.SubShapeAllSorted(Partition_Trou_pte_bas, geompy.ShapeType["EDGE"])


Edge_Trou_pte_b_0 = All_Edge_Partition_Trou_pte_bas[0]
Edge_Trou_pte_b_1 = All_Edge_Partition_Trou_pte_bas[1]
Edge_Trou_pte_b_2 = All_Edge_Partition_Trou_pte_bas[2]
Edge_Trou_pte_b_3 = All_Edge_Partition_Trou_pte_bas[3]
Edge_Trou_pte_b_4 = All_Edge_Partition_Trou_pte_bas[4]
Edge_Trou_pte_b_5 = All_Edge_Partition_Trou_pte_bas[5]


Edge_Trou_pte_b_0_mod = grille_cyl_pte.getEdgeJ( 0 , 0 , 0 )
Edge_Trou_pte_b_2_mod = grille_cyl_pte.getEdgeJ( 0 , 1 , 0 )
Edge_Trou_pte_b_4_mod = grille_cyl_pte.getEdgeJ( 0 , 2 , 0 )
Edge_Trou_pte_b_5_mod = grille_cyl_pte.getEdgeJ( 0 , 3 , 0 )
Edge_Trou_pte_b_3_mod = grille_cyl_pte.getEdgeJ( 0 , 4 , 0 )
Edge_Trou_pte_b_1_mod = grille_cyl_pte.getEdgeJ( 0 , 5 , 0 )

Edge_Trou_pte_b_0_mod.addAssociation( Edge_Trou_pte_b_0, 0., 1. )
Edge_Trou_pte_b_1_mod.addAssociation( Edge_Trou_pte_b_1, 0., 1. )
Edge_Trou_pte_b_2_mod.addAssociation( Edge_Trou_pte_b_2, 0., 1. )
Edge_Trou_pte_b_3_mod.addAssociation( Edge_Trou_pte_b_3, 0., 1. )
Edge_Trou_pte_b_4_mod.addAssociation( Edge_Trou_pte_b_4, 0., 1. )
Edge_Trou_pte_b_5_mod.addAssociation( Edge_Trou_pte_b_5, 0., 1. )



#====================================
# CREATION DES FACES DES ARRONDIES
#====================================
All_Face_Arron_Bielle = geompy.SubShapeAllSorted(Bielle_geom, geompy.ShapeType["FACE"])

Face_Arron_1 = All_Face_Arron_Bielle[2]
Face_Arron_2 = All_Face_Arron_Bielle[3]
Face_Arron_3 = All_Face_Arron_Bielle[8]
Face_Arron_4 = All_Face_Arron_Bielle[9]

Quad_Arron_1 = grille_cyl_pte.getQuadJK( 1 , 1 , 0 )
Quad_Arron_2 = grille_cyl_pte.getQuadJK( 1 , 4 , 0 )
Quad_Arron_3 = grille_cyl_grd.getQuadJK( 1 , 1 , 0 )
Quad_Arron_4 = grille_cyl_grd.getQuadJK( 1 , 4 , 0 )



#=================================================
# VERTEX, EDGES, FACES DANS L'ARBRE D'ETUDE SALOME
#=================================================

geompy.addToStudy( y_h_g , "y_h_g" )
geompy.addToStudy( y_b_g , "y_b_g" )
geompy.addToStudy( v_h_p , "v_h_p" )
geompy.addToStudy( v_b_p , "v_b_p" )
geompy.addToStudy( x_h_g , "x_h_g" )
geompy.addToStudy( x_b_g , "x_b_g" )
geompy.addToStudy( z_h_g , "z_h_g" )
geompy.addToStudy( z_b_g , "z_b_g" )
geompy.addToStudy( w_h_p , "w_h_p" )
geompy.addToStudy( w_b_p , "w_b_p" )
geompy.addToStudy( u_h_p , "u_h_p" )
geompy.addToStudy( u_b_p , "u_b_p" )
geompy.addToStudy(y_h_g_t, "y_h_g_t")
geompy.addToStudy(y_b_g_t, "y_b_g_t")
geompy.addToStudy(z_h_g_t, "z_h_g_t")
geompy.addToStudy(z_b_g_t, "z_b_g_t")
geompy.addToStudy(w_h_g_t, "w_h_g_t")
geompy.addToStudy(w_b_g_t, "w_b_g_t")
geompy.addToStudy(v_h_g_t, "v_h_g_t")
geompy.addToStudy(v_b_g_t, "v_b_g_t")
geompy.addToStudy(u_h_g_t, "u_h_g_t")
geompy.addToStudy(u_b_g_t, "u_b_g_t")
geompy.addToStudy(x_h_g_t, "x_h_g_t")
geompy.addToStudy(x_b_g_t, "x_b_g_t")
geompy.addToStudy(y_h_p_t, "y_h_p_t")
geompy.addToStudy(y_b_p_t, "y_b_p_t")
geompy.addToStudy(z_h_p_t, "z_h_p_t")
geompy.addToStudy(z_b_p_t, "z_b_p_t")
geompy.addToStudy(w_h_p_t, "w_h_p_t")
geompy.addToStudy(w_b_p_t, "w_b_p_t")
geompy.addToStudy(v_h_p_t, "v_h_p_t")
geompy.addToStudy(v_b_p_t, "v_b_p_t")
geompy.addToStudy(u_h_p_t, "u_h_p_t")
geompy.addToStudy(u_b_p_t, "u_b_p_t")
geompy.addToStudy(x_b_p_t, "x_b_p_t")
geompy.addToStudy(u_h_g, "u_h_g")
geompy.addToStudy(u_b_g, "u_b_g")
geompy.addToStudy(x_h_p, "x_h_p")
geompy.addToStudy(x_b_p, "x_b_p")
geompy.addToStudy(w_h_g, "w_h_g")
geompy.addToStudy(w_b_g, "w_b_g")
geompy.addToStudy(z_h_p, "z_h_p")
geompy.addToStudy(z_b_p, "z_b_p")
geompy.addToStudy(v_h_g, "v_h_g")
geompy.addToStudy(v_b_g, "v_b_g")
geompy.addToStudy(y_h_p, "y_h_p")
geompy.addToStudy(y_b_p, "y_b_p")


geompy.addToStudy( Edge_Trou_grd_h , "Edge_Trou_grd_h" )
geompy.addToStudy( Edge_Trou_grd_b , "Edge_Trou_grd_b" )
geompy.addToStudy( Edge_Trou_pte_h , "Edge_Trou_pte_h" )
geompy.addToStudy( Edge_Trou_pte_b , "Edge_Trou_pte_b" )
geompy.addToStudy( Edge_Trou_grd_h_0 , "Edge_Trou_grd_h_0" )
geompy.addToStudy( Edge_Trou_grd_h_2 , "Edge_Trou_grd_h_2" )
geompy.addToStudy( Edge_Trou_grd_h_4 , "Edge_Trou_grd_h_4" )
geompy.addToStudy( Edge_Trou_grd_h_5 , "Edge_Trou_grd_h_5" )
geompy.addToStudy( Edge_Trou_grd_h_3 , "Edge_Trou_grd_h_3" )
geompy.addToStudy( Edge_Trou_grd_h_1 , "Edge_Trou_grd_h_1" )
geompy.addToStudy( Edge_Trou_grd_b_0 , "Edge_Trou_grd_b_0" )
geompy.addToStudy( Edge_Trou_grd_b_2 , "Edge_Trou_grd_b_2" )
geompy.addToStudy( Edge_Trou_grd_b_4 , "Edge_Trou_grd_b_4" )
geompy.addToStudy( Edge_Trou_grd_b_5 , "Edge_Trou_grd_b_5" )
geompy.addToStudy( Edge_Trou_grd_b_3 , "Edge_Trou_grd_b_3" )
geompy.addToStudy( Edge_Trou_grd_b_1 , "Edge_Trou_grd_b_1" )
geompy.addToStudy( Edge_Trou_pte_h_0 , "Edge_Trou_pte_h_0" )
geompy.addToStudy( Edge_Trou_pte_h_2 , "Edge_Trou_pte_h_2" )
geompy.addToStudy( Edge_Trou_pte_h_4 , "Edge_Trou_pte_h_4" )
geompy.addToStudy( Edge_Trou_pte_h_5 , "Edge_Trou_pte_h_5" )
geompy.addToStudy( Edge_Trou_pte_h_3 , "Edge_Trou_pte_h_3" )
geompy.addToStudy( Edge_Trou_pte_h_1 , "Edge_Trou_pte_h_1" )
geompy.addToStudy( Edge_Trou_pte_b_0 , "Edge_Trou_pte_b_0" )
geompy.addToStudy( Edge_Trou_pte_b_2 , "Edge_Trou_pte_b_2" )
geompy.addToStudy( Edge_Trou_pte_b_4 , "Edge_Trou_pte_b_4" )
geompy.addToStudy( Edge_Trou_pte_b_5 , "Edge_Trou_pte_b_5" )
geompy.addToStudy( Edge_Trou_pte_b_3 , "Edge_Trou_pte_b_3" )
geompy.addToStudy( Edge_Trou_pte_b_1 , "Edge_Trou_pte_b_1" )

geompy.addToStudy( Edge_Arr_grd_g_h , "Edge_Arr_grd_g_h" )
geompy.addToStudy( Edge_Arr_grd_d_h , "Edge_Arr_grd_d_h" )
geompy.addToStudy( Edge_Arr_pte_g_h , "Edge_Arr_pte_g_h" )
geompy.addToStudy( Edge_Arr_pte_d_h , "Edge_Arr_pte_d_h" )

geompy.addToStudy( Edge_Arr_grd_g_b , "Edge_Arr_grd_g_b" )
geompy.addToStudy( Edge_Arr_grd_d_b , "Edge_Arr_grd_d_b" )
geompy.addToStudy( Edge_Arr_pte_g_b , "Edge_Arr_pte_g_b" )
geompy.addToStudy( Edge_Arr_pte_d_b , "Edge_Arr_pte_d_b" )

geompy.addToStudy( Edge_Ray_grd_g_h , "Edge_Ray_grd_g_h" )
geompy.addToStudy( Edge_Ray_grd_d_h , "Edge_Ray_grd_d_h" )
geompy.addToStudy( Edge_Ray_grd_g_b , "Edge_Ray_grd_g_b" )
geompy.addToStudy( Edge_Ray_grd_d_b , "Edge_Ray_grd_d_b" )


geompy.addToStudy(Face_Arron_1, "Face_Arron_1")
geompy.addToStudy(Face_Arron_2, "Face_Arron_2")
geompy.addToStudy(Face_Arron_3, "Face_Arron_3")
geompy.addToStudy(Face_Arron_4, "Face_Arron_4")


geompy.addToStudy( Partition_Trou_grd_haut , "Partition_Trou_grd_haut" )
geompy.addToStudy( Partition_Trou_grd_bas ,  "Partition_Trou_grd_bas" )
geompy.addToStudy( Partition_Trou_pte_haut , "Partition_Trou_pte_haut" )
geompy.addToStudy( Partition_Trou_pte_bas ,  "Partition_Trou_pte_bas" )



#====================================
# CREATION MAILLAGE
#====================================


#=================================================
# Definir les groupes d elements pour le maillage
#=================================================

# On definit 3 groupes de mailles

# groupe d edges (arretes)

Edge_grp = doc.addEdgeGroup("Edge_grp")
Nbr_Edg = doc.countEdge()
#print Nbr_Edg
for i in range(Nbr_Edg):
    Edge_i = doc.getEdge(i)
    Edge_grp.addElement(Edge_i)


# groupe de quads (faces)
Quad_grp = doc.addQuadGroup("Quad_grp")
Nbr_Qad = doc.countQuad()
#print Nbr_Qad
for i in range(Nbr_Qad):
    Quad_i = doc.getQuad(i)
    Quad_grp.addElement(Quad_i)

# groupe d hexas (solids)
Hexa_grp = doc.addHexaGroup("Hexa_grp")
Nbr_Hex = doc.countHexa()
#print Nbr_Hex
for i in range(Nbr_Hex):
    Hexa_i = doc.getHexa(i)
    Hexa_grp.addElement(Hexa_i)



# groupe de noeuds de vertex pour tout le modele 
Vertex_Nod_Grp = doc.addVertexNodeGroup("Vertex_Nod_Grp")
Nbr_Vx = doc.countVertex()
#print Nbr_Vx

for i in range(Nbr_Vx):
    Vertex_i = doc.getVertex(i)
    Vertex_Nod_Grp.addElement(Vertex_i)

#====================================
# Definir une loi de discretisation
#====================================
# definir une loi: le choix de la loi reste aux utilisateurs
Law = doc.addLaw( "Uniform" , 4 )

#n = doc.countLaw()
#print n

# chercher les propagations du modele
N_Propa = doc.countPropagation()

for j in range(N_Propa):
    Propa = doc.getPropagation(j)
    Propa.setLaw( Law )# appliquer la loi de discretisation sur tout le modele et generer le maillage

#====================================
# Générer des maillages
#====================================

print  " --- MAILLAGE HEXAHEDRIQUE --- "
mesh_hexas = hexablock.mesh(doc, "Bielle:hexas")

print "Nombre d hexaedres:"   , mesh_hexas.NbHexas()
print "Nombre de quadrangles:", mesh_hexas.NbQuadrangles()
print "Nombre de segments:"   , mesh_hexas.NbEdges()
print "Nombre de noeuds:"     , mesh_hexas.NbNodes()

print  " --- MAILLAGE QUADRANGULAIRE --- "
mesh_quads = hexablock.mesh(doc, "Bielle:quads", 2)

print "Nombre d hexaedres:"   , mesh_quads.NbHexas()
print "Nombre de quadrangles:", mesh_quads.NbQuadrangles()
print "Nombre de segments:"   , mesh_quads.NbEdges()
print "Nombre de noeuds:"     , mesh_quads.NbNodes()

print  " --- MAILLAGE 1D --- "
mesh_edges = hexablock.mesh(doc, "Bielle:edges", 1)

print "Nombre d hexaedres:"   , mesh_edges.NbHexas()
print "Nombre de quadrangles:", mesh_edges.NbQuadrangles()
print "Nombre de segments:"   , mesh_edges.NbEdges()
print "Nombre de noeuds:"     , mesh_edges.NbNodes()

print  " --- MAILLAGE des sommets --- "
mesh_nodes = hexablock.mesh(doc, "Bielle:nodes", 0)

print "Nombre d hexaedres:"   , mesh_nodes.NbHexas()
print "Nombre de quadrangles:", mesh_nodes.NbQuadrangles()
print "Nombre de segments:"   , mesh_nodes.NbEdges()
print "Nombre de noeuds:"     , mesh_nodes.NbNodes()
