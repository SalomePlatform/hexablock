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
# ================================================================= save_schema
class FileVtk :
    def __init__ (self, doc, radical):
        self.document = doc
        self.count    = 0
        self.radical  = radical

    def save (self) :
        """
        sauvegarde vtk du modele de bloc
        """
        self.count += 1
        file_name = os.path.join(self.radical + str(self.count) + '.vtk')
        self.document.saveVtk(file_name)

# ======================================================= make_grid
def make_grid (doc, nr, na, nl) :

    ori  = doc.addVertex ( 0, 0, 0)
    vx   = doc.addVector ( 1 ,0, 0)
    vz   = doc.addVector ( 0, 0, 1)

    dr = 1
    da = 360
    dl = 1

    grid = doc.makeCylindrical (ori, vx,vz, dr,da,dl, nr,na,nl, False)
    return grid


# ========================================================== test_revolution
def test_revolution () :
    doc  = hexablock.addDocument ("default")
    vtk  = FileVtk (doc, "Revolution");
    nr = 1
    na = 6
    nl = 1
    grid = make_grid (doc, nr, na, nl)
    vtk.save ()

    liste = [ ]
    for nx in range (nr) :
        for ny in range (na) :
            cell = grid.getQuadIJ (nx, ny, nl)
            print " ... cell = ", cell
            liste.append (cell);

    center = doc.addVertex (0, -10, 0);
    axis   = doc.addVector (1, 0, 0);
    angles = [5, 10, 15, 20, 30, 20, 15, 10, 5 ]

    vtk.save ()
    bloc = doc.revolutionQuads  (liste, center, axis, angles);
    vtk.save ()
    return doc

# ========================================================== test_prism
def test_prism () :
    doc  = hexablock.addDocument("prism")
    vtk  = FileVtk (doc, "prism");
    nr = 1
    na = 6
    nl = 1
    grid = make_grid (doc, nr, na, nl)
    vtk.save ()

    liste = [ ]
    for nx in range (nr) :
        for ny in range (na) :
            cell = grid.getQuadIJ (nx, ny, nl)
            print " ... cell = ", cell
            liste.append (cell);

    axis = doc.addVector (1, 1, 1);

    bloc = doc.prismQuads  (liste, axis, 3)
    vtk.save ()
    return doc

# ========================================================== test_prism
def test_prism_vec () :
    doc  = hexablock.addDocument("prism_vec")
    vtk  = FileVtk (doc, "prism_vec");
    nr = 1
    na = 6
    nl = 1
    grid = make_grid (doc, nr, na, nl)
    vtk.save ()

    liste = [ ]
    for nx in range (nr) :
        for ny in range (na) :
            cell = grid.getQuadIJ (nx, ny, nl)
            print " ... cell = ", cell
            liste.append (cell);

    axis = doc.addVector (1, 1, 1);
    hauteurs = [1, 3, 7, 15]

    bloc = doc.prismQuadsVec (liste, axis, hauteurs, 0)
    vtk.save ()
    return doc

# ================================================================= Begin

###  doc = test_revolution ()
###  doc = test_prism ()
doc = test_prism_vec ()

law = doc.addLaw("Uniform", 3)

for j in range(doc.countPropagation()):
    propa = doc.getPropagation(j)
    propa.setLaw(law) 

mesh_hexas = hexablock.mesh(doc, "maillage:hexas")

