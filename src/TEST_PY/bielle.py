# -*- coding: latin-1 -*-

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

db  = False
doc = hexablock.addDocument()

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

## r_grd = R
## r_grd_t = R/2.0

r_pte = R
r_pte_t = R/2.0

#longueur = 200.0
## xpetit = -0.1595
## xgrand = 1.35739
xpetit = 0.0
xgrand = 1.35739 + 0.1595
longueur = (xgrand - xpetit)/2.0
hauteur = 0.019999999553*2
## hauteur = 40.

# Taille du grand cylindre
## dr_grd = R
## da_grd = 360
## dl_grd = hauteur

## nr_grd = 1
## na_grd = 6
## nl_grd = 1

dr_pte = R
da_pte = 360
dl_pte = hauteur

nr_pte = 1
na_pte = 6
nl_pte = 1


# TESTS : sauvegarde
f_mod_avant = open(os.path.join(os.environ['TMP'],
                                "bielle_model_avant.txt"), 'w')
f_geom = open(os.path.join(os.environ['TMP'],
                           "bielle_geom.txt"), 'w')
f_mod_apres = open(os.path.join(os.environ['TMP'],
                                "bielle_model_apres.txt"), 'w')

# end TESTS

#=============================
# Vectors Creation 
#=============================

dx = doc.addVector(longueur, 0, 0)
dy = doc.addVector(0, longueur, 0)
dz = doc.addVector(0, 0, longueur)

#=================================================
# Creation of cylindrical grid centers
#=================================================

## c_grd = doc.addVertex(0, 0, 0)
## c_pte = doc.addVertex(2*longueur, 0, 0)
## dx_prime = doc.addVectorVertices(c_grd, c_pte)

c_pte = doc.addVertex(xpetit, 0, 0)
c_grd = doc.addVertex(2*longueur, 0, 0)
dx_prime = doc.addVectorVertices(c_pte, c_grd)


#=================================================
# Big cylindrical grid creation
#=================================================

# @todo reprendre ici : inverser petite grille et grande grille (pour
# retrouver l'axe des x dans le meme sens que la geometrie)
# creer d'abord la petite grille cylindrique

grille_cyl_pte = doc.makeCylindrical(c_pte, dx, dz, dr_pte, da_pte, dl_pte, nr_pte, na_pte, nl_pte, False)

# We obtain a liste containing 6 equal hexaedra
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

# TESTS :
if db :
   print "x1 : ", x1.getX(), " ", x1.getY(), " ", x1.getZ()
   print "x2 : ", x2.getX(), " ", x2.getY(), " ", x2.getZ()
   print "x3 : ", x3.getX(), " ", x3.getY(), " ", x3.getZ()
   print "x4 : ", x4.getX(), " ", x4.getY(), " ", x4.getZ()

   x1_t = doc.findVertex(R, 0, hauteur)
   x2_t = doc.findVertex(alpha_x/2.0, alpha_y/2.0, hauteur)
   print "x1_t <-> mod_first petit cylindre trou haut"
   print "x1_t : ", x1_t.getX(), " ", x1_t.getY(), " ", x1_t.getZ()
   print "x2_t : ", x2_t.getX(), " ", x2_t.getY(), " ", x2_t.getZ()

# end TESTS

quad_11 = doc.findQuad(x1, x2)
quad_12 = doc.findQuad(x1, x3)

#=================================
#  Small cylindrical grid creation
#=================================

grille_cyl_grd = doc.makeTranslation(grille_cyl_pte, dx_prime)

# We obtain a liste containing 6 equal hexaedra
y1 = doc.findVertex(2*longueur - 2*R, 0 , hauteur)
y2 = doc.findVertex(2*longueur - alpha_x, alpha_y, 0)
y3 = doc.findVertex(2*longueur - alpha_x, -alpha_y, 0)
y4 = doc.findVertex(2*longueur - 2*R, 0, 0)

quad_21 = doc.findQuad(y1, y2)
quad_22 = doc.findQuad(y1, y3)

#==================================
# Joining the two cylindrical grids
#==================================
model_biell_fin = doc.joinQuads([quad_11, quad_12], quad_21, x1, y1, x4, y4, 1)

