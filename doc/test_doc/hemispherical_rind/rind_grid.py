# -*- coding: latin-1 -*-
# Copyright (C) 2009-2013  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
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

####### Rind Grid Test ##########

import hexablock


# ======================================================= test_rind
def test_rind () :

    doc  = hexablock.addDocument ("default")

    center  = doc.addVertex (0,0,0)
    radius  = 8
    radint  = 7
    orig    = doc.addVertex (0,0,0)
    vz      = doc.addVector (0,0,1)
    vx      = doc.addVector (1,0,0)
    radhole = 1
    nrad    = 3
    nang    = 16
    nhaut   = 8

    doc.makeRind (center, vx, vz, radius, radint, radhole, orig, 
                  nrad, nang, nhaut)
    #####  doc.saveVtk ("rind.vtk")
    return doc

# ================================================================= Test
doc = test_rind  ()
