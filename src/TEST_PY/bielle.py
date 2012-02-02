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
import geompy
import hexablock
import math

STEP_PATH = os.path.expandvars("$HEXABLOCK_ROOT_DIR/bin/salome/crank.stp")


#=============================
# CREATION DOCUMENT
#=============================

doc = hexablock.addDocument("bielle")

#=============================
# MODEL CREATION
#=============================

# For the connecting rod, two cylindrical grids have to be build and
# the quadrangles have to be prismed between these wo grids

#=============================
# PARAMETRES
#=============================

#R = 40.0
R = 0.095168291790720005

r_pte = R
r_pte_t = R/2.0

xpetit = 0.0
xgrand = 1.35739 + 0.1595
longueur = (xgrand - xpetit)/2.0
hauteur = 0.019999999553*2

dr_pte = R
da_pte = 360
dl_pte = hauteur

nr_pte = 1
na_pte = 6
nl_pte = 1

# TESTS : sauvegarde
f_mod_apres = open(os.path.join(os.environ['TMP'],
                                "bielle_model_apres.txt"), 'w')

# end TESTS


# @todo JPL le 08/07/2011 :
# refaire le mod�le de blocs avec des pentagones pour les grilles
# cylindriques, et 1 hexaedre reliant ces 2 grilles.

#=============================
# Vectors Creation 
#=============================

dx = doc.addVector(longueur, 0, 0)
dy = doc.addVector(0, longueur, 0)
dz = doc.addVector(0, 0, longueur)

#=================================================
# Creation of cylindrical grid centers
#=================================================

c_pte = doc.addVertex(xpetit, 0, 0)
c_grd = doc.addVertex(2*longueur, 0, 0)
dx_prime = doc.addVectorVertices(c_pte, c_grd)

#=================================================
# small cylindrical grid creation
#=================================================

grille_cyl_pte = doc.makeCylindrical(c_pte, dx, dz, dr_pte, da_pte, dl_pte, nr_pte, na_pte, nl_pte, False)

#=================================
#  Small cylindrical grid creation
#=================================

grille_cyl_grd = doc.makeTranslation(grille_cyl_pte, dx_prime)

# TEST :
file_name = os.path.join(os.environ['TMP'], 'bielle0.vtk')
doc.saveVtk(file_name)


#==================================
# Joining the two cylindrical grids
#==================================

mod_x1 = grille_cyl_pte.getVertexIJK(1, 0, 1)
mod_x2 = grille_cyl_pte.getVertexIJK(1, 1, 0)
mod_x3 = grille_cyl_pte.getVertexIJK(1, 5, 0)
mod_x4 = grille_cyl_pte.getVertexIJK(1, 0, 0)
quad_11 = doc.findQuad(mod_x1, mod_x2)
quad_12 = doc.findQuad(mod_x1, mod_x3)

mod_y1 = grille_cyl_grd.getVertexIJK(1, 3, 1)
mod_y2 = grille_cyl_grd.getVertexIJK(1, 2, 0)
mod_y3 = grille_cyl_grd.getVertexIJK(1, 4, 0)
mod_y4 = grille_cyl_grd.getVertexIJK(1, 3, 0)

quad_21 = doc.findQuad(mod_y1, mod_y2)
quad_22 = doc.findQuad(mod_y1, mod_y3)

model_biell_fin = doc.joinQuads([quad_11, quad_12], quad_21, mod_x1, mod_y1, mod_x4, mod_y4, 1)

# TEST :
file_name = os.path.join(os.environ['TMP'], 'bielle1.vtk')
doc.saveVtk(file_name)


#=======================
# CREATION ASSOCIATION
#=======================

# JPL le 08/07/2011 :
# la premi�re solution :
# 1. 4 associations par ligne fermee pour les trous
# 2. 2 associations par ligne fermee pour les contours
# => pose probl�me car les points associ�s par cette m�thode sont
# uniform�ment r�partis sur le contour => �a d�pend de la longueur
# entre les 2 grilles cylindriques.
# REM : a retester apr�s la correction d'Alain pour voir ce que �a
# donne