## # temporaire : sauvegarde du modele de blocs :
##  file_name = os.path.join(os.environ['TMP'], 'bielle.vtk')
##  model_biell_fin.saveVtk(file_name)
##  file_name = os.path.join(os.environ['TMP'], 'grille_cyl_pte.vtk')
##  grille_cyl_pte.saveVtk(file_name)
##  file_name = os.path.join(os.environ['TMP'], 'grille_cyl_grd.vtk')
##  grille_cyl_grd.saveVtk(file_name)
## # fin temporaire

### for j in [4, 5, 0, 1  ] :
    ### grille_cyl_grd.getEdgeJ (1, j, 1).setScalar (7)
##  grille_cyl_grd.getVertexIJK (1, 2, 1).setScalar (8)
##  grille_cyl_grd.getVertexIJK (1, 4, 1).setScalar (4)

#### for j in [1, 2, 3, 4] :
    #### grille_cyl_pte.getEdgeJ(1, j, 1).setScalar (7)
##  grille_cyl_pte.getVertexIJK (1, 1, 1).setScalar (8)
##  grille_cyl_pte.getVertexIJK (1, 5, 1).setScalar (4)

if db : 
   doc.saveVtk ('bielle.vtk')
   doc.dump  ()

#=======================================================
# Recover vertices of the hexa model for the association
#=======================================================

# NB:
# h = top (haut)
# b = bottom (bas)
# g = big (grand)
# p = small (petit)
# t = hole (trou)

bielle_geom = geompy.ImportFile(STEP_PATH, "STEP")

geompy.addToStudy(bielle_geom, "bielle_geom")

pt_a = geompy.MakeVertex(0, 0, hauteur/2.)
face_haut = geompy.GetFaceNearPoint(bielle_geom, pt_a)

pt_b = geompy.MakeVertex(0, 0, -hauteur/2.)
face_bas = geompy.GetFaceNearPoint(bielle_geom, pt_b)

edge_haut_droite = geompy.GetEdgesByLength(face_haut, 0.136, 0.137)
edge_haut_gauche = geompy.GetEdgesByLength(face_haut, 0.131, 0.132)

# Paramètres pour :
# - récupération des points.
# - et associations.

u_1 = 1.0
u_3 = 0.0

# dictionnaire des vertices de la géométrie (grande et petite grilles) :
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
# la création des vertex de la geometrie se fait obligatoirement
# dans cet ordre :
geom_vert_grd.append(geompy.MakeVertexOnSurface(face_haut, 1, 0.5))  # y_h_e_g
edge_haut_grd = geompy.GetEdgeNearPoint(bielle_geom, geom_vert_grd[0])
geom_vert_grd.append(geompy.MakeVertexOnCurve(edge_haut_grd, u_3))  # x_h_e_g
geom_vert_grd.append(geompy.MakeVertexOnCurve(edge_haut_grd, u_1))  # z_h_e_g
geom_vert_grd.insert(2, geompy.MakeVertexOnCurve(edge_haut_droite, 1))  # u_h_e_g
geom_vert_grd.insert(3, geompy.MakeVertexOnCurve(edge_haut_gauche, 0))  # w_h_e_g
edge_v_grd = geompy.MakeLineTwoPnt(geom_vert_grd[2], geom_vert_grd[3])
geom_vert_grd.insert(3, geompy.MakeVertexOnCurve(edge_v_grd, 0.5))  # v_h_e_g
# les vertex sont rangés dans cet ordre :
# y_h_e_g, x_h_e_g, u_h_e_g, v_h_e_g, w_h_e_g, z_h_e_g

# 1.1.2 sommets internes (trou) :
edge_haut_grd_trou = geompy.GetEdgesByLength(face_haut, 0.147, 0.148)
# y_h_t_g, x_h_t_g, u_h_t_g, v_h_t_g, w_h_t_g, z_h_t_g
for val in [0, 0.8, 0.6, 0.5, 0.4, 0.2]:
    geom_vert_grd.append(geompy.MakeVertexOnCurve(edge_haut_grd_trou, val))

# 1.2 face du bas :
# pour tous les vertex de la face du bas, on les crée par référence à ceux déjà crées (dans le meme ordre :
# sommets externes puis internes) :
geom_vert_grd_bas = [geompy.MakeVertexWithRef(vertex, 0.0, 0.0, -hauteur) for vertex in geom_vert_grd]
geom_vert_grd.extend(geom_vert_grd_bas)

# pour l'utilisation :
# geom_vert_grd[dic_vert_names["y_h_t"]] # par exemple

