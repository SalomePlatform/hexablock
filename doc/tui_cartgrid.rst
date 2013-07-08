:tocdepth: 3


.. _tuicartgrid:

====================
Make cartesian grids
====================

.. _tuicartgridsimple:

Simple Cartesian Grid
=====================

To add a simple cartesian grid to the model, the following data are required:

- *nb X*: The number of hexahedra along the X axis
- *nb Y*: The number of hexahedra along the Y axis
- *nb Z*: The number of hexahedra along the Z axis


Make a Simple Cartesian Grid::

    elts = doc.makeCartesianTop(nbX, nbY, nbZ)

GUI command: :ref:`guicartgridsimple`


.. _tuicartgriduniform:

Uniform Cartesian Grid
======================

To add a uniform cartesian grid to the model, the following data are required:

- *origin*: The vertex of the origin
- *vec X* : The X vector
- *vec Y* : The Y vector
- *vec Z* : The Z vector
- *len X* : The length of an hexahedra on the X axis
- *len Y* : The length of an hexahedra on the Y axis
- *len Z* : The length of an hexahedra on the Z axis
- *nb X*  : The number of hexahedra on the X axis
- *nb Y*  : The number of hexahedra on the Y axis
- *nb Z*  : The number of hexahedra on the Z axis

Make a Uniform Cartesian Grid::

    elts = doc.makeCartesianUni(orig, vecX, vecY, vecZ, lenX, lenY, lenZ, nbX, nbY, nbZ)
  
GUI command: :ref:`guicartgriduniform`  


.. _tuicartgridcustom:

Custom Cartesian Grid
=====================

To add a custom cartesian grid to the model, the following data are required:

- *origin*: The vertex of the origin
- *vec X* : The X vector
- *vec Y* : The Y vector
- *vec Z* : The Z vector
- *tx*    : A list of x coordinates in ascendant order
- *ty*    : A list of y coordinates in ascendant order
- *tz*    : A list of z coordinates in ascendant order

Make a Custom Cartesian Grid::

    elts = doc.makeCartesian(orig, vecX, vecY, vecZ, tx, ty, tz)
    
GUI command: :ref:`guicartgridcustom`


Operations on *elts*: :ref:`tuielements2`


Example
=======

.. literalinclude:: test_doc/cartesian_grid/cartesian_grid.py
   :linenos:
