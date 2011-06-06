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

import SALOME
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

# Pour l'intersection des trois cylindres on doit s enservir des modeles predefinis des cylindres deja decoupes

#=============================
# PARAMETRES
#=============================

R = 5.0

r_pte = R

r_moy = 2*R

r_grd = 4*R

l = 200
h = 100

# Taille du 1er petit cylindre
dr_pte_1 = 20
da_pte_1 = 360
dl_pte_1 = 200

nr_pte_1 = 1
na_pte_1 = 4 #remplissage a l interieur de 1 hexa
nl_pte_1 = 1

# Taille du 1er grand cylindre
dr_grd_1 = 50
da_grd_1 = 360
dl_grd_1 = 150

nr_grd_1 = 1
na_grd_1 = 12 #remplissage a l interieur de 8 hexas
nl_grd_1 = 1

# Taille du 2eme petit cylindre
dr_pte_2 = 50
da_pte_2 = 360
dl_pte_2 = 200

nr_pte_2 = 1
na_pte_2 = 12 #remplissage a l interieur de 8 hexas
nl_pte_2 = 1

# Taille du 2eme grand cylindre
dr_grd_2 = 100
da_grd_2 = 360
dl_grd_2 = 400

nr_grd_2 = 1
na_grd_2 = 18 #remplissage a l interieur de 8 hexas
nl_grd_2 = 1



#=============================
# Creation des vecteurs
#=============================

DX = doc.addVector( 1 , 0 , 0 )
DY = doc.addVector( 0 , 1 , 0 )
DZ = doc.addVector( 0 , 0 , 1 )


#===================================
# Creation des centres de cylindres
#===================================

C_pte_1 = doc.addVertex( -h , 0 , h )
C_grd_1 = doc.addVertex( 0 , 0 , 0 )

C_pte_2 = doc.addVertex( 0 , 0 , 3*h )
C_grd_2 = doc.addVertex( -2*h , 0 , 4*h )

#C_pte_1 = doc.addVertex( 190 , 0 , 220 )
#C_grd_1 = doc.addVertex( 300 , 0 , 400 )

#C_pte_2 = doc.addVertex( 200 , 0 , 400 )
#C_grd_2 = doc.addVertex( 0 , 0 , 0 )


#================================
# Creation du 1er petit cylindre
#================================

Cyl_pte_1 = doc.addCylinder( C_pte_1 , DX , dr_pte_1 , dl_pte_1 )
#Cyl_pte_1 = doc.addCylinder( C_pte_1 , DX , 5 , 100 )
#Cyl_pte_1.saveVtk("/tmp/Cyl_pte_1.vtk")

#================================
# Creation du 1er grand cylindre
#================================

Cyl_grd_1 = doc.addCylinder( C_grd_1 , DZ , dr_grd_1 , dl_grd_1 )
#Cyl_grd_1 = doc.addCylinder( C_grd_1 , DZ , 10 , 200 )
#Cyl_grd_1.saveVtk("/tmp/Cyl_grd_1.vtk")

#================================
# Creation du 2eme petit cylindre
#================================

Cyl_pte_2 = doc.addCylinder( C_pte_2 , DZ , dr_pte_2 , dl_pte_2 )
#Cyl_pte_2 = doc.addCylinder( C_pte_2 , DZ , 10 , 200 )
#Cyl_pte_2.saveVtk("/tmp/Cyl_pte_2.vtk")

#================================
# Creation du 2eme grand cylindre
#================================

Cyl_grd_2 = doc.addCylinder( C_grd_2 , DX , dr_grd_2 , dl_grd_2 )
#Cyl_grd_2 = doc.addCylinder( C_grd_2 , DX , 20 , 200 )
#Cyl_grd_2.saveVtk("/tmp/Cyl_grd_2.vtk")



#=================================
# Intersection cylindres decoupes
#=================================


Cyl_Decou_Cyl_pte_grd_1 = doc.makeCylinders( Cyl_pte_1 , Cyl_grd_1 )

Cyl_Decou_Cyl_pte_grd_2 = doc.makeCylinders( Cyl_pte_2 , Cyl_grd_2 )



#====================================================================
# Recuperation des quads qui servent a fusionner les deux partitions
#====================================================================



# reperer les quads du 1er decoupe des cylindres
# on a besoin des quads IJ dans le plan XY

#ni = 2
#nj = 4
#nk = 4

#0 <= i < ni = 2
#0 <= j < nj = 4
#0 <= k < nk + 1 = 5