# 2. petit cylindre :
# REM : le modele petit cylindre a ete cree par translation / au grand
# cylindre. Les vertices de la geometrie sont donc ete crées de manière
# similaire (# d'une symmetrie / miroir)
geom_vert_pte = []

# 2.1 face du haut :
# 2.1.1 sommets externes :

# la création des vertex de la geometrie se fait obligatoirement
# dans cet ordre :
geom_vert_pte.append(geompy.MakeVertexOnSurface(face_haut, 0, 0.5))  # v_h_e_p
edge_haut_pte = geompy.GetEdgeNearPoint(bielle_geom, geom_vert_pte[0])
geom_vert_pte.append(geompy.MakeVertexOnCurve(edge_haut_pte, u_3))  # w_h_e_p
geom_vert_pte.insert(0, geompy.MakeVertexOnCurve(edge_haut_pte, u_1))  # u_h_e_p
geom_vert_pte.insert(0, geompy.MakeVertexOnCurve(edge_haut_droite, 0))  # x_h_e_p
geom_vert_pte.append(geompy.MakeVertexOnCurve(edge_haut_gauche, 1))  # z_h_e_p
edge_v_pte = geompy.MakeLineTwoPnt(geom_vert_pte[0], geom_vert_pte[4])
geom_vert_pte.insert(0, geompy.MakeVertexOnCurve(edge_v_pte, 0.5))  # y_h_e_p
# les vertex sont rangés dans cet ordre :
# y_h_e_p, x_h_e_p, u_h_e_p, v_h_e_p, w_h_e_p, z_h_e_p


# 2.1.2 sommets internes (trou) :
edge_haut_pte_trou = geompy.GetEdgesByLength(face_haut, 0.094, 0.095)
# y_h_t_p, x_h_t_p, u_h_t_p, v_h_t_p, w_h_t_p, z_h_t_p
for val in [0, 0.9, 0.65, 0.5, 0.35, 0.1]:
    geom_vert_pte.append(geompy.MakeVertexOnCurve(edge_haut_pte_trou, val))

# 2.2 face du bas :
# pour tous les vertex de la face du bas, on les crée par référence à ceux déjà crées (dans le meme ordre :
# sommets externes puis internes) :
geom_vert_pte_bas = [geompy.MakeVertexWithRef(vertex, 0.0, 0.0, -hauteur) for vertex in geom_vert_pte]
geom_vert_pte.extend(geom_vert_pte_bas)


#=======================
# CREATION ASSOCIATION
#=======================

# JPL (le 29/04/2011)
# pour l'association, il suffit de faire 6 appels à la méthode
# associateClosedLine() :
# 1 pour la ligne du trou haut du grand cylindre
# 1 pour la ligne du trou bas du grand cylindre
# 1 pour la ligne du trou haut du petit cylindre
# 1 pour la ligne du trou bas du petit cylindre
# 1 pour la ligne "externe" de la bielle, en haut
# 1 pour la ligne "externe" de la bielle, en bas

all_edges_bielle = geompy.SubShapeAllSorted(bielle_geom, geompy.ShapeType["EDGE"])

# dictionnaire des edges de la géométrie :
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

# 1. ligne du trou haut du petit cylindre
# =======================================

# modele de blocs :
mod_start = grille_cyl_pte.getEdgeJ(0, 0, 1)
mod_first = mod_start.getVertex(1)
# table des edges :
mod_line = [grille_cyl_pte.getEdgeJ(0, j, 1) for j in range(1, 6)]

# geometrie :
e_t_p_h = all_edges_bielle[dic_edge_names["edge_trou_pte_h"]]
li_vert = geom_vert_pte[dic_vert_names['x_h_t']:dic_vert_names['z_h_t']+1]
li_vert.reverse()  # z_h_t_p, w_h_t_p, v_h_t_p, u_h_t_p, x_h_t_p
part_trou_pte_haut = geompy.MakePartition([e_t_p_h],
                                          li_vert,
                                          [], [], geompy.ShapeType["EDGE"], 0, [], 0)
all_edge_part_trou_pte_haut = geompy.SubShapeAllSorted(part_trou_pte_haut, geompy.ShapeType["EDGE"])

geo_start = all_edge_part_trou_pte_haut[5]  # [y_h_t_p; x_h_t_p]
par_start = 0.0
geo_line  = [all_edge_part_trou_pte_haut[i] for i in [3, 1, 0, 2, 4]]

