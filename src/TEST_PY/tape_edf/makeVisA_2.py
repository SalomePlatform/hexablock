
# Hexa : Creation de VisA_2

import hexablock
from hexa_tape import *

# ============================================================== build
def build () :
   name  = 'VisA_2'
   geom  = 'False'
   doc   = add_document (name, geom)

   raz_points ()
   add_point (473.706,186.68,44.05)
   add_point (481.156,186.68,44.05)
   add_point (463.756,186.68,44.05)
   add_point (463.756,186.68,14.3)
   add_point (473.706,186.68,-2.45)
   add_point (481.156,186.68,-2.45)
   add_point (471.206,186.68,44.05)
   add_point (471.206,186.68,-2.45)
   add_point (463.756,186.68,-2.45)
   add_point (473.706,186.68,-23.95)
   add_point (481.156,186.68,-23.95)
   add_point (471.206,186.68,-23.95)
   add_point (463.756,186.68,-23.95)
   add_point (473.706,186.68,-27.95)
   add_point (480.106,186.68,-27.95)
   add_point (471.206,186.68,-27.95)
   add_point (481.156,186.68,31.05)
   add_point (464.806,186.68,-27.95)
   add_point (473.706,186.68,31.05)
   add_point (481.656,186.68,-2.66131)
   add_point (463.256,186.68,-2.66131)
   add_point (481.656,186.68,-23.95)
   add_point (463.256,186.68,-23.95)
   add_point (463.756,186.68,31.05)
   add_point (480.606,186.68,-27.95)
   add_point (464.306,186.68,-27.95)
   add_point (471.206,186.68,31.05)
   add_point (486.706,186.68,44.05)
   add_point (458.206,186.68,44.05)
   add_point (486.706,186.68,31.05)
   add_point (458.206,186.68,31.05)
   add_point (473.706,186.68,14.3)
   add_point (481.156,186.68,14.3)
   add_point (471.206,186.68,14.3)
   gen_vis ()
   return doc

# ============================================================== Begin

if __name__ == '__main__' :
   doc = build ()
   mesh_hexas = mailler ()
