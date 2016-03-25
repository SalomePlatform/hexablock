# -*- coding: latin-1 -*-
# Copyright (C) 2009-2016  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
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

# HexaBlock : Module principal

import hexablock_swig

hexa = hexablock_swig.hex_instance ()

# ======================================================== what
def what () :
   hexa.what ()

# ======================================================== countDocument
def countDocument () :
   return hexa.countDocument ()

# ======================================================== getDocument
def getDocument (nro) :
   return  hexa.getDocument (nro)

# ======================================================== removeDocument
def removeDocument (doc) :
   return  hexa.removeDocument (doc)

# ======================================================== addDocument
def addDocument (nomdoc) :
   return  hexa.addDocument (nomdoc)

# ======================================================== loadDocument
def loadDocument (filename) :
   return  hexa.loadDocument (filename)

# ======================================================== findDocument
def findDocument (name) :
   return  hexa.findDocument (name)

# ======================================================== mesh
def mesh (doc, name=None, dim=3, container="FactoryServer"):
   return  0

# ======================================================== mesh
def mesh (doc, name=None, dim=3, container="FactoryServer"):
   return  0

# ======================================================== mesh
def mesh (doc, name=None, dim=3, container="FactoryServer"):
   return  0

# ======================================================== dump
def dump (doc, mesh=None, full=False) :
   return  [ 36, 15, 36, 17]

# ======================================================= printMessage
def printMessage () :

    nl = hexa.sizeofMessage()
    if nl == 0 : 
       return
    for ni in range (nl) :
        mess = hexa.getMessageLine (ni)
        print mess