# TESTS :
geompy.addToStudy(geo_start, "geo_start")
for num, line in enumerate(geo_line):
    geompy.addToStudy(line, "line_" + str(num))


for j in range(6):
    edge = grille_cyl_pte.getEdgeJ(0, j, 1)
    vertex = edge.getVertex(0)
    value = " ".join(["mod vertex : j =", str(j), " i =", str(i),
                      str(vertex.getX()), str(vertex.getY()), str(vertex.getZ()), '\n'])
    f_mod_avant.write(str(value))
    pass

# y_h_t_p correspond a mod_first. Ensuite on tourne dans le sens trigonometrique
for name in ['y_h_t', 'x_h_t', 'u_h_t', 'v_h_t', 'w_h_t', 'z_h_t']:
    coords = geompy.PointCoordinates(geom_vert_pte[dic_vert_names[name]])
    value = " ".join([name, str(coords[0]), str(coords[1]), str(coords[2]), '\n'])
    f_geom.write(str(value))
    pass
# end TESTS

## association :
ier = doc.associateClosedLine(mod_first, mod_start, mod_line,
                              geo_start, par_start, geo_line)

# TESTS :
for j in range(6):
    edge = grille_cyl_pte.getEdgeJ(0, j, 1)
    vertex = edge.getVertex(0)
    value = " ".join(["mod vertex : j =", str(j), " i =", str(i),
                      str(vertex.getX()), str(vertex.getY()), str(vertex.getZ()), '\n'])
    f_mod_apres.write(str(value))
    pass
# end TESTS


# 2. ligne du trou bas du petit cylindre
# =======================================

# modele de blocs :
mod_start = grille_cyl_pte.getEdgeJ(0, 0, 0)
mod_first = mod_start.getVertex(1)
del(mod_line[:])
# table des edges :
mod_line = [grille_cyl_pte.getEdgeJ(0, j, 0) for j in range(1, 6)]

# geometrie :
e_t_p_b = all_edges_bielle[dic_edge_names["edge_trou_pte_b"]]
del(li_vert[:])
li_vert = geom_vert_pte[dic_vert_names['x_b_t']:dic_vert_names['z_b_t']+1]
li_vert.reverse()  # z_b_t_p, w_b_t_p, v_b_t_p, u_b_t_p, x_b_t_p
part_trou_pte_bas = geompy.MakePartition([e_t_p_b],
                                         li_vert,
                                         [], [], geompy.ShapeType["EDGE"], 0, [], 0)
all_edge_part_trou_pte_bas = geompy.SubShapeAllSorted(part_trou_pte_bas, geompy.ShapeType["EDGE"])

geo_start = all_edge_part_trou_pte_bas[5]
par_start = 0.0
del(geo_line[:])
geo_line = [all_edge_part_trou_pte_bas[i] for i in [3, 1, 0, 2, 4]]

# TESTS :
geompy.addToStudy(geo_start, "geo_start")
for num, line in enumerate(geo_line):
    geompy.addToStudy(line, "line_" + str(num))


for j in range(6):
    edge = grille_cyl_pte.getEdgeJ(0, j, 0)
    vertex = edge.getVertex(0)
    value = " ".join(["mod vertex : j =", str(j), " i =", str(i),
                      str(vertex.getX()), str(vertex.getY()), str(vertex.getZ()), '\n'])
    f_mod_avant.write(str(value))
    pass

# y_b_t_p correspond a mod_first. Ensuite on tourne dans le sens trigonometrique
for name in ['y_b_t', 'x_b_t', 'u_b_t', 'v_b_t', 'w_b_t', 'z_b_t']:
    coords = geompy.PointCoordinates(geom_vert_pte[dic_vert_names[name]])
    value = " ".join([name, str(coords[0]), str(coords[1]), str(coords[2]), '\n'])
    f_geom.write(str(value))
    pass
# end TESTS


# association :
ier = doc.associateClosedLine(mod_first, mod_start, mod_line,
                              geo_start, par_start, geo_line)

# TESTS :
for j in range(6):
    edge = grille_cyl_pte.getEdgeJ(0, j, 0)
    vertex = edge.getVertex(0)
    value = " ".join(["mod vertex : j =", str(j), " i =", str(i),
                      str(vertex.getX()), str(vertex.getY()), str(vertex.getZ()), '\n'])
    f_mod_apres.write(str(value))
    pass
