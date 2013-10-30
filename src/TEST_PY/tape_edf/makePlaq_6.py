
# Hexa : Creation de Plaq_6

import hexablock
from hexa_tape import *

# ============================================================== build
def build () :
   name  = 'Plaq_6'
   geom  = 'False'
   doc   = add_document (name, geom)

   raz_points ()
   add_point (9,508,33)
   add_point (9,508,38.85)
   add_point (-9,508,38.85)
   add_point (-9,508,33)
   add_point (17.5,508,33)
   add_point (17.5,508,38.85)
   add_point (-17.5,508,38.85)
   add_point (-17.5,508,33)
   gen_cylinder ()
   return doc

# ============================================================== Begin

if __name__ == '__main__' :
   doc = build ()
   mesh_hexas = mailler ()
