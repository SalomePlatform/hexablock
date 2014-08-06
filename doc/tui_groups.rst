:tocdepth: 3

.. _tuigroups:

====================================
Define groups on the model of blocks
====================================

Three kinds of group of meshing elements:

- group of hexahedra:

    HexaBlocks.HexaCell 

- group of quadrangles:

    HexaBlocks.QuadCell

- group of edges: 

    HexaBlocks.EdgeCell 

Four kinds of group of nodes:

- group of nodes in hexahedra:

    HexaBlocks.HexaNode

- group of nodes in quadrangles:

    HexaBlocks.QuadNode

- group of nodes in edges:

    HexaBlocks.EdgeNode

- group of nodes on vertices:

    HexaBlocks.VertexNode



Add a new group in the document::

    grp = doc.addHexaGroup(name)
    grp = doc.addQuadGroup(name)
    grp = doc.addEdgeGroup(name)
    grp = doc.addQuadNodeGroup(name)
    grp = doc.addHexaNodeGroup(name)
    grp = doc.addEdgeNodeGroup(name)
    grp = doc.addVertexNodeGroup(name)

Remove a group from the document::

    doc.removeGroup(grp)

Get the number of groups of the document::

    ng = doc.countGroup()

Get a group of the document::

    grp = doc.getGroup(index)


Find a group by his name in the document::

    g = doc.findGroup(name)

Get the name of a group::

    name = grp.getName()

Set the name of a group::

    grp.setName(name)

Get the kind of group among the 7 possibilities::

    kind = grp.getKind()


Add an element in a group::

    grp.addElement(element)

Get the number of elements of a group::

    nb = grp.countElement()

Get an element of a group::

    el = grp.getElement(index)

Remove an element of a group::

    grp.removeElement(index)

Clear all elements of a group::

    grp.clearElement()


GUI command: :ref:`guigroups`
