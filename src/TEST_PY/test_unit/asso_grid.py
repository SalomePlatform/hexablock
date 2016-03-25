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
import hexablock
import math

STEP_PATH = os.path.expandvars("$HEXABLOCK_ROOT_DIR/bin/salome/crank.stp")

#==========================================================================

doc = hexablock.addDocument ("default")


orig1 = doc.addVertex ( 0, 0,0);
vz    = doc.addVector (0,0,1);
vx    = doc.addVector (1,0,0);

dr = 1.0
dl = 1.0
nr = 2
nl = 3
na = 8

c1 = doc.makeCylindrical (orig1, vx,vz,dr, 300, dl, nr, na, nl, False);

#====================================
# Definir une loi de discretisation
#====================================
law = doc.addLaw("Uniform", 4)

for j in range(doc.countPropagation()):
    propa = doc.getPropagation(j)
    propa.setLaw(law) 

mesh_hexas = hexablock.mesh(doc, "Grille:hexas")

print "Nombre d hexaedres:", mesh_hexas.NbHexas()
print "Nombre de quadrangles:", mesh_hexas.NbQuadrangles()
print "Nombre de segments:", mesh_hexas.NbEdges()
print "Nombre de noeuds:", mesh_hexas.NbNodes()


