######## Make Symmetry Point ########

import hexablock


# ======================================================= make_grid
def make_grid (doc) :

    ori  = doc.addVertex ( 0, 0, 0)
    vz   = doc.addVector ( 0, 0, 1)
    vx   = doc.addVector ( 1 ,0, 0)

    dr = 1
    da = 360
    dl = 1

    nr = 1
    na = 6
    nl = 1
    grid = doc.makeCylindrical (ori, vx,vz, dr,da,dl, nr,na,nl, False)
    #####  doc .saveVtk ("cyl_grid.vtk")
    return grid


# ======================================================= test_sym_point
def test_sym_point () :

    doc  = hexablock.addDocument ("default")
    grid = make_grid (doc)

    orig = grid.getVertex(3)
    grid2 = doc.makeSymmetryPoint (grid, orig)

    #####  doc .saveVtk ("sym_point.vtk")
    return doc
    

# ================================================================= Begin
doc = test_sym_point  ()

