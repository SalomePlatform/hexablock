
# Hexa : Creation de Plaq_2

import hexablock
from hexa_tape import *

# ============================================================== build
def build () :
   name  = 'Plaq_2'
   geom  = 'False'
   doc   = add_document (name, geom)

   raz_points ()
   add_point (481.456,186.68,22.2)
   add_point (481.456,186.68,28.05)
   add_point (463.456,186.68,28.05)
   add_point (463.456,186.68,22.2)
   add_point (489.956,186.68,22.2)
   add_point (489.956,186.68,28.05)
   add_point (454.956,186.68,28.05)
   add_point (454.956,186.68,22.2)
   gen_cylinder ()
   return doc

# ============================================================== Begin

if __name__ == '__main__' :
   doc = build ()
   mesh_hexas = mailler ()
