
# Hexa : Creation de VisA_3

import hexablock
from hexa_tape import *

# ============================================================== build
def build () :
   name  = 'VisA_3'
   geom  = 'False'
   doc   = add_document (name, geom)

   raz_points ()
   add_point (371.293,348.04,44.05)
   add_point (378.743,348.04,44.05)
   add_point (361.343,348.04,44.05)
   add_point (361.343,348.04,14.3)
   add_point (371.293,348.04,-2.45)
   add_point (378.743,348.04,-2.45)
   add_point (368.793,348.04,44.05)
   add_point (368.793,348.04,-2.45)
   add_point (361.343,348.04,-2.45)
   add_point (371.293,348.04,-23.95)
   add_point (378.743,348.04,-23.95)
   add_point (368.793,348.04,-23.95)
   add_point (361.343,348.04,-23.95)
   add_point (371.293,348.04,-27.95)
   add_point (377.693,348.04,-27.95)
   add_point (368.793,348.04,-27.95)
   add_point (378.743,348.04,31.05)
   add_point (362.393,348.04,-27.95)
   add_point (371.293,348.04,31.05)
   add_point (379.243,348.04,-2.66131)
   add_point (360.843,348.04,-2.66131)
   add_point (379.243,348.04,-23.95)
   add_point (360.843,348.04,-23.95)
   add_point (361.343,348.04,31.05)
   add_point (378.193,348.04,-27.95)
   add_point (361.893,348.04,-27.95)
   add_point (368.793,348.04,31.05)
   add_point (384.293,348.04,44.05)
   add_point (355.793,348.04,44.05)
   add_point (384.293,348.04,31.05)
   add_point (355.793,348.04,31.05)
   add_point (371.293,348.04,14.3)
   add_point (378.743,348.04,14.3)
   add_point (368.793,348.04,14.3)
   gen_vis ()
   return doc

# ============================================================== Begin

if __name__ == '__main__' :
   doc = build ()
   mesh_hexas = mailler ()