# end TESTS


# 3. ligne du trou haut du grand cylindre
# =======================================

# modele de blocs :
mod_start = grille_cyl_grd.getEdgeJ(0, 0, 1)  # 1er edge
mod_first = mod_start.getVertex(1)  # 1er vertex
# table des edges :
del(mod_line[:])
mod_line = [grille_cyl_grd.getEdgeJ(0, j, 1) for j in range(1, 6)]

# geometrie :
e_t_g_h = all_edges_bielle[dic_edge_names["edge_trou_grd_h"]]
del(li_vert[:])
li_vert = geom_vert_grd[dic_vert_names['x_h_t']:dic_vert_names['z_h_t']+1]
li_vert.reverse()  # z_h_t_g, w_h_t_g, v_h_t_g, u_h_t_g, x_h_t_g
part_trou_grd_haut = geompy.MakePartition([e_t_g_h],
                                          li_vert,
                                          [], [], geompy.ShapeType["EDGE"], 0, [], 0)
all_edge_part_trou_grd_haut = geompy.SubShapeAllSorted(part_trou_grd_haut, geompy.ShapeType["EDGE"])

geo_start = all_edge_part_trou_grd_haut[5]  # [y_h_t_g; x_h_t_g]
par_start = 0.0  # param 1ere ligne
del(geo_line[:])
geo_line = [all_edge_part_trou_grd_haut[i] for i in [3, 1, 0, 2, 4]]


# TESTS :
geompy.addToStudy(geo_start, "geo_start")
for num, line in enumerate(geo_line):
    geompy.addToStudy(line, "line_" + str(num))


for j in range(6):
    edge = grille_cyl_grd.getEdgeJ(0, j, 1)
    vertex = edge.getVertex(0)
    value = " ".join(["mod vertex : j =", str(j), " i =", str(i),
                      str(vertex.getX()), str(vertex.getY()), str(vertex.getZ()), '\n'])
    f_mod_avant.write(str(value))
    pass

# y_h_t_g correspond a mod_first. Ensuite on tourne dans le sens trigonometrique
for name in ['y_h_t', 'x_h_t', 'u_h_t', 'v_h_t', 'w_h_t', 'z_h_t']:
    coords = geompy.PointCoordinates(geom_vert_grd[dic_vert_names[name]])
    value = " ".join([name, str(coords[0]), str(coords[1]), str(coords[2]), '\n'])
    f_geom.write(str(value))
    pass
# end TESTS

# association :
ier = doc.associateClosedLine(mod_first, mod_start, mod_line,
                              geo_start, par_start, geo_line)

# TESTS :
for j in range(6):
    edge = grille_cyl_grd.getEdgeJ(0, j, 1)
    vertex = edge.getVertex(0)
    value = " ".join(["mod vertex : j =", str(j), " i =", str(i),
                      str(vertex.getX()), str(vertex.getY()), str(vertex.getZ()), '\n'])
    f_mod_apres.write(str(value))
    pass
# end TESTS


# 4. ligne du trou bas du grand cylindre
# =======================================

# modele de blocs :
mod_start = grille_cyl_grd.getEdgeJ(0, 0, 0)
mod_first = mod_start.getVertex(1)
del(mod_line[:])
# table des edges :
mod_line = [grille_cyl_grd.getEdgeJ(0, j, 0) for j in range(1, 6)]

# geometrie :
e_t_g_b = all_edges_bielle[dic_edge_names["edge_trou_grd_b"]]
del(li_vert[:])
li_vert = geom_vert_grd[dic_vert_names['x_b_t']:dic_vert_names['z_b_t']+1]
li_vert.reverse()  # z_b_t_g, w_b_t_g, v_b_t_g, u_b_t_g, x_b_t_g
part_trou_grd_bas = geompy.MakePartition([e_t_g_b],
                                         li_vert,
                                         [], [], geompy.ShapeType["EDGE"], 0, [], 0)
all_edge_part_trou_grd_bas = geompy.SubShapeAllSorted(part_trou_grd_bas, geompy.ShapeType["EDGE"])

geo_start = all_edge_part_trou_grd_bas[5]
par_start = 0.0  # entre 0 et 1 (abscisse curviligne)
del(geo_line[:])
geo_line = [all_edge_part_trou_grd_bas[i] for i in [3, 1, 0, 2, 4]]

