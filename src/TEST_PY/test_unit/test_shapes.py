# -*- coding: latin-1 -*-

# Francis KLOSS : 2011-2012 : CEA-Saclay, DEN, DM2S, SFME, LGLS, F-91191 Gif-sur-Yvette, France
# =============================================================================================

import math

import geompy
import hexablock

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



