
####### Partial Hemispherical Grid Test ##########

import hexablock


# ======================================================= test_part_sphere
def test_part_sphere () :

    doc  = hexablock.addDocument ("default")

    center  = doc.addVertex (0,0,0)
    radius  = 8
    orig    = doc.addVertex (0,0,-4)
    vz      = doc.addVector (0,0,1)
    vx      = doc.addVector (1,0,0)
    angle   = 90
    radhole = 1
    nrad    = 3
    nang    = 4
    nhaut   = 8

    doc.makePartSphere (center, vx, vz, radius, radhole, orig, angle, 
                        nrad, nang, nhaut)
    doc.saveVtk ("partial_hemispherical.vtk")
    return doc

# ================================================================= Test
doc = test_part_sphere  ()

