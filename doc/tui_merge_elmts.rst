:tocdepth: 3

.. _tuimergeelements:

============== 
Merge elements
==============


Merge 2 quadrangles::

    l = doc.mergeQuads(qa, qb, va1, vb1, va2, vb2)
 
where:

- qa and qb are quadrangles. qa is the target quadrangle.
- va1, vb1, va2, vb2 are vertices

va1 : vertex of the quadrangle qa to merge with the vertex vb1 of the
quadrangle qb.

va2 : vertex of the quadrangle qa to merge with the vertex vb2 of the
quadrangle qb.

va1, vb1, va2, vb2 can be set to `None` if the quadrangles are contiguous.

Merge 2 edges::

    l = doc.mergeEdges(e1, e2, v1, v2)

where:

- e1 and e2 are edges
- v1 is the vertex of the edge e1 to merge with the vertex v2 of the edge e2


Merge 2 vertices::

    l = doc.mergeVertices(v1, v2)

where v1 and v2 are vertices.

GUI command: :ref:`guimergeelements`
