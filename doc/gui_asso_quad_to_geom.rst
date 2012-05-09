:tocdepth: 3

.. _guiassoquadtogeom:

=============================================
Associate the model of blocks to the geometry
=============================================

The association of the model of blocks to the geometry proceeds in three steps:

- associate all vertices,
- associate the needed edges,
- associate the needed quadrangles.

To facilitate the association of the model to the geometry, a more easy way exists:
the association of lines taken from the model to lines taken from the geometry.

The interaction for an association of an element of the model of blocks to the geometry
proceeds in three stages:

- select the model element(s) to associate,
- select the shape(s) of the geometry to associate to,
- make the association,

.. _guiassovertex:

Associate to a vertex of the geometry
=====================================

To associate a vertex of the model of blocks to a vertex of the geometry in
the **Main Menu** select **Model -> Make Vertex association**

**Arguments:**

- vertex of the model of blocks,
- vertex of the geometry.

The dialogue box to associate to a vertex of the geometry is:

.. image:: _static/gui_ass_vertex.png
   :align: center

.. centered::
   Associate to a Vertex of the Geometry

.. _guiassoedge:

Associate to edge or lines of the geometry
==========================================

To associate edge(s) of the model of blocks to line(s) of the geometry in
the **Main Menu** select **Model -> Make Edge Association**

**Arguments:**

- starting vertex (only if "closed line" is selected),
- one or several edges of the model of blocks,
- one or several lines of the geometry,
- choose to associate a closed line or an opened line,
- Start: starting point on the line,
- End: ending point on the line.

The dialogue box to associate to edges or wires of the geometry is:

.. image:: _static/gui_ass_edge.png
   :align: center

.. centered::
   Associate to Edges or Wires of the Geometry

.. _guiassoface:

Associate to a face of the geometry
===================================

To associate a quadrangle of the model of blocks to a face of the geometry in
the **Main Menu** select **Model -> Make Quadrangle association**

**Arguments:**

- quadrangle of the model of blocks,
- list of faces of the geometry.

The dialogue box to associate to a face or a shell of the geometry is:

.. image:: _static/gui_ass_quad.png
   :align: center

.. centered::
   Associate to a Face or a Shell of the Geometry


TUI command: :ref:`tuiassoquadtogeom`
