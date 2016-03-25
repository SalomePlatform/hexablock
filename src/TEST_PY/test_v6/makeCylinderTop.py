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

import hexablock

# ================================================ rename
def rename (elt, pref, indices) :
    if elt == None : return
    elt.setName (pref + indices)
# ================================================ begin
doc = hexablock.addDocument ("model")
nr = 3
na = 8
nh = 3

grid = doc.makeCylinderTop (nr, na, nh)
for i in range (nr+1) :
   for j in range (na) :
      for k in range (nh+1) :
          name  = "(%d,%d,%d)" % (i,j,k)
          vertex =  grid.getVertexIJK (i,j,k)
          ei     =  grid.getEdgeI (i,j,k)
          ej     =  grid.getEdgeJ (i,j,k)
          ek     =  grid.getEdgeK (i,j,k)
          rename (vertex, 'v', name)
          rename (ei,    'ei', name)
          rename (ej,    'ej', name)
          rename (ek,    'ek', name)

doc.save    ("cylinder")
doc.saveVtk ("cylinder.vtk")
doc.addLaws (1, True)
mesh_hexas = hexablock.mesh(doc)