# TESTS :
geompy.addToStudy(geo_start, "geo_start")
for num, line in enumerate(geo_line):
    geompy.addToStudy(line, "line_" + str(num))


for j in range(6):
    edge = grille_cyl_grd.getEdgeJ(0, j, 0)
    vertex = edge.getVertex(0)
    value = " ".join(["mod vertex : j =", str(j), " i =", str(i),
                      str(vertex.getX()), str(vertex.getY()), str(vertex.getZ()), '\n'])
    f_mod_avant.write(str(value))
    pass

# y_b_t_p correspond a mod_first. Ensuite on tourne dans le sens trigonometrique
for name in ['y_b_t', 'x_b_t', 'u_b_t', 'v_b_t', 'w_b_t', 'z_b_t']:
    coords = geompy.PointCoordinates(geom_vert_grd[dic_vert_names[name]])
    value = " ".join([name, str(coords[0]), str(coords[1]), str(coords[2]), '\n'])
    f_geom.write(str(value))
    pass
# end TESTS

# association :
ier = doc.associateClosedLine(mod_first, mod_start, mod_line,
                              geo_start, par_start, geo_line)

# TESTS :
for j in range(6):
    edge = grille_cyl_grd.getEdgeJ(0, j, 0)
    vertex = edge.getVertex(0)
    value = " ".join(["mod vertex : j =", str(j), " i =", str(i),
                      str(vertex.getX()), str(vertex.getY()), str(vertex.getZ()), '\n'])
    f_mod_apres.write(str(value))
    pass
# end TESTS


# 5    : Les lignes "externes" de la bielle
# 5. ligne "externe" de la bielle, en haut
# ========================================

mod_ext_start = [ None, None ]
mod_ext_first = [ None, None ] 

mod_ext_line = [[], []]
for k in [0, 1] :
    mod_ext_start [k] = grille_cyl_grd.getEdgeJ (1, 1, k)
    mod_ext_first [k] = mod_ext_start[k].getVertex(1)
    for j in [4, 5, 0] :
        mod_ext_line[k].append (grille_cyl_grd.getEdgeJ (1, j, k))

    for j in [1, 2, 3, 4] :
        mod_ext_line[k].append (grille_cyl_pte.getEdgeJ (1, j, k))

    v1 = grille_cyl_grd.getVertexIJK (1, 2, k)
    v2 = grille_cyl_pte.getVertexIJK (1, 1, k)
    mod_ext_line[k].append (doc.findEdge (v1, v2))

    v1 = grille_cyl_grd.getVertexIJK (1, 4, k)
    v2 = grille_cyl_pte.getVertexIJK (1, 5, k)
    mod_ext_line[k].append (doc.findEdge (v1, v2))


# geometrie :
# les edges de la geometrie a prendre en compte sont :
# (en partant du grand cylindre)
# edge_arr_grd_d_h, edge_ray_grd_h, edge_arr_grd_g_h, edge_long_g_h,
# edge_arr_pte_g_h, edge_ray_pte_h, edge_arr_pte_d_h, edge_long_d_h

geo_start = all_edges_bielle[dic_edge_names["edge_arr_grd_d_h"]]
par_start = 0.0

e_r_g_h = all_edges_bielle[dic_edge_names["edge_ray_grd_h"]]
# x_h_e_g, y_h_e_g, z_h_e_g :
del(li_vert[:])
li_vert.append(geom_vert_grd[dic_vert_names['x_h_e']])
li_vert.append(geom_vert_grd[dic_vert_names['y_h_e']])
li_vert.append(geom_vert_grd[dic_vert_names['z_h_e']])
part_ray_grd_haut = geompy.MakePartition([e_r_g_h],
                                         li_vert,
                                         [], [], geompy.ShapeType["EDGE"], 0, [], 0)
all_edge_part_ray_grd_haut = geompy.SubShapeAllSorted(part_ray_grd_haut, geompy.ShapeType["EDGE"])

del(geo_line[:])
all_edge_part_ray_grd_haut.reverse()
geo_line = all_edge_part_ray_grd_haut[:]
geo_line.append(all_edges_bielle[dic_edge_names["edge_arr_grd_g_h"]])
geo_line.append(all_edges_bielle[dic_edge_names["edge_long_g_h"]])
geo_line.append(all_edges_bielle[dic_edge_names["edge_arr_pte_g_h"]])

