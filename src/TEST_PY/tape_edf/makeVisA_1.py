
# Hexa : Creation de VisA_1

import hexablock
from hexa_tape import *

# ============================================================== build
def build () :
   name  = 'VisA_1'
   geom  = 'False'
   doc   = add_document (name, geom)

   raz_points ()
   add_point (509.25,1.44329e-15,44.05)
   add_point (516.7,1.44329e-15,44.05)
   add_point (499.3,-3.77847e-16,44.05)
   add_point (499.3,0,14.3)
   add_point (509.25,-3.71925e-15,-2.45)
   add_point (516.7,-3.71925e-15,-2.45)
   add_point (506.75,-1.79986e-16,44.05)
   add_point (506.75,0,-2.45)
   add_point (499.3,-2.94246e-15,-2.45)
   add_point (509.25,-6.10623e-15,-23.95)
   add_point (516.7,-6.10623e-15,-23.95)
   add_point (506.75,0,-23.95)
   add_point (499.3,-5.55483e-16,-23.95)
   add_point (509.25,-6.55032e-15,-27.95)
   add_point (515.65,-6.55032e-15,-27.95)
   add_point (506.75,4.20585e-17,-27.95)
   add_point (516.7,0,31.05)
   add_point (500.35,6.92606e-16,-27.95)
   add_point (509.25,0,31.05)
   add_point (517.2,-3.74271e-15,-2.66131)
   add_point (498.8,7.83761e-16,-2.66131)
   add_point (517.2,-6.10623e-15,-23.95)
   add_point (498.8,-9.3834e-16,-23.95)
   add_point (499.3,1.06544e-15,31.05)
   add_point (516.15,-6.55032e-15,-27.95)
   add_point (499.85,3.09749e-16,-27.95)
   add_point (506.75,1.53081e-16,31.05)
   add_point (522.25,1.44329e-15,44.05)
   add_point (493.75,3.01832e-16,44.05)
   add_point (522.25,0,31.05)
   add_point (493.75,1.74512e-15,31.05)
   add_point (509.25,-1.85962e-15,14.3)
   add_point (516.7,-1.85962e-15,14.3)
   add_point (506.75,0,14.3)
   gen_vis ()
   return doc

# ============================================================== Begin

if __name__ == '__main__' :
   doc = build ()
   mesh_hexas = mailler ()