# deuxi�me solution :
# 1. 4 associations par ligne fermee pour les trous
# => pour chaque association, 6 edges du modele <-> 1 ligne de la geometrie
# 2. 4 associations par lignes ouvertes pour les contours externes des
# grilles cylindriques
# => pour chaque association, 4 edges du modele <-> 3 lignes de la geometrie
# 3. 4 associations de points restants (mod_x1, mod_x4, mod_y1, mod_y4)

# => cette solution pose probl�me (� cause de l'association par ligne
# ouverte)

bielle_geom = geompy.ImportFile(STEP_PATH, "STEP")
doc.setShape(bielle_geom)
geompy.addToStudy(bielle_geom, "bielle_geom")
all_edges_bielle = geompy.SubShapeAllSorted(bielle_geom, geompy.ShapeType["EDGE"])



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


all_faces_bielle = geompy.SubShapeAllSorted(bielle_geom, geompy.ShapeType["FACE"])
# dictionnaire des faces de la geometrie :
# key = nom, value = indice dans all_faces_bielle
dic_face_names = {"face_ray_pte": 0, "face_trou_pte": 1, "face_pte_g": 2,
                  "face_pte_d": 3, "face_long_g": 4, "face_long_d": 5,
                  "face_bas": 6, "face_haut": 7, "face_grd_g": 8,
                  "face_grd_d": 9, "face_trou_grd": 10,
                  "face_ray_grd": 11
                  }


# la clef correspond a la geometrie, la valeur a l'indice en z dans le
# modele de bloc (grille cylindrique)
# NOTE : les dictionnaires ordonn�s ne sont pas encore introduits dans
# la version 2.6.6 de python (Salome 6.2). On ne connait donc pas
# l'ordre de bouclage (haut/bas ou bas/haut) mais �a n'a pas
# d'importance :
dico_haut_bas = {"h": 1, "b": 0}

# 1. lignes internes (trou) haut/bas du petit cylindre
# ====================================================
for z in dico_haut_bas.iteritems():

    # REM : la direction de la ligne geometrique est dans le sens anti-trigonometrique
    # => on parcourt le modele en sens inverse

##     # modele de blocs :
##     mod_start = grille_cyl_pte.getEdgeJ(0, 5, z[1])
##     mod_first = mod_start.getVertex(1)
##     # table des edges :
##     mod_line = [grille_cyl_pte.getEdgeJ(0, j, z[1]) for j in range(4, -1, -1)]

    # modele de blocs :
    mod_start = grille_cyl_pte.getEdgeJ(0, 0, z[1])
    mod_first = mod_start.getVertex(0)
    # table des edges :
    mod_line = [grille_cyl_pte.getEdgeJ(0, j, z[1]) for j in range(1, 6)]


    # geometrie : 1 seule ligne
    edge_hole_in = all_edges_bielle[dic_edge_names["edge_trou_pte_"+z[0]]]
    geo_start = edge_hole_in
    par_start = 0.0
    geo_line  = []

    # association :
    ier = doc.associateClosedLine(mod_first, mod_start, mod_line,
                                  geo_start, par_start, geo_line)


# 2. lignes internes (trou) haut/bas du grand cylindre
# =====================================================
for z in dico_haut_bas.iteritems():

    # REM : la direction de la ligne geometrique est dans le sens anti-trigonometrique
    # => on parcourt le modele en sens inverse

