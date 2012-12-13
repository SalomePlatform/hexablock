:tocdepth: 3

.. _tuihexahedron:

==========
Hexahedron
==========

Manage the hexahedral of the document.


Add an hexahedron in the document::

    hexa_1 = doc.addHexa(q_A, q_B, q_C, q_D, q_E, q_F)

    hexa_2 = doc.addHexaVertices(ACE, ACF, ADE, ADF, BCE, BCF, BDE, DBF)

Notice: the arguments follow the convention explain in :ref:`annexe`

Get the number of all hexahedra of the document::

    sum_a = doc.countHexa()

Get an hexahedron of the document::

    hexa_i = doc.getHexa(i)
    
Get a vertex of an hexahedron::

	vertex_i = hexa_i.getVertex(i)

Get an edge of an hexahedron::

	edge_i = hexa_i.getEdge(i)

Get a quad of an hexahedron::

	quad_i = hexa_i.getQuad(i)
	
Get the number of used hexahedrons in the model of blocks::

    nb_used_hexa = doc.countUsedHexa()
    
Get the hexahedrons used by the model of blocks

.. literalinclude:: test_doc/hexa/hexa.py

Find an hexahedron in the document::

    hexa = doc.findHexa(vertex_1, vertex_2)
    
Add an hexahedron with 5 quads::

	doc.addHexa5Quads (qa, qc, qd, qe, qf)
	
Add an hexahedron with 4 quads::

	doc.addHexa4Quads (qa, qd, qe, qf)
	
Add an hexahedron with 3 quads::

	doc.addHexa3Quads (qa, qd, qe)
	
Add an hexahedron with 2 quads::

	doc.addHexa2Quads (qa, qb)

More operations: *Hexa* inherits :ref:`Element <tuielement>`.

GUI command: :ref:`guihexahedron`
