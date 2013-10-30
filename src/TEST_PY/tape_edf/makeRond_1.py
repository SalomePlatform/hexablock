
# Hexa : Creation de Rond_1

import hexablock
from hexa_tape import *

# ============================================================== build
def build () :
   name  = 'Rond_1'
   geom  = 'False'
   doc   = add_document (name, geom)

   raz_points ()
   add_point (518.5,0,28.05)
   add_point (518.5,0,31.05)
   add_point (497.5,1.28588e-15,31.05)
   add_point (497.5,1.28588e-15,28.05)
   add_point (526.2,0,28.05)
   add_point (526.2,0,31.05)
   add_point (489.8,2.22886e-15,31.05)
   add_point (489.8,2.22886e-15,28.05)
   gen_cylinder ()
   return doc

# ============================================================== Begin

if __name__ == '__main__' :
   doc = build ()
   mesh_hexas = mailler ()
