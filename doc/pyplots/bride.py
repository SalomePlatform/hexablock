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

import os
import GEOM
import geompy
import smesh
import hexablock
import math
import SALOMEDS

k1 = 1

OPT_QUAD_IK = 1
OPT_FIRST = 2

count = 1
def save_schema(doc):
    """
    sauvegarde vtk du modele de bloc
    """
    global count
    file_name = os.path.join(os.environ['TMP'], 'bride' + str(count) + '.vtk')
    doc.saveVtk(file_name)
    count += 1
    pass

def merge_quads(doc, quart, demi, ni1, nj1, ni2, nj2, option=0) :
    """
    fusion des quadrangles entre les 2 grilles cylindriques :
    """

    prems = False
    if option == OPT_FIRST:
        prems = True

    quad_ik = False
    if option == OPT_QUAD_IK:
        quad_ik = True

    orig = None
    if quad_ik:
        orig = grille_cyl_quart.getQuadIK(ni1, nj1, k1)
    else:
        orig = grille_cyl_quart.getQuadJK(ni1, nj1, k1)

    dest = grille_cyl_demi.getQuadJK(ni2, nj2, k1)


# JPL le 10/05/2011 :
# closeQuads() n'est pas accessible en python
# a priori fonctionne avec mergeQuads()
##     if prems:
    if True:
        iq1 = 0
        if quad_ik:
            iq1 = 1
        iq3 = 1 - iq1
        v1 = dest.getVertex(iq1)
        v3 = dest.getVertex(iq3)
        
        v2 = orig.getVertex(0)
        v4 = orig.getVertex(1)

        doc.mergeQuads(dest, orig, v1, v2, v3, v4)
        pass
    else:
##         doc.closeQuads(dest, orig)
        print "closeQuads() : not yet implemented"
        pass

    return None

BREP_PATH = os.path.expandvars("$HEXABLOCK_ROOT_DIR/bin/salome/bride.brep")

#=============================
# CREATION DOCUMENT
#=============================

doc = hexablock.addDocument()

#=============================
# PARAMETRES
#=============================

height = 1.0

# cylinder grid 1 :
nr1 = 8
na1 = 4
nl1 = 5
dr1 = 1.0
da1 = 45.0  # angle
dl1 = height

# cylinder grid 2 :
nr2 = 3
na2 = 8
nl2 = nl1
dr2 = 0.5
da2 = 180.0  # angle
dl2 = dl1

#=============================
# Creation du modele de blocs
#=============================

# JPL (le 09/05/2011)
# repris de test_bride_abu.cxx (version la plus a jour dans ~/IHMHEXA/Alain/models):
# et de BRIDE.py (Karima) pour les "vraies" coordonn�es :

#=================================================
# Creation des centres des grilles cylindriques
#=================================================

center1 = doc.addVertex(0, 0, height)
center2 = doc.addVertex(6, 0, height)

dx = doc.addVector(height, 0, 0)
dz = doc.addVector(0, 0, height)

# Creation des grilles cylindriques initiales
#============================================

# 1. 1 ere grille (quart) :
#==========================

# JPL (le 10/05/2011) : vu la geometrie, on ne remplit pas le centre :
## grille_cyl_quart = doc.makeCylindrical(orig1, dx, dz, dr_q, 45.0, dl,
##                                        nr_q, na_q, dim_z, True)
grille_cyl_quart = doc.makeCylindrical(center1, dx, dz, dr1, da1, dl1,
                                       nr1, na1, nl1, False)


# temporaire : sauvegarde du modele de blocs :
save_schema(doc)
# fin temporaire

# Elagage  :
for nk in range(2, nl1):
    for nj in range(na1):
        ideb = 2
        if nk == nl1 - 1:
            ideb = 1
        for ni in range(ideb, nr1):
            doc.removeHexa(grille_cyl_quart.getHexaIJK(ni, nj, nk))
            pass
        pass
    pass

# temporaire : sauvegarde du modele de blocs :
save_schema(doc)
# fin temporaire

# Semelle :
k0 = 0
for nj in range(na1):
    for ni in range(2, nr1):
        doc.removeHexa(grille_cyl_quart.getHexaIJK(ni, nj, k0))
        pass
    pass

