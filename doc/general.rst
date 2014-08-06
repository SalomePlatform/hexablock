:tocdepth: 3

.. _generale:

#########################
Introduction to HEXABLOCK 
#########################

With SALOME framework, a CAD component (GEOM) and a meshing component (SMESH) are developed.
These tools enable to construct hexahedral meshes:

- by means of a CAD cutting in blocks topologically equivalent to hexahedra with GEOM component,

- and generate a hexahedral mesh with SMESH component.

The HEXABLOCK component of SALOME is designed to help users to build hexahedral meshes:

- with also the block method,

-  but without modify the geometry.

Like other SALOME components, HEXABLOCK can also be used in two ways:

- by using Graphic User Interface,

- by writing Python Script.


Stages to obtain hexahedral mesh
================================

HEXABLOCK component enables to define a mesh with hexahedra in 5 stages:

.. toctree::
   :maxdepth: 1

   cad.rst
   block_method.rst
   intro_association.rst
   intro_groups.rst
   intro_mesh.rst

.. toctree::
   :maxdepth: 1

   intro_python.rst
   annexe.rst
