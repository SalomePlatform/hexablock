:tocdepth: 3


.. _tuicomponent:

=========
Component
=========

Acces to HexaBlock component in python language::

  import hexablock

Add a new empty document in the session::

    doc = hexablock.addDocument(name)


Add a new document loaded from a *xml* file::

    doc2 = hexablock.loadDocument(filename)

Get the number of opened document in the session::

    nd = hexablock.countDocument()

Get the document number *i*::

    doc_i = hexablock.getDocument(i)
    
Remove a document in the session::

    hexablock.removeDocument(doc_i)
    
Set the current study::

	hexablock.SetCurrentStudy(theStudy)

Get a document from the current study::

	doc = hexablock.getFromStudy(doc_entry)
	
Add a document in the current study::

	hexablock.addToStudy(doc)

A tool to dump a model of block::

    hexablock.dump(doc, [mesh, [full]])

- doc: the document to dump
- mesh: an optional mesh of this document
- full:

  - False: to have a short dump (by default)
  - True: to have a full dump


GUI command: :ref:`guicomponent`
