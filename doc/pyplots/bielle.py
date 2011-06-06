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
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

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

doc = hexablock.addDocument()

#=============================
# CREATION DU MODELE
#=============================

# Pour la bielle on doit creer deux grilles cylindriques et prismer les quadrangles entre ces deux dernieres

#=============================
# PARAMETRES
#=============================

R = 40.0
r_grd = R
r_grd_t = R/2.0

## r_pte = R
## r_pte_t = R/2.0

longueur = 200.0
hauteur = 0.019999999553*2
## hauteur = 40.

# Taille du grand cylindre
dr_grd = R
da_grd = 360
dl_grd = hauteur

nr_grd = 1
na_grd = 6
nl_grd = 1

#=============================
# Creation des vecteurs
#=============================

dx = doc.addVector(longueur, 0, 0)
dy = doc.addVector(0, longueur, 0)
dz = doc.addVector(0, 0, longueur)

#=================================================
# Creation des centres des grilles cylindriques
#=================================================

c_grd = doc.addVertex(0, 0, 0)
c_pte = doc.addVertex(2*longueur, 0, 0)
dx_prime = doc.addVectorVertices(c_grd, c_pte)

#=================================================
# Creation de la grande grille cylindrique
#=================================================

grille_cyl_grd = doc.makeCylindrical(c_grd, dx, dz, dr_grd, da_grd, dl_grd, nr_grd, na_grd, nl_grd, False)

print "grille_cyl_grd nb edges = ", grille_cyl_grd.countEdge()

# on obtient une liste qui contient 6 hexaedres egaux

# 30 = pi/6
# 60 = pi/3
# 45 = pi/4
# 90 = pi/2
# 180 = pi

alpha_x = 2*R*math.cos(math.pi/3)
alpha_y = 2*R*math.sin(math.pi/3)

x1 = doc.findVertex(2*R, 0, hauteur)
x2 = doc.findVertex(alpha_x, alpha_y, 0)
x3 = doc.findVertex(alpha_x, -alpha_y, 0)
x4 = doc.findVertex(2*R, 0, 0)

quad_11 = doc.findQuad(x1, x2)
quad_12 = doc.findQuad(x1, x3)

#=================================================
# Creation de la petite grille cylindrique
#=================================================

grille_cyl_pte = doc.makeTranslation(grille_cyl_grd, dx_prime)

print "grille_cyl_pte nb edges = ", grille_cyl_pte.countEdge()

# on obtient une liste qui contient 6 hexaedres egaux

y1 = doc.findVertex(2*longueur - 2*R, 0 , hauteur)
y2 = doc.findVertex(2*longueur - alpha_x, alpha_y, 0)
y3 = doc.findVertex(2*longueur - alpha_x, -alpha_y, 0)
y4 = doc.findVertex(2*longueur - 2*R, 0, 0)

quad_21 = doc.findQuad(y1, y2)
quad_22 = doc.findQuad(y1, y3)

#=================================================
# Assemblage des deux grilles cylindriques
#=================================================
model_biell_fin = doc.joinQuads([quad_11, quad_12], quad_21, x1, y1, x4, y4, 1)

print "model_biell_fin nb edges = ", model_biell_fin.countEdge()

# temporaire : sauvegarde :
file_name = os.path.join(os.environ['TMP'], 'bielle.vtk')
model_biell_fin.saveVtk(file_name)
file_name = os.path.join(os.environ['TMP'], 'grille_cyl_pte.vtk')
grille_cyl_pte.saveVtk(file_name)
file_name = os.path.join(os.environ['TMP'], 'grille_cyl_grd.vtk')
grille_cyl_grd.saveVtk(file_name)

#===================================================================
# Recuperation des points de la geometrie bielle pour l association
#===================================================================

# NB:
# h = haut
# b = bas
# g = grand
# p = petit
# t = trou

bielle_geom = geompy.ImportFile(STEP_PATH, "STEP")

geompy.addToStudy(bielle_geom, "bielle_geom")

pt_a = geompy.MakeVertex(0, 0, hauteur/2.)
face_haut = geompy.GetFaceNearPoint(bielle_geom, pt_a)

