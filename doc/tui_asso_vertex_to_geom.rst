:tocdepth: 3

.. _tuiassovertex2:

========================================
TUI: Definition of point of the geometry
========================================

Firt you need to define the Geom point you want to associate your vertex to.


Define a point by its coordinates::

    geom_object_vertex = geompy.MakeVertex(X, Y, Z)

Define a point by reference::

    geom_object_vertex = geompy.MakeVertexWithRef(Reference, X,Y,Z)

Define a point by an Edge and a Parameter::

    geom_object_vertex = geompy.MakeVertexOnCurve(Edge,Parameter)

Define a point by an Edge and a Length::

    geom_object_vertex = geompy.MakeVertexOnCurveByLength(Edge,Length,StartPoint=None)

Define a point by an Edge and Coordinates::

    geom_object_vertex = geompy.MakeVertexOnCurveByCoord(Edge,X,Y,Z)

Define a point by intersection of two Lines or Wires (or a Wire and a Line)::

    geom_object_vertex = geompy.MakePointOnLinesIntersection(myLine1,myWire1)

Define a point by its position (U-V parameters) on a face::

	geom_object_vertex = geompy.MakeVertexOnSurface(myFace,myUParameter,myVParameter)
	
Define a point by its position (Coordinates) on a face::

	geom_object_vertex = geompy.MakeVertexOnSurface(myFace,X,Y,Z)
	
	
GUI: :ref:`guiassovertex`
