:tocdepth: 3


.. _tuicuthexa:

=============
Cut hexahedra
=============

.. _tuicuthexauniform:

Uniform Cut
===========

To make a uniform cut of an hexahedra in python mode, you need the following arguments:

- *edge*    : an edge on the hexahedra to cut.
- *nbCuts*: the number of cuts.

Use the function **cutUni**::

	elts = doc.cutUni(edge, nbCuts)
	

GUI command: :ref:`guicuthexauniform`


.. _tuicuthexacustom:

Custom Cut
==========

To make a custom cut of an hexahedra you need the following arguments:

- *edge*: an edge on the hexahedra to cut.
- *tl*  : the list of the lengths of the layers in ascendant order.

Use the funtion **cut**::

	elts = doc.cut(edge, tl)
	
GUI command: :ref:`guicuthexacustom`

Operations on *elts*: :ref:`tuielements2`


Example
=======

.. literalinclude:: test_doc/cutHexa/cut_hexa.py
   :linenos:

   