##     # modele de blocs :
##     mod_start = grille_cyl_grd.getEdgeJ(0, 5, z[1])
##     mod_first = mod_start.getVertex(1)
##     # table des edges :
##     mod_line = [grille_cyl_grd.getEdgeJ(0, j, z[1]) for j in range(4, -1, -1)]

    mod_start = grille_cyl_grd.getEdgeJ(0, 0, z[1])
    mod_first = mod_start.getVertex(1)
    # table des edges :
    mod_line = [grille_cyl_grd.getEdgeJ(0, j, z[1]) for j in range(1, 6)]

    # geometrie : 1 seule ligne
    edge_hole_in = all_edges_bielle[dic_edge_names["edge_trou_grd_"+z[0]]]
    geo_start = edge_hole_in
    par_start = 0.0
    geo_line  = []

    # association :
    ier = doc.associateClosedLine(mod_first, mod_start, mod_line,
                                  geo_start, par_start, geo_line)


# 3. lignes externes haut/bas du petit cylindre
# =============================================
for z in dico_haut_bas.iteritems():

    # JPL le 08/07/2011 : on utilise ici l'association par ligne
    # ouverte. Avec le nouvelle version, les points sont r�partis de
    # mani�re �quidistance sur la ligne geometrique form�e par la
    # concat�nation des lignes fournies en entr�e.

    # modele de blocs :
    mod_start = grille_cyl_pte.getEdgeJ(1, 1, z[1])
    # table des edges :
    mod_line = [grille_cyl_pte.getEdgeJ(1, j, z[1]) for j in [2, 3, 4]]

    # geometrie :
    # les edges de la geometrie correspondant sont, dans l'ordre (par
    # exemple pour le haut) :
    # edge_arr_pte_d_h, edge_ray_pte_h, edge_arr_pte_g_h
    geo_start = all_edges_bielle[dic_edge_names["edge_arr_pte_d_"+z[0]]]

    geo_line  = []
    geo_line.append(all_edges_bielle[dic_edge_names["edge_ray_pte_"+z[0]]])
    geo_line.append(all_edges_bielle[dic_edge_names["edge_arr_pte_g_"+z[0]]])

    # association :
    # la premi�re est la derni�re ligne sont orient�es "dans le
    # mauvais sens" => on fournit cette info :
    par_start = 0.0
    par_end = 1.0
    ier = doc.associateOpenedLine(mod_start, mod_line,
                                  geo_start, par_start, geo_line, par_end)


## # 4. lignes externes haut/bas du grand cylindre
## # =============================================
for z in dico_haut_bas.iteritems():

    # JPL le 08/07/2011 : on utilise ici l'association par ligne
    # ouverte. Avec le nouvelle version, les points sont r�partis de
    # mani�re �quidistance sur la ligne geometrique form�e par la
    # concat�nation des lignes fournies en entr�e.

    # modele de blocs :
    mod_start = grille_cyl_grd.getEdgeJ(1, 4, z[1])
    # table des edges :
    mod_line = [grille_cyl_grd.getEdgeJ(1, j, z[1]) for j in [5, 0, 1]]

    # geometrie :
    # les edges de la geometrie correspondant sont, dans l'ordre (par
    # exemple pour le haut) :
    # edge_arr_grd_g_h, edge_ray_grd_h, edge_arr_grd_d_h
    geo_start = all_edges_bielle[dic_edge_names["edge_arr_grd_g_"+z[0]]]

    geo_line  = []
    geo_line.append(all_edges_bielle[dic_edge_names["edge_ray_grd_"+z[0]]])
    geo_line.append(all_edges_bielle[dic_edge_names["edge_arr_grd_d_"+z[0]]])

    # association :
    # la premi�re est la derni�re ligne sont orient�es "dans le
    # mauvais sens" => on fournit cette info :
    par_start = 0.0
    par_end = 1.0
    ier = doc.associateOpenedLine(mod_start, mod_line,
                                  geo_start, par_start, geo_line, par_end)

# JPL le 26/07/2011 :
# l'association des edges n'est pas necessaire (implicite)

# 6. association des 4 points restants (x1, x4, y1, y4) :
# =======================================================

