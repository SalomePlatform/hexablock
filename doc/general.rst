:tocdepth: 3

.. _generale:

###############################
Introduction to HEXABLOCK 
###############################

Within the SALOME framework, a CAD module (GEOM) and a meshing module (SMESH) are developped. This tools enable the construction of hexaedrical meshes by means of a CAD cutting in blocks (the method of blocks)  topologically equivalent to hexaedra. Once this cutting performed, the mesh construction is easy, but the method of blocks can be touchy if the piece to mesh is complex.

The Hexablock module aims at helping users to build hexaedrical meshes via this method by blocks. The fondamental choice is to execute the method by blocks where the user has to determine the needed blocks
-
The HexaBlock component is destined for: 

- the construction of the real CAD (see :ref:`cad`)

- the creation of a model of blocks (see :ref:`blockmethod`)
	
- the association of this model of blocks to the geometry (see :ref:`assomodelgeo`)

- the mesh generation (see :ref:`generatemesh`)



.. toctree::
   :maxdepth: 3

   cad.rst
   block_method.rst
   intro_association.rst
   intro_groups.rst
   intro_mesh.rst
   intro_python.rst
   annexe.rst
   
