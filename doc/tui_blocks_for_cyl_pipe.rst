:tocdepth: 3


.. _tuiblockscylinderpipe:

=====================================
Make blocks for a cylinder and a pipe
=====================================

Make blocks for a cylinder
==========================

::

	 elts = doc.makeCylinder(cyl, vb, nr, na, nl)

cyl: cylinder

vb: vector on the base of the cylinder to start hexahedra.

nr: number of blocks on radial.

na: number of angular section.

nl: number of blocks along the axis of the cylinder.


Example
-------

::

        import hexablock
        doc = hexablock.addDocument()

        base = doc.addVertex(0, 0, 0)
        direction = doc.addVector(0, 0, 1)
        radius = 4
        height = 5

        cyl = doc.addCylinder(base, direction, radius, height)

        vb = doc.addVector(1, 0, 0)
        nr = radius
        na = 9  
        nl = height
        elts = doc.makeCylinder(cyl, vb, nr, na, nl)

.. image:: _static/cylinder.png
   :align: center

.. centered::
   Cylinder


Make blocks for a pipe
======================

::

	 elts = doc.makePipe(pi, vb, nr, na, nl)

Example
-------

::

        import hexablock
        doc = hexablock.addDocument()

        orig = doc.addVertex(50, 0, 0)
        vz = doc.addVector(0, 0, 1)
        int_radius = 3
        ext_radius = 4
        height = 5
        pi = doc.addPipe(orig, vz, int_radius, ext_radius, height)

        vb = doc.addVector(1, 0, 0)
        nr = 4
        na = 9
        nl = 5
        elts = doc.makePipe(pi, vb, nr, na, nl)

.. image:: _static/pipe.png
   :align: center

.. centered::
   Pipe

GUI command: :ref:`guiblockscylinderpipe`