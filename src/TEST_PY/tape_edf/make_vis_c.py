
# Hexa : Generation de la vis C4 (emplace make_VisC_4.py)

import hexablock

# ============================================================== build
def build () :

   nom   = 'VisC_4'
   doc   = hexablock.addDocument (nom)

   x0 = 216.112
   y0 = 459.739

   tx = [217.362, 223.762, 224.112, 224.262,
         224.812, 225.312, 230.362, 237.362]
   tr = [1.25, 7.65, 8.0, 8.15, 8.7, 9.2, 14.25, 21.25]
   tz = [ -29.25, -25.25, -3.75, 13, 29.75, 37.75, 48.25, 61.75]

   ta = [ 0, 90, 180, 270, 360 ]
   tr = [1.25, 8.7, 14.25, 21.25]
   tz = [ -29.25, -3.75, 13, 29.75, 37.75, 48.25, 61.75]

   orig = doc.addVertex (x0, y0, 0)
   vx   = doc.addVector (1, 0, 0)
   vz   = doc.addVector (0, 0, 1)
   grid = doc.makePipe (orig, vx, vz, tr, ta, tz)
   doc.saveVtk ('visa01.vtk');

   for na in range (4) :
       grid.getHexaIJK (2, na, 5).remove()
       grid.getHexaIJK (2, na, 4).remove()
       grid.getHexaIJK (1, na, 4).remove()
       for nz in range (3) :
           grid.getHexaIJK (2, na, nz).remove()
           grid.getHexaIJK (1, na, nz).remove()

   doc.saveVtk ('visa02.vtk');
   return doc

# ============================================================== Begin


if __name__ == '__main__' :
   doc = build()
   law = doc.addLaw("Uniform", 5)

   for j in range(doc.countPropagation()):
       propa = doc.getPropagation(j)
       propa.setLaw(law)

   mesh_hexas = hexablock.mesh(doc)

### doc.dump()


