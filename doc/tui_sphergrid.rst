:tocdepth: 3


.. _tuisphergrid:

====================
Make spherical grids
====================


To add a spherical grid, the following data are required:

- the center (vertex) of the central hexahedron: *c*
- the radius of the internal hexahedron: *r*
- the number of embedded hexahedra: *n*
- the coefficient of growth:*k*


Make a spherical grid::

    elts = doc.makeSpherical(c, r, n, k)

Operations on *elts*: :ref:`tuielements2`


The result is an array of hexahedra starting with the central and
other hexahedral are arranged layer by layer from the center. Each
layer contains six hexahedral always in the following order: two along
the X axis, then 2 for the Y axis and 2 for the Z axis.


Example
-------

.. literalinclude:: test_doc/grid/spherical_grid.py
   :linenos:


GUI command: :ref:`guisphergrid`
