:tocdepth: 3

.. _generale:

#########################
Introduction to HEXABLOCK 
#########################

.. Within the SALOME framework, a CAD module (GEOM) and a meshing
   module (SMESH) are developped. This tools enable the construction
   of hexaedrical meshes by means of a CAD cutting in blocks (the
   method of blocks) topologically equivalent to hexahedra. Once this
   cutting performed, the mesh construction is easy, but the method of
   blocks can be touchy if the piece to mesh is complex.

Goals
=====

The Hexablock module is designed to helpe users to build hexahedrical meshes
via the method by blocks. The two fundamental choices are:

- To execute the method by blocks where the user has to determine the needed blocks.
- To simplify all other tasks to obtain the mesh (in particular by
  using directly the geometry to mesh).

Solution
========

Finding a coumpound of blocks is in charge of users.
 
Hexablock enables to define a mesh with hexahedra in five stages:

- Load and create a geometry without constraint.
- Search a compound of blocks (mental working).
- Create a model of blocks:

  - Logic realization of this division with operations on blocks.
  - Regardless of the geometry to be meshed in terms of shape and placement.

- Associate this model of blocks to the geometry:

  - Associate all the vertices of the model to the geometry.
  - If necessary, associate edges of the model to the geometry.
  - If necessary, associate quads of the model to the geometry.

- Generate the mesh:

  - Create discretization laws.
  - Discretize the edges.
  - Define mesh groups.
  - Setting the mesh generation.

Two ways of working are possible:

- *Graphic User Interface*
- *Python Scripting*


Presentation of the stages
==========================

- The construction of the real CAD (see :ref:`cad`).

- The creation of a model of blocks (see :ref:`blockmethod`).

- The association of this model of blocks to the geometry (see :ref:`assomodelgeo`).

- The mesh generation (see :ref:`generatemesh`).



.. toctree::
   :maxdepth: 3

   cad.rst
   block_method.rst
   intro_association.rst
   intro_groups.rst
   intro_mesh.rst
   intro_python.rst
   annexe.rst
   
