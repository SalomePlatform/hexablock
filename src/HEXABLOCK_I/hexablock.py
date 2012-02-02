# -*- coding: latin-1 -*-

#  Copyright (C) 2009-2012  CEA/DEN, EDF R&D
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

# Francis KLOSS - 2011-2012 - CEA-Saclay, DEN, DM2S, SFME, LGLS, F-91191 Gif-sur-Yvette, France
# =============================================================================================

import salome
import smesh

from HEXABLOCK_ORB import *
import HEXABLOCKPlugin

geompy = smesh.geompy

# Load HEXABLOCK componant
# ------------------------

component = salome.lcc.FindOrLoadComponent("FactoryServer", "HEXABLOCK")
component = component._narrow(HEXABLOCK_Gen)

for k in dir(component):
    if k[0] == '_':
        continue
    globals()[k] = getattr(component, k)

del k

# Add laws on propagations based on min or max segment length
# -----------------------------------------------------------

def addLaws(doc, lg, lgmax=True):
    laws = {}
    n = doc.countPropagation()
    for i in xrange(n):
        p = doc.getPropagation(i)
        if lgmax:
            m = 0
        else:
            m = (max)
        for e in p.getEdges():
            a = e.getAssociations()

            if a == []:
                vam = e.getVertex(0)
                vag = vam.getAssociation()
                if vag == None:
                    vax = vam.getX()
                    vay = vam.getY()
                    vaz = vam.getZ()
                else:
                    vax, vay, vaz = geompy.PointCoordinates(vag)

                vbm = e.getVertex(1)
                vbg = vbm.getAssociation()
                if vbg == None:
                    vbx = vbm.getX()
                    vby = vbm.getY()
                    vbz = vbm.getZ()
                else:
                    vbx, vby, vbz = geompy.PointCoordinates(vbg)

                l = ( (vbx-vax)**2 + (vby-vay)**2 + (vbz-vaz)**2 )**0.5

            else:
                l = 0.0
                for gdf in a:
                    le, su, vo = geompy.BasicProperties(gdf.geomObj)
                    l += le * (gdf.fin - gdf.debut)

            if ( lgmax and l>m ) or ( (not lgmax) and l<m ):
                m = l

        nn = m / lg
        if lgmax and (int(nn) != nn):
            nn = int(nn)
        else:
            nn = int(nn) - 1

        try:
            law = laws[nn]
        except:
            law = doc.addLaw("u_"+str(nn), nn)
            laws[nn] = law

        p.setLaw(law)

# Display informations about a document
# -------------------------------------

def dump(doc, mesh=None, full=False):
    if full:
        hn = doc.countUsedHexa()
        print "Model dump: number of hexas: ", hn
        for hi in xrange(hn):
            hh = doc.getUsedHexa(hi)
            print "  hexa: ", hi, "name: ", hh.getName()
            for fi in xrange(6):
                ff = hh.getQuad(fi)
                fa = ff.getAssociations()
                print "    quadrangle: ", fi, "name: ", ff.getName(), " associated: ", fa!=[]
                for ei in xrange(4):
                    ee = ff.getEdge(ei)
                    ea = ee.getAssociations()
                    print "      edge: ", ei, "name: ", ee.getName(), " associated: ", ea!=[]
                    for vi in xrange(2):
                        vv = ee.getVertex(vi)
                        va = vv.getAssociation()
                        print "        vertex: ", vi, "name: ", vv.getName(), " associated: ", va!=None,
                        print " -> x= ", vv.getX(), " y= ", vv.getY(), " z= ", vv.getZ()

    print "Model vertices    number: ", doc.countUsedVertex()
    print "Model edges       number: ", doc.countUsedEdge()
    print "Model quadrangles number: ", doc.countUsedQuad()
    print "Model blocks      number: ", doc.countUsedHexa()

    if mesh != None:
        print "Mesh nodes       number: ", mesh.NbNodes()
        print "Mesh segments    number: ", mesh.NbEdges()
        print "Mesh quadrangles number: ", mesh.NbQuadrangles()
        print "Mesh hexas       number: ", mesh.NbHexas()

# Mesh a document
# ---------------

def mesh(doc, name=None, dim=3, container="FactoryServer"):
    study = salome.myStudy

    if type(doc) == type(""):
        sobject = study.FindObjectID(doc)
        builder = study.NewBuilder()
        ok, ior = builder.FindAttribute(sobject, "AttributeIOR")
        obj = salome.orb.string_to_object(ior.Value())
        doc = obj._narrow(Document)

    brep = doc.getBrep()
    if brep == "":
        shape = geompy.MakeBox(0, 0, 0,  1, 1, 1)
    else:
        tmpfile = "/tmp/hexablock.brep"
        fic = file(tmpfile, 'w')
        fic.write(brep)
        fic.close()
        shape = geompy.ImportBREP(tmpfile)

    if name == None:
        name = doc.getName()

    geompy.addToStudy(shape, name)
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
