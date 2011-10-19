:tocdepth: 3

.. _tuihexahedron:

==========
Hexahedron
==========


Add an hexahedron in the document::

	 h1 = doc.addHexa(q_A, q_B, q_C, q_D, q_E, q_F)

	 h2 = doc.addHexaVertices(ACE, ACF, ADE, ADF, BCE, BCF, BDE, DBF)

Get the number of hexahedra of the document::

	 nh = doc.countHexa()

Get an hexahedron of the document::

 	hexa = doc.getHexa(i)

GUI command: :ref:`guihexahedron`



