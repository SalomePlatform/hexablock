
# Hexa : Creation de Plaq_1

import hexablock
from hexa_tape import *

# ============================================================== build
def build () :
   name  = 'Plaq_1'
   geom  = 'False'
   doc   = add_document (name, geom)

   raz_points ()
   add_point (517,0,22.2)
   add_point (517,0,28.05)
   add_point (499,1.10218e-15,28.05)
   add_point (499,1.10218e-15,22.2)
   add_point (525.5,0,22.2)
   add_point (525.5,0,28.05)
   add_point (490.5,2.14313e-15,28.05)
   add_point (490.5,2.14313e-15,22.2)
   gen_cylinder ()
   return doc

# ============================================================== Begin

if __name__ == '__main__' :
   doc = build ()
   mesh_hexas = mailler ()
