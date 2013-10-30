
# Hexa : Creation de Rond_4

import hexablock
from hexa_tape import *

# ============================================================== build
def build () :
   name  = 'Rond_4'
   geom  = 'False'
   doc   = add_document (name, geom)

   raz_points ()
   add_point (226.612,459.739,28.05)
   add_point (226.612,459.739,29.75)
   add_point (205.612,459.739,29.75)
   add_point (205.612,459.739,28.05)
   add_point (234.312,459.739,28.05)
   add_point (234.312,459.739,29.75)
   add_point (197.912,459.739,29.75)
   add_point (197.912,459.739,28.05)
   gen_cylinder ()
   return doc

# ============================================================== Begin

if __name__ == '__main__' :
   doc = build ()
   mesh_hexas = mailler ()
