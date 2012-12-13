:tocdepth: 3

.. _tuiassocs:

============
Associations
============

To remove an element's association(s)::

	em.clearAssociation()
 
**em** can be a document:

	*clearAssociation()* remove all associations of the elements in the document.
 	
**em** can be a vertex, an edge, a quad or a set of elements:

	in this case, *clearAssociation()*  remove all associations of the given type.
 	
GUI command: :ref:`guiassocs`