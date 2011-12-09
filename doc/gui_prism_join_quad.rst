:tocdepth: 3


.. _guiprismjoinquad:


========================== 
Prism and join quadrangles
==========================

.. _prismquad:

Prism Quadrangles
=================

To prism from quadrangles in the **Main Menu** select **Model -> Prism
quad**.

From a list of quadrangles, a set of hexaedra is created.

**Arguments:** list of quadrangles + 1 vector (direction) + number of layers

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

To join quadrangles in the **Main Menu** select **Model -> Join quad**

.. image:: _static/gui_join_quads.png
   :align: center

.. centered::
   Join Quadrangles Dialog Box


The following data are required :

- From
        - QA : the set of quads to be joined and from which hexahedrons will be created ( select quads  )
	- QA_1 : a vertex belonging to the set of quads ( select a vertex )
	- QA_2 : a vertex belonging to the set of quads ( select a vertex )

- To
        - QB : a quad from the set of targeted quads ( select a quad )
	- QB_1 : the target vertex of QA_1. belongs to QB ( select a vertex )
	- QB_2 : the target vertex of QA_2. belongs to QB ( select a vertex )

- Grid
        - size : number of layers of hexahedrons issued from the operation


TUI command: :ref:`tuiprismjoinquad`
