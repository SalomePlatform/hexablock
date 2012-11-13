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
  
Set Debug level (long)::
  
	doc.setLevel(debug_level)

Removes elements of type Quad Edge or Vertex not belonging to an Hexa::

	doc.purge()
	
Associate a shape to the document::

	doc.setShape(shape)

Gets the shape associated to the document:: 
	
	doc.getShape()

Gets the Brep associated to the document::

	doc.getBrep()
	
	
GUI command: :ref:`guidocument`
