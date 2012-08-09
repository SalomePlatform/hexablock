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

GUI command: :ref:`guihexahedron`