pt_b = geompy.MakeVertex(0, 0, -hauteur/2.)
face_bas = geompy.GetFaceNearPoint(bielle_geom, pt_b)

edge_haut_droite = geompy.GetEdgesByLength(face_haut, 0.136, 0.137)
edge_haut_gauche = geompy.GetEdgesByLength(face_haut, 0.131, 0.132)

# Param�tres pour :
# - r�cup�ration des points.
# - et associations.

# JPL le 05/05/2011 : pour x_h_g et z_h_g (resp. w_h_p et u_h_p), on teste en les pla�ant �
# l'intersection des 2 edges :
## u_1 = 0.75  # 0.8
## u_3 = 0.25  # 0.2
u_1 = 1.0
u_3 = 0.0

# dictionnaire des vertices de la g�om�trie (grande et petite grilles) :
# key = nom, value = indice dans geom_vert_grd (resp. geom_vert_pte)
# on commence par les vertex externes du haut du grand (resp. petit) cylindre,
# ensuite :
# 1. vertex internes (trou),
# 2. puis vertex du bas (externes puis internes)
dic_vert_names = {}
ind = 0
for z in ['h', 'b']:  # haut/bas
    for x in ['e', 't']:  # externe/interne(trou)
        for y in ['y', 'x', 'u', 'v', 'w', 'z']:  # 
            key = '_'.join([y, z, x])
            dic_vert_names[key] = ind
            ind += 1
            pass
        pass
    pass

# 1. grand cylindre :
geom_vert_grd = []  # liste des vertex du grand cylindre

# 1.1 face du haut :
# 1.1.1 sommets externes :
# la cr�ation des vertex de la geometrie se fait obligatoirement
# dans cet ordre :
geom_vert_grd.append(geompy.MakeVertexOnSurface(face_haut, 1, 0.5))  # y_h_e_g
edge_haut_grd = geompy.GetEdgeNearPoint(bielle_geom, geom_vert_grd[0])
geom_vert_grd.append(geompy.MakeVertexOnCurve(edge_haut_grd, u_3))  # x_h_e_g
geom_vert_grd.append(geompy.MakeVertexOnCurve(edge_haut_grd, u_1))  # z_h_e_g
geom_vert_grd.insert(2, geompy.MakeVertexOnCurve(edge_haut_droite, 1))  # u_h_e_g
geom_vert_grd.insert(3, geompy.MakeVertexOnCurve(edge_haut_gauche, 0))  # w_h_e_g
edge_v_grd = geompy.MakeLineTwoPnt(geom_vert_grd[2], geom_vert_grd[3])
geom_vert_grd.insert(3, geompy.MakeVertexOnCurve(edge_v_grd, 0.5))  # v_h_e_g
# les vertex sont rang�s dans cet ordre :
# y_h_e_g, x_h_e_g, u_h_e_g, v_h_e_g, w_h_e_g, z_h_e_g

# 1.1.2 sommets internes (trou) :
edge_haut_grd_trou = geompy.GetEdgesByLength(face_haut, 0.147, 0.148)
# y_h_t_g, x_h_t_g, u_h_t_g, v_h_t_g, w_h_t_g, z_h_t_g
for val in [0, 0.8, 0.6, 0.5, 0.4, 0.2]:
    geom_vert_grd.append(geompy.MakeVertexOnCurve(edge_haut_grd_trou, val))

# 1.2 face du bas :
# pour tous les vertex de la face du bas, on les cr�e par r�f�rence � ceux d�j� cr�es (dans le meme ordre :
# sommets externes puis internes) :
geom_vert_grd_bas = [geompy.MakeVertexWithRef(vertex, 0.0, 0.0, -hauteur) for vertex in geom_vert_grd]
geom_vert_grd.extend(geom_vert_grd_bas)

# pour l'utilisation :
# geom_vert_grd[dic_vert_names["y_h_t"]] # par exemple

print "nombre vertex grand cylindre geom = ", len(geom_vert_grd)

