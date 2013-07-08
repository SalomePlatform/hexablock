:tocdepth: 3


.. _tuiprismjoinquad:

============================ 
Extrude and Join Quadrangles
============================

There are two different methods to build hexahedra from quadrangles:

- Extrusion of quadrangles
- Join two sets of quadrangles

.. _tuiextrudequad:

Extrude a quadrangle or quadrangles
===================================

.. _tuiextrudequadssimple:

Simple Extrude
--------------

To extrude one quad or a set of quads the following data are required:

- *quad*    : the quad to be extruded (for one quad extrusion).
- *quads*   : the list of quadrangles to be extruded (for multiple quads extrusion).
- *nbLayers*: the number of layers of hexahedra resulting from the extrusion.

One quad extrusion::

	elts = doc.extrudeQuadTop(quad, nbLayers)

Extrusion of a set of quads::

	elts = doc.extrudeQuadsTop(quads, nbLayers)


GUI command: :ref:`guiextrudequadssimple`


.. _tuiextrudequadsuniform:

Uniform Extrude
---------------

The following data are required:

- *quad*    : the quad to be extruded (for one quad extrusion).
- *quads*   : the list of quadrangles to be extruded (for multiple quads extrusion).
- *dir*     : the direction of the extrusion.
- *length*  : the length of hexahedra resulting from the extrusion.
- *nbLayers*: the number of layers of hexahedra resulting from the extrusion.

One quad extrusion::

	elts = doc.extrudeQuadUni(quad, dir, length, nbLayers)
	
Extrusion of a set of quads::

	elts = doc.extrudeQuadsUni(quads, dir, length, nbLayers)


GUI command: :ref:`guiextrudequadsuniform`


.. _tuiextrudequadscustom:

Custom Extrude
--------------

The following data are required:

- *quad* : the quad to be extruded (for one quad extrusion).
- *quads*: the list of quadrangles to be extruded (for multiple quads extrusion).
- *dir*  : the direction of the extrusion.
- *th*   : a list of heights (in ascendant order because the size is computed from the origin) to specify the size of each layer of extrusion.

One quad extrusion::

	elts = doc.extrudeQuad(quad, dir, th)
	
Extrusion of a set of quads::

	elts = doc.extrudeQuads(quads, dir, th)
	

GUI command: :ref:`guiextrudequadscustom`

Operations on *elts*: :ref:`tuielements2`


Example
-------

.. literalinclude:: test_doc/extrudeQuad/extrude_quad.py
   :linenos:



.. _tuijoinquad:

Join quadrangle or quadrangles
==============================

.. _tuijoinquadsuniform:

Uniform Join
------------

The following data are required:

- *quad*      : the quad to be joined and from which hexahedra will be created (one quad case).
- *quads*     : the set of quadrangles to be joined and from which hexahedra will be created (multiple quad case).
- *fromPointA*: a vertex belonging to the first quadrangle of the set of quadrangles.
- *fromPointB*: a vertex, consecutive to previous Point a, belonging to the first quadrangle.
    
- *quadDest*  : a quadrangle from the set of targeted quadrangles.
- *toPointA*  : the target vertex (in the target quadrangle) for *fromPointA*.
- *toPointB*  : the target vertex (in the target quadrangle) for *fromPointB*.

- *nbLayers*  : number of layers of hexahedra issued from the operation.

To join one quad to an other use the function **joinQuadUni**::

	elts = doc.joinQuadUni(quad, quadDest, fromPointA, fromPointB, toPointA, toPointB, nbLayers)

To join a set of quads to another quad use the function **joinQuadsUni**::

	elts = doc.joinQuadsUni(quads, quadDest, fromPointA, fromPointB, toPointA, toPointB, nbLayers)

GUI command: :ref:`guijoinquadsuniform`


.. _tuijoinquadscustom:

Custom Join
-----------

The following data are required:

- *quad*      : the quad to be joined and from which hexahedra will be created (one quad case).
- *quads*     : the set of quadrangles to be joined and from which hexahedra will be created (multiple quads case).
- *fromPointA*: a vertex belonging to the first quadrangle of the set of quadrangles.
- *fromPointB*: a vertex, consecutive to previous Point a, belonging to the first quadrangle.
    
- *quadDest*  : a quadrangle from the set of targeted quadrangles.
- *toPointA*  : the target vertex (in the target quadrangle) for *fromPointA*.
- *toPointB*  : the target vertex (in the target quadrangle) for *fromPointB*.

- *th*    : a list of heights to specify the size of each layer of hexahedra issued from the operation.

To make a custom join of one quad to another use the function **joinQuad**::

	elts = doc.joinQuad(quad, quadDest, fromPointA, fromPointB, toPointA, toPointB, th)
	
To make a custom join of a set of quads to another quad use the function **joinQuads**::

	elts = doc.joinQuads(quads, quadDest, fromPointA, fromPointB, toPointA, toPointB, th)

GUI command: :ref:`guijoinquadscustom`

Operations on *elts*: :ref:`tuielements2`


Example
-------

.. literalinclude:: test_doc/joinQuad/join_quad.py
   :linenos:
