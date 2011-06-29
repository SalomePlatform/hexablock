:tocdepth: 3


.. _tuielements:

========
Elements
========

Add an empty set of elements::

	 elts = doc.addElements()

Add an element in a set of elements::

	 elts.addElement(element)

Remove an element in a set of elements::

	 elts.removeElement(element)

Remove all elements in a set of elements::

	 elts.clearElement()

Access to the elements of the document::
	
	 nel = doc.countElements()
	 elts = doc.getElements(index)

GUI command: :ref:`guielements`

