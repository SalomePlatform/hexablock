# -*- coding: latin-1 -*-
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

