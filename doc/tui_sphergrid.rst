:tocdepth: 3


.. _tuisphergrid:

====================
Make spherical grids
====================


To add a spherical grid, the following data are required:

- the vertex of the central hexahedron: *pt*
- the vector corrsponding to the diametrically opposite vertex to the
  initial vertex: *dv*
- the number of embedded hexahedra: *n*
- **todo??????** :*k*


Make a spherical grid::

	 elts = doc.makeSpherical(pt, dv, n, k)

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


.. image:: _static/sph_grid.PNG
   :align: center

.. centered::
   Spherical Grid

.. image:: _static/sph_grid2.PNG
   :align: center

.. centered::
   Spherical Grid

GUI command: :ref:`guisphergrid`