# NB:
# h = top (haut)
# b = bottom (bas)
# g = big (grand)
# p = small (petit)
# t = hole (trou)

# JPL le 08/07/2011 :
# seuls 4 points de la geometrie sont n�cessaires (pour l'association
# vertex/point : pour le reste on utilise l'association par lignes) :
# dictionnaire geom_vertices
# il s'agit de points des grilles cylindriques, � l'int�rieur
# de la bielle, ayant servis au joinQuads (ie: mod_x1, mod_x4, mod_y1, mod_y4)


## pt_a = geompy.MakeVertex(0, 0, hauteur/2.)
## face_haut = geompy.GetFaceNearPoint(bielle_geom, pt_a)
## pt_b = geompy.MakeVertex(0, 0, -hauteur/2.)
## face_bas = geompy.GetFaceNearPoint(bielle_geom, pt_b)

face_haut = all_faces_bielle[dic_face_names["face_haut"]]

edge_haut_droite = geompy.GetEdgesByLength(face_haut, 0.136, 0.137)
edge_haut_gauche = geompy.GetEdgesByLength(face_haut, 0.131, 0.132)

# 1. grand cylindre :
y_h_g = geompy.MakeVertexOnSurface(face_haut, 1, 0.5)
u_h_g = geompy.MakeVertexOnCurve(edge_haut_droite, 1)
w_h_g = geompy.MakeVertexOnCurve(edge_haut_gauche, 0)
edge_v_grd = geompy.MakeLineTwoPnt(u_h_g, w_h_g)

geo_y1 = geompy.MakeVertexOnCurve(edge_v_grd, 0.5)
geo_y4 = geompy.MakeVertexWithRef(geo_y1, 0.0, 0.0, -hauteur)

# vertex cote grande grille cylindrique :
mod_y1.setAssociation(geo_y1)
mod_y4.setAssociation(geo_y4)

# 2. petit cylindre :
# REM : le modele grand cylindre a ete cree par translation / au petit
# cylindre.
v_h_p = geompy.MakeVertexOnSurface(face_haut, 0, 0.5)
x_h_p = geompy.MakeVertexOnCurve(edge_haut_droite, 0)
z_h_p = geompy.MakeVertexOnCurve(edge_haut_gauche, 1)
edge_v_pte = geompy.MakeLineTwoPnt(x_h_p, z_h_p)

geo_x1 = geompy.MakeVertexOnCurve(edge_v_pte, 0.5)
geo_x4 = geompy.MakeVertexWithRef(geo_x1, 0.0, 0.0, -hauteur)

# vertex cote petite grille cylindrique :
mod_x1.setAssociation(geo_x1)
mod_x4.setAssociation(geo_x4)

# 7. association des faces :
# REM : l'association des faces internes ne semble pas necessaire (cylindres)
# pas d'association des face_ray_XXX : pas necessaire ? De toute
# fa�on, on a pour chaque 2 quads du mod�le...
# les associations de face_haut, face_bas, face_long_g et face_long_d
# ne sont pas necessaires ?

# a decommenter donc si necessaire :
## quad1 = grille_cyl_pte.getQuadJK(1, 1, 0)
## quad1.addAssociation(all_faces_bielle[dic_face_names["face_pte_d"]])
## quad2 = grille_cyl_pte.getQuadJK(1, 4, 0)
## quad2.addAssociation(all_faces_bielle[dic_face_names["face_pte_g"]])
## quad3 = grille_cyl_grd.getQuadJK(1, 1, 0)
## quad3.addAssociation(all_faces_bielle[dic_face_names["face_grd_d"]])
## quad4 = grille_cyl_grd.getQuadJK(1, 4, 0)
## quad4.addAssociation(all_faces_bielle[dic_face_names["face_grd_g"]])


#############################################################################################
#############################################################################################
# TEST :

file_name = os.path.join(os.environ['TMP'], 'bielle2.vtk')
doc.saveVtk(file_name)

