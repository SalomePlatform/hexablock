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
import SALOME
import geompy
import smesh
import hexablock
import math



# chemin du fichier BREP contenant la CAO
BREP_PATH = os.path.expandvars("$HEXABLOCK_ROOT_DIR/bin/salome/tees.brep")
#===================================================================
#               LE MODELE:
#===================================================================


#=============================
# PARAMETRES
#=============================

#=============================
# CREATION DOCUMENT
#=============================
doc = hexablock.addDocument()

#=============================
# CREATION DU MODELE
#=============================
CYL1 = 0
CYL2 = 1


S_E    = 0
S_NE   = 1
S_N    = 2
S_NW   = 3
S_W    = 4
S_SW   = 5
S_S    = 6
S_SE   = 7
S_MAXI = 8

##dy = doc.addVector ( 0, 1, 0 )
dx1 = doc.addVector( -1, 0, 0 )
dz1 = doc.addVector( 0, 0, 1 )


c_pte_1 = doc.addVertex ( 190., 0., 220. )
c_grd_1 = doc.addVertex ( 300., 0., 400. )
cyl_pte_1 = doc.addCylinder( c_pte_1 , dz1 , 20 , 360 ) #580-220
cyl_grd_1 = doc.addCylinder( c_grd_1 , dx1 , 50 , 140 )
cross1 = doc.makeCylinders( cyl_pte_1 , cyl_grd_1 )


dx2 = doc.addVector( -1, 0, 0 )
dz2 = doc.addVector( 0, 0, 1 )
c_pte_2 = doc.addVertex( 110. , 0. , 400. )
c_grd_2 = doc.addVertex( 0. , 0. , 0. )

cyl_pte_2 = doc.addCylinder( c_pte_2 , dx2 , 50, 220 )
cyl_grd_2 = doc.addCylinder( c_grd_2 , dz2 , 100, 800 )
cross2    = doc.makeCylinders( cyl_pte_2, cyl_grd_2 )

# Qq constantes
coul1 = 5
coul2 = 3

# Pour les bicylindres
xhint   = 0   # indice x pour les hexa interieurs
xhext   = 1   # indice x pour les hexa exterieurs
xvint   = 0   # indice x pour les vertex interieurs
xvext   = 2   # indice x pour les vertex exterieurs
zbas    = 0
zhhaut1  = 5   # hauteur max quad  petit cylindre
zhhaut2  = 3   # hauteur max quad  gros cylindre
zvhaut1  = 6   # hauteur max vertex du petit  cylindre
zvhaut2  = 4   # hauteur max vertex du gros  cylindre

# La jointure
hQuads = []
for ny in range(8):
    hQuads+=[cross2.getQuadIJ(CYL1, xhext, ny, zbas)]

for ny in range(4):
    hQuads+=[cross2.getQuadIJ(CYL1, xhint, ny, zbas)]

qstart0 = cross2.getQuadIJ (CYL1, xhext, S_E, zbas)
qtarget = cross1.getQuadIJ (CYL2, xhext, S_S, zvhaut2)

vb0 = qtarget.getVertex (0)
vb1 = qtarget.getVertex (1)
vh0 = qstart0.getVertex (0)
vh1 = qstart0.getVertex (1)

hauteur = 3
joint = doc.joinQuads (hQuads, qtarget, vh0,vb0, vh1,vb1, hauteur)

# Elimination bout en trop
for ny in range(8):
    h = cross2.getHexaIJK (CYL1, xhext, ny, zhhaut1)
    doc.removeHexa(h)
    if ny<4:
        h = cross2.getHexaIJK (CYL1, xhint, ny, zhhaut1)
        doc.removeHexa(h)


#===================================================================
# Recuperation edges du model pour l'association
#===================================================================

##=========================
##       EDGES
##=========================
### BIG CYLINDER
top_BIG    = []
bottom_BIG = []
for ny in range(8):
    bottom_BIG += [ cross2.getEdgeJ(CYL2, 2, ny, 0) ]

for ny in range(8):
    top_BIG += [ cross2.getEdgeJ(CYL2, 2, ny, zvhaut2) ]

### MEDIUM CYLINDER
left_MEDIUM_BIG = []
right_MEDIUM    = []

for ny in range(8):
    left_MEDIUM_BIG += [cross2.getEdgeJ (CYL1, 2, ny, 1)]

for ny in range(8):
    right_MEDIUM += [ cross1.getEdgeJ (CYL2, 2, ny, 0) ]

### SMALL CYLINDER
top_SMALL           = []
top_SMALL_MEDIUM    = []
bottom_SMALL_MEDIUM = []
bottom_SMALL        = []

for ny in range(8):
    top_SMALL += [ cross1.getEdgeJ(CYL1, 2, ny, zvhaut1) ]
for ny in range(8):
    top_SMALL_MEDIUM += [ cross1.getEdgeJ(CYL1, 2, ny, zvhaut1-1) ]
for ny in range(8):
    bottom_SMALL_MEDIUM += [ cross1.getEdgeJ(CYL1, 2, ny, 1) ]
