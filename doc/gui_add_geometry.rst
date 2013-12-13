:tocdepth: 3

.. _guiaddgeometry:

============================
Add a geometry to a document
============================

Only geometries added to a document can be used by that document.
Vertices, Edges and Quadrangles of the model of blocks of a document can only be associated to those of a geometry that 
has already been added to that document.

To add a geometry to a document:

- *1-* Import the geometry (.brep, .step, ...) in the GEOM Module
- *2-* Activate HexaBlock
- *3-* Open your document or create a new one 
- *4-* In the **Main Menu** select **Association -> Add geometry** 
- *5-* Select the imported geometry and click the **Apply** button of the **Add geometry** Dialog Box
 
 
 .. image:: _static/gui_addshape.png
   :align: center

.. centered::
   Adding geometry to a document
    
    
TUI command: :ref:`tuiaddgeometry`