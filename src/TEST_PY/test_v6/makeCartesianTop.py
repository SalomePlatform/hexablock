
import hexablock

doc = hexablock.addDocument ("model")

doc.makeCartesianTop (6, 4, 2)

doc.saveVtk ("model1.vtk")
