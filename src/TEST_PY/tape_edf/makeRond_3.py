
# Hexa : Creation de Rond_3

import hexablock
from hexa_tape import *

# ============================================================== build
def build () :
   name  = 'Rond_3'
   geom  = 'False'
   doc   = add_document (name, geom)

   raz_points ()
   add_point (380.543,348.04,28.05)
   add_point (380.543,348.04,31.05)
   add_point (359.543,348.04,31.05)
   add_point (359.543,348.04,28.05)
   add_point (388.243,348.04,28.05)
   add_point (388.243,348.04,31.05)
   add_point (351.843,348.04,31.05)
   add_point (351.843,348.04,28.05)
   gen_cylinder ()
   return doc

# ============================================================== Begin

if __name__ == '__main__' :
   doc = build ()
   mesh_hexas = mailler ()
