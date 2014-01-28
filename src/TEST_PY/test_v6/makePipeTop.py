
import hexablock

doc = hexablock.addDocument ("model")

doc.makePipeTop (3, 8, 3)

###  doc.saveVtk ("model1.vtk")
doc.addLaws (1, True)
mesh_hexas = hexablock.mesh(doc)
