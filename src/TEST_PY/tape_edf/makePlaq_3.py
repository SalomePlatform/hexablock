
# Hexa : Creation de Plaq_3

import hexablock
from hexa_tape import *

# ============================================================== build
def build () :
   name  = 'Plaq_3'
   geom  = 'False'
   doc   = add_document (name, geom)

   raz_points ()
   add_point (379.043,348.04,22.2)
   add_point (379.043,348.04,28.05)
   add_point (361.043,348.04,28.05)
   add_point (361.043,348.04,22.2)
   add_point (387.543,348.04,22.2)
   add_point (387.543,348.04,28.05)
   add_point (352.543,348.04,28.05)
   add_point (352.543,348.04,22.2)
   gen_cylinder ()
   return doc

# ============================================================== Begin

if __name__ == '__main__' :
   doc = build ()
   mesh_hexas = mailler ()
