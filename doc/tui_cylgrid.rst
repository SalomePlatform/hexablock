:tocdepth: 3


.. _tuicylgrid:

======================
Make cylindrical grids
======================

To add a regular cylindrical grid, the following data are required:

- the coordinates (vertex) of the cylinder base center: *pt*
- the vectors defining the axis and the direction of the cylinder: *vex, vez*
- the radial, angular and axial sizes: *dr, da, dl*
- the radial, angular and axial elements number: *nr, na, nl*
- to fill or not the central part of hexahedra: *fill*

To add an irregular cylindrical grid, the following data are required:

- the coordinates (vertex) of the cylinder base center: *pt*
- the vectors defining the axis and the direction of the cylinder: *vex, vez*
- the radial, angular and axial sizes: *dr (a list of radius), da (a list of angles), dl (a list of heights)*
- to fill or not the central part of hexahedra: *fill*

Make a regular cylindrical grid::

    elts = doc.makeCylindrical(pt, vex, vez, dr, da, dl, nr, na, nl, fill)

Make an irregular cylindrical grid::

	elts = doc.makeCylindricals (pt, vex, vez, dr, da, dl, False)
	
Operations on *elts*: :ref:`tuielements2`


The result is an array of hexahedra which are arranged along the
radial axis first, then the angular axis, then according to the layers
and then possibly hexahedra filling the central part.


The filling the central part of the cylinder is made ​accordingly to the two following cases:

.. image:: _static/cyl_grid2.png
   :align: center

.. centered::
   Filling of the central part of the cylinder in the case the number of angular elements is odd na = 5.

.. image:: _static/cyl_grid1.png
   :align: center

.. centered::
   Filling of the central part of the cylinder in the case the number of angular elements is even na = 4.


Example (regular mode)
----------------------

Filling rule is illustrated by different case-tests:


.. literalinclude:: test_doc/grid/cylindrical_grid.py
   :linenos:


.. image:: _static/cylindrical_grids.png
   :align: center

.. centered::
   Cylindrical Grids (regular)


Example (irregular mode)
------------------------

.. literalinclude:: test_doc/grid/irregular_cylindrical.py
   :linenos:


.. image:: _static/irregular_cylindrical.png
   :align: center

.. centered::
   Irregular cylindrical grid
   
   
GUI command: :ref:`guicylgrid`
