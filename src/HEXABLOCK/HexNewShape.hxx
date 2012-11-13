
// class : Gestion des formes associees (Hexa 5)

#ifndef __NEW_SHAPE_H_
#define __NEW_SHAPE_H_

#include "HexEltBase.hxx"

#ifndef NO_CASCADE
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopoDS_Shape.hxx>
#endif

BEGIN_NAMESPACE_HEXA

class NewShape : public EltBase  
{
public :
   NewShape (Document* dad);

   void                 setShape (const TopoDS_Shape& shape);
   const TopoDS_Shape&  getShape ();

   virtual int countVertex ()   { return tab_vertex.size ();  }
   virtual int countEdge   ()   { return tab_edge.size ();    }
   virtual int countFace   ()   { return tab_face.size ();    }

   int getVertexId (int nro);
   int getEdgeId   (int nro);
   int getFaceId   (int nro);

   const TopoDS_Shape& getGeoShape (int id);

   string getBrep ();
   void   setBrep (string& brep);
   //void   saveXml (XmlWriter* xml);

private : 
                                // ---------------------------- Parcours
   void parseShape  (const TopoDS_Shape& shape);
   void push_level  ()     { sh_level ++ ; }
   void pop_level   ()     { sh_level -- ; }
   void addSubShape (const TopoDS_Shape& shape);

private :     // ________________________________________________________
   TopoDS_Shape geo_shape;
   EnumShape    sh_origin;
   string       sh_ident;
   string       geo_brep;
   bool         brep_defined, shape_defined;

   TopTools_IndexedMapOfShape map_shape;
   std::vector <SubShape*>    tab_face;
   std::vector <SubShape*>    tab_edge;
   std::vector <SubShape*>    tab_vertex;

                         // Pour le parsing
   int sh_level;
   int sh_indice;
   int sh_face_max;
   int sh_edge_max;
   int sh_vertex_max;
};
END_NAMESPACE_HEXA
#endif
