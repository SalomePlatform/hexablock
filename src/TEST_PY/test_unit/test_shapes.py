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

# Francis KLOSS : 2011-2013 : CEA-Saclay, DEN, DM2S, SFME, LGLS, F-91191 Gif-sur-Yvette, France
# =============================================================================================

import math
import hexablock
geompy = hexablock.geompy

# Construire le modèle de bloc
# ============================

doc = hexablock.addDocument("bielle")

# Charger la géométrie
# ====================

bielle = geompy.ImportSTEP("bielle.stp")
shape  = doc.addShape (bielle, "bielle")


# Sélectionner des sous-parties de la géométrie
# ---------------------------------------------

sommets = geompy.SubShapeAllIDs (bielle, geompy.ShapeType["VERTEX"])
aretes  = geompy.SubShapeAllIDs (bielle, geompy.ShapeType["EDGE"])

print " ... Aretes  = ", aretes
print " ... Sommets = ", sommets

nbfaces  = shape.countFace ()
nbedges  = shape.countEdge ()
nbvertex = shape.countVertex ()

for n in range (nbfaces) :
    print " Face nro %d = %s" % (n , shape.getNameFace (n))
print

for n in range (nbedges) :
    print " Edge nro %d = %s" % (n , shape.getNameEdge (n))
print

for n in range (nbvertex) :
    print " Vertex nro %d = %s" % (n , shape.getNameVertex (n))



