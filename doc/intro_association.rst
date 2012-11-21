:tocdepth: 3

.. _assomodelgeo:

===========================================
Associate a model of blocks to the geometry
===========================================

The association of the model of blocks to the geometry, need to follow 3 successive steps:

- associate all vertices of the model of blocks to points taken from the geometry
- associate only the necessary edges of the model of blocks to edges taken from the geometry
- associate only the necessary quadrangles of the model of blocks to faces taken from the geometry


.. image:: _static/association2.png
   :align: center

.. centered::
   Rod connecting

.. _assoelemts:

Associate the vertices of the model of blocks to points taken from the geometry
===============================================================================

This step is essential, all the vertices of the model of blocks must be associated to point taken from the geometry.

There are 4 ways for this association:

- associate a vertex of the model of blocks to a vertex of the geometry

- associate a vertex of the model of blocks to a point taken from an edge of the geometry

- associate a vertex of the model of blocks to a point taken from a face of the geometry

- associate a vertex of the model of blocks to a new point of the space

.. _assoedges:

Associate the edges of the model of blocks to lines taken from the geometry
===========================================================================

Only the necessary edges of the model of blocks need to be associated to an edge or a set of edges of the geometry.

Shortcut: association by lines

Associate the quadrangles of the model of blocks to faces taken from the geometry
=================================================================================

Only the necessary quadrangles of the model of blocks need to be associated to a face or a set of face of the geometry.

Shortcut: such association is not required if faces of the geometry are:

- planar faces
- cylindrical faces
- conical faces

.. _assolines:

Shortcut for some associations: use implicit association
========================================================
