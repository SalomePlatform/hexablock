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

####### Make Pipe Test ########## 

import hexablock
        
doc  = hexablock.addDocument ("Make Pipe Test")

# Simple Pipe -----

nbR = 8
nbA = 10
nbH = 8

pipe0 = doc.makePipeTop (nbR, nbA, nbH)
pipe0.saveVtk ("makePipeTop.vtk")

# Uniform Pipe -----

orig = doc.addVertex (0, 0, 0)
vx   = doc.addVector (1, 0, 0)
vz   = doc.addVector (0, 0, 1)
rint = 1
rext = 3
angle = 360
hauteur = 2

pipe1 = doc.makePipeUni (orig, vx, vz, rint, rext, angle, hauteur, nbR, nbA, nbH)
pipe1.saveVtk ("makePipeUni.vtk");
   

# Custom Pipe -----

origin = doc.addVertex (0, 5, 0)
tr = [10, 20, 30, 40]
ta = [45, 90, 135, 180, 225]
th = [5, 30, 40, 60]

pipe2 = doc.makePipe(origin, vx, vz, tr, ta, th)
pipe2.saveVtk("makePipe.vtk")