e_r_p_h = all_edges_bielle[dic_edge_names["edge_ray_pte_h"]]
# w_h_e_p, v_h_e_p, u_h_e_p :
del(li_vert[:])
li_vert.append(geom_vert_pte[dic_vert_names['w_h_e']])
# JPL (le 09/05/2011) :
# normalement, on devrait pouvoir utiliser ici le vertex v_h_e_p pour
# faire la partition de l'edge, mais ca ne fonctionne pas...
# on cree donc un vertex supplementaire :
## li_vert.append(geom_vert_pte[dic_vert_names['v_h_e']])
v_h_e_p_bis = geompy.MakeVertexOnCurve(e_r_p_h, 0.5)
li_vert.append(v_h_e_p_bis)
li_vert.append(geom_vert_pte[dic_vert_names['u_h_e']])
part_ray_pte_haut = geompy.MakePartition([e_r_p_h],
                                         li_vert,
                                         [], [], geompy.ShapeType["EDGE"], 0, [], 0)
all_edge_part_ray_pte_haut = geompy.SubShapeAllSorted(part_ray_pte_haut, geompy.ShapeType["EDGE"])

## # tmp :
## print "all_edge_part_ray_pte_haut nb edges = ", len(all_edge_part_ray_pte_haut)
## for num, ed in enumerate(all_edge_part_ray_pte_haut):
##     geompy.addToStudy(ed, "all_edge_part_ray_pte_haut" + str(num))
##     pass
## # => l'unique renvoye ici correspond a edge_ray_pte_h (on devrait
## # avoir 2 edges

all_edge_part_ray_pte_haut.reverse()
geo_line.extend(all_edge_part_ray_pte_haut[:])
geo_line.append(all_edges_bielle[dic_edge_names["edge_arr_pte_d_h"]])
geo_line.append(all_edges_bielle[dic_edge_names["edge_long_d_h"]])

# association :
ier = doc.associateClosedLine(mod_ext_first[1], mod_ext_start[1], mod_ext_line[1],
                              geo_start, par_start, geo_line)

# 6. ligne "externe" de la bielle, en bas
# ========================================

# geometrie :
# les edges de la geometrie a prendre en compte sont :
# (en partant du grand cylindre)
# edge_arr_grd_d_h, edge_ray_grd_h, edge_arr_grd_g_h, edge_long_g_h,
# edge_arr_pte_g_h, edge_ray_pte_h, edge_arr_pte_d_h, edge_long_d_h

geo_start = all_edges_bielle[dic_edge_names["edge_arr_grd_d_b"]]
par_start = 0.0

e_r_g_b = all_edges_bielle[dic_edge_names["edge_ray_grd_b"]]
# x_b_e_g, y_b_e_g, z_b_e_g :
del(li_vert[:])
li_vert.append(geom_vert_grd[dic_vert_names['x_b_e']])
li_vert.append(geom_vert_grd[dic_vert_names['y_b_e']])
li_vert.append(geom_vert_grd[dic_vert_names['z_b_e']])
part_ray_grd_bas = geompy.MakePartition([e_r_g_b],
                                         li_vert,
                                         [], [], geompy.ShapeType["EDGE"], 0, [], 0)
all_edge_part_ray_grd_bas = geompy.SubShapeAllSorted(part_ray_grd_bas, geompy.ShapeType["EDGE"])

del(geo_line[:])
all_edge_part_ray_grd_bas.reverse()
geo_line = all_edge_part_ray_grd_bas[:]
geo_line.append(all_edges_bielle[dic_edge_names["edge_arr_grd_g_b"]])
geo_line.append(all_edges_bielle[dic_edge_names["edge_long_g_b"]])
geo_line.append(all_edges_bielle[dic_edge_names["edge_arr_pte_g_b"]])

e_r_p_b = all_edges_bielle[dic_edge_names["edge_ray_pte_b"]]
# w_h_e_p, v_h_e_p, u_h_e_p :
del(li_vert[:])
li_vert.append(geom_vert_pte[dic_vert_names['w_b_e']])
# JPL (le 09/05/2011) :
# normalement, on devrait pouvoir utiliser ici le vertex v_h_e_p pour
# faire la partition de l'edge, mais ca ne fonctionne pas...
# on cree donc un vertex supplementaire :
## li_vert.append(geom_vert_pte[dic_vert_names['v_b_e']])
v_b_e_p_bis = geompy.MakeVertexOnCurve(e_r_p_b, 0.5)
li_vert.append(v_b_e_p_bis)
li_vert.append(geom_vert_pte[dic_vert_names['u_b_e']])
part_ray_pte_bas = geompy.MakePartition([e_r_p_b],
                                         li_vert,
                                         [], [], geompy.ShapeType["EDGE"], 0, [], 0)
