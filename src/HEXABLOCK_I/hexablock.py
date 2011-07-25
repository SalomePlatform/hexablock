# -*- coding: latin-1 -*-
#  Copyright (C) 2009-2011  CEA/DEN, EDF R&D
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

# Francis KLOSS - 2011 - CEA-Saclay, DEN, DM2S, SFME, LGLS, F-91191 Gif-sur-Yvette, France
# ========================================================================================

import salome
import smesh
from HEXABLOCK_ORB import *
import HEXABLOCKPlugin

component = salome.lcc.FindOrLoadComponent("FactoryServer", "HEXABLOCK")
component = component._narrow(HEXABLOCK_Gen)

for k in dir(component):
    if k[0] == '_':
        continue
    globals()[k] = getattr(component, k)

del k

def mesh(name, doc, dim=3, container="FactoryServer"):
    geompy = smesh.geompy
    study  = salome.myStudy

    if type(doc) == type(""):
        sobject = study.FindObjectID(doc)
        builder = study.NewBuilder()
        ok, ior = builder.FindAttribute(sobject, "AttributeIOR")
        obj = salome.orb.string_to_object(ior.Value())
        doc = obj._narrow(Document)

    shape = doc.getShape()
    if shape == None:
        shape = geompy.MakeBox(0, 0, 0,  1, 1, 1)

    component = salome.lcc.FindOrLoadComponent(container, "SMESH")
    component.init_smesh(study, geompy.geom)
    meshexa = component.Mesh(shape)

    so = "libHexaBlockEngine.so"

    algo = smesh.SMESH._objref_SMESH_Gen.CreateHypothesis(component, "HEXABLOCK_3D", so)
    meshexa.mesh.AddHypothesis(shape, algo)

    hypo = smesh.SMESH._objref_SMESH_Gen.CreateHypothesis(component, "HEXABLOCK_Parameters", so)
    meshexa.mesh.AddHypothesis(shape, hypo)

    hypo.SetDocument(doc)
    hypo.SetDimension(dim)

    meshexa.Compute()

    return meshexa
