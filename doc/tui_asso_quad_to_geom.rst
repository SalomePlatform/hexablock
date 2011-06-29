:tocdepth: 3

.. _tuiassoquadtogeom:

=============================================================
Associate a quadrangle of the model of blocks to the geometry
=============================================================

Associate to a vertex of the geometry::

 	vx.setAssociation(geom_object_vertex)

Get the association::

 	gov = vx.getAssociation()

Associate to an edge or a wire of the geometry::

	 l = edg.addAssociation(geom_object_1D, debut, val)

**todo debut ? val ?**

Get the associations::

	 edge_associations = edg.getAssociations()

Associate to a face or a shell of the geometry::

 	l = quad.addAssociation(geom_object_2D)

Give the association::

 	go2d = quad.getAssociations()

Associate Line
==============

To associate an opened line to the geometry, the following data have
to be mentionned:

- the starting edge : *mstart*
- *todo Edges* : *mline*
- *todo Shape* : *gstart*
- *todo double* : *pstart*
- *todo Shapes* : *gline*
- *todo double* : *pend*

Associate an opened line::

        l = doc.associateOpenedLine(mstart, mline, gstart, pstart, gline, pend) 

To associate an closed line to the geometry, the following data have
to be mentionned:

- the starting vertex : *mfirst*
- the starting edge : *mstart*
- *todo Edges* : *mline*
- *todo Shape* : *gstart*
- *todo double* : *pstart*
- *todo Shapes* : *gline*

Associate an opened line::

        l = doc.associateClosedLine(mfirst, mstart, mline, gstart, pstart, gline) 

Example
=======

::

*todo*

GUI command: :ref:`guiassoquadtogeom`
