
#### Quad Revolution Test #####

import hexablock
import os


# ================================================================= save_schema
class FileVtk :
    def __init__ (self, doc, radical):
        self.document = doc
        self.count    = 0
        self.radical  = radical

    def save (self) :
        """
        Save the model in a vtk file
        """
        self.count += 1
        file_name = os.path.join(self.radical + str(self.count) + '.vtk')
        self.document.saveVtk(file_name)

# ======================================================= make_grid
def make_grid (doc, nr, na, nl) :

    ori  = doc.addVertex ( 0, 0, 0)
    vx   = doc.addVector ( 1 ,0, 0)
    vz   = doc.addVector ( 0, 0, 1)

    dr = 1
    da = 360
    dl = 1

    grid = doc.makeCylindrical (ori, vx,vz, dr,da,dl, nr,na,nl, False)
    return grid


# ========================================================== test_revolution
def test_revolution () :
    doc  = hexablock.addDocument ("default")
    vtk  = FileVtk (doc, "Revolution");
    nr = 1
    na = 6
    nl = 1
    grid = make_grid (doc, nr, na, nl)
    vtk.save ()

    liste = [ ]
    for nx in range (nr) :
        for ny in range (na) :
            cell = grid.getQuadIJ (nx, ny, nl)
            liste.append (cell);

    center = doc.addVertex (0, -10, 0);
    axis   = doc.addVector (1, 0, 0);
    angles = [5, 10, 15, 20, 30, 20, 15, 10, 5 ]

    vtk.save ()
    bloc = doc.revolutionQuads  (liste, center, axis, angles);
    vtk.save ()
    return doc

# ================================================================= Test
doc = test_revolution ()

