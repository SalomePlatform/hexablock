# -*- coding: utf-8 -*-
####### Test for Cut Hexa ###############

import hexablock


doc = hexablock.addDocument("Cut Hexa Test")

size_x = 2
size_y = 1
size_z = 1

grid  = doc.makeCartesianTop (size_x,size_y,size_z)
arete = grid.getEdgeK (0, 0, 0)

doc.saveVtk ("decoupe1.vtk")
nbCuts = 2
grid2 = doc.cutUni (arete, nbCuts)
doc.saveVtk ("decoupe2.vtk")

#tablen = []
#reste  = 1
#abscisse = 0
#for nl in range(5):
#    reste     /= 2
#    abscisse  += reste
#    tablen.append (abscisse)
tablen = [2, 3, 8, 14, 18]
arete = grid2.getEdge(0)
grid3 = doc.cut(arete, tablen)
doc.saveVtk ("decoupe3.vtk")
