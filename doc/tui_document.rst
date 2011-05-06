:tocdepth: 3

.. _tuidocument:

========
Document
========

Give the file associate to a document::

 	doc_file = doc.getFile()

Set the file associate to a document::

 	doc.setFile(filename)

Save a document in a file::

 	doc.saveFile()

Say if the document is saved in the file::

 	is_saved = doc.isSavedFile()


Set the tolerance need to find element::

 	doc.setTolerance(0.0001)

Get the tolerance::

 	tol = doc.getTolerance()

Set the level for messages::

	 doc.setLevel(2)

Get the level for messages::

 	level = doc.getLevel()


GUI command: :ref:`guidocument`
