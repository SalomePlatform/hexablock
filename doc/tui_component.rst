:tocdepth: 3


.. _tuicomponent:

=========
Component
=========

Acces to HexaBlocks component::

  import hexablock

Add a new empty document in the document::

	doc = hexablock.addDocument()


Add a loaded document from a "xml" file::

	doc2 = hexablock.loadDocument(filename)

Get the number of opened document in the session::

	nd = hexablock.countDocument()

Get the document number "i"::

	doc_i = hexablock.getDocument(i)

Remove a document in the session::

	hexablock.removeDocument(doc_i)


GUI command: :ref:`guicomponent`
