# !/bin/python
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

# Hexa : Creation d'hexaedres 

import hexablock
import os

#---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8

doc  = hexablock.addDocument ("default")
vx   = doc.addVector (1,0,0)
vy   = doc.addVector (0,1,0)
vz   = doc.addVector (0,0,1)

vxy  = doc.addVector (1,1,0)

nbr_files = 0


# ======================================================= save_vtk
def save_vtk () :

    global nbr_files
    nom = "lecas%d.vtk" % nbr_files
    nbr_files += 1
    doc.saveVtk (nom)

# ======================================================= carre
def carre (x) :
    return x*x

# ======================================================= get_center
def get_center (quad) :
    px = 0
    py = 0
    pz = 0
    for nv in range (4) :
        vertex = quad.getVertex (nv) 
        px += vertex.getX() / 4
        py += vertex.getY() / 4
        pz += vertex.getZ() / 4
    return [ px, py, pz ]
# ======================================================= nearest
def nearest (grid, vertex) :
    nbre = grid.countVertex()
    dmin   = 1e+6
    result = None
    px = vertex.getX()
    py = vertex.getY()
    pz = vertex.getZ()
    for nro in range (nbre) :
        v1 = grid.getVertex (nro)
        d2 = carre(px-v1.getX()) + carre(py-v1.getY()) + carre(pz-v1.getZ()) 
        if (d2 < dmin) :
           result = v1
           dmin   = d2

    print  vertex.getName () , px, py, pz, " -> ", result.getName()
    return result

# ======================================================= nearest_quad
def nearest_quad (grid, quad) :
    dmin   = 1e+16
    result = None
    [ox, oy, oz]   = get_center (quad)
    nbre   = grid.countQuad ()
    for nro in range (nbre) :
        q1  = grid.getQuad (nro)
        if q1 != None :
           [px, py, pz] = get_center (q1)
           d2 = carre(px-ox) + carre(py-oy) + carre(pz-oz) 
           if (d2 < dmin) :
              result = q1
              dmin   = d2

    print  quad.getName () , px, py, pz, " -> ", result.getName()
    return result

# ======================================================= insert_cylinder
def insert_cylinder (plaque, nx, ny) :

    hexa   = plaque.getHexaIJK (nx,   ny,   0)
    xmin =  666  ;   ymin = xmin ;   zmin = xmin
    xmax = -666  ;   ymax = xmax ;   zmax = xmax

    tabv1 = []
    for nv in range (8) :
        node = hexa.getVertex (nv)
        xmin = min (xmin, node.getX())   ;  xmax = max (xmax, node.getX())
        ymin = min (ymin, node.getY())   ;  ymax = max (ymax, node.getY())
        zmin = min (zmin, node.getZ())   ;  zmax = max (zmax, node.getZ())
        tabv1.append (node)
        
    doc.removeHexa (hexa)
    save_vtk ()

    dx    = (xmax - xmin)/2
    dz    = (zmax - zmin)/2
    xorig = (xmin + xmax)/2 
    yorig = (ymin + ymax)/2 
    zorig = (zmin + zmax)/2 - dz

    orig = doc.addVertex (xorig, yorig, zorig)
    nr = 1
    na = 4
    nh = 1
    rext  = dx
    rint  = rext/2
    haut  = 1
    angle = 360
    pipe  = doc.makePipeUni (orig, vxy,vz, rint,rext,angle,haut, nr,na,nh)

    hexablock.what () 

    tabquad  = []
    tabv0 = []
    for nq in range (4) :
        quad = pipe.getQuadJK (1, nq, 0)
        tabquad.append (quad)

    print  " .. tabquad[0] = ", tabquad[0].getName ()
    cible = nearest_quad (plaque, tabquad[0])
    tabquad[0]. setColor (5)
    cible . setColor (5)
    save_vtk ()

    va1   = tabquad[0].getVertex (0)
    va2   = tabquad[0].getVertex (1)
    vb1   = cible.nearestVertex  (va1)
    vb2   = cible.nearestVertex  (va2)
    doc.setLevel (1)
    doc.joinQuadsUni (tabquad, cible, va1, vb1, va2, vb2, 1)
    hexablock.what () 
    save_vtk ()

    return
    doc.setLevel (1)
    for nv in range (8) :
        ier = doc.mergeVertices (tabv0[nv], tabv1[nv])
        print "ier = ", ier
        save_vtk ()


# ======================================================= test_2013
def test_2013 () :

    orig = doc.addVertex (0,0,0)

    lx  = 3
    ly  = lx
    lz  = 1
    nx  = 3
    ny  = nx
    nz  = 1

    plaque = doc.makeCartesianUni (orig, vx,vy,vz, lx, ly, lz, nx,ny,nz)
    save_vtk ()

    insert_cylinder (plaque, 1, 1)
    return doc

# ================================================================= Begin

doc = test_2013  ()
doc.addLaws (0.1, True)

mesh_hexas = hexablock.mesh (doc)
