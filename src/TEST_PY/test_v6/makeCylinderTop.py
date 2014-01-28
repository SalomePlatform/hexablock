
import hexablock

# ================================================ rename
def rename (elt, pref, indices) :
    if elt == None : return
    elt.setName (pref + indices)
# ================================================ begin
doc = hexablock.addDocument ("model")
nr = 3
na = 8
nh = 3

grid = doc.makeCylinderTop (nr, na, nh)
for i in range (nr+1) :
   for j in range (na) :
      for k in range (nh+1) :
          name  = "(%d,%d,%d)" % (i,j,k)
          vertex =  grid.getVertexIJK (i,j,k)
          ei     =  grid.getEdgeI (i,j,k)
          ej     =  grid.getEdgeJ (i,j,k)
          ek     =  grid.getEdgeK (i,j,k)
          rename (vertex, 'v', name)
          rename (ei,    'ei', name)
          rename (ej,    'ej', name)
          rename (ek,    'ek', name)

doc.save    ("cylinder")
doc.saveVtk ("cylinder.vtk")
doc.addLaws (1, True)
mesh_hexas = hexablock.mesh(doc)
