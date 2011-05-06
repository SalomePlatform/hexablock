#

import os
import GEOM
import SALOMEDS
import hexablock

print "test propagation..."
doc = hexablock.addDocument()

size_x = 2
size_y = 1
size_z = 1

orig = doc.addVertex(0, 0, 0)
dirVr = doc.addVector(1, 1, 1)

grid = doc.makeCartesian1(orig, dirVr, size_x, size_y, size_z, 0, 0,
                           0)

for nro in range(doc.countPropagation()):
    prop = doc.getPropagation(nro)
    edges = prop.getEdges()

    print("____________________________________ Prop nro %d" % (nro))
    for edge in edges:
# test impossible en python car impossible de recuperer le nom des vertex
        if edge.getWay():
            print "( ", edge.getVertex(0).name, ",", edge.getVertex(1).name, " )"
        else:
            print "( ", edge.getVertex(1).getName(), ",", edge.getVertex(0).getName(), " )"
        pass
    pass



file_name = os.path.join(os.environ['TMP'], 'propagation.vtk')
doc.saveVtk(file_name)

print "...test propagation OK"


