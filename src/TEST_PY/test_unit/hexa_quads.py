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

# ================================================================= add_grid
def add_grid (doc, size_x, size_y, size_z) :
    orig = doc.addVertex(0, 0, 0)

    vx = doc.addVector (1, 0, 0)
    vy = doc.addVector (0, 1, 0)
    vz = doc.addVector (0, 0, 1)
    grid = doc.makeCartesian (orig, vx, vy, vz, size_x, size_y, size_z)
    return grid

# ========================================================== test_hexa_quads_5
def test_hexa_quads_5 () :
    doc  = hexablock.addDocument ("default")
    vtk  = FileVtk (doc, "HexaQuads5");
    grid = add_grid (doc, 3, 3, 2)
    vtk.save ()

    hexa = grid.getHexaIJK (1,1,1)

    qa = hexa.getQuad (0)
    qb = hexa.getQuad (1)
    qc = hexa.getQuad (2)
    qd = hexa.getQuad (3)
    qe = hexa.getQuad (4)
    qf = hexa.getQuad (5)

    doc.removeQuad (qb)
    vtk.save ()

    doc.addHexa5Quads (qa, qc, qd, qe, qf);
    vtk.save ()

    doc.removeElements (grid)
    vtk.save ()

# ========================================================== test_hexa_quads_ab
def test_hexa_quads_ab () :
    doc  = hexablock.addDocument ("default")
    vtk  = FileVtk (doc, "HexaQuadsAB");
    grid = add_grid (doc, 1, 1, 3)
    vtk.save ()

    hexa = grid.getHexaIJK (0,0,1)

    qa = hexa.getQuad (0)
    qb = hexa.getQuad (1)
    qc = hexa.getQuad (2)
    qd = hexa.getQuad (3)
    qe = hexa.getQuad (4)
    qf = hexa.getQuad (5)

    doc.removeQuad (qc)
    doc.removeQuad (qd)
    doc.removeQuad (qe)
    doc.removeQuad (qf)
    vtk.save ()

    doc.addHexa2Quads (qa, qb)
    vtk.save ()

    doc.removeElements (grid)
    vtk.save ()

# ========================================================== test_hexa_quads_ac
def test_hexa_quads_ac () :
    doc  = hexablock.addDocument ("default")
    vtk  = FileVtk (doc, "HexaQuadsAC");
    grid = add_grid (doc, 2, 1, 2)
    vtk.save ()

    hexa = grid.getHexaIJK (1,0,1)

    qa = hexa.getQuad (0)
    qb = hexa.getQuad (1)
    qc = hexa.getQuad (2)
    qd = hexa.getQuad (3)
    qe = hexa.getQuad (4)
    qf = hexa.getQuad (5)

    doc.removeQuad (qb)
    doc.removeQuad (qc)
    doc.removeQuad (qd)
    doc.removeQuad (qf)
    vtk.save ()

    doc.addHexa2Quads (qa, qe)
    vtk.save ()

    doc.removeElements (grid)
    vtk.save ()

# ======================================================= test_hexa_quads_acd
def test_hexa_quads_acd () :

    doc  = hexablock.addDocument ("default")
    vtk  = FileVtk (doc, "HexaQuadsACD");
    grid = add_grid (doc, 3, 1, 2)
    vtk.save ()

    hexa = grid.getHexaIJK (1,0,1)

    qa = hexa.getQuad (0)
    qb = hexa.getQuad (1)
    qc = hexa.getQuad (2)
    qd = hexa.getQuad (3)
    qe = hexa.getQuad (4)
    qf = hexa.getQuad (5)

    doc.removeQuad (qb)
    doc.removeQuad (qc)
    doc.removeQuad (qd)
    vtk.save ()

    doc.addHexa3Quads (qa, qe, qf)
    vtk.save ()

    doc.removeElements (grid)
    vtk.save ()


# ======================================================= test_hexa_quads_ace
def test_hexa_quads_ace () :

    doc  = hexablock.addDocument ("default")
    vtk  = FileVtk (doc, "HexaQuadsACE");
    grid = add_grid (doc, 2, 2, 2)
    vtk.save ()

    hexa = grid.getHexaIJK (1,0,1)

    qa = hexa.getQuad (0)
    qb = hexa.getQuad (1)
    qc = hexa.getQuad (2)
    qd = hexa.getQuad (3)
    qe = hexa.getQuad (4)
    qf = hexa.getQuad (5)

    doc.removeQuad (qb)
    doc.removeQuad (qc)
    doc.removeQuad (qf)
    vtk.save ()

    doc.addHexa3Quads (qa, qd, qe)
    vtk.save ()

    doc.removeElements (grid)
    vtk.save ()

# ======================================================= test_hexa_quads_abcd
def test_hexa_quads_abcd () :

    doc  = hexablock.addDocument ("default")
    vtk  = FileVtk (doc, "HexaQuadsABCD");
    grid = add_grid (doc, 3, 1, 3)
    vtk.save ()

    hexa = grid.getHexaIJK (1,0,1)

    qa = hexa.getQuad (0)
    qb = hexa.getQuad (1)
    qc = hexa.getQuad (2)
    qd = hexa.getQuad (3)
    qe = hexa.getQuad (4)
    qf = hexa.getQuad (5)

    doc.removeQuad (qc)
    doc.removeQuad (qd)
    vtk.save ()

    doc.addHexa4Quads (qa, qb, qe, qf)
    vtk.save ()

    doc.removeElements (grid)
    vtk.save ()

# ======================================================= test_hexa_quads_abce
def test_hexa_quads_abce () :

    doc  = hexablock.addDocument ("default")
    vtk  = FileVtk (doc, "HexaQuadsABCE");
    grid = add_grid (doc, 3, 2, 2)
    vtk.save ()

    hexa = grid.getHexaIJK (1,0,1)

    qa = hexa.getQuad (0)
    qb = hexa.getQuad (1)
    qc = hexa.getQuad (2)
    qd = hexa.getQuad (3)
    qe = hexa.getQuad (4)
    qf = hexa.getQuad (5)

    doc.removeQuad (qc)
    doc.removeQuad (qb)
    vtk.save ()

    doc.addHexa4Quads (qa, qd, qe, qf)
    vtk.save ()

    doc.removeElements (grid)
    vtk.save ()


# ================================================================= Begin

test_hexa_quads_5  ()
test_hexa_quads_ab ()
test_hexa_quads_ac ()
test_hexa_quads_acd ()
test_hexa_quads_ace ()
test_hexa_quads_abcd ()
test_hexa_quads_abce ()
