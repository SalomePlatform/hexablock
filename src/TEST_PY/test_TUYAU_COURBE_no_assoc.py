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

#===============================================
# By Karima DEBCHI YATAGHENE Nov 2009 at CS 
#===============================================

import os
#import GEOM
import geompy
import smesh
import hexablock
import math
#import SALOMEDS


BREP_PATH = os.path.expandvars("$HEXA_ROOT_DIR/bin/salome/tuyau.brep")

#=============================
# CREATION DOCUMENT
#=============================

doc = hexablock.addDocument()

#=============================
# CREATION DU MODELE
#=============================

# Pour le tuyau on doit creer une grille cylindrique

#=============================
# PARAMETRES
#=============================

R = 4.5

r = 4.5
r_t = 3.6

h = 75.0

# Taille du cylindre
dr = R
da = 360
dl = h

nr = 1
na = 4
nl = 1


#=============================
# Creation des vecteurs
#=============================

dx = doc.addVector(h, 0, 0)
dy = doc.addVector(0, h, 0)
dz = doc.addVector(0, 0, h)


#=================================================
# Creation du centre de la grille cylindrique
#=================================================

c = doc.addVertex(0, 0, 0)

#=================================================
# Creation de la grille cylindrique
#=================================================

grille_cyl = doc.makeCylindrical(c, dx, dz, dr, da, dl, nr, na, nl, False)

# on obtient une liste qui contient 4 hexaedres eguaux

# 30° = pi/6
# 60° = pi/3
# 45° = pi/4
# 90° = pi/2
# 180° = pi


#=================================================
# Model tuyau final
#=================================================

model_tuyau_fin = grille_cyl


#model_tuyau_fin.saveVtk("/tmp/tuyau.vtk")
#grille_cyl.saveVtk("/tmp/grille_cyl.vtk")



#===================================================================
# Recuperation des vertex du model hexa tuyau pour l'association
#===================================================================

## NB:
## h = haut
## b = bas
## g = grand
## p = petit
## t = trou


# Face du haut grand rayon

x_mod_h = doc.findVertex( 2*R , 0 , h )
y_mod_h = doc.findVertex( 0 , 2*R , h )
z_mod_h = doc.findVertex( -2*R , 0 , h )
u_mod_h = doc.findVertex( 0 , -2*R , h )

# Face du haut petit rayon

x_mod_h_t = doc.findVertex( R , 0 , h )
y_mod_h_t = doc.findVertex( 0 , R , h )
z_mod_h_t = doc.findVertex( -R , 0 , h )
u_mod_h_t = doc.findVertex( 0 , -R , h )


# Face du bas grand rayon

x_mod_b = doc.findVertex( 2*R , 0 , 0 )
y_mod_b = doc.findVertex( 0 , 2*R , 0 )
z_mod_b = doc.findVertex( -2*R , 0 , 0 )
u_mod_b = doc.findVertex( 0 , -2*R , 0 )

# Face du bas petit rayon

x_mod_b_t = doc.findVertex( R , 0 , 0 )
y_mod_b_t = doc.findVertex( 0 , R , 0 )
z_mod_b_t = doc.findVertex( -R , 0 , 0 )
u_mod_b_t = doc.findVertex( 0 , -R , 0 )


#===================================================================
# Recuperation des points de la geometrie tuyau pour l'association
#===================================================================
Tuyau_geom = geompy.Import(BREP_PATH, "BREP")
geompy.addToStudy(Tuyau_geom, "Tuyau_geom")


Pt_A = geompy.MakeVertex( h , 20 , 4.5 )
Edge_haut_droite_grd = geompy.GetEdgeNearPoint(Tuyau_geom, Pt_A)


Pt_B = geompy.MakeVertex( h , 20 , 3.6 )
Edge_haut_droite_pte = geompy.GetEdgeNearPoint(Tuyau_geom, Pt_B)


Pt_C = geompy.MakeVertex( h , 20 , -4.5 )
Edge_haut_gauche_grd = geompy.GetEdgeNearPoint(Tuyau_geom, Pt_C)



Pt_D = geompy.MakeVertex( h , 20 , -3.6 )
Edge_haut_gauche_pte = geompy.GetEdgeNearPoint(Tuyau_geom, Pt_D)



# NB:
# h = haut
# b = bas
# g = grand
# p = petit
# t = trou

# Face tuyau du haut
x_h = geompy.MakeVertexOnCurve(Edge_haut_droite_grd, 0.5)
y_h = geompy.MakeVertexOnCurve(Edge_haut_droite_grd, 1)
z_h = geompy.MakeVertexOnCurve(Edge_haut_gauche_grd, 0.5)
u_h = geompy.MakeVertexOnCurve(Edge_haut_gauche_grd, 0)