#quad_decoup1_x0_y0_z4 = Cyl_Decou_Cyl_pte_grd_1.getQuadIJ( 1 , 0 , 0 , 4 )

#vx_1 = Cyl_Decou_Cyl_pte_grd_1.getVertexIJK( 1 , 0 , 0 , 4 )
#vx_2 = Cyl_Decou_Cyl_pte_grd_1.getVertexIJK( 1 , 1 , 2 , 4 )


#ni = 2
#nj = 8
#nk = 4

#0 <= i < ni = 2
#0 <= j < nj = 8
#0 <= k < nk + 1 = 5

#quad_decoup1_x1_y0_z4 = Cyl_Decou_Cyl_pte_grd_1.getQuadIJ( 1 , 1 , 0 , 4 )

#vx_1 = Cyl_Decou_Cyl_pte_grd_1.getVertexIJK( 1 , 2 , 0 , 4 )
#vx_2 = Cyl_Decou_Cyl_pte_grd_1.getVertexIJK( 1 , 2 , 1 , 4 )




# reperer les quads du 2eme decoupe des cylindres
# on a besoin des quads IJ dans le plan XY

#ni = 2
#nj = 4
#nk = 4

#0 <= i < ni = 2
#0 <= j < nj = 4
#0 <= k < nk + 1 = 5

quad_decoup2_x0_y0_z0 = Cyl_Decou_Cyl_pte_grd_2.getQuadIJ( 0 , 0 , 0 , 0 )
quad_decoup2_x0_y1_z0 = Cyl_Decou_Cyl_pte_grd_2.getQuadIJ( 0 , 0 , 1 , 0 )
quad_decoup2_x0_y2_z0 = Cyl_Decou_Cyl_pte_grd_2.getQuadIJ( 0 , 0 , 2 , 0 )
quad_decoup2_x0_y3_z0 = Cyl_Decou_Cyl_pte_grd_2.getQuadIJ( 0 , 0 , 3 , 0 )

#list_quad_decoup2_x0 = [ quad_decoup2_x0_y0_z0 , quad_decoup2_x0_y1_z0 , quad_decoup2_x0_y2_z0 , quad_decoup2_x0_y3_z0 ]

#vx_1_prim = Cyl_Decou_Cyl_pte_grd_2.getVertexIJK( 0 , 0 , 0 , 0 )
#vx_2_prim = Cyl_Decou_Cyl_pte_grd_2.getVertexIJK( 0 , 1 , 2 , 0 )



#ni = 2
#nj = 8
#nk = 4

#0 <= i < ni = 2
#0 <= j < nj = 8
#0 <= k < nk + 1 = 5

#for j in [ 0 , 1 , 2 , 3 , 4 , 5 , 6 , 7 ]:
    #for i in [ 0 , 1 ]:
	#quad_list = Cyl_Decou_Cyl_pte_grd_2.getQuadIJ( 0 , i , j , 0 )

quad_decoup2_x1_y0_z0 = Cyl_Decou_Cyl_pte_grd_2.getQuadIJ( 0 , 1 , 0 , 0 )
quad_decoup2_x1_y1_z0 = Cyl_Decou_Cyl_pte_grd_2.getQuadIJ( 0 , 1 , 1 , 0 )
quad_decoup2_x1_y2_z0 = Cyl_Decou_Cyl_pte_grd_2.getQuadIJ( 0 , 1 , 2 , 0 )
quad_decoup2_x1_y3_z0 = Cyl_Decou_Cyl_pte_grd_2.getQuadIJ( 0 , 1 , 3 , 0 )
quad_decoup2_x1_y4_z0 = Cyl_Decou_Cyl_pte_grd_2.getQuadIJ( 0 , 1 , 4 , 0 )
quad_decoup2_x1_y5_z0 = Cyl_Decou_Cyl_pte_grd_2.getQuadIJ( 0 , 1 , 5 , 0 )
quad_decoup2_x1_y6_z0 = Cyl_Decou_Cyl_pte_grd_2.getQuadIJ( 0 , 1 , 6 , 0 )
quad_decoup2_x1_y7_z0 = Cyl_Decou_Cyl_pte_grd_2.getQuadIJ( 0 , 1 , 7 , 0 )