# temporaire : sauvegarde du modele de blocs :
save_schema(doc)
# fin temporaire

# @todo JPL : peut-on fusionner les edges du haut des hexaedres du haut du deuxieme
# rang ? (cf. GEOM). Si oui revoir aussi l'association

# 2. 2�me grille (demi) :
#========================
grille_cyl_demi = doc.makeCylindrical(center2, dx, dz, dr2, da2, dl2,
                                      nr2, na2, nl2, True)

# temporaire : sauvegarde du modele de blocs :
save_schema(doc)
# fin temporaire

ni0 = [0, nr2, 2, 1, 0]  # en fonction de z (ie : nk) 
for nk in range(0, nl2):
    for nj in range(na2):  # elagage suivant toute la demi-circonference
        for ni in range(ni0[nk], nr2):
            doc.removeHexa(grille_cyl_demi.getHexaIJK(ni, nj, nk))            
            pass
        pass
    pass


# temporaire : sauvegarde du modele de blocs :
save_schema(doc)
# fin temporaire

# 3. creusement des fondations de demi dans quart :
#==================================================
for nj in range(2):
    for ni in range(3, nr1 - 1):
        doc.removeHexa(grille_cyl_quart.getHexaIJK(ni, nj, k1))
        pass
    pass

# temporaire : sauvegarde du modele de blocs :
save_schema(doc)
# fin temporaire

# 4. Fusion des bords :
#======================
merge_quads(doc, grille_cyl_quart, grille_cyl_demi, 7, 0, nr2, 0, OPT_FIRST)
merge_quads(doc, grille_cyl_quart, grille_cyl_demi, 7, 1, nr2, 1)
for ni1 in range(2, 6):
    merge_quads(doc, grille_cyl_quart, grille_cyl_demi, 8 - ni1, 2, nr2, ni1, OPT_QUAD_IK)
    pass
merge_quads(doc, grille_cyl_quart, grille_cyl_demi, 3, 1, nr2, 6)
merge_quads(doc, grille_cyl_quart, grille_cyl_demi, 3, 0, nr2, 7)

# temporaire : sauvegarde du modele de blocs :
save_schema(doc)
# fin temporaire

###########
# Geometry
###########

bride_geom = geompy.ImportFile(BREP_PATH, "BREP")

geompy.addToStudy(bride_geom, "bride_geom")


# parametres de la geometrie :

r1 = 12.0
r1_t = 7.88
r2 = 20.0
r2_t = 2.0

##############
# Association
##############

# association vertex/points de la grille 1
# (tous les vertex qui ne sont pas fusionnes avec ceux de la grille #
# 2)

dz_geom = geompy.MakeVectorDXDYDZ(0., 0., 1.)

# les vertex du cylindre 1 sont crees de bas en haut, en tournant dans
# le sens trigonometrique :

# 6 vertex sont necessaires / axe z (pour associer aux 6 vertices du
# modele) :

z_val = [-1, 1.5, 21.5, 34, 46.5]  # nl1 + 1 valeurs
for ni in range(nr1 + 1):
    # suivant ni, valeurs des x pour les (nl1 + 1) points (selon l'axe z)
    x = []
    z = []
    nb_z = 0  # nombre de points suivant l'axe z
    if ni == 0:
        z = z_val
        x = [r1_t] * len(z)
        pass
    elif ni == 1:
        z = z_val
        x = [r1_t + 2.77] * len(z)
        pass
    elif ni == 2:
        z = z_val[0:-1]  # tout sauf le dernier
        x_last = r1_t + 2.77
        x = [24.0, 24.0, 19.0, (19.0 - x_last)/2 + x_last, x_last]  # pour le 4 eme point, moyenne
        # entre le 3 eme et le 5 eme
        pass
    elif ni == 3:
        z = z_val[1:3]
        x = [24.0, 19.0]
        pass
    elif ni == 4:
        z = z_val[1:3]
        x = [26.5, 21.0]  # a revoir pour le premier point ??
        pass
    elif ni == 8:
        z = z_val[1:3]
        x = [47.5] * 2
        pass
    else:  # ni = 5, 6, 7
        z = z_val[1:3]
        x = [26.5 + (47.5 - 26.5)/4*(ni - 4)] * 2
        pass
    pass

    nb_z = len(z)

    # creation des points pour y = 0 :
    vert_grid1_xi = [geompy.MakeVertex(xi, 0, zi) for (xi, zi) in \
                     zip(x, z)]

    # les points suivants sont crees par rotation de PI/16 suivant
    # l'axe z / aux precedents :
    angle = math.pi/4.0/na1  # PI/4 (45 degres), divise par 4
    for j in range(na1):
        li = [geompy.MakeRotation(v, dz_geom, angle) for v in vert_grid1_xi[-nb_z:]]
        vert_grid1_xi.extend(li)
        pass

    # ajout des points a l'etude et association :
    # les vertex fusionnes ou correspondant a des hexaedres effaces ne
    # sont pas pris en compte.
    for nj in range(na1 + 1):
        for nk in range(nb_z):
            if (ni <= 2) or (3 <= ni <= 7 and nj >= na1 - 1) or \
                   (ni == 8 and (nj == 0  or nj >= na1 - 1)):
                v_mod = grille_cyl_quart.getVertexIJK(ni, nj, nk)
                v_geo = vert_grid1_xi[nk + nj*nb_z]
                geompy.addToStudy(v_geo, "vert_grid1_x" + str(ni) + "_y" + \
                                  str(nj) + "_z" + str(nk))
                v_mod.setAssociation(v_geo)
                pass
            pass
        pass
    
    pass

