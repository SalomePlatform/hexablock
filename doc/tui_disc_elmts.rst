:tocdepth: 3

.. _tuidisconnectelements:

===================
Disconnect elements
===================

Disconnect a quadrangle::

    elts = doc.disconnectQuad(hexa, quad)

Disconnect an edge::

    elts = doc.disconnectEdge(hexa, edge)
    
Disconnect edges::

	elts =  doc.disconnectEdges(hexas, edges)
 
Disconnect a vertex::

    elts = doc.disconnectVertex(hexa, ver)

Operations on *elts*... :ref:`tuielements2`

GUI command: :ref:`guidisconnectelements`
