
# Hexa : Creation de Rond_2

import hexablock
from hexa_tape import *

# ============================================================== build
def build () :
   name  = 'Rond_2'
   geom  = 'False'
   doc   = add_document (name, geom)

   raz_points ()
   add_point (482.956,186.68,28.05)
   add_point (482.956,186.68,31.05)
   add_point (461.956,186.68,31.05)
   add_point (461.956,186.68,28.05)
   add_point (490.656,186.68,28.05)
   add_point (490.656,186.68,31.05)
   add_point (454.256,186.68,31.05)
   add_point (454.256,186.68,28.05)
   gen_cylinder ()
   return doc

# ============================================================== Begin

if __name__ == '__main__' :
   doc = build ()
   mesh_hexas = mailler ()
