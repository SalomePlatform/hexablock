
// class : Gestion des formes associees (Hexa 5)

// Copyright (C) 2009-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef __NEW_SHAPE_H_
#define __NEW_SHAPE_H_

#include "HexEltBase.hxx"

#ifndef SWIG
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <BRep_Builder.hxx>
#endif

BEGIN_NAMESPACE_HEXA

class HexaExport NewShape : public EltBase
{
public :         // for TUI
   virtual int countVertex ()   { return tab_vertex.size ();  }
   virtual int countEdge   ()   { return tab_edge.size ();    }
   virtual int countFace   ()   { return tab_face.size ();    }

   int getVertex (int nro);
   int getEdge   (int nro);
   int getFace   (int nro);

   cpchar getNameVertex  (int nro);
   cpchar getNameEdge    (int nro);
   cpchar getNameFace    (int nro);

   NewShape (Document* dad, EnumShape type=SH_NONE);

#ifndef SWIG
public :         // for GUI
   cpchar              getBrep ();
   const TopoDS_Shape& getShape ();

   EnumShape    getOrigin ()        { return sh_origin ; }
   VertexShape* getVertexShape (int nro);
   EdgeShape*   getEdgeShape (int nro);
   FaceShape*   getFaceShape (int nro);

public :         // for Others
   void  setShape (const TopoDS_Shape& shape, EnumShape type=SH_IMPORT);
   void  setBrep  (rcstring brep);
   int   addPoint (double* coord);
   void  addAssociation (Vertex* elt, int subid, double param);
   void  addAssociation (Edge*   elt, int subid, double pmin, double pmax);
   void  addAssociation (Quad*   elt, int subid);

   VertexShape* findVertex   (int subid);
   VertexShape* findVertex   (double point[]);
   EdgeShape*   findEdge     (int subid);
   EdgeShape*   findEdge     (double p1[], double p2[]);
   FaceShape*   findFace     (int subid);

   SubShape*    findSubShape (int subid);


   const TopoDS_Shape& getGeoShape (int subid);

   int   saveBrep ();
   void  saveXml  (XmlWriter* xml);

   void openShape ();
   int  addCircle (double* center, double rad, double* norm, double* base);
   int  addSphere (double* center, double radius);
   int  transfoShape   (Matrix& matrix, SubShape* shape);
   int  translateShape (double  dir[],  SubShape* shape);
   void closeShape();

private :
   int  updateShape ();
   int  updateBrep  ();
                                // ---------------------------- Parcours
   void parseShape  (const TopoDS_Shape& shape);
   void push_level  ()     { sh_level ++ ; }
   void pop_level   ()     { sh_level -- ; }
   void addSubShape (const TopoDS_Shape& shape);

private :     // ________________________________________________________
   TopoDS_Shape    geo_shape;

   EnumShape    sh_origin;
   string       geo_brep;
   bool         brep_defined, shape_defined;

   TopTools_IndexedMapOfShape map_shape;
   std::vector <FaceShape*>    tab_face;
   std::vector <EdgeShape*>    tab_edge;
   std::vector <VertexShape*>  tab_vertex;

                         // Pour Construire les shapes internes
   int curr_subid;
   TopoDS_Compound geo_compound;
   BRep_Builder    geo_builder;

   std::vector <Vertex*>  asso_vertex;
   Quads      asso_quad;
   Edges      asso_edge;
   RealVector asso_pmin, asso_pmax, asso_param;
   IntVector  asso_edid, asso_quid, asso_vxid;
                         // Pour le parsing
   int sh_level;
   int sh_indice;
   int sh_face_max;
   int sh_edge_max;
   int sh_vertex_max;
#endif
};
END_NAMESPACE_HEXA
#endif