list_quad_decoup2_x1 = [ quad_decoup2_x1_y2_z0 , quad_decoup2_x0_y0_z0 , quad_decoup2_x0_y1_z0 , quad_decoup2_x0_y2_z0 , quad_decoup2_x0_y3_z0 , quad_decoup2_x1_y0_z0 , quad_decoup2_x1_y1_z0 , quad_decoup2_x1_y2_z0 , quad_decoup2_x1_y3_z0 , quad_decoup2_x1_y4_z0 , quad_decoup2_x1_y5_z0 , quad_decoup2_x1_y6_z0 , quad_decoup2_x1_y7_z0 ]

quad_decoup1_x1_y0_z4 = Cyl_Decou_Cyl_pte_grd_1.getQuadIJ( 1 , 1 , 0 , 4 )

vx_1 = Cyl_Decou_Cyl_pte_grd_1.getVertexIJK( 1 , 2 , 0 , 4 )
vx_2 = Cyl_Decou_Cyl_pte_grd_1.getVertexIJK( 1 , 2 , 1 , 4 )

vx_1_prim = Cyl_Decou_Cyl_pte_grd_2.getVertexIJK( 0 , 2 , 2 , 0 )
vx_2_prim = Cyl_Decou_Cyl_pte_grd_2.getVertexIJK( 0 , 2 , 3 , 0 )


jointure = doc.joinQuads( list_quad_decoup2_x1 , quad_decoup1_x1_y0_z4 , vx_1_prim , vx_1 , vx_2_prim , vx_2 , 3 )




#jointure.saveVtk("/tmp/jointure.vtk")
#doc.saveVtk("/tmp/inter.vtk")


##==========================
## CREATION ASSOCIATION
##==========================


##========================================
## INTERSECTION DES TROIS CYLINDRES GEOM
##========================================

#Inter_Cylin_geom = geompy.Import("/export/home/PR_HEX/SCRIPTS/cao_cea/tees.brep", "BREP")

#geompy.addToStudy(Inter_Cylin_geom, "Inter_Cylin_geom")
#36

## Remarque:
### 1- On a utiliser les memes parametres de la geometrie des trois cylindres dans l'espace
###    pour construire notre modele hexa ( intersection des trois cylindres )
### 2- On conclu qu'on a pas besoin d'une association pts par pts comme les 3 cas tests precedemment
###    realises
### 3- l'association sa se fait automatiquement
### 4- donc on passe directement a la creation du maillage









##====================================
## CREATION GROUPES DU MAILLAGE
##====================================

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





##====================================
## CREATION MAILLAGE
##====================================


##====================================
## Definir une loi de discretisation
##====================================


# definir une loi: le choix de la loi reste aux utilisateurs

Law = doc.addLaw( "Uniform" , 4 )

#petit test
n = doc.countLaw()
print n

# chercher les propagations du modele

N_Propa = doc.countPropagation()

for j in range(N_Propa):
	Propa = doc.getPropagation(j)
	Propa.setLaw( Law )# appliquer la loi de discretisation sur tout le modele et generer le maillage

try:
    mesh = hexablock.mesh("test_INTER_3_CYLINDRE_no_assoc", doc)
except SALOME.SALOME_Exception, ex:
    print "Mesh computation failed, exception caught:"
    print "    ", ex.details.text
except:
    import traceback
    print "Mesh computation failed, exception caught:"
    traceback.print_exc()


print "Nombre d'hexaedres:"   , mesh.NbHexas()
print "Nombre de quadrangles:", mesh.NbQuadrangles()
print "Nombre de segments:"   , mesh.NbEdges()
print "Nombre de noeuds:"     , mesh.NbNodes()


#nQuad = doc.countQuad();
#print "countQuad = ",nQuad

###qds = [2,6,10,14]
##qds = [2,6,10,14]
##for j in qds:
#for j in range(nQuad):
  #q = doc.getQuad(j)
  ##ok = smesh.computeQuad(q, False)
  #ok = smesh.computeQuad(q, True)
  #print "quad j->",j
  #print "ok ->",ok


## --------------------
## Maillage hexa�drique
## --------------------
#mesh = mesh.ExportMED( "3cyl.med", 1 )
#maillages = smesh.CreateMeshesFromMED( "3cyl.med" )

#ijk = maillages[0][0]

#ijk.Hexahedron()

#ijk.Compute()

#print  4*" -- ijk --- "
#print "Nombre d hexaedres:", ijk.NbHexas()
#print "Nombre de quadrangles:", ijk.NbQuadrangles()
#print "Nombre de segments:", ijk.NbEdges()
#print "Nombre de noeuds:", ijk.NbNodes()











