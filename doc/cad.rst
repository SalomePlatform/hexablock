:tocdepth: 3

.. _cad:

=======
The CAD
=======



The geometry to mesh is defined by a set of independant shapes, these
shapes are external references to the GEOM module of SALOME. The
shapes of this set can be faces and/or solids. In Hexablock module you
can:

- use a geometry defined in GEOM module
- import geometrical objects from BREP, IGES, STEP files.

The TUI script to import geometrical objects from a BREP file is::

	import os
	import GEOM
	import SALOMEDS
	import hexablock

	# Example of import from STEP 
	# get a path of the STEP file containing the CAD
	STEP_PATH = os.path.expandvars("$HEXABLOCK_ROOT_DIR/bin/salome/crank.stp")

The following images depict different CADs for the construction of models:

.. image:: _static/cad_bielle.PNG
   :align: center

.. centered::
   Connecting rod 

(see :ref:`intropython`)


.. image:: _static/bride.PNG
   :align: center

.. centered::
   Bridle

.. image:: _static/intersec_cyl.PNG
   :align: center

.. centered::
   Intersection of cylinders


.. image:: _static/tuyau_courbe.PNG
   :align: center

.. centered::
   Bent (curved) pipe


