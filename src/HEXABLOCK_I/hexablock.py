# -*- coding: latin-1 -*-

# Francis KLOSS - 2009-2010 - CEA-Saclay, DEN, DM2S, SFME, LGLS, F-91191 Gif-sur-Yvette, France
# =============================================================================================

import salome
from HEXABLOCK_ORB import *
import smesh
import HEXABLOCKPlugin

component = salome.lcc.FindOrLoadComponent("FactoryServer", "HEXABLOCK")

component = component._narrow(HEXABLOCK_Gen)

for k in dir(component):
    if k[0] == '_':
        continue
    globals()[k] = getattr(component, k)

del k

def mesh(name, doc, dim=3):
    geom = smesh.geompy.MakeBox(0, 0, 0,  1, 1, 1)
    smesh.geompy.addToStudy(geom, name)
    mesh = smesh.Mesh(geom)

    so = "libHexaBlockEngine.so"

    algo = smesh.SMESH._objref_SMESH_Gen.CreateHypothesis(smesh.smesh, "HEXABLOCK_3D", so)
    mesh.mesh.AddHypothesis(geom, algo)

    hypo = smesh.SMESH._objref_SMESH_Gen.CreateHypothesis(smesh.smesh, "HEXABLOCK_Parameters", so)
    mesh.mesh.AddHypothesis(geom, hypo)

    hypo.SetDocument(doc)
    hypo.SetDimension(dim)

    mesh.Compute()

    return mesh
