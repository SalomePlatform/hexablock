:tocdepth: 3

.. _mesh:

============================
Generate the hexahedral mesh
============================

The 3 steps to generate a mesh with HEXABLOCK are:

- define all discretization laws
- apply a law on a set of edges (propagation)
- generate the hexahedral mesh


Define a law of discretization
==============================

A discretization law is defined by four properties:

- the name of the law
- the number of nodes for the discretization law
- the kind of distribution of the law 
    - uniform
    - arithmetic
    - geometric
- the coefficient for the arithmetic and geometric law.

Set a law of discretization on a propagation
============================================

From the model of blocks, an edges partition is determined, each part
characterizes a propagation set. To each propagation is associated a
discretization law and an application way to this law.

By default, the law "Default" is applied on every propagation.

It is possible to use the same law for different propagations.

.. image:: _static/ex2.png
   :align: center

.. centered::
   Example of propagation-linked edges sub-set 

.. _generatemesh:

Generate the hexahedral mesh
============================

To generate the hexahedral mesh, the following parameters are possible:

- give the name of the mesh
- give the dimension of the cells:
    - 3 : to generate hexahedra
    - 2 : to generate only quadrangles
    - 1 : to generate only segments
- give the container for SMESH component

Annexe: mesh algorithm for faces
================================

The mesh algorithm for faces has 2 steps:

- first, mesh with a linear approximation,
- second, make projection of the nodes on the geometric face.

Linear approximation of the discretization of faces
'''''''''''''''''''''''''''''''''''''''''''''''''''

.. image:: _static/meshing1.png
   :align: center


Geometric face and meshing by approximation
'''''''''''''''''''''''''''''''''''''''''''

.. image:: _static/meshing2.png
   :align: center


Geometric face and meshing by approximation and projection
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

.. image:: _static/meshing3.png
   :align: center
 
