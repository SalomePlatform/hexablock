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
geompy = hexablock.geompy
## import os
## import math

nbr_fic   = 0
the_name  = "pave_troue"
the_doc   = None
the_grid  = None
                       ## Constantes
Epsilon  = 1e-3
BadIndex = -1
DirX = 0
DirY = 1
DirZ = 2
X    = 100000.0
X0   = X-1
HOK  = 0
HERR = 1
YES  = 1
NO   = 0
BOF  = -1
                       ## Parametrage
db             = True
maillage_actif = True

# ======================================================= save_vtk
def save_vtk () :

    global nbr_fic
    nbr_fic += 1
    filename = "%s_%02d.vtk" % (the_name,nbr_fic)
    the_doc.saveVtk (filename)


# ======================================================= make_toit
def make_toit () :

    ichem  = 5
    jchem  = 3

    for ni in range (imax) :
        for nj in range (jmax) :
            if ni != ichem or nj != jchem :
               the_doc.removeHexa (the_grid.getHexaIJK (ni,nj,kmax))

# ======================================================= make_trou1
def make_trou1 () :

    ic1   = 2
    jc1   = 2
    nc1   = 3

    for ni in range (nc1) :
        for nj in range (nc1) :
            for nk in range (kmax) :
                the_doc.removeHexa (the_grid.getHexaIJK (ic1+ni, jc1+nj, nk))

    xc1   = 65
    yc1   = 25
    orig1 = the_doc.addVertex (xc1, yc1, 0)
    vxy   = the_doc.addVector (1,1,0)

    rayon  = 5
    rext   = 8
    nbsect = 12
    theta    = 360.0 / nbsect
    cyl1_ray = [ rayon, rext]
    cyl1_ang = []
    for na in range (nbsect+1) :
        cyl1_ang.append (na*theta)
    cyl1_h = tab_z
    cyl1_h.pop()
    cyl1_h.insert (0,0)
    cyl1 = the_doc.makePipe (orig1, vxy, vz, cyl1_ray, cyl1_ang, cyl1_h)

                     ### Test visu

    va1 = cyl1    .getVertexIJK (1,         1,       0)
    va2 = cyl1    .getVertexIJK (1,         1,       1)
    vc1 = the_grid.getVertexIJK (ic1+nc1-1, jc1+nc1, 0)
    vc2 = the_grid.getVertexIJK (ic1+nc1-1, jc1+nc1, 1)

    cible = the_grid.getQuadIK (ic1+2, jc1+nc1, 0)
    tab_quads = []
    for nk in range (len(cyl1_h)-1) :
        for nj in range (len(cyl1_ang)-1) :
            tab_quads.append (cyl1.getQuadJK (1, nj, nk))

    if db :
       va1.setColor (5)
       vc1.setColor (5)
       save_vtk ()

       va2.setColor (5)
       vc2.setColor (5)
       save_vtk ()

       cible.setColor        (5)
       tab_quads[0].setColor (5)
       save_vtk ()
                     ### Jointure 

    the_doc.joinQuadsUni (tab_quads, cible, va1, vc1, va2, vc2, 1)
    hexablock.what () 
    return cyl1

# ======================================================= make_trou2
def make_trou2 () :

    ic2   = 7
    kc2   = 1
    for nj in range (jmax) :
        the_doc.removeHexa (the_grid.getHexaIJK (ic2, nj, kc2))

    xc2   = 105
    zc2   = 20
    orig2 = the_doc.addVertex (xc2, 0, zc2)
    vxz   = the_doc.addVector (1,0,1)

    rayon  = 5
    rext   = 8
    nbsect = 4
    theta  = 360.0 / nbsect
    cyl2_ray = [ rayon, rext]
    cyl2_ang = []
    for na in range (nbsect+1) :
        cyl2_ang.append (na*theta)
    cyl2_h = tab_y
    cyl2_h.insert (0,0)
    cyl2 = the_doc.makePipe (orig2, vxz, vy, cyl2_ray, cyl2_ang, cyl2_h)
                     ### Test visu

    va1 = cyl2    .getVertexIJK (1,     0, 0)
    va2 = cyl2    .getVertexIJK (1,     0, 1)
    vc1 = the_grid.getVertexIJK (ic2+1, 0, kc2+1)
    vc2 = the_grid.getVertexIJK (ic2+1, 1, kc2+1)

    cible = the_grid.getQuadJK (ic2+1, 0, kc2)
    tab_quads = []
    for nk in range (len(cyl2_h)-1) :
        for nj in range (len(cyl2_ang)-1) :
            tab_quads.append (cyl2.getQuadJK (1, nj, nk))

    if db :
       va1.setColor (5)
       vc1.setColor (5)
       save_vtk ()

       va2.setColor (5)
       vc2.setColor (5)
       save_vtk ()

       cible.setColor        (5)
       tab_quads[0].setColor (5)
       save_vtk ()
                     ### Jointure 

    the_doc.joinQuadsUni (tab_quads, cible, va1, vc1, va2, vc2, 1)
    hexablock.what () 
    return cyl2

# ======================================================= trait_vertical
def trait_vertical () :

    ivert  = 3
    for nk in range (kmax) :
        for nj in range (2) :
            the_doc.removeHexa (the_grid.getHexaIJK (ivert,nj,nk))

# ================================================================= make_pave
def make_pave () :

    global the_doc, the_grid, tab_y, tab_z, vz, vy

    the_doc  = hexablock.addDocument (the_name)
    vx   = the_doc.addVector (1,0,0)
    vy   = the_doc.addVector (0,1,0)
    vz   = the_doc.addVector (0,0,1)
    orig = the_doc.addVertex (0, 0, 0)
    orig.setColor (5)

    tab_x = [50, 62.5, 67.5, 70, 75, 85, 105, 120]
    tab_y = [10, 15, 22.5, 25, 27.5, 40]
    tab_z = [25, 40, 50, 53]
                                            ### Modifs 
    tab_x  = [50, 55, 62.5, 67.5, 75, 85, 95, 115, 120]
    tab_y  = [10, 15, 22.5, 27.5, 35, 40]
    tab_z  = [10, 30, 40, 50, 53]

    rayon = 5

    the_grid = the_doc.makeCartesian (orig, vx, vy, vz, tab_x, tab_y, tab_z)
    save_vtk ()

    global imax,  jmax,  kmax 
    imax = len (tab_x)
    jmax = len (tab_y)
    kmax = len (tab_z) - 1

    make_toit ()

    the_doc.removeHexa (the_grid.getHexaIJK (0,0,kmax-1))

                                   ###    Trou 1
    make_trou1 ()
    save_vtk ()
                                   ###    Trou 2
    make_trou2 ()
    save_vtk ()
                                ### Les trous
                                ### Maillage
    trait_vertical ()

    if maillage_actif :
       print "  ________________________ Maillage"
       the_doc.addLaws (2, True)
       mesh_hexas = hexablock.mesh(the_doc)

    the_doc.purge ()
    the_doc.save (the_name)
    save_vtk ()
    save_vtk ()
    save_vtk ()
    save_vtk ()
    save_vtk ()
    return the_doc

# ================================================================= Begin
doc = make_pave  ()



