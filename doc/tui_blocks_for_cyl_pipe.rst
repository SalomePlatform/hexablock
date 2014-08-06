:tocdepth: 3


.. _tuiblockscylinderpipe:

================================
Cylinders and Pipes Construction
================================

.. _tuicylinder:

Cylinder
========

.. _tuicylindersimple:

Simple Cylinder
---------------

To make a simple cylinder grid in python mode, the following data are required:

- *nbR* : number of hexahedra on radial.
- *nbA* : number of hexahedra along the perimeter of the cylinder.
- *nbH* : number of hexahedra along the axis of the cylinder.

Use the function **makeCyinderTop**::

	elts = doc.makeCylinderTop(nbR, nbA, nbH)

GUI command: :ref:`guicylindersimple`


.. _tuicylinderuniform:

Uniform Cylinder
----------------

The following data are required:

- *origin* : origin's coordinates of the cylinder (select a vertex).
- *vx*     : the base of the cylinder (select a vector).
- *vz*	   : the axis of the cylinder (select a vector).
- *rint*   : the radius of the hole in the cylinder .
- *rext*   : the radius of the cylinder.
- *angle*  : angle of the cylinder around the Z axis.
- *hauteur*: the height of the cylinder.
- *nbR*    : number of hexahedra on radial.
- *nbA*    : number of hexahedra along the perimeter of the cylinder.
- *nbH*    : number of hexahedra along the axis of the cylinder.

Use the function **makeCylinderUni** to  make a uniform cylinder::

	elts = doc.makeCylinderUni(origin, vx, vz, rint, rext, angle, hauteur, nbR, nbA, nbH)

GUI command: :ref:`guicylinderuniform`


.. _tuicylindercustom:

Custom Cylinder
---------------

The following data are required:

- *origin* : origin's coordinates of the cylinder (select a vertex).
- *vx*     : the base of the cylinder (select a vector).
- *vz*	   : the axis of the cylinder (select a vector).
- *tr*     : a list of radiuses in ascendant order.
- *ta*     : a list of angles in ascendant order. 
- *th*     : a list of heights in ascendant order.

Use the function **makeCylinder** to make a custom cylinder::

	elts = doc.makeCylinder(origin, vx, vz, tr, ta, th)
	
GUI command: :ref:`guicylindercustom`

Operations on *elts*: :ref:`tuielements2`


Example
-------

.. literalinclude:: test_doc/cylinder/cylinder.py
   :linenos:


.. _tuicylinders:

Make Cylinders
==============

To make **two cylinders in T shape** the following data are required for each cylinder:

- *orig* : the origin of the cylinder (select a vertex).
- *vz*	 : the axis of the cylinder (select a vector).
- *rext* : the radius of the cylinder.
- *h*    : the height of the cylinder. 

*One of the two cylinders must be bigger than the other.*

Use the function **makeCylinders**::

	elts = doc.makeCylinders (orig1, vz1, rext1, h1, orig2, vz2, rext2, h2)

GUI command: :ref:`guicylinders`

Operations on *elts*: :ref:`tuielements2`


Example
-------

.. literalinclude:: test_doc/cylinder/cylinders.py
   :linenos:
	

.. _tuipipe:

Pipe
====

.. _tuipipesimple:

Simple Pipe
-----------

To make a simple pipe grid in python mode, the following data are required:

- *nbR* : number of hexahedra on radial.
- *nbA* : number of hexahedra along the perimeter of the pipe.
- *nbH* : number of hexahedra along the axis of the pipe.

Use the function **makePipeTop**::

	elts = doc.makePipeTop(nbR, nbA, nbH)

GUI command: :ref:`guipipesimple`


.. _tuipipeuniform:

Uniform Pipe
------------

The following data are required:


- *origin* : the origin's coordinates of the pipe (select a vertex).
- *vx*     : the base of the pipe (select a vector).
- *vz*	   : the axis of the pipe (select a vector).
- *rint*   : the radius of the hole in the pipe .
- *rext*   : the radius of the pipe.
- *angle*  : angle of the pipe around the Z axis.
- *hauteur*: the height of the pipe.
- *nbR*    : number of hexahedra on radial.
- *nbA*    : number of hexahedra along the perimeter of the pipe.
- *nbH*    : number of hexahedra along the axis of the pipe.

Use the function **makePipeUni** to  make a uniform pipe::

	elts = doc.makePipeUni(origin, vx, vz, rint, rext, angle, hauteur, nbR, nbA, nbH)

GUI command: :ref:`guipipeuniform`


.. _tuipipecustom:

Custom Pipe
-----------

The following data are required:

- *origin* : origin's coordinates of the pipe (select a vertex).
- *vx*     : the base of the pipe (select a vector).
- *vz*	   : the axis of the pipe (select a vector).
- *tr*     : a list of radiuses in ascendant order.
- *ta*     : a list of angles in ascendant order. 
- *th*     : a list of heights in ascendant order.

Use the function **makePipe** to make a custom pipe::

	elts = doc.makePipe(origin, vx, vz, tr, ta, th)
	
GUI command: :ref:`guipipecustom`

Operations on *elts*: :ref:`tuielements2`


Example
-------

.. literalinclude:: test_doc/pipe/pipe.py
   :linenos:
   


.. _tuipipes:

Make Pipes
==========

To make **two pipes in T shape** the following data are required for each pipe:

- *orig* : the origin of the pipe (select a vertex).
- *vz*	 : the axis of the pipe (select a vector).
- *rint* : the internal radius of the pipe.
- *rext* : the radius of the pipe.
- *h*    : the height of the pipe. 

*One of the two pipes must be bigger than the other.*

Use the function **makepipes**::

	elts = doc.makePipes(orig1, vz1, rint1, rext1, h1, orig2, vz2, rint2, rext2, h2)

GUI command: :ref:`guipipes`

Operations on *elts*: :ref:`tuielements2`


Example
-------

.. literalinclude:: test_doc/pipe/pipes.py
   :linenos:
