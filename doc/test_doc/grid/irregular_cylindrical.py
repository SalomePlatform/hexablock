    
####### Make Irregular Cylindrical Test ########## 

import hexablock

    
doc  = hexablock.addDocument ("cyl:hexas")

orig = doc.addVertex (0, 0, 0)
vz   = doc.addVector (0, 0, 1)
vx   = doc.addVector (1 ,0, 0)


tdr = [ 1, 1, 1, 1 ]
tda = [ 45, 45,  45, 45,  45, 45,  45, 45 ]
tdl = [ 1, 1, 1  ]

tdr = [ 1, 2, 0.5 ]
tda = [ 30, 30, 30, 10, 10, 10,  10, 10, 10,  10, 10, 10,
        30, 30, 30, 10, 10, 10,  10, 10, 10,  10, 10, 10 ]
tdl = [ 1, 2, 3  ]


c1 = doc.makeCylindricals (orig, vx,vz, tdr, tda, tdl, False)

#####  doc.saveVtk ("cylindricals.vtk")
