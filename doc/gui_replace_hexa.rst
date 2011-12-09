:tocdepth: 3

.. _guireplacehexa:

==========
Replace hexa
==========

To **Replace hexa** in the **Main Menu** select **Model -> Replace Hexa**

.. image:: _static/gui_replacehexa.png
   :align: center

.. centered::
      Replace Hexa Dialog Box


The following data are required:

- Result Name
	- name  : name of the elements created ( **optionnal** )

- Quad(s) source
        - the set of quads to be extruded ( select quads )
	- s1, s2, s3 : vertices from the set of quads. Along with (d1,d2,d3) define the way the operation will be completed ( select vertices )

- Quad destination
        - d1, d2, d3 : vertices which define the target quad. As a result of the operation, each one are replaced : s1->d1, s2->d2, s3->d3  ( select vertices )


The result is CS_TODO:

.. image:: _static/replacehexa.png
   :align: center

.. centered::
   Replace hexa

TUI command: :ref:`tuireplacehexa`


