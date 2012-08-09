:tocdepth: 3


.. _guiprismjoinquad:


========================== 
Prism and join quadrangles
==========================

.. _prismquad:

Prism Quadrangles
=================

To prism from quadrangles in the **Main Menu** select **Model -> Prism Quadrangles**.

From a list of quadrangles, a set of hexahedra is created.

**Arguments:** list of quadrangles, a vector (direction) and the number of layers.

The dialogue box to prism from quadrangles is:


.. image:: _static/gui_prism_quads.png
   :align: center

.. centered::
   Prism From Quadrangles



Example:

.. image:: _static/prisme_gui.png
   :align: center

.. centered::
   Prism Quads



.. _joinquad:

Join Quadrangles
================

To join quadrangles in the **Main Menu** select **Model -> Join Quadrangles**.

.. image:: _static/gui_join_quads.png
   :align: center

.. centered::
   Join Quadrangles Dialog Box


The following data are required:

- From
    - Quadrangles: the set of quadrangles to be joined and from which hexahedra will be created.
    - Point a: a vertex belonging to the set of quadrangles.
    - Point b: a vertex belonging to the set of quadrangles.

- To
    - Quadrangle: a quadrangle from the set of targeted quadrangles.
    - Point a: the target vertex (in the target quadrangle) for *From.Point a*.
    - Point b: the target vertex (in the target quadrangle) for *From.Point b*.

- Grid
    - size: number of layers of hexahedra issued from the operation.


TUI command: :ref:`tuiprismjoinquad`
