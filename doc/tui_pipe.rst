:tocdepth: 3


.. _tuipipe:

====
Pipe
====
To add a pipe, the following data have to be mentioned:

- the coordinates of the pipe base
- the direction of the pipe
- height and radius of the pipe
- the inner radius
- the outer radius


Add a pipe in the document::

    p = doc.addPipe(base, direction, int_radius, ext_radius, height)

Get the values of a pipe::

    ver = p.getBase()
    vec = p.getDirection()
    int_r = p.getInternal_radius()
    ext_r = p.getRadius()
    h = p.getHeight()

Access to the pipes of the document::

    np = doc.countPipe()
    pik = doc.getPipe(index)

More operations: *Pipe* inherits :ref:`Element <tuielement>`.

GUI command: :ref:`guipipe`

