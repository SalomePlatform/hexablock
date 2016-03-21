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

# ======================================================= test_disco_edges
def test_disco_edges () :

    nom  = "test_disco_edges"
    doc  = hexablock.addDocument(nom)
    doc.setLevel (0)

    ori = doc.addVertex ( 0, 0, 0)
    vz  = doc.addVector ( 0, 0, 1)
    vy  = doc.addVector ( 0, 1, 1)
    vx  = doc.addVector ( 1 ,0, 0)

    size_x = 1
    size_y = 2
    size_z = 2

    grid = doc.makeCartesian (ori, vx, vy, vz, size_x, size_y, size_z)

    t_hexas = []
    t_edges = []
    doc.setLevel (1)
    
    for nk in range (size_z) :
        t_hexas.append (grid.getHexaIJK (0,0,nk))
        t_edges.append (grid.getEdgeK   (0,1,nk))

    disco_edges =  doc.disconnectEdges (t_hexas, t_edges)
    return doc

# ================================================================= Begin

doc = test_disco_edges  ()

law = doc.addLaw("Uniform", 4)

for j in range(doc.countPropagation()):
    propa = doc.getPropagation(j)
    propa.setLaw(law) 

mesh_hexas = hexablock.mesh(doc)
muv, mue, muq, muh = hexablock.dump(doc, mesh_hexas)
