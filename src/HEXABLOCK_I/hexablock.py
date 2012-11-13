# -*- coding: latin-1 -*-

# Copyright (C) 2009-2012  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

# Francis KLOSS - 2011-2012 - CEA-Saclay, DEN, DM2S, SFME, LGLS, F-91191 Gif-sur-Yvette, France
# =============================================================================================

import salome
import smesh

from HEXABLOCK_ORB import *
import HEXABLOCKPlugin

geompy = smesh.geompy

# Gives the component name
# -----------------------------------------------------------

def moduleName() :
    return "HEXABLOCK"

# Load HEXABLOCK componant
# ------------------------

component = salome.lcc.FindOrLoadComponent("FactoryServer", moduleName())
component = component._narrow(HEXABLOCK_Gen)

for k in dir(component):
    if k[0] == '_':
        continue
    globals()[k] = getattr(component, k)

del k

# Gives the component 
# -----------------------------------------------------------

def getEngine() :
    return component

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
                        print "        vertex: ", vi, "name: ", vv.getName(), " associated: ", va!=None
                        print "          model: x= ", vv.getX(), " y= ", vv.getY(), " z= ", vv.getZ()
                        if va!=None:
                            x, y, z = geompy.PointCoordinates(va)
                            print "          assoc: x= ", x, " y= ", y, " z= ", z

    uv = doc.countUsedVertex()
    ue = doc.countUsedEdge()
    uq = doc.countUsedQuad()
    uh = doc.countUsedHexa()

    print "Model vertices    number: ", uv
    print "Model edges       number: ", ue
    print "Model quadrangles number: ", uq
    print "Model blocks      number: ", uh

    if mesh != None:
        print "Mesh nodes       number: ", mesh.NbNodes()
        print "Mesh segments    number: ", mesh.NbEdges()
        print "Mesh quadrangles number: ", mesh.NbQuadrangles()
        print "Mesh hexas       number: ", mesh.NbHexas()

    return uv, ue, uq, uh

# Mesh a document
# ---------------

def mesh(doc, name=None, dim=3, container="FactoryServer"):
    study = salome.myStudy

    if type(doc) == type(""):
        doc = component.findDocument (doc)

    shape = doc.getShape()
    if shape == None:
        shape = geompy.MakeBox(0, 0, 0,  1, 1, 1)

    if (name == None) or (name == ""):
        name = doc.getName()

    geompy.addToStudy(shape, name)
    comp_smesh = salome.lcc.FindOrLoadComponent(container, "SMESH")
    comp_smesh.init_smesh(study, geompy.geom)
    meshexa = comp_smesh.Mesh(shape)

    so = "libHexaBlockEngine.so"

    algo = smesh.SMESH._objref_SMESH_Gen.CreateHypothesis(comp_smesh, "HEXABLOCK_3D", so)
    meshexa.mesh.AddHypothesis(shape, algo)

    hypo = smesh.SMESH._objref_SMESH_Gen.CreateHypothesis(comp_smesh, "HEXABLOCK_Parameters", so)
    meshexa.mesh.AddHypothesis(shape, hypo)

    hypo.SetDocument(doc)
    hypo.SetDimension(dim)

    meshexa.Compute()

    return meshexa

# Get a document from the current study
# -------------------------------------

def getFromStudy(entry):
    study    = salome.myStudy
    sobject  = study.FindObjectID(entry)
    if sobject == None :
       print " **** Entry ", entry, " is undefined"
       return None

    builder  = study.NewBuilder()
    ok, attname = builder.FindAttribute(sobject, "AttributeName")
    docname  = attname.Value()
    doc = component.findDocument(docname)
    if doc == None :
       print " **** Entry ", entry, " doesn't correspond to an HexaBlock Document"

    return doc

# Find or create HexaBlock Study Component
# -------------------------------------

def findOrCreateComponent( study, builder ):
    father = study.FindComponent( moduleName() )
    if father is None:
       father = builder.NewComponent( moduleName() )
       attr = builder.FindOrCreateAttribute( father, "AttributeName" )
       attr.SetValue( "HexaBlock" )
       attr = builder.FindOrCreateAttribute( father, "AttributePixMap" )
       ### attr.SetPixMap( "ICON_OBJBROWSER_HEXABLOCK" )
       attr.SetPixMap( "ICO_MODULE_HEXABLOCK_SMALL" )
       builder.DefineComponentInstance( father, getEngine() )

    return father

# Add a document in the current study
# -------------------------------------

def addToStudy(doc):
    if doc == None :
       print " *** addToStudy : Bad Document Pointer"
       return

    study   = salome.myStudy
    builder = study.NewBuilder()
    father  = findOrCreateComponent( study, builder )
    name    = doc.getName ()

    present = study.FindObjectByName(name, moduleName())
    if present != [] :
       print " *** addToStudy : Document ", name, "is already in the study"
       return

    object  = builder.NewObject( father )
    attr    = builder.FindOrCreateAttribute( object, "AttributeName" )
    attr.SetValue( name )
    return object.GetID ()
