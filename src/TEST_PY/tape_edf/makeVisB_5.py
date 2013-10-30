
# Hexa : Creation de VisB_5

import hexablock
from hexa_tape import *

# ============================================================== build
def build () :
   name  = 'VisB_5'
   geom  = 'False'
   doc   = add_document (name, geom)

   raz_points ()
   add_point (133.388,490.513,54.85)
   add_point (140.838,490.513,54.85)
   add_point (123.438,490.513,54.85)
   add_point (123.438,490.513,18.6)
   add_point (133.388,490.513,-4.65)
   add_point (140.838,490.513,-4.65)
   add_point (130.888,490.513,54.85)
   add_point (130.888,490.513,-4.65)
   add_point (123.438,490.513,-4.65)
   add_point (133.388,490.513,-25.15)
   add_point (140.838,490.513,-25.15)
   add_point (130.888,490.513,-25.15)
   add_point (123.438,490.513,-25.15)
   add_point (133.388,490.513,-30.15)
   add_point (139.788,490.513,-30.15)
   add_point (130.888,490.513,-30.15)
   add_point (140.838,490.513,41.85)
   add_point (124.488,490.513,-30.15)
   add_point (133.388,490.513,41.85)
   add_point (141.338,490.513,-4.86131)
   add_point (122.938,490.513,-4.86131)
   add_point (141.338,490.513,-25.15)
   add_point (122.938,490.513,-25.15)
   add_point (123.438,490.513,41.85)
   add_point (140.288,490.513,-30.15)
   add_point (123.988,490.513,-30.15)
   add_point (130.888,490.513,41.85)
   add_point (146.388,490.513,54.85)
   add_point (117.888,490.513,54.85)
   add_point (146.388,490.513,41.85)
   add_point (117.888,490.513,41.85)
   add_point (133.388,490.513,18.6)
   add_point (140.838,490.513,18.6)
   add_point (130.888,490.513,18.6)
   gen_vis ()
   return doc

# ============================================================== Begin

if __name__ == '__main__' :
   doc = build ()
   mesh_hexas = mailler ()
