
# Hexa : Creation de Plaq_5

import hexablock
from hexa_tape import *

# ============================================================== build
def build () :
   name  = 'Plaq_5'
   geom  = 'False'
   doc   = add_document (name, geom)

   raz_points ()
   add_point (141.138,490.513,33)
   add_point (141.138,490.513,38.85)
   add_point (123.138,490.513,38.85)
   add_point (123.138,490.513,33)
   add_point (149.638,490.513,33)
   add_point (149.638,490.513,38.85)
   add_point (114.638,490.513,38.85)
   add_point (114.638,490.513,33)
   gen_cylinder ()
   return doc

# ============================================================== Begin

if __name__ == '__main__' :
   doc = build ()
   mesh_hexas = mailler ()
