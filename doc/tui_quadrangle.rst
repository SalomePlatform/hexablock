:tocdepth: 3


.. _tuiquadrangles:

===========
Quadrangles
===========

Add a quadrangle in the document::

 	q1 = doc.addQuad(ed1, ed2, ed3, ed4)
 	q2 = doc.addQuadVertices(ver1, ver2, ver3, ver4)

Set of index: 0	1  2  3.

The vertices and edges follow the outline of the quadrangle.

Get the number of quadrangles of the document::

 	nq = doc.countQuad()

Get a quadrangle of the document::

 	q = doc.getQuad(i)

Get an edge of a quadrangle::

 	ed_i = q1.getEdge(0 or 1 or 2 or 3)

Get a vertex of a quadrangle::
 
	ve_j = q1.getVertex(0 or 1 or 2 or 3)
 
Find a quadrangle in the document::
 
	qua = doc.findQuad(ver1, ver2)

GUI command: :ref:`guiquadrangles`
