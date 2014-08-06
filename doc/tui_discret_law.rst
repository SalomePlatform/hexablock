:tocdepth: 3

.. _tuidiscretelaw:

==============================
Define a law of discretization
==============================

Add a new law of discretization in the document::

    law_1 = doc.addLaw(name, nb_nodes)
    
Add laws on propagations based on min or max segment length::

	hexablock.addLaws(doc, lg, lgmax)

Get the number of the law in the document::

    nl = doc.countLaw()

Get a law of the document::

    law_i = doc.getLaw(i)

Remove a law from the document::

    doc.removeLaw(law_1)

Find a law of discretization in the document::

    law_1 = doc.findLaw(name)

Get the name of the law::

    name = law_1.getName()

Change the name of the law::

    law_1.setName(name)

Get the number of nodes for the law of discretization::

    nb = law_1.getNodes()

Set the number of nodes for the law of discretization::

    law_1.setNodes(nb)

Get the kind of the law (uniform, arithmetic, geometric)::

    kd = law_1.getKind()

Set the kind of law of discretization::

    law_1.setKind(kind)

Get the coefficient of the law::

    coef = law_1.getCoefficient()

Set the coefficient of the law::

    law_1.setCoefficent(coef)


Get the default law. This law is applied when there is no law on a propagation::

    default_law = doc.getLaw(0)

It is possible to change the default law::

    default_law.setNodes(10)

And the default law is not removable

GUI command: :ref:`guidiscretelaw`