for ny in range(8):
    bottom_SMALL += [ cross1.getEdgeJ(CYL1, 2, ny, 0) ]


#===================================================================
#               LA GEOMETRIE
#===================================================================

#===================================================================
# Recuperation des �l�ments de la geometrie tuyau pour l'association
#===================================================================
tees_ = geompy.Import(BREP_PATH, "BREP")

##=========================
##       EDGES
##=========================
all_edges_ = geompy.SubShapeAllSorted( tees_ , geompy.ShapeType["EDGE"] )

### BIG CYLINDER
top_BIG_    = all_edges_[1]
bottom_BIG_ = all_edges_[0]

### MEDIUM CYLINDER
left_MEDIUM_BIG_ = [ all_edges_[3], all_edges_[5] , all_edges_[4], all_edges_[2], ]
right_MEDIUM_    = all_edges_[20]

### SMALL CYLINDER
top_SMALL_            = all_edges_[13]
top_SMALL_MEDIUM_     = [ all_edges_[11], all_edges_[16] ]
bottom_SMALL_MEDIUM_  = [ all_edges_[10], all_edges_[9], all_edges_[14], all_edges_[15] ]
bottom_SMALL_         = all_edges_[12]


#==========================================================
#               LES ASSOCIATIONS
#==========================================================

##===========================================
##          EDGES
##===========================================
def associateEdges( edges, edges_, i_assoc, oppositeWay = False ):
    nb_points = float( len(i_assoc) )
    #print "nb_points ", nb_points
    for i, i_ in i_assoc.items():
        e  = edges[i]
        e0 = e.getVertex(0)
        e1 = e.getVertex(1)

        if oppositeWay:
            u0 = (i_+1.)/nb_points
            u1 = i_/nb_points
            e.addAssociation( edges_ , u1, u0 )
        else:
            u0 = i_/nb_points
            u1 = (i_+1.)/nb_points
            e.addAssociation( edges_ , u0, u1 )

        e0_ = geompy.MakeVertexOnCurve( edges_, u0 )
        e1_ = geompy.MakeVertexOnCurve( edges_, u1 )
        e0.setAssociation( e0_ )
        e1.setAssociation( e1_ )



## BIG CYLINDER
assoc   = { 4:0, 5:1, 6:2, 7:3, 0:4, 1:5, 2:6, 3:7 }
associateEdges( top_BIG   , top_BIG_,    assoc )
associateEdges( bottom_BIG, bottom_BIG_, assoc )

## SMALL CYLINDER
assoc = { 4:6, 5:7, 6:0, 7:1, 0:2, 1:3, 2:4, 3:5 }
associateEdges( top_SMALL   , top_SMALL_,    assoc )
associateEdges( bottom_SMALL, bottom_SMALL_, assoc )


## SMALL-MEDIUM CYLINDER

### Top ( un peu 'touchy' )
e = top_SMALL_MEDIUM[0]
e_0  = e.getVertex(0)
e_0_ = geompy.MakeVertexOnCurve( top_SMALL_MEDIUM_[0], 0.)
e_0.setAssociation( e_0_ )
top_SMALL_MEDIUM_0_vertices_ = [ e_0_ ]

for i, e in enumerate( top_SMALL_MEDIUM[0:6] ):
    e_1 = e.getVertex(1)
    e_1_ = geompy.MakeVertexOnCurve( top_SMALL_MEDIUM_[0], (i+1.)/6.)
    e_1.setAssociation( e_1_ )
    top_SMALL_MEDIUM_0_vertices_ += [ e_1_ ]

partition_ = geompy.MakePartition( [ top_SMALL_MEDIUM_[0] ],
    top_SMALL_MEDIUM_0_vertices_, [], [], geompy.ShapeType["EDGE"], 0, [], 0 )
top_SMALL_MEDIUM_0_edges_ = geompy.SubShapeAllSorted( partition_, geompy.ShapeType["EDGE"] )


assoc = { 0:2, 1:0, 2:1, 3:3, 4:4, 5:5 }
for i,i_ in assoc.items():
    e  = top_SMALL_MEDIUM[i]
    e_ = top_SMALL_MEDIUM_0_edges_[i_]
    e.addAssociation( e_, 0., 1. )

assoc = { 6:0, 7:1 }
associateEdges( top_SMALL_MEDIUM, top_SMALL_MEDIUM_[1], assoc )


### Bottom
assocs = [ { 0:1, 1:0  }, { 2:1, 3:0 },  { 4:0, 5:1 }, { 6:0, 7:1 } ] # 4 edges
associateEdges( bottom_SMALL_MEDIUM, bottom_SMALL_MEDIUM_[0], assocs[0], True )
associateEdges( bottom_SMALL_MEDIUM, bottom_SMALL_MEDIUM_[1], assocs[1], True )
associateEdges( bottom_SMALL_MEDIUM, bottom_SMALL_MEDIUM_[2], assocs[2] )
associateEdges( bottom_SMALL_MEDIUM, bottom_SMALL_MEDIUM_[3], assocs[3] )



