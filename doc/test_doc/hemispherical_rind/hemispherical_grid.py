
####### Hemispherical Grid Test ##########

import hexablock


# ======================================================= test_sphere
def test_sphere () :

    doc  = hexablock.addDocument ("default")

    center  = doc.addVertex (0,0,0)
    radius  = 8
    orig    = doc.addVertex (0,0,0)
    vz      = doc.addVector (0,0,1)
    vx      = doc.addVector (1,0,0)
    radhole = 1
    nrad    = 3
    nang    = 16
    nhaut   = 8

    doc.makeSphere (center, vx, vz, radius, radhole, orig, nrad, nang, nhaut)
    doc.saveVtk ("hemispherical.vtk")
    return doc

# ================================================================= Test
doc = test_sphere  ()

