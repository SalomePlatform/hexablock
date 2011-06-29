:tocdepth: 3

.. _blockmethod:

===============================
Principle: the method by blocks
===============================

A block is a volume defined by:

- 8 vertices
- 12 edges
	- *straight line*, *curve*
- 6 quadrangular faces
	- *planar face*, *3D face*

A compound of blocks is a volume that for each face is:

- either a free face (in one block)
- or a face shared between 2 blocks

The method by blocks consists in:

- finding a coumpound of blocks
- this coumpound of blocks has to be equivalent to the initial geometry to mesh.

Applicative choices
===================
Finding a coumpound of blocks is in charge of users.
 
Hexablock enables to define a mesh with hexahedra in five stages:

	1. Load and create a geometry without constraint
	2. Search a compound of blocks (mental working)
	3. Create a model of blocks
	4. Associate this model of blocks to the geometry
	5. Generate the mesh

- Two ways of working:

	- *Graphic User Interface*
	- *Python Scripting*

.. _modelbloc:

Create a model of blocks
========================

A model of block is a compound of blocks based on a topological definition. The model of blocks construction can be build by using the two following methods:

1. Construct and associate blocks (hexahedra)
	- the main operations for creating blocks are:
		- Basics: create, remove,transform
		- construct hexahedra grids: cartesian, cylindrical, spherical
		
	- blocks association operations:
		- merge, cut, disconnect hexahedra 
		- prism, join quadrangles
		- merge two quadrangles of two blocks
		- merge two edges
		- merge two vertices
		- disconnect two vertices
		- create hexahedra between two existing quadrangles

2. Construct hexahedras via predifined models:
	- cylinder
	- pipe
	- two intersected cylinders
	- two intersected pipes

Examples of simple models of blocks:
-------------------------------------

.. image:: _static/ex1.PNG
   :align: center

.. centered::
   2 blocks linked by 1 face


.. image:: _static/EX4.PNG
   :align: center

.. centered::
   2 blocks linked by 2 edges







