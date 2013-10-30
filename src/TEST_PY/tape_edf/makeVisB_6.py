
# Hexa : Creation de VisB_6

import hexablock
from hexa_tape import *

# ============================================================== build
def build () :
   name  = 'VisB_6'
   geom  = 'False'
   doc   = add_document (name, geom)

   raz_points ()
   add_point (1.25,508,54.85)
   add_point (8.7,508,54.85)
   add_point (-8.7,508,54.85)
   add_point (-8.7,508,18.6)
   add_point (1.25,508,-4.65)
   add_point (8.7,508,-4.65)
   add_point (-1.25,508,54.85)
   add_point (-1.25,508,-4.65)
   add_point (-8.7,508,-4.65)
   add_point (1.25,508,-25.15)
   add_point (8.7,508,-25.15)
   add_point (-1.25,508,-25.15)
   add_point (-8.7,508,-25.15)
   add_point (1.25,508,-30.15)
   add_point (7.65,508,-30.15)
   add_point (-1.25,508,-30.15)
   add_point (8.7,508,41.85)
   add_point (-7.65,508,-30.15)
   add_point (1.25,508,41.85)
   add_point (9.2,508,-4.86131)
   add_point (-9.2,508,-4.86131)
   add_point (9.2,508,-25.15)
   add_point (-9.2,508,-25.15)
   add_point (-8.7,508,41.85)
   add_point (8.15,508,-30.15)
   add_point (-8.15,508,-30.15)
   add_point (-1.25,508,41.85)
   add_point (14.25,508,54.85)
   add_point (-14.25,508,54.85)
   add_point (14.25,508,41.85)
   add_point (-14.25,508,41.85)
   add_point (1.25,508,18.6)
   add_point (8.7,508,18.6)
   add_point (-1.25,508,18.6)
   gen_vis ()
   return doc

# ============================================================== Begin

if __name__ == '__main__' :
   doc = build ()
   mesh_hexas = mailler ()
