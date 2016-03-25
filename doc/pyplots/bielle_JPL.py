#!/usr/bin/python
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
# modifié par JPL le 19 avril 2010
# (reprise de BIELLE.last.test.py)
#===============================================

# @todo idée à terme : ne pas avoir à faire "a la mano" l'association de TOUS les vertex du modèle, mais faire
# en sorte qu'un certain nombre d'associations soient faites automatiquement.
# typiquement, pour une grille cylindrique (ex de 12 vertex comme celles du script) :
# 4 points doivent etre obligatoirement associés à la main
# les 8 autres devraient pouvoir l'etre automatiquement
#
# pour cela, il serait utile de pouvoir récupérer les propriétés de la grille cylindrique (qui ne sont pour le moment
# pas conservées toutes conservées dans le moteur HEXA, ou en tout cas pas toutes accessibles avec l'API python)

import GEOM
import geompy
import smesh
import HexaBlocks
import math
import SALOMEDS

def get_vertices_from_cylindrical(grille_cyl, nr, na, nl=1):
    """
    fonction qui prend en entrée une liste d'hexaèdres renvoyés par makeCylindrical, dans le cas FALSE uniquement
    (ie : pas de remplissage de la partie centrale)
    => du type grid_type
    
    renvoie une liste de sommets, dans cet ordre :

    pour la grande grille cylindrique, la liste est constituée, dans l'ordre :
    1) des vertex de la face du haut (k = 1) => na * 2
    a) vertex externes (i = 1) => na
    b) vertex internes (i = 0) => na
    2) des vertex de la face du bas (k = 0) => na * 2
    a) vertex externes (i = 1) => na
    b) vertex internes (i = 0) => na
    
    @todo makeCylindrical renvoie une grille cylindrique
    ie : une liste d'hexaèdres
    pbmes :
    1) une fois la grille cylindrique crée, elle est renvoyée sous forme d'un tableau d'hexaèdres
    dans un certain ordre (cf. SPECS 2.3.6), mais pas sous forme d'objet "grille cylindrique"
    => cette fonction ne pourra pas à terme etre intégrée sous forme de méthode.
    2) impossible de faire des tests sur le cylindre (vide ou pas, récupération directe de nr, na, nl, ...)
    """

    # dans cette première version, on considère une grille cylindrique avec nr = 1 et nl = 1
    if (nr !=1) or (nl != 1):
        print "nr ou nl invalide(s) !!"
        return None
    
    vertices = []
    li_z = [nl, 0] # indice nl => face du haut, indice 0 => face du bas
    li_x = [nr, 0] # indice nr => externe, indice 0 => interne
    li_y = range(na) # indices de 0 à (na-1)
    for k in li_z: # suivant z
        for i in li_x: # suivant x
            for j in li_y: # suivant y 
                vertices.append(grille_cyl.getVertexIJK(i , j , k))
                
    return vertices
    
def set_association_from_cylindrical(model_vertices, geom_vertices):
    """
    réalise l'association entre :
    1) les sommets issus d'une grille cylindrique (résultat de l'appel à get_vertices_from_cylindrical())
    2) les sommets de la geometrie

    l'association est faite dans l'ordre des vertex de la liste du modèle ie :
    1) les sommets externes de la face du haut
    2) les sommets internes de la face du haut
    3) les sommets externes de la face du bas
    4) les sommets internes de la face du bas
    """
    
    for num, vertex in enumerate(model_vertices):
        vertex.setAssociation(geom_vertices[num])
        
    pass

#=============================
# CREATION DOCUMENT
#=============================

doc = HexaBlocks.addDocument()

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

grille_cyl_pte = doc.makeTranslation(grille_cyl_grd, dx_prime )

# on obtient une liste qui contient 6 hexaedres eguaux

y1 = doc.findVertex(2*l - 2*R , 0 , h)
y2 = doc.findVertex(2*l - alpha_x , alpha_y , 0)
y3 = doc.findVertex(2*l - alpha_x , -alpha_y , 0)
y4 = doc.findVertex(2*l - 2*R , 0 , 0)

quad_21 = doc.findQuad(y1, y2)
quad_22 = doc.findQuad(y1, y3)

