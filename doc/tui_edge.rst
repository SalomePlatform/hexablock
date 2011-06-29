:tocdepth: 3


.. _tuiedge:

=====
Edges
=====

Add an edge in the document::

 	ed = doc.addEdge(vx_1, vx_2)

Set of index: 0   1.


Get the number of edges in the document::

	 ne = doc.countEdge()

Get an edge of the document::

 	edg = doc.getEdge(i)

Get the vertices of an edge::

 	ver_a = ed.getVertex(0)
 	ver_b = ed.getVertex(1)

Find an edge in the document::

 	edg = doc.findEdge(ver1, ver2)


GUI command: :ref:`guiedge`
