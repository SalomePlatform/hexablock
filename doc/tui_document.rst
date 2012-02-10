:tocdepth: 3

.. _tuidocument:

========
Document
========

Give the name of a document::

 	name = doc.getName()

Set the name of a document::

 	doc.setName(name)

Save a document in a "XML" file::

 	doc.save(filename)

Save the model of blocks of a document in a "VTK" file::

 	doc.saveVtk(filename)

Set the tolerance need to find element::

 	doc.setTolerance(0.0001)

Get the tolerance::

 	tol = doc.getTolerance()


GUI command: :ref:`guidocument`
