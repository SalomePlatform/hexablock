
####### Partial Rind Grid Test ##########

import hexablock


# ======================================================= test_part_rind
def test_part_rind () :

    doc  = hexablock.addDocument ("default")

    center  = doc.addVertex (0,0,0)
    radius  = 8
    radint  = 7
    orig    = doc.addVertex (0,0,0)
    vz      = doc.addVector (0,0,1)
    vx      = doc.addVector (1,0,0)
    angle   = 90
    radhole = 1
    nrad    = 3
    nang    = 14
    nhaut   = 8

    doc.makePartRind (center, vx, vz, radius, radint, radhole, orig, angle,
                      nrad, nang, nhaut)
    doc.saveVtk ("part_rind.vtk")
    return doc

# ================================================================= Begin
doc = test_part_rind  ()
