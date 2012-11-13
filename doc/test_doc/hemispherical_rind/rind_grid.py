
####### Rind Grid Test ##########

import hexablock


# ======================================================= test_rind
def test_rind () :

    doc  = hexablock.addDocument ("default")

    center  = doc.addVertex (0,0,0)
    radius  = 8
    radint  = 7
    orig    = doc.addVertex (0,0,0)
    vz      = doc.addVector (0,0,1)
    vx      = doc.addVector (1,0,0)
    radhole = 1
    nrad    = 3
    nang    = 16
    nhaut   = 8

    doc.makeRind (center, vx, vz, radius, radint, radhole, orig, 
                  nrad, nang, nhaut)
    doc.saveVtk ("rind.vtk")
    return doc

# ================================================================= Test
doc = test_rind  ()
