:tocdepth: 3


.. _tuisphergrid:

====================
Make spherical grids
====================


To add a spherical grid, the following data are required:

- the center (vertex) of the central hexahedron: *pt*
- the vector corresponding to the diametrically opposite vertex to the
  initial vertex: *dv*
- the number of embedded hexahedra: *n*
- the coefficient of growth:*k*


Make a spherical grid::

    elts = doc.makeSpherical(pt, dv, n, k)

The result is an array of hexahedra starting with the central and
other hexahedral are arranged layer by layer from the center. Each
layer contains six hexahedral always in the following order: two along
the X axis, then 2 for the Y axis and 2 for the Z axis.


Example
-------

::

 import hexablock
 doc = hexablock.addDocument()

 orig = doc.addVertex(0, 0, 0)

 direction = doc.addVector(1, 1, 1)

 dx = doc.addVector(1, 0, 0)
 dy = doc.addVector(0, 1, 0)
 dz = doc.addVector(0, 0, 1)

 n = 2
 k = 0.8

 grid = doc.makeSpherical(orig, direction, n, k)


GUI command: :ref:`guisphergrid`
