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

####### Make Cylinders Test ########## 

import hexablock

    
doc  = hexablock.addDocument ("default")

ori1 = doc.addVertex ( 0, 0, 0)
ori2 = doc.addVertex (-5, 0, 5)
vz   = doc.addVector ( 0, 0, 1)
vx   = doc.addVector ( 1 ,0, 0)

nr1 = 2
nl1 = 10
nr2 = 1
nl2 = 10

cyl1 = doc.addCylinder (ori1, vz, nr1, nl1)
cyl2 = doc.addCylinder (ori2, vx, nr2, nl2)
elts = doc.makeCylinders (cyl1, cyl2)

#####  doc.saveVtk ("make_cylinders.vtk")
