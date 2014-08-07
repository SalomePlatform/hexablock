:tocdepth: 3


.. _tuicylinder:

========
Cylinder
========

To add a cylinder, the following data have to be mentioned:

- the coordinates of the cylinder base
- the direction of the cylinder
- height and radius of the cylinder


Add a cylinder in the document::

    cyl = doc.addCylinder(base, direction, radius, height)

Get the values of a cylinder::

    ver = cyl.getBase()
    vec = cyl.getDirection()
    r = cyl.getRadius()
    h = cyl.getHeigth()

Access to the cylinders of the document::

    nc = doc.countCylinder()
    cylk = doc.getCylinder(ind)

More operations: *Cylinder* inherits :ref:`Element <tuielement>`.

GUI command: :ref:`guicylinder`
