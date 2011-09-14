# -*- coding: latin-1 -*-
# Hexa : Creation d'hexaedres 

import hexablock
import os
import geompy

#---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8
# ================================================================= save_schema
class FileVtk :
    def __init__ (self, doc, radical):
        self.document = doc
        self.count    = 0
        self.radical  = radical

    def save (self) :
        """
        sauvegarde vtk du modele de bloc
        """
        self.count += 1
        file_name = os.path.join(self.radical + str(self.count) + '.vtk')
        self.document.saveVtk(file_name)

# ================================================================= add_grid
def add_grid (doc, size_x, size_y, size_z) :
    orig = doc.addVertex(0, 0, 0)

    vx = doc.addVector (1, 0, 0)
    vy = doc.addVector (0, 1, 0)
    vz = doc.addVector (0, 0, 1)
    grid = doc.makeCartesian (orig, vx, vy, vz, size_x, size_y, size_z)
    return grid

# ========================================================== test_revolution
doc  = hexablock.addDocument()
vtk  = FileVtk (doc, "Revolution");
dimx = 11
dimz = 2
grid = add_grid (doc, dimx, dimx, dimz)
vtk.save ()

mx = dimx/2;
liste = [ ]

for nx in range (dimx) :
    cell = grid.getQuadIJ (nx, mx, dimz)
    liste.append (cell);
    if (nx!=mx) :
       cell = grid.getQuadIJ (mx, nx, dimz); 
       liste.append (cell);
       cell.setScalar (5);

center = doc.addVertex (0, -10, 0);
axis   = doc.addVector (1, 0, 0);
angles = [5, 10, 15, 20, 30, 20, 15, 10, 5 ]

vtk.save ()
bloc = doc.revolutionQuads  (liste, center, axis, angles);
vtk.save ()

