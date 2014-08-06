:tocdepth: 3


.. _tuiquadrangles:

===========
Quadrangles
===========

Manage the quadrangles of the document.

Add a quadrangle in the document::

    quad_1 = doc.addQuad(edge_1, edge_2, edge_3, edge_4)
    quad_2 = doc.addQuadVertices(vertex_1, vertex_2, vertex_3, vertex_4)

Notice: the vertices and edges follow the outline of the quadrangle.

Get the number of all quadrangles of the document::

    sum_a = doc.countQuad()

Get the number of used quadrangles in the model of blocks::

    sum_u = doc.countUsedQuad()
    
Get the quads used by the model of blocks

.. literalinclude:: test_doc/quad/quad.py


Get a quadrangle of the document::

    quad_i = doc.getQuad(i)

Get an edge of a quadrangle::

    edge_e = quad_i.getEdge(0 or 1 or 2 or 3)

Get a vertex of a quadrangle::
 
    vertex_v = quad_i.getVertex(0 or 1 or 2 or 3)
 
Find a quadrangle in the document::
 
    quad_f = doc.findQuad(vertex_1, vertex_2)

More operations: *Quad* inherits :ref:`Element <tuielement>`.

GUI command: :ref:`guiquadrangles`
