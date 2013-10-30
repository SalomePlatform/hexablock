
# Hexa : Assemblage des composants de la tape 

import hexablock
import smesh
from hexa_tape import *

import makeAnneauSupport
import makeCouvercleCentral
import makeCouvercleLateral
import makeEtrierLateral
import makeJointPlat5mm
import makeJointRebord64mm
import makePlaq_1
import makePlaq_2
import makePlaq_3
import makePlaq_4
import makePlaq_5
import makePlaq_6
import makeRenfortCentral
import makeRond_1
import makeRond_2
import makeRond_3
import makeRond_4
import makeRond_5
import makeRond_6
import makeVisA_1
import makeVisA_2
import makeVisA_3
import makeVisB_5
import makeVisB_6
import make_vis_c
import makeVisTF1
import makeVisTF2
import makeVisTF3

# ============================================================== Begin

tab_piece = []
tab_piece.append (makeAnneauSupport.build ())
tab_piece.append (makeCouvercleCentral.build ())
tab_piece.append (makeCouvercleLateral.build ())
tab_piece.append (makeEtrierLateral.build ())
tab_piece.append (makeJointPlat5mm.build ())
tab_piece.append (makeJointRebord64mm.build ())
tab_piece.append (makePlaq_1.build ())
tab_piece.append (makePlaq_2.build ())
tab_piece.append (makePlaq_3.build ())
tab_piece.append (makePlaq_4.build ())
tab_piece.append (makePlaq_5.build ())
tab_piece.append (makePlaq_6.build ())
tab_piece.append (makeRenfortCentral.build ())
tab_piece.append (makeRond_1.build ())
tab_piece.append (makeRond_2.build ())
tab_piece.append (makeRond_3.build ())
tab_piece.append (makeRond_4.build ())
tab_piece.append (makeRond_5.build ())
tab_piece.append (makeRond_6.build ())
tab_piece.append (makeVisA_1.build ())
tab_piece.append (makeVisA_2.build ())
tab_piece.append (makeVisA_3.build ())
tab_piece.append (makeVisB_5.build ())
tab_piece.append (makeVisB_6.build ())
tab_piece.append (make_vis_c.build ())      ### not generated
tab_piece.append (makeVisTF1.build ())
tab_piece.append (makeVisTF2.build ())
tab_piece.append (makeVisTF3.build ())

tab_mesh  = []
for doc in tab_piece :
    print ' ... Maillage de ', doc.getName()
    mesh = mailler (doc)
    tab_mesh.append (mesh)

global_mesh = smesh.Concatenate (tab_mesh, False, False, allGroups=False)
