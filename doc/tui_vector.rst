:tocdepth: 3

.. _tuivector:

======
Vector
======

Add a vector in the document::

	 vec1 = doc.addVector(dx, dy, dz)
	 vec2 = doc.addVectorVertices(ver1, ver2)

Get the values of a vector::

 	dx = vec1.getDX()
 	dy = vec1.getDY()
 	dz = vec1.getDZ()


Get the number of vector in the document::

	 nv = doc.countVector()

Get a vector of the document::

	 vk = doc.getVector(i)

GUI command: :ref:`guivector`