# 2. petit cylindre :
# REM : le modele petit cylindre a ete cree par translation / au grand
# cylindre. Les vertices de la geometrie sont donc ete cr�es de mani�re
# similaire (# d'une symmetrie / miroir)
geom_vert_pte = []

# 2.1 face du haut :
# 2.1.1 sommets externes :

# la cr�ation des vertex de la geometrie se fait obligatoirement
# dans cet ordre :
geom_vert_pte.append(geompy.MakeVertexOnSurface(face_haut, 0, 0.5))  # v_h_e_p
edge_haut_pte = geompy.GetEdgeNearPoint(bielle_geom, geom_vert_pte[0])
geom_vert_pte.append(geompy.MakeVertexOnCurve(edge_haut_pte, u_3))  # w_h_e_p
geom_vert_pte.insert(0, geompy.MakeVertexOnCurve(edge_haut_pte, u_1))  # u_h_e_p
geom_vert_pte.insert(0, geompy.MakeVertexOnCurve(edge_haut_droite, 0))  # x_h_e_p
geom_vert_pte.append(geompy.MakeVertexOnCurve(edge_haut_gauche, 1))  # z_h_e_p
edge_v_pte = geompy.MakeLineTwoPnt(geom_vert_pte[0], geom_vert_pte[4])
geom_vert_pte.append(geompy.MakeVertexOnCurve(edge_v_pte, 0.5))  # y_h_e_p
# les vertex sont rang�s dans cet ordre :
# y_h_e_p, x_h_e_p, u_h_e_p, v_h_e_p, w_h_e_p, z_h_e_p


# 2.1.2 sommets internes (trou) :
edge_haut_pte_trou = geompy.GetEdgesByLength(face_haut, 0.094, 0.095)
# y_h_t_p, x_h_t_p, u_h_t_p, v_h_t_p, w_h_t_p, z_h_t_p
for val in [0, 0.9, 0.65, 0.5, 0.35, 0.1]:
    geom_vert_pte.append(geompy.MakeVertexOnCurve(edge_haut_pte_trou, val))

# 2.2 face du bas :
# pour tous les vertex de la face du bas, on les cr�e par r�f�rence � ceux d�j� cr�es (dans le meme ordre :
# sommets externes puis internes) :
geom_vert_pte_bas = [geompy.MakeVertexWithRef(vertex, 0.0, 0.0, -hauteur) for vertex in geom_vert_pte]
geom_vert_pte.extend(geom_vert_pte_bas)

print "nombre vertex petit cylindre geom = ", len(geom_vert_pte)

#=======================
# CREATION ASSOCIATION
#=======================

# JPL (le 29/04/2011)
# pour l'association, il suffit de faire 6 appels � la m�thode
# associateClosedLine() :
# 1 pour la ligne du trou haut du grand cylindre
# 1 pour la ligne du trou bas du grand cylindre
# 1 pour la ligne du trou haut du petit cylindre
# 1 pour la ligne du trou bas du petit cylindre
# 1 pour la ligne "externe" de la bielle, en haut
# 1 pour la ligne "externe" de la bielle, en bas

all_edges_bielle = geompy.SubShapeAllSorted(bielle_geom, geompy.ShapeType["EDGE"])

print "nb edges geom = ", len(all_edges_bielle)

# dictionnaire des edges de la g�om�trie :
# key = nom, value = indice dans all_edges_bielle
dic_edge_names = {"edge_ray_pte_b": 0, "edge_ray_pte_h": 1,
                  "edge_trou_pte_b": 2, "edge_trou_pte_h" :3,
                  "edge_arr_pte_g_b": 7, "edge_arr_pte_g_h": 8,
                  "edge_arr_pte_d_b": 9, "edge_arr_pte_d_h": 10,
                  "edge_arr_grd_g_b": 19, "edge_arr_grd_g_h": 20,
                  "edge_arr_grd_d_b": 21, "edge_arr_grd_d_h": 22,              
                  "edge_trou_grd_b": 25, "edge_trou_grd_h": 26,
                  "edge_ray_grd_b": 27, "edge_ray_grd_h": 28,
                  "edge_long_g_b": 13, "edge_long_g_h": 14,
                  "edge_long_d_b": 15, "edge_long_d_h": 16
                  }

