:tocdepth: 3


.. _tuiquadrevolution:

=============== 
Quad Revolution
===============

.. _tuiquadsrevolutionuniform:

Uniform Quad Revolution
=======================

To make a uniform  quad revolution you need:

- *quad*    : the quadrangle from which hexahedrons will be created (only for one quad revolution).
- *quads*   : a set of quadrangles from which hexahedrons will be created (only for multiple quads revolution).
- *center*  : center of rotation (a vertex).
- *axis*    : axis of rotation (a vector).
- *angle*   : Specify the rotation's angle at each step.
- *nbLayers*: the number of steps.

One quad revolution::

	elts = doc.revolutionQuadUni(quad, center, axis, angle, nbLayers)

Revolution of a set of quads::

	elts = doc.revolutionQuadsUni(quads, center, axis, angle, nbLayers)
	

GUI command: :ref:`guiquadsrevolutionuniform`


.. _tuiquadsrevolutioncustom:

Custom Quad Revolution
======================

To make a custom quad revolution you need:

- *quad*    : the quadrangle from which hexahedrons will be created (only for one quad revolution).
- *quads*   : a set of quadrangles from which hexahedrons will be created (only for multiple quads revolution).
- *center*  : center of rotation (a vertex).
- *axis*    : axis of rotation (a vector).
- *angles*  : a set of angles (in degrees). Specify the rotation's angle at each step.

One quad revolution::

	elts = doc.revolutionQuad(quad, center, axis, angles)
	
Revolution of a set of quads::

	elts = doc.revolutionQuads(quads, center, axis, angles)
	
	
GUI command: :ref:`guiquadsrevolutioncustom`

Operations on *elts*: :ref:`tuielements2`


Example
=======

.. literalinclude:: test_doc/quadRevolution/revolution_quad.py
   :linenos:


	