# association vertex/points de la grille 2
# (tous les vertex qui ne sont pas fusionnes avec ceux de la grille #
# 1)
## dz_geom2 = geompy.MakeVectorDXDYDZ(33.5, 0., 1.)
pt_a = geompy.MakeVertex(33.5, 0, 0)
pt_b = geompy.MakeVertex(33.5, 0, 1.)
dz_geom2 = geompy.MakeVector(pt_a, pt_b)

# les vertex du cylindre 2 sont crees de bas en haut, en tournant dans
# le sens trigonometrique :

# REM : pour l'instant on met de cote la partie centrale du cylindre
# (6 vertex selon z) => a faire a la fin. Ils sont d'ailleurs ranges
# apres les autres dans le modele


# 6 vertex sont necessaires / axe z (pour associer aux 6 vertices du
# modele) :

z_val = [-1, 1.5, 21.5, 24, 36, 41.5]  # nl2 + 1 valeurs
for ni in range(nr2 + 1):
    # suivant ni, valeurs des x pour les (nl1 + 1) points (selon l'axe z)
    x = []
    z = []
    nb_z = 0  # nombre de points suivant l'axe z
    if ni == 0:
        z = z_val
        x = [39.5] * len(z)
        pass
    elif ni == 1:
        z = z_val[1:-1]  # tout sauf le dernier et le premier
        x = [42.5] * len(z)
        pass
    elif ni == 2:
        z = z_val[1:-2]  # tout sauf les 2 derniers et le premier
        x = [46.] * len(z)
        pass
    elif ni == 3:
        z = z_val[1:3]
        x = [46.7] * len(z)  # valeur a revoir ??
        pass
    pass

    nb_z = len(z)

    # creation des points pour y = 0 :
    vert_grid2_xi = [geompy.MakeVertex(xi, 0, zi) for (xi, zi) in \
                     zip(x, z)]

    # les points suivants sont crees par rotation de PI/16 suivant
    # l'axe z / aux precedents :
    angle = math.pi/na2  # PI (180 degres), divise par 8
    for j in range(na2):
        li = [geompy.MakeRotation(v, dz_geom2, angle) for v in vert_grid2_xi[-nb_z:]]
        vert_grid2_xi.extend(li)
        pass

    # ajout des points a l'etude et association :
    for nj in range(na2 + 1):
        for nk in range(nb_z):
            v_mod = grille_cyl_demi.getVertexIJK(ni, nj, nk)
            v_geo = vert_grid2_xi[nk + nj*nb_z]
            geompy.addToStudy(v_geo, "vert_grid2_x" + str(ni) + "_y" + \
                              str(nj) + "_z" + str(nk))
            v_mod.setAssociation(v_geo)
            pass
        pass

    pass

# association des vertex communs grille1/grille2
# REM : cette etape n'est pas necessaire ? En effet, les vertex ayant
# ete fusionnes, l'association a ete faite avec la grille 2
