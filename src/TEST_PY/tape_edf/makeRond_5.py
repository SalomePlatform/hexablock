
# Hexa : Creation de Rond_5

import hexablock
from hexa_tape import *

# ============================================================== build
def build () :
   name  = 'Rond_5'
   geom  = 'False'
   doc   = add_document (name, geom)

   raz_points ()
   add_point (142.638,490.513,38.85)
   add_point (142.638,490.513,41.85)
   add_point (121.638,490.513,41.85)
   add_point (121.638,490.513,38.85)
   add_point (150.338,490.513,38.85)
   add_point (150.338,490.513,41.85)
   add_point (113.938,490.513,41.85)
   add_point (113.938,490.513,38.85)
   gen_cylinder ()
   return doc

# ============================================================== Begin

if __name__ == '__main__' :
   doc = build ()
   mesh_hexas = mailler ()