all_edge_part_ray_pte_bas = geompy.SubShapeAllSorted(part_ray_pte_bas, geompy.ShapeType["EDGE"])

all_edge_part_ray_pte_bas.reverse()
geo_line.extend(all_edge_part_ray_pte_bas[:])
geo_line.append(all_edges_bielle[dic_edge_names["edge_arr_pte_d_b"]])
geo_line.append(all_edges_bielle[dic_edge_names["edge_long_d_b"]])

# association :
ier = doc.associateClosedLine(mod_ext_first[0], mod_ext_start[0], mod_ext_line[0],
                              geo_start, par_start, geo_line)

for nz in range (2) :
    v1 = grille_cyl_grd.getVertexIJK (1, 3, nz)
    v2 = grille_cyl_grd.getVertexIJK (0, 3, nz)
    v3 = grille_cyl_grd.getVertexIJK (0, 0, nz)
    v4 = grille_cyl_grd.getVertexIJK (1, 0, nz)

    v1.setX (v2.getX() + v3.getX() - v4.getX())
    v1.setY (v4.getY())
    v1.setZ (v4.getZ())
    v1.setScalar (7)

    if db : 
        print "Coord en x (grd) : ", v1.getX(), v2.getX(), v3.getX(), v4.getX() 
        print "Coord en y (grd) : ", v1.getY(), v2.getY(), v3.getY(), v4.getY() 
        print "Coord en z (grd) : ", v1.getZ(), v2.getZ(), v3.getZ(), v4.getZ() 
        print 

    v1 = grille_cyl_pte.getVertexIJK (1, 3, nz)
    v2 = grille_cyl_pte.getVertexIJK (0, 3, nz)
    v3 = grille_cyl_pte.getVertexIJK (0, 0, nz)
    v4 = grille_cyl_pte.getVertexIJK (1, 0, nz)

    v4.setX (v3.getX() + v2.getX() - v1.getX())
    v4.setY (v1.getY())
    v4.setZ (v1.getZ())
    if db : 
        print "Coord en x (pte) : ", v1.getX(), v2.getX(), v3.getX(), v4.getX()
        print "Coord en y (pte) : ", v1.getY(), v2.getY(), v3.getY(), v4.getY()
        print "Coord en z (pte) : ", v1.getZ(), v2.getZ(), v3.getZ(), v4.getZ()
        print 


if db : 
   doc.saveVtk ('bielle2.vtk')

## #=================================================
## # VERTEX, EDGES, FACES DANS L'ARBRE D'ETUDE SALOME
## #=================================================

# vertices :
for key, value in dic_vert_names.iteritems():
    geompy.addToStudy(geom_vert_grd[value], key + '_g')
    geompy.addToStudy(geom_vert_pte[value], key + '_p')

## # edges :
## for key, value in dic_edge_names.iteritems():
##     geompy.addToStudy(all_edges_bielle[value], key)

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

# chercher les propagations du modele
for j in range(doc.countPropagation()):
    propa = doc.getPropagation(j)
    propa.setLaw(law)  # appliquer la loi de discretisation sur tout le modele et generer le maillage

#====================================
# Générer des maillages
#====================================

print  " --- MAILLAGE HEXAHEDRIQUE --- "
mesh_hexas = hexablock.mesh("Bielle:hexas", doc)

## print "Nombre d hexaedres:", mesh_hexas.NbHexas()
## print "Nombre de quadrangles:", mesh_hexas.NbQuadrangles()
## print "Nombre de segments:", mesh_hexas.NbEdges()
## print "Nombre de noeuds:", mesh_hexas.NbNodes()

## file_name = os.path.join(os.environ['TMP'], 'grille_cyl_grd2.vtk')
## grille_cyl_grd.saveVtk(file_name)

## file_name = os.path.join(os.environ['TMP'], 'bielle.vtk')
## doc.saveVtk(file_name)

# TESTS :
####   f_mod_avant.close()
####   f_geom.close()
####   f_mod_apres.close()
