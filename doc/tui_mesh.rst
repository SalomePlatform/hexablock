:tocdepth: 3

.. _tuimesh:


=================
Generate the mesh
=================

Use the SMESH component of SALOME::

    import smesh
    m = hexablock.mesh(name, doc)

Export to a MED file::

    m.ExportMED("/tmp/mesh.med")

Get some information about the mesh::

    print "Number of hexahedra: ", m.NbHexas()
    print "Number of quadrangles: ", m.NbQuadrangles()
    print "Number of segments: ", m.NbEdges()
    print "Number of nodes: ", m.NbNodes()

GUI command: :ref:`guimesh`