# affichage des points des 2 grilles cylindriques avec les
# nouvelles coordonn�es (apres association)

for k in range(2):
    # petite grille cylindrique
    i = 0
    for j in range(6):
        vertex = grille_cyl_pte.getVertexIJK(i, j, k)
        value = " ".join(["i =", str(i), "j =", str(j),
                          str(vertex.getX()), str(vertex.getY()), str(vertex.getZ()), '\n'])
        f_mod_apres.write(str(value))
        pass

    f_mod_apres.write("stop\n")

    i = 1
    # on n'inclut pas les vertex associ�s individuellement (qui ne
    # changent pas de coordonn�es...) : => j = 1 -> 5
    for j in range(1, 6):
        vertex = grille_cyl_pte.getVertexIJK(i, j, k)
        value = " ".join(["i =", str(i), "j =", str(j),
                          str(vertex.getX()), str(vertex.getY()), str(vertex.getZ()), '\n'])
        f_mod_apres.write(str(value))
        pass

    f_mod_apres.write("stop\n")

    # grande grille cylindrique
    i = 0
    for j in range(6):
        vertex = grille_cyl_grd.getVertexIJK(i, j, k)
        value = " ".join(["i =", str(i), "j =", str(j),
                          str(vertex.getX()), str(vertex.getY()), str(vertex.getZ()), '\n'])
        f_mod_apres.write(str(value))
        pass

    f_mod_apres.write("stop\n")

    i = 1
    # on n'inclut pas les vertex associ�s individuellement (qui ne
    # changent pas de coordonn�es...) (j = 3)
    # on ecrit les points dans le meme ordre que l'association :
    for j in [4, 5, 0, 1, 2]:
        vertex = grille_cyl_grd.getVertexIJK(i, j, k)
        value = " ".join(["i =", str(i), "j =", str(j),
                          str(vertex.getX()), str(vertex.getY()), str(vertex.getZ()), '\n'])
        f_mod_apres.write(str(value))
        pass
    # end TEST

    f_mod_apres.write("stop\n")

#############################################################################################
#############################################################################################    


## #=================================================
## # VERTEX, EDGES, FACES DANS L'ARBRE D'ETUDE SALOME
## #=================================================

# vertices :
geompy.addToStudy(geo_x1, 'vertex_x1')
geompy.addToStudy(geo_x4, 'vertex_x4')
geompy.addToStudy(geo_y1, 'vertex_y1')
geompy.addToStudy(geo_y4, 'vertex_y4')

# edges :
for key, value in dic_edge_names.iteritems():
    geompy.addToStudy(all_edges_bielle[value], key)

# faces (juste pour les tests) :
for i, face in enumerate(all_faces_bielle):
    geompy.addToStudy(face, "face_"+str(i))

#====================================
# CREATION MAILLAGE
#====================================


#=================================================
# Definir les groupes d elements pour le maillage
#=================================================

# On definit 3 groupes de mailles

# JPL (le 09/05/2011) :
# @todo a revoir : apres correction des bugs "countXXX()" dans le moteur

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

# TEST : pour avoir le mod�le de blocs :
## law = doc.addLaw("Uniform", 0)

# chercher les propagations du modele
for j in range(doc.countPropagation()):
    propa = doc.getPropagation(j)
    propa.setLaw(law)  # appliquer la loi de discretisation sur tout le modele et generer le maillage

#====================================
# G�n�rer des maillages
#====================================

print  " --- MAILLAGE HEXAHEDRIQUE --- "
mesh_hexas = hexablock.mesh(doc)

print "Nombre d hexaedres:", mesh_hexas.NbHexas()
print "Nombre de quadrangles:", mesh_hexas.NbQuadrangles()
print "Nombre de segments:", mesh_hexas.NbEdges()
print "Nombre de noeuds:", mesh_hexas.NbNodes()

# TESTS :
f_mod_apres.close()
