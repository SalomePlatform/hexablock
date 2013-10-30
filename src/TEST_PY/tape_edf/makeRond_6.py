
# Hexa : Creation de Rond_6

import hexablock
from hexa_tape import *

# ============================================================== build
def build () :
   name  = 'Rond_6'
   geom  = 'False'
   doc   = add_document (name, geom)

   raz_points ()
   add_point (10.5,508,38.85)
   add_point (10.5,508,41.85)
   add_point (-10.5,508,41.85)
   add_point (-10.5,508,38.85)
   add_point (18.2,508,38.85)
   add_point (18.2,508,41.85)
   add_point (-18.2,508,41.85)
   add_point (-18.2,508,38.85)
   gen_cylinder ()
   return doc

# ============================================================== Begin

if __name__ == '__main__' :
   doc = build ()
   mesh_hexas = mailler ()