x_h_t = geompy.MakeVertexOnCurve(Edge_haut_droite_pte, 0.5)
y_h_t = geompy.MakeVertexOnCurve(Edge_haut_droite_pte, 1)
z_h_t = geompy.MakeVertexOnCurve(Edge_haut_gauche_pte, 0.5)
u_h_t = geompy.MakeVertexOnCurve(Edge_haut_gauche_pte, 0)



Pt_E = geompy.MakeVertex( 0 , 0 , 2 )
Edge_bas_droite_grd = geompy.GetEdgeNearPoint(Tuyau_geom, Pt_E)


Pt_F = geompy.MakeVertex( 0 , 0 , 1.6 )
Edge_bas_droite_pte = geompy.GetEdgeNearPoint(Tuyau_geom, Pt_F)


Pt_G = geompy.MakeVertex( 0 , 0 , -2 )
Edge_bas_gauche_grd = geompy.GetEdgeNearPoint(Tuyau_geom, Pt_G)



Pt_H = geompy.MakeVertex( 0 , 0 , -1.6 )
Edge_bas_gauche_pte = geompy.GetEdgeNearPoint(Tuyau_geom, Pt_H)



# Face tuyau du bas
x_b = geompy.MakeVertexOnCurve(Edge_bas_droite_grd, 0.5)
y_b = geompy.MakeVertexOnCurve(Edge_bas_droite_grd, 1)
z_b = geompy.MakeVertexOnCurve(Edge_bas_gauche_grd, 0.5)
u_b = geompy.MakeVertexOnCurve(Edge_bas_gauche_grd, 0)



x_b_t = geompy.MakeVertexOnCurve(Edge_bas_droite_pte, 0.5)
y_b_t = geompy.MakeVertexOnCurve(Edge_bas_droite_pte, 1)
z_b_t = geompy.MakeVertexOnCurve(Edge_bas_gauche_pte, 0.5)
u_b_t = geompy.MakeVertexOnCurve(Edge_bas_gauche_pte, 0)




bp_law = doc.getLaw(0)
bp_law.setNodes(4)

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



Nbr_elm_qad_haut = Quad_grp_haut.countElement()
print "Nbr_elm_qad_haut ->",Nbr_elm_qad_haut


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


Nbr_elm_qad_bas = Quad_grp_bas.countElement()
print "Nbr_elm_qad_bas->",Nbr_elm_qad_bas


# groupe de Edges

Edge_Mod_ext_h = doc.findEdge( y_mod_h , y_mod_b )
Edge_Mod_int_h = doc.findEdge( y_mod_h_t , y_mod_b_t )

Edge_Mod_int_b = doc.findEdge( u_mod_h_t , u_mod_b_t )
Edge_Mod_ext_b = doc.findEdge( u_mod_h , u_mod_b )

Edge_Mod_ext_g = doc.findEdge( x_mod_h , x_mod_b )
Edge_Mod_int_g = doc.findEdge( x_mod_h_t , x_mod_b_t )

Edge_Mod_int_d = doc.findEdge( z_mod_h_t , z_mod_b_t )
Edge_Mod_ext_d = doc.findEdge( z_mod_h , z_mod_b )


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
print "doc.countVertex()->",Nbr_Vx

for i in range(Nbr_Vx):
    Vertex_i = doc.getVertex(i)
    Vertex_Nod_Grp.addElement(Vertex_i)


Nbr_elm_vex = Vertex_Nod_Grp.countElement()

if Nbr_elm_vex == Nbr_Vx:
    print "Vertex node group OK (on group :%s  on model : %s)"%(Nbr_elm_vex, Nbr_Vx)
else:
    print "Vertex node group KO (on group :%s  on model : %s)"%(Nbr_elm_vex, Nbr_Vx)


#====================================
# Definir une loi de discretisation
#====================================


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
mesh = hexablock.mesh("test_TUYAU_COURBE_no_assoc", doc)

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
## Maillage hexaédrique
## --------------------
#mesh = mesh.ExportMED( "tuyau.noassoc.med", 1 )
#maillages = smesh.CreateMeshesFromMED( "tuyau.noassoc.med" )

#ijk = maillages[0][0]

#ijk.Hexahedron()

#ijk.Compute()

#print  " --- MAILLAGE HEXAHEDRIQUE --- "
#print "Nombre d hexaedres:", ijk.NbHexas()
#print "Nombre de quadrangles:", ijk.NbQuadrangles()
#print "Nombre de segments:", ijk.NbEdges()
#print "Nombre de noeuds:", ijk.NbNodes()
