:tocdepth: 3

.. _tuihemisphere:

============================
Hemisphere Grid Construction
============================

.. _tuisphere:

Sphere
======

.. _tuispheresimple:

Simple Sphere
-------------

To create a Simple Sphere Grid in python mode, you need the following arguments:

- *nbR*  : number of hexahedra on radial.
- *nbA*  : number of hexahedra along the perimeter of the sphere.
- *nbH*  : number of hexahedra along the axis of the sphere.
    
Use the function **makeSphereTop**::
     
	elts = doc.makeSphereTop(nbR, nbA, nbH)

GUI command: :ref:`guisphereandrindsimple`


.. _tuisphereuniform:

Uniform Sphere
--------------

The following data are required:

- *center*: center coordinates of the sphere (a vertex).
- *vx*    : the base of the sphere (a vector).
- *vz*	  : the axis of the hole (a vector).
- *rtrou* : the radius of the hole in the sphere.
- *rext*  : the radius of the sphere.
- *angle* : angle of the sphere around the Z axis.
- *vplan* : the vertex along the vertical axis where the rind will be cut.
- *nr*    : number of hexahedra on radial.
- *na*    : number of hexahedra along the perimeter of the sphere.
- *nh*    : number of hexahedra along the axis of the sphere.

Use the function **makeSphereUni** to make a uniform sphere grid::

	elts = doc.makeSphereUni (center, vx, vz, rtrou, rext, angle, vplan, nr, na, nh)
	
GUI command: :ref:`guisphereuniform`
	
.. _tuispherecustom:

Custom Sphere
-------------

The following data are required:

- *center*: center coordinates of the sphere (a vertex).
- *vx*    : the base of the sphere (a vector).
- *vz*	  : the axis of the sphere (a vector).
- *trad*  : a list of radiuses in ascendant order.
- *tang*  : a list of angles in ascendant order. 
- *tphi*  : a list of heights in ascendant order.

Use the function **makeSphere** to make a custom sphere::

	elts = doc.makeSphere(center, vx, vz, trad, tang, tphi)

GUI command: :ref:`guispherecustom`
	
Operations on *elts*: :ref:`tuielements2`


Example
-------

.. literalinclude:: test_doc/hemisphere/sphere.py
   :linenos:



.. _tuirind:

Rind
====

.. _tuirindsimple:

Simple Rind
-----------

To create a Simple Rind Grid in python mode, you need the following arguments:

- *nbR*  : number of hexahedra on radial.
- *nbA*  : number of hexahedra along the perimeter of the rind.
- *nbH*  : number of hexahedra along the axis of the rind.
    
Use the function **makeRindTop**::
     
	elts = doc.makeRindTop(nbR, nbA, nbH)

GUI command: :ref:`guisphereandrindsimple`


.. _tuirinduniform:

Uniform Rind
------------

The following data are required:

- *center*: center coordinates of the rind (a vertex).
- *vx*    : the base of the rind (a vector).
- *vz*	  : the axis of the hole (a vector).
- *rtrou* : the radius of the hole in the rind.
- *rint*  : the internal radius.
- *rext*  : the radius of the rind.
- *angle* : angle of the rind around the Z axis.
- *vplan* : the vertex along the vertical axis where the rind will be cut.
- *nr*    : number of hexahedra on radial.
- *na*    : number of hexahedra along the perimeter of the rind.
- *nh*    : number of hexahedra along the axis of the rind.

Use the function **makeRindUni** to make a uniform rind grid::

	elts = doc.makeRindUni(center, vx, vz, rtrou, rint, rext, angle, vplan, nr, na, nh)
	
GUI command: :ref:`guirinduniform`
	
.. _tuirindcustom:

Custom Rind
-----------

The following data are required:

- *center*: center coordinates of the rind grid (a vertex).
- *vx*    : the base of the rind grid (a vector).
- *vz*	  : the axis of the rind (a vector).
- *trad*  : a list of radiuses in ascendant order.
- *tang*  : a list of angles in ascendant order. 
- *tphi*  : a list of heights in ascendant order.

Use the function **makeRind** to make a custom rind grid::

	elts = doc.makeRind(center, vx, vz, trad, tang, tphi)

GUI command: :ref:`guirindcustom`
	
Operations on *elts*: :ref:`tuielements2`


Example
-------

.. literalinclude:: test_doc/hemisphere/rind.py
   :linenos:



.. _tuiconcentric:

Concentric (Spherical) Grid
===========================

.. _tuiconcentricsimple:

Simple Concentric
-----------------

To create a Simple Concentric Grid in python mode, you need the following arguments:

- *nbLayers*: the number of nested hexahedra.
- *crit*    : the criteria.

Use the function **makeSphericalTop**::
     
	elts = doc.makeSphericalTop(nbLayers, crit)

GUI command: :ref:`guiconcentricsimple`


.. _tuiconcentricuniform:

Uniform Concentric
------------------

The following data are required:

- *center*  : center coordinates of the concentric.
- *vx*      : the base of the concentric (a vector).
- *vz*	    : the axis of the concentric (a vector).
- *rayon*   : the radius.
- *nbLayers*: the number of nested hexahedra.
- *crit*    : the criteria.

Use the function **makeSphericalUni** to make a uniform concentric::

	elts = doc.makeSphericalUni(center, vx, vz, rayon, nbLayers, crit)
	
GUI command: :ref:`guiconcentricuniform`
	
.. _tuiconcentriccustom:

Custom Concentric
-----------------

The following data are required:

- *center*  : center coordinates of the concentric.
- *vx*      : the base of the concentric (a vector).
- *vz*	    : the axis of the concentric (a vector).
- *tr*      : a list of radiuses in ascendant order.
- *crit*    : the criteria.

Use the function **makeSpherical** to make a custom concentric grid::

	elts = doc.makeSpherical (center, vx, vz, tr, crit)

GUI command: :ref:`guiconcentriccustom`
	
Operations on *elts*: :ref:`tuielements2`


Example
-------

.. literalinclude:: test_doc/hemisphere/concentric.py
   :linenos:
