:tocdepth: 3

.. _generale:

###############################
Introduction to HEXABLOCK 
###############################

The HexaBlock module aims at helping users to build hexaedrical meshes. The two fondamental choices are:

- to execute the method by blocks where the user has to determine the needed blocks
- to use directly the geometry to mesh (no need to create a new geometry) in order to obtain the mesh.

The HexaBlock component is destined for: 

- the creation of a model of blocks (see :ref:`blockmethod`)
	
- the association of this model of blocks to the geometry (see :ref:`assomodelgeo`)

- the mesh generation (see :ref:generatemesh`)



.. toctree::
   :maxdepth: 2

   block_method.rst
   mesh.rst
   intro_python.rst
   gui.rst
   tui.rst