# REM : erreur dans le script initial (inversion grd/pte sur edge_ray_...)

# 1. ligne du trou haut du grand cylindre
# =======================================

# modele de blocs :
mod_start = grille_cyl_grd.getEdgeJ(0, 0, 1)  # 1er edge
mod_first = mod_start.getVertex(0)  # 1er vertex
## print "mod_first : ", mod_first.getX(), " ", mod_first.getY(), " ", mod_first.getZ()
mod_line = [grille_cyl_grd.getEdgeJ(0, j, 1) for j in [5, 1, 4, 2, 3]]
# table des edges

# geometrie :
e_t_g_h = all_edges_bielle[dic_edge_names["edge_trou_grd_h"]]
li_vert = geom_vert_grd[dic_vert_names['x_h_t']:dic_vert_names['z_h_t']+1]
li_vert.reverse()  # z_h_t_g, w_h_t_g, v_h_t_g, u_h_t_g, x_h_t_g
part_trou_grd_haut = geompy.MakePartition([e_t_g_h],
                                          li_vert,
                                          [], [], geompy.ShapeType["EDGE"], 0, [], 0)
all_edge_part_trou_grd_haut = geompy.SubShapeAllSorted(part_trou_grd_haut, geompy.ShapeType["EDGE"])

print "all_edge_part_trou_grd_haut len = ", len(all_edge_part_trou_grd_haut)

geo_start = all_edge_part_trou_grd_haut[0]  # 1ere ligne
par_start = 0.0  # param 1ere ligne
geo_line = all_edge_part_trou_grd_haut[1:6]  # table des lignes

# association :
ier = doc.associateClosedLine(mod_first, mod_start, mod_line,
                              geo_start, par_start, geo_line)

# 2. ligne du trou bas du grand cylindre
# =======================================

# modele de blocs :
mod_start = grille_cyl_grd.getEdgeJ(0, 0, 0)
mod_first = mod_start.getVertex(0)
mod_line = [grille_cyl_grd.getEdgeJ(0, j, 0) for j in [5, 1, 4, 2, 3]]

# geometrie :
e_t_g_b = all_edges_bielle[dic_edge_names["edge_trou_grd_b"]]
li_vert = geom_vert_grd[dic_vert_names['x_b_t']:dic_vert_names['z_b_t']+1]
li_vert.reverse()  # z_b_t_g, w_b_t_g, v_b_t_g, u_b_t_g, x_b_t_g
part_trou_grd_bas = geompy.MakePartition([e_t_g_b],
                                         li_vert,
                                         [], [], geompy.ShapeType["EDGE"], 0, [], 0)
all_edge_part_trou_grd_bas = geompy.SubShapeAllSorted(part_trou_grd_bas, geompy.ShapeType["EDGE"])

print "all_edge_part_trou_grd_bas len = ", len(all_edge_part_trou_grd_bas)

geo_start = all_edge_part_trou_grd_bas[0]
par_start = 0.0
geo_line = all_edge_part_trou_grd_bas[1:6]

# association :
ier = doc.associateClosedLine(mod_first, mod_start, mod_line,
                              geo_start, par_start, geo_line)

# 3. ligne du trou haut du petit cylindre
# =======================================

# modele de blocs :
mod_start = grille_cyl_pte.getEdgeJ(0, 0, 1)
mod_first = mod_start.getVertex(0)
mod_line = [grille_cyl_pte.getEdgeJ(0, j, 1) for j in [5, 1, 4, 2, 3]]

# geometrie :
e_t_p_h = all_edges_bielle[dic_edge_names["edge_trou_pte_h"]]
li_vert = geom_vert_pte[dic_vert_names['x_h_t']:dic_vert_names['z_h_t']+1]
li_vert.reverse()  # z_h_t_p, w_h_t_p, v_h_t_p, u_h_t_p, x_h_t_p
part_trou_pte_haut = geompy.MakePartition([e_t_p_h],
                                          li_vert,
                                          [], [], geompy.ShapeType["EDGE"], 0, [], 0)