#=================================================
# Assemblage des deux grilles cylindriques
#=================================================

# a priori ne sert que pour créer bielle.vtk : normal ??
model_biell_fin = doc.joinQuads(quad_list, quad_21, x1, y1, x4, y4, 1)


model_biell_fin.saveVtk("/tmp/bielle.vtk")
grille_cyl_pte.saveVtk("/tmp/grille_cyl_pte.vtk")
grille_cyl_grd.saveVtk("/tmp/grille_cyl_grd.vtk")


#===================================================================
# Recuperation des vertex du modele hexa bielle pour l association
#===================================================================

# pour la grande grille cylindrique, la liste est constituée, dans l'ordre :
# 1) des vertex de la face du haut (k = 1) => 12
#    a) vertex externes (i = 1) => 6
#    b) vertex internes (i = 0) => 6
# 2) des vertex de la face du bas (k = 0) => 12
#    a) vertex externes (i = 1) => 6
#    b) vertex internes (i = 0) => 6

mod_grd = get_vertices_from_cylindrical(grille_cyl_grd, nr_grd, na_grd, nl_grd)

# pour la petite grille cylindrique, la liste est constituée, dans l'ordre :
# 1) des vertex de la face du haut (k = 1) => 12
#    a) vertex externes (i = 1) => 6
#    b) vertex internes (i = 0) => 6
# 2) des vertex de la face du bas (k = 0) => 12
#    a) vertex externes (i = 1) => 6
#    b) vertex internes (i = 0) => 6

mod_pte = get_vertices_from_cylindrical(grille_cyl_pte, nr_pte, na_pte, nl_pte)

#===================================================================
# Recuperation des points de la geometrie bielle pour l association
#===================================================================

Bielle_geom = geompy.Import("/home/plumecoq/Karima/karima/hexa/cao_cea/crank.stp", "STEP")

geompy.addToStudy(Bielle_geom, "Bielle_geom")

Pt_A = geompy.MakeVertex(0, 0, h/2.)
Face_haut = geompy.GetFaceNearPoint(Bielle_geom, Pt_A)

Pt_B = geompy.MakeVertex(0, 0, -h/2.)
Face_bas = geompy.GetFaceNearPoint(Bielle_geom, Pt_B)

Edge_haut_droite = geompy.GetEdgesByLength(Face_haut, 0.136, 0.137)
Edge_haut_gauche = geompy.GetEdgesByLength(Face_haut, 0.131, 0.132)

# 1. grand cylindre :
geom_vert_grd = []
# 1.1 face du haut :
# 1.1.1 sommets externes :

# pour l'instant, la création des vertex de la geometrie se fait obligatoirement
# dans cet ordre :
y_h_g = geompy.MakeVertexOnSurface(Face_haut, 1, 0.5)

Edge_haut_grd = geompy.GetEdgeNearPoint(Bielle_geom, y_h_g)
x_h_g = geompy.MakeVertexOnCurve(Edge_haut_grd, 0)
z_h_g = geompy.MakeVertexOnCurve(Edge_haut_grd, 1)
u_h_g = geompy.MakeVertexOnCurve(Edge_haut_droite, 1)
w_h_g = geompy.MakeVertexOnCurve(Edge_haut_gauche, 0)

Edge_v_grd = geompy.MakeLineTwoPnt(u_h_g, w_h_g)
v_h_g = geompy.MakeVertexOnCurve(Edge_v_grd, 0.5)

geom_vert_grd.extend([y_h_g, x_h_g, z_h_g, u_h_g, w_h_g, v_h_g])

# 1.1.2 sommets internes :
Edge_haut_grd_trou = geompy.GetEdgesByLength(Face_haut, 0.147, 0.148)
y_h_g_t = geompy.MakeVertexOnCurve(Edge_haut_grd_trou, 0)
z_h_g_t = geompy.MakeVertexOnCurve(Edge_haut_grd_trou, 0.2)
w_h_g_t = geompy.MakeVertexOnCurve(Edge_haut_grd_trou, 0.35)
v_h_g_t = geompy.MakeVertexOnCurve(Edge_haut_grd_trou, 0.5)
u_h_g_t = geompy.MakeVertexOnCurve(Edge_haut_grd_trou, 0.65)
x_h_g_t = geompy.MakeVertexOnCurve(Edge_haut_grd_trou, 0.8)