## MEDIUM CYLINDER
assocs = [ { 0:0, 1:1  }, { 2:0, 3:1 },  { 4:1, 5:0 }, { 6:1, 7:0 } ]
associateEdges( left_MEDIUM_BIG, left_MEDIUM_BIG_[0], assocs[0] )
associateEdges( left_MEDIUM_BIG, left_MEDIUM_BIG_[1], assocs[1] )
associateEdges( left_MEDIUM_BIG, left_MEDIUM_BIG_[2], assocs[2], True )
associateEdges( left_MEDIUM_BIG, left_MEDIUM_BIG_[3], assocs[3], True )

assoc = { 0:3, 1:2, 2:1, 3:0, 4:7, 5:6, 6:5, 7:4 }
associateEdges( right_MEDIUM, right_MEDIUM_,  assoc , True )



#allfaces_ = geompy.SubShapeAllSorted(tees_, geompy.ShapeType["FACE"])
#for i,f in enumerate( allfaces_ ):
    #geompy.addToStudy( f,   "allfaces"+str(i) )

#for ny in range(8):
    #q = cross2.getQuadJK(CYL2,2,ny,0)
    #q.addAssociation( allfaces_[2] )

#for ny in range(8):
    #q = cross2.getQuadJK(CYL2,2,ny,1)
    #if q:
        #q.addAssociation( allfaces_[2] )




##==========================================================
##       LA GEOMETRIE DANS L'ARBRE D'ETUDE SALOME
##==========================================================
geompy.addToStudy( tees_,           "Tees" )

#### BIG CYLINDER
geompy.addToStudy( top_BIG_,        "top_BIG" )
geompy.addToStudy( bottom_BIG_,     "bottom_BIG" )

#### SMALL CYLINDER
geompy.addToStudy( top_SMALL_,       "top_SMALL" )
for i,e in enumerate(top_SMALL_MEDIUM_):
    geompy.addToStudy( e, "top_SMALL_MEDIUM"+str(i) )
for i,e in enumerate(top_SMALL_MEDIUM_0_edges_):
    geompy.addToStudy( e, "top_SMALL_MEDIUM_0_edges_"+str(i) )
for i,e in enumerate(bottom_SMALL_MEDIUM_):
    geompy.addToStudy( e, "bottom_SMALL_MEDIUM"+str(i) )
geompy.addToStudy( bottom_SMALL_,    "bottom_SMALL" )

#### MEDIUM CYLINDER
geompy.addToStudy( right_MEDIUM_,    "right_MEDIUM" )
for i,e in enumerate(left_MEDIUM_BIG_):
    geompy.addToStudy( e,   "left_MEDIUM_BIG"+str(i) )



#====================================
# CREATION GROUPES DU MAILLAGE
#====================================

#On definit 3 groupes de mailles

#groupe d edges (aretes)
Edge_grp = doc.addEdgeGroup("Edge_grp")
Nbr_Edg = doc.countEdge()
for i in range(Nbr_Edg):
    Edge_i = doc.getEdge(i)
    Edge_grp.addElement(Edge_i)

# groupe de quads (faces)
Quad_grp = doc.addQuadGroup("Quad_grp")
Nbr_Qad = doc.countQuad()
for i in range(Nbr_Qad):
    Quad_i = doc.getQuad(i)
    Quad_grp.addElement(Quad_i)

# groupe d hexas (solids)
Hexa_grp = doc.addHexaGroup("Hexa_grp")
Nbr_Hex = doc.countHexa()
for i in range(Nbr_Hex):
    Hexa_i = doc.getHexa(i)
    Hexa_grp.addElement(Hexa_i)

# groupe de noeuds de vertex pour tout le modele 
Vertex_Nod_Grp = doc.addVertexNodeGroup("Vertex_Nod_Grp")
Nbr_Vx = doc.countVertex()
for i in range(Nbr_Vx):
    Vertex_i = doc.getVertex(i)
    Vertex_Nod_Grp.addElement(Vertex_i)



##====================================
## CREATION MAILLAGE
##====================================


##====================================
## Definir une loi de discretisation
##====================================


# definir une loi: le choix de la loi reste aux utilisateurs
Law = doc.addLaw( "Uniform" , 4 )
#Law = doc.addLaw( "Uniform" , 8 )

# chercher les propagations du modele
N_Propa = doc.countPropagation()

for j in range(N_Propa):
    Propa = doc.getPropagation(j)
    Propa.setLaw( Law )# appliquer la loi de discretisation sur tout le modele et generer le maillage

try:
    mesh = hexablock.mesh("Inter_3_Cylindre", doc)
#    mesh = .hexablock.mesh("Inter_3_Cylindre:quads", doc, 2)
except SALOME.SALOME_Exception, ex:
    print "Mesh computation failed, exception caught:"
    print "    ", ex.details.text
except:
    import traceback
    print "Mesh computation failed, exception caught:"
    traceback.print_exc()


print "Nombre d'hexaedres:", mesh.NbHexas()
print "Nombre de quadrangles:", mesh.NbQuadrangles()
print "Nombre de segments:", mesh.NbEdges()
print "Nombre de noeuds:", mesh.NbNodes()