all_edge_part_trou_pte_haut = geompy.SubShapeAllSorted(part_trou_pte_haut, geompy.ShapeType["EDGE"])

print "all_edge_part_trou_pte_haut len = ", len(all_edge_part_trou_pte_haut)

geo_start = all_edge_part_trou_pte_haut[0]
par_start = 0.0
geo_line = all_edge_part_trou_pte_haut[1:6]

# association :
ier = doc.associateClosedLine(mod_first, mod_start, mod_line,
                              geo_start, par_start, geo_line)

# 4. ligne du trou bas du petit cylindre
# =======================================

# modele de blocs :
mod_start = grille_cyl_pte.getEdgeJ(0, 0, 0)
mod_first = mod_start.getVertex(0)
mod_line = [grille_cyl_pte.getEdgeJ(0, j, 0) for j in [5, 1, 4, 2, 3]]

# geometrie :
e_t_p_b = all_edges_bielle[dic_edge_names["edge_trou_pte_b"]]
li_vert = geom_vert_pte[dic_vert_names['x_b_t']:dic_vert_names['z_b_t']+1]
li_vert.reverse()  # z_b_t_p, w_b_t_p, v_b_t_p, u_b_t_p, x_b_t_p
part_trou_pte_bas = geompy.MakePartition([e_t_p_b],
                                         li_vert,
                                         [], [], geompy.ShapeType["EDGE"], 0, [], 0)
all_edge_part_trou_pte_bas = geompy.SubShapeAllSorted(part_trou_pte_bas, geompy.ShapeType["EDGE"])

print "all_edge_part_trou_pte_bas len = ", len(all_edge_part_trou_pte_bas)

geo_start = all_edge_part_trou_pte_bas[0]
par_start = 0.0
geo_line = all_edge_part_trou_pte_bas[1:6]

# association :
ier = doc.associateClosedLine(mod_first, mod_start, mod_line,
                              geo_start, par_start, geo_line)

# 5. ligne "externe" de la bielle, en haut
# ========================================

# les edges de la geometrie a prendre en compte sont :
# (en partant du grand cylindre)
# edge_arr_grd_d_h, edge_ray_grd_h, edge_arr_grd_g_h, edge_long_g_h,
# edge_arr_pte_g_h, edge_ray_pte_h, edge_arr_pte_d_h, edge_long_d_h

# modele de blocs :
mod_start = grille_cyl_grd.getEdgeJ(1, 4, 1)
mod_first = mod_start.getVertex(0)
mod_line = [grille_cyl_grd.getEdgeJ(1, j, 1) for j in [3, 2, 1]]  # a revoir ??
#mod_line.extend(model_biell_fin.getEdge????)  # @todo edge haut gauche

mod_line.extend([grille_cyl_pte.getEdgeJ(1, j, 1) for j in [1, 0, 5, 4]])
#mod_line.extend(model_biell_fin.getEdgeJ????)  # edge haut droit

# geometrie :
geo_start = all_edges_bielle[dic_edge_names["edge_arr_grd_d_h"]]
par_start = 0.0
e_r_g_h = all_edges_bielle[dic_edge_names["edge_ray_grd_h"]]
# x_h_e_g, y_h_e_g, z_h_e_g :
li_vert.append(geom_vert_grd[dic_vert_names['x_h_e']])
li_vert.append(geom_vert_grd[dic_vert_names['y_h_e']])
li_vert.append(geom_vert_grd[dic_vert_names['z_h_e']])
part_ray_grd_haut = geompy.MakePartition([e_r_g_h],
                                         li_vert,
                                         [], [], geompy.ShapeType["EDGE"], 0, [], 0)
all_edge_part_ray_grd_haut = geompy.SubShapeAllSorted(part_ray_grd_haut, geompy.ShapeType["EDGE"])
print "all_edge_part_ray_grd_haut nb edges = ", len(all_edge_part_ray_grd_haut)
geo_line = all_edge_part_ray_grd_haut[:]
geo_line.append(all_edges_bielle[dic_edge_names["edge_arr_grd_g_h"]])
geo_line.append(all_edges_bielle[dic_edge_names["edge_long_g_h"]])
geo_line.append(all_edges_bielle[dic_edge_names["edge_arr_pte_g_h"]])