geom_vert_grd.extend([y_h_g_t, x_h_g_t, z_h_g_t, u_h_g_t, w_h_g_t, v_h_g_t])

# 1.2 face du bas :
# pour tous les vertex de la face du bas, on les crée par référence à ceux déjà crées (dans le meme ordre :
# sommets externes puis internes) :
geom_vert_grd_bas = [geompy.MakeVertexWithRef(vertex, 0.0, 0.0, -h) for vertex in geom_vert_grd]
geom_vert_grd.extend(geom_vert_grd_bas)


print "nombre vertex grand cylindre geom = ", len(geom_vert_grd)

# ajout des vertex de la géométrie à l'étude :
for num, vertex in enumerate(geom_vert_grd):
#    print "ajout de v_grd"+str(num)
    geompy.addToStudy(vertex, "v_grd"+str(num))

# association :
set_association_from_cylindrical(mod_grd, geom_vert_grd)

# 2. petit cylindre :
geom_vert_pte = []
# 2.1 face du haut :
# 2.1.1 sommets externes :

# pour l'instant, la création des vertex de la geometrie se fait obligatoirement
# dans cet ordre :
v_h_p = geompy.MakeVertexOnSurface(Face_haut, 0, 0.5)

Edge_haut_pte = geompy.GetEdgeNearPoint(Bielle_geom, v_h_p)
w_h_p = geompy.MakeVertexOnCurve(Edge_haut_pte, 0)
u_h_p = geompy.MakeVertexOnCurve(Edge_haut_pte, 1)
x_h_p = geompy.MakeVertexOnCurve(Edge_haut_droite, 0)
z_h_p = geompy.MakeVertexOnCurve(Edge_haut_gauche, 1)

Edge_v_pte = geompy.MakeLineTwoPnt(x_h_p, z_h_p)
y_h_p = geompy.MakeVertexOnCurve(Edge_v_pte, 0.5)

geom_vert_pte.extend([y_h_p, x_h_p, z_h_p, u_h_p, w_h_p, v_h_p])

# 2.1.2 sommets internes :
Edge_haut_pte_trou = geompy.GetEdgesByLength(Face_haut, 0.094, 0.095)
y_h_p_t = geompy.MakeVertexOnCurve(Edge_haut_pte_trou, 0)
z_h_p_t = geompy.MakeVertexOnCurve(Edge_haut_pte_trou, 0.2)
w_h_p_t = geompy.MakeVertexOnCurve(Edge_haut_pte_trou, 0.35)
v_h_p_t = geompy.MakeVertexOnCurve(Edge_haut_pte_trou, 0.5)
u_h_p_t = geompy.MakeVertexOnCurve(Edge_haut_pte_trou, 0.65)
x_h_p_t = geompy.MakeVertexOnCurve(Edge_haut_pte_trou, 0.8)

geom_vert_pte.extend([y_h_p_t, x_h_p_t, z_h_p_t, u_h_p_t, w_h_p_t, v_h_p_t])

# 2.2 face du bas :
# pour tous les vertex de la face du bas, on les crée par référence à ceux déjà crées (dans le meme ordre :
# sommets externes puis internes) :
geom_vert_pte_bas = [geompy.MakeVertexWithRef(vertex, 0.0, 0.0, -h) for vertex in geom_vert_pte]
geom_vert_pte.extend(geom_vert_pte_bas)

print "nombre vertex petit cylindre geom = ", len(geom_vert_pte)

# ajout des vertex de la géométrie à l'étude :
for num, vertex in enumerate(geom_vert_pte):
#    print "ajout de v_pte"+str(num)
    geompy.addToStudy(vertex, "v_pte"+str(num))

# association :
set_association_from_cylindrical(mod_pte, geom_vert_pte)

le meme ordre :
# sommets externes puis internes) :
geom_vert_pte_bas = [geompy.MakeVertexWithRef(vertex, 0.0, 0.0, -h) for vertex in geom_vert_pte]
geom_vert_pte.extend(geom_vert_pte_bas)

print "nombre vertex petit cylindre geom = ", len(geom_vert_pte)

# ajout des vertex