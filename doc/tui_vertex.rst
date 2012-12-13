:tocdepth: 3

.. _tuivertex:

========
Vertices
========

Manage the vertices of the model of blocks of a document.


Add a new vertex of the model of blocks in the document::

    vertex_1 = doc.addVertex(x, y, z)

Get the number of all vertices of the document::

    sum_a = doc.countVertex()

Get the number of vertices used only by the model of blocks::

    sum_u = doc.countUsedVertex()
    
Get the vertices used by the model of blocks

.. literalinclude:: test_doc/vertex/vertex.py

Get a vertex of the document::

    vertex_i = doc.getVertex(i)

Get the coordinates of a vertex::

    x = vertex_i.getX()
    y = vertex_i.getY()
    z = vertex_i.getZ()

Set the coordinates of a vertex::

    vertex_i.setX(x1)
    vertex_i.setY(y1)
    vertex_i.setZ(z1)

Find a vertex in the document (with a tolerance)::

    vertex_f = doc.findVertex(x, y, z)

More operations: *Vertex* inherits :ref:`Element <tuielement>`.

GUI command: :ref:`guivertex`
