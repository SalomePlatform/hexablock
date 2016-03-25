# -*- coding: utf-8 -*-
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

####### Make Cylinders Test ########## 

import hexablock
import math

    
doc  = hexablock.addDocument ("Make Cylinders Test")

rext1 =  2*math.sqrt (2.0)
rext2 =  3*math.sqrt (2.0)
h1   =  16
h2   =  16
xl1  =  -8

orig1  = doc.addVertex ( 0, 0,  xl1)
orig2  = doc.addVertex (-8, 0,  0)
vz1   = doc.addVector ( 0, 0,  1)
vz2   = doc.addVector ( 1, 0,  0)
   
cylinders = doc.makeCylinders (orig1, vz1, rext1, h1, orig2, vz2, rext2, h2)
cylinders.saveVtk("makeCylinders.vtk")
