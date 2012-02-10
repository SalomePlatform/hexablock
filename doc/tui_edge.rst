:tocdepth: 3


.. _tuiedge:

=====
Edges
=====

Manage the edges of the document.

Add an edge in the document::

 	edge_12 = doc.addEdge(vertex_1, vertex_2)

Get the number of all edges in the document::

	 sum_a = doc.countEdge()

Get the number of edges used only in the model of blocks::

	 sum_u = doc.countUsedEdge()

Get an edge of the document::

 	edge_i = doc.getEdge(i)

Get the vertices of an edge::

 	vertex_a = edge_i.getVertex(0)
 	vertex_b = edge_i.getVertex(1)

Find an edge in the document::

 	edge_f = doc.findEdge(vertex_1, vertex_2)


GUI command: :ref:`guiedge`