e_r_p_h = all_edges_bielle[dic_edge_names["edge_ray_pte_h"]]
# w_h_e_p, v_h_e_p, u_h_e_p :
li_vert.append(geom_vert_pte[dic_vert_names['w_h_e']])
li_vert.append(geom_vert_pte[dic_vert_names['v_h_e']])
li_vert.append(geom_vert_pte[dic_vert_names['u_h_e']])
part_ray_pte_haut = geompy.MakePartition([e_r_p_h],
                                         li_vert,
                                         [], [], geompy.ShapeType["EDGE"], 0, [], 0)
all_edge_part_ray_pte_haut = geompy.SubShapeAllSorted(part_ray_pte_haut, geompy.ShapeType["EDGE"])

print "all_edge_part_ray_pte_haut nb edges = ", len(all_edge_part_ray_pte_haut)
# @todo a revoir => normalement 2 (en fait 1)

geo_line.extend(all_edge_part_ray_grd_haut[:])
geo_line.append(all_edges_bielle[dic_edge_names["edge_arr_pte_d_h"]])
geo_line.append(all_edges_bielle[dic_edge_names["edge_long_d_h"]])

print "geo_line len = ", len(geo_line)  # normalement 10

# association :
ier = doc.associateClosedLine(mod_first, mod_start, mod_line,
                              geo_start, par_start, geo_line)

# 6. ligne "externe" de la bielle, en bas
# ========================================

# @todo

## #=================================================
## # VERTEX, EDGES, FACES DANS L'ARBRE D'ETUDE SALOME
## #=================================================

# vertices :
for key, value in dic_vert_names.iteritems():
    geompy.addToStudy(geom_vert_grd[value], key + '_g')
    geompy.addToStudy(geom_vert_pte[value], key + '_p')

# edges :
for key, value in dic_edge_names.iteritems():
    geompy.addToStudy(all_edges_bielle[value], key)

#====================================
# CREATION MAILLAGE
#====================================


#=================================================
# Definir les groupes d elements pour le maillage
#=================================================

# On definit 3 groupes de mailles

# groupe d edges (arretes)
edge_grp = doc.addEdgeGroup("Edge_grp")
for i in range(doc.countEdge()):
    edge_grp.addElement(doc.getEdge(i))

# groupe de quads (faces)
quad_grp = doc.addQuadGroup("Quad_grp")
for i in range(doc.countQuad()):
    quad_grp.addElement(doc.getQuad(i))

# groupe d hexas (solids)
hexa_grp = doc.addHexaGroup("Hexa_grp")
for i in range(doc.countHexa()):
    hexa_grp.addElement(doc.getHexa(i))

# groupe de noeuds de vertex pour tout le modele 
vertex_nod_grp = doc.addVertexNodeGroup("Vertex_Nod_Grp")
for i in range(doc.countVertex()):
    vertex_nod_grp.addElement(doc.getVertex(i))

#====================================
# Definir une loi de discretisation
#====================================
# definir une loi: le choix de la loi reste aux utilisateurs
law = doc.addLaw("Uniform", 4)

#n = doc.countLaw()
#print n

# chercher les propagations du modele
for j in range(doc.countPropagation()):
    propa = doc.getPropagation(j)
    propa.setLaw(law)  # appliquer la loi de discretisation sur tout le modele et generer le maillage

#====================================
# G�n�rer des maillages
#====================================

print  " --- MAILLAGE HEXAHEDRIQUE --- "
mesh_hexas = hexablock.mesh("Bielle:hexas", doc)

print "Nombre d hexaedres:", mesh_hexas.NbHexas()
print "Nombre de quadrangles:", mesh_hexas.NbQuadrangles()
print "Nombre de segments:", mesh_hexas.NbEdges()
print "Nombre de noeuds:", mesh_hexas.NbNodes()

file_name = os.path.join(os.environ['TMP'], 'bielle.vtk')
doc.saveVtk(file_name)
