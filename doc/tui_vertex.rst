:tocdepth: 3

.. _tuivertex:

========
Vertices
========

Model data are exclusively composed of blocks and sub-sets composing these blocks:

- the set of hexahedra composing the model
- the set of quadrangles, edges and vertices composing these hexahedra


Add a new vertex in the document::

 	ve = doc.addVertex(x, y, z)

Get the number of vertices of the document::

 	sum_v = doc.countVertex()


Get a vertex of the document::

 	v = doc.getVertex(i)

Get the coordinates of a vertex::

 	x = ve.getX()
 	y = ve.getY()
 	z = ve.getZ()

Set the coordinates of a vertex::

 	ve.setX(x1)
 	ve.setY(y1)
 	ve.setZ(z1)

Find a vertex in the document (with a tolerance)::

 	ve = doc.findVertex(x, y, z)


GUI command: :ref:`guivertex`
