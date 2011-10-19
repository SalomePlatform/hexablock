:tocdepth: 3

.. _tuimakeelements:

======================================
Make elements by transforming elements
======================================

Rotate and translate for any kind of elements::

 	elts = doc.makeTranslation(element, vec)
	elts = doc.makeRotation(element, ver, vec, angle)

and "element" could be Vertex, Edge, Quad, Hexa, Vector, Cylinder,
Pipe and Elements and the result "elts" is always an object of type
"Elements".

Example
-------

::

  import hexablock
  doc = hexablock.addDocument()

  size_x = 1
  size_y = 1
  size_z = 2

  orig = doc.addVertex(0, 0, 0)
  dirVr = doc.addVector(1, 1, 1)

  grid = doc.makeCartesian1(orig, dirVr, size_x, size_y, size_z, 0, 0, 0)
  orig.setScalar(2)

  devant = doc.addVector(5, 0, 0)
  grid2 = doc.makeTranslation(grid, devant)

  grid4 = doc.makeRotation(grid2, orig, dirVr, 45)


GUI command: :ref:`guimakeelements`
