:tocdepth: 3

.. _tuivector:

======
Vector
======

Add a vector in the document::

     vector_1 = doc.addVector(dx, dy, dz)
     vector_2 = doc.addVectorVertices(vertex_1, vertex_2)

Get the values of a vector::

    dx = vector_1.getDX()
    dy = vector_1.getDY()
    dz = vector_1.getDZ()

Get the number of vector in the document::

     sum = doc.countVector()

Get a vector of the document::

     vector_i = doc.getVector(i)
     
Get the norm of a vector::

	norm = vector_i.getNorm()
	
Get the angle on X::

	angle = vector_i.getAngleX()
	
More operations: *Vector* inherits :ref:`Element <tuielement>`.

GUI command: :ref:`guivector`
