:tocdepth: 3


.. _tuicartgrid:

====================
Make cartesian grids
====================

To add a cartesian grid to the model, the following data are required:

- the vertex of the initial hexahedron: *pt*
- the vectors corresponding to the diametrically opposite vertices to the initial vertex: *vx, vy, vz*
- the number of repetition of this hexahedra along the three axes: *ni, nj, nk*

Make a Cartesian grid::

    elts = doc.makeCartesian(pt, vx, vy, vz, ni, nj, nk)

or::

    elts = doc.makeCartesian1(pt, vec1, px, py, pz, mx, my, mz)


Access to the number of elements of the grid::

    vn = elts.countVertex()
    en = elts.countEdge()
    qn = elts.countQuad()
    hn = elts.countHexa()

Access to all elements of the grid::

    vi = elts.getVertex(i)
    ej = elts.getEdge(j)
    qk = elts.getQuad(k)
    hl = elts.getHexa(l)

Specialized access to all vertices of the grid::

    ve_xyz = elts.getVertexIJK(i, j, k)
    
    with:
        0 <= i < ni+1
        0 <= j < nj+1
        0 <= k < nk+1

Specialized access to all edges // vx axis of the grid::

    ed_a = elts.getEdgeI(i, j, k)
    
    with:
        0 <= i < ni
        0 <= j < nj+1
        0 <= k < nk+1

Specialized access to all edges // vy axis of the grid::

    ed_b = elts.getEdgeJ(i, j, k)
    
    with:
        0 <= i < ni+1
        0 <= j < nj
        0 <= k < nk+1

Specialized access to all edges // vz axis of the grid::

    ed_c = elts.getEdgeK(i, j, k)
    
    with:
        0 <= i < ni+1
        0 <= j < nj+1
        0 <= k < nk

Specialized access to all quadrangles // on plane vx, vy::

    qu_a = elts.getQuadIJ(i, j, k)

    with:
        0 <= i < ni
        0 <= j < nj
        0 <= k < nk+1

Specialized access to all quadrangles // on plane vx, vz::

    qu_b = elts.getQuadIK(i, j, k)
    
    with:
        0 <= i < ni
        0 <= j < nj+1
        0 <= k < nk

Specialized access to all quadrangles // on plane vy, vz::

    qu_c = elts.getQuadJK(i, j, k)
    
    with:
        0 <= i < ni+1
        0 <= j < nj
        0 <= k < nk

Specialized access to all hexahedra of the grid::

    he_xyz = elts.getHexaIJK(i, j, k)
    
    with:
        0 <= i < ni
        0 <= j < nj
        0 <= k < nk

Example
-------

::
    
    import hexablock
    doc = hexablock.addDocument()

    orig = doc.addVertex(0, 0, 0)

    dx = doc.addVector(1, 0, 0)
    dy = doc.addVector(0, 1, 0)
    dz = doc.addVector(0, 0, 1)

    ni = 15 
    nj = 12
    nk = 8

    grid = doc.makeCartesian(orig, dx, dy, dz, ni, nj, nk)


.. image:: _static/cartgrid3.png
   :align: center

.. centered::
   Cartesian Grid


GUI command: :ref:`guicartgrid`
