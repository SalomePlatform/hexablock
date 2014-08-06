:tocdepth: 3


.. _tuiaddgeometry:

============================
Add a geometry to a document
============================

To add a geometry to a document in python mode, you need the following arguments:

- *doc*        : the document
- *geom*       : the geometry
- *shape_name* : the name of the geometry

Use the function **addShape**::

    doc   = hexablock.addDocument (doc_name)
    geom  = geompy.ImportBREP  (shape_name + ".brep")
    shape = hexablock.addShape (doc, geom , shape_name)
	
GUI command: :ref:`guiaddgeometry`
