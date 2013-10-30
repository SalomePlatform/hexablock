
# Hexa : Creation de Plaq_4

import hexablock
from hexa_tape import *

# ============================================================== build
def build () :
   name  = 'Plaq_4'
   geom  = 'False'
   doc   = add_document (name, geom)

   raz_points ()
   add_point (225.112,459.739,22.2)
   add_point (225.112,459.739,28.05)
   add_point (207.112,459.739,28.05)
   add_point (207.112,459.739,22.2)
   add_point (233.612,459.739,22.2)
   add_point (233.612,459.739,28.05)
   add_point (198.612,459.739,28.05)
   add_point (198.612,459.739,22.2)
   gen_cylinder ()
   return doc

# ============================================================== Begin

if __name__ == '__main__' :
   doc = build ()
   mesh_hexas = mailler ()
