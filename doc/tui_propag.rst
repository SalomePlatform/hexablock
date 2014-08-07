:tocdepth: 3

.. _tuipropag:

=============================================
Discretization defined on the model of blocks
=============================================

Get the number of propagation existing on the model of blocks::

    np = doc.countPropagation()

Get a propagation of the model of blocks::

    propa = doc.getPropagation(index)

Find a propagation by an edge of the model of blocks::

    propa = doc.findPropagation(edge)

Get all edges of the propagation and the way in the propagation::

    edges = propa.getEdges()
    bo = propa.getWay()

Set a law of discretization for a propagation::

    propa.setLaw(law)

Get the law of the propagation for a propagation::

    law = propa.getLaw()

Set the way where the law is applied on the propagation::

    propa.setWay(bo)


GUI command: :ref:`guipropag`
