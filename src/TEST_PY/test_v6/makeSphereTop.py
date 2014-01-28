
import hexablock

doc = hexablock.addDocument ("model")

doc.makeSphereTop (3, 8, 6)

doc.saveVtk ("model1.vtk")
