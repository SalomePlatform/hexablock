
// C++ : Gestion des Shapes (Hexa v5)

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

#include "HexNewShape.hxx"
#include "HexSubShape.hxx"
#include "HexVertexShape.hxx"
#include "HexEdgeShape.hxx"
#include "HexFaceShape.hxx"

#include "HexQuad.hxx"
#include "HexEdge.hxx"
#include "HexMatrix.hxx"

#include "HexKas_functions.hxx"

#include "HexXmlWriter.hxx"
#include "HexXmlTree.hxx"

#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Compound.hxx>

#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepBuilderAPI_Transform.hxx>

#include <BRep_Tool.hxx>


#include <gp_Pnt.hxx>
#include <gp_Pln.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Circ.hxx>

#include <Basics_OCCTVersion.hxx>

BEGIN_NAMESPACE_HEXA

static bool db  = on_debug ();  // == getenv ("HEXA_DB") > 0
static bool db0 = db;

// ====================================================== Constructeur
NewShape::NewShape (Document* dad, EnumShape type)
        : EltBase  (dad, EL_SHAPE)
{
   sh_origin    =  type;
   brep_defined =  shape_defined = false;

   sh_level     = sh_indice      = 0;
   sh_face_max  = sh_edge_max    = sh_vertex_max = 0;

   curr_subid = 2;
}
// ====================================================== setShape
void NewShape::setShape (const TopoDS_Shape& shape, EnumShape type)
{
   sh_origin = type;
   geo_shape = shape;
   geo_brep  = "";

   shape_defined = true;
   brep_defined  = false;

   map_shape .Clear ();
   tab_face  .clear ();
   tab_edge  .clear ();
   tab_vertex.clear ();

   sh_level    = sh_indice   = 0;
   sh_face_max = sh_edge_max = sh_vertex_max = 0;

   if (db)
      cout <<  " ------------------- setShape " << el_name << endl;
   parseShape (geo_shape);
}
// ====================================================== setBrep
void NewShape::setBrep (rcstring brep)
{
   geo_brep     = brep;
   brep_defined = true;
   updateShape ();
}
// ====================================================== getGeoShape
const TopoDS_Shape& NewShape::getGeoShape (int id)
{
   const TopoDS_Shape& shape = map_shape.FindKey (id);
   return shape;
}
// ====================================================== getShape
const TopoDS_Shape& NewShape::getShape ()
{
   return geo_shape;
}
// ====================================================== openShape
void NewShape::openShape ()
{
   geo_builder.MakeCompound (geo_compound);
   curr_subid = 2;

   asso_edge.clear ();
   asso_pmin.clear ();
   asso_pmax.clear ();
   asso_edid.clear ();
   asso_quad.clear ();
   asso_quid.clear ();
}
// ====================================================== addCircle
int NewShape::addCircle (double* milieu, double rayon, double* normale,
                         double* base)
{
   gp_Pnt gp_center (milieu [dir_x], milieu [dir_y], milieu [dir_z]);
   gp_Vec gp_vx     (base   [dir_x], base   [dir_y], base   [dir_z]);
   gp_Vec gp_norm   (normale[dir_x], normale[dir_y], normale[dir_z]);

   gp_Ax2  gp_axes (gp_center, gp_norm, gp_vx);
   gp_Circ gp_circ (gp_axes,   rayon);

   TopoDS_Edge  geom_circ = BRepBuilderAPI_MakeEdge(gp_circ).Edge();
   geo_builder.Add (geo_compound, geom_circ);

   if (db)
   cout << " .... AddCircle subid " << curr_subid << " : rayon=" << rayon
        << " centre = " << milieu[dir_x] << "," << milieu[dir_y]
        << "," << milieu[dir_z]
        << " norm = " << normale[dir_x] << "," << normale[dir_y]
        << "," << normale[dir_z]
        << endl;

   int  subid  = curr_subid;
   curr_subid += 2;
   return subid;
}
// ====================================================== addSphere
int NewShape::addSphere (double* milieu, double radius)
{
   gp_Pnt  gp_center (milieu [dir_x], milieu [dir_y], milieu [dir_z]);
   gp_Ax2  gp_axis (gp_center, gp_Dir(0,0,1), gp_Dir(1,0,0));

   BRepPrimAPI_MakeSphere make_sphere (gp_axis, radius);
   make_sphere.Build();
   TopoDS_Shape   geom_sphere = make_sphere.Face();
   geo_builder.Add (geo_compound, geom_sphere);

   if (db)
   cout << " .... AddSphere subid " << curr_subid << " : rayon=" << radius
        << " centre = " << milieu[dir_x] << "," << milieu[dir_y]
        << "," << milieu[dir_z] << endl;

   int  subid  = curr_subid;
   curr_subid += 7;
   return subid;
}
// ====================================================== transfoShape
int NewShape::transfoShape (Matrix& matrice, SubShape* shape)
{
   gp_Trsf transfo;
   double             a11,a12,a13,a14, a21,a22,a23,a24, a31,a32,a33,a34;
   matrice.getCoeff  (a11,a12,a13,a14, a21,a22,a23,a24, a31,a32,a33,a34);
#if OCC_VERSION_LARGE > 0x06070100
   transfo.SetValues (a11,a12,a13,a14, a21,a22,a23,a24, a31,a32,a33,a34);
#else
   transfo.SetValues (a11,a12,a13,a14, a21,a22,a23,a24, a31,a32,a33,a34,
                      Epsil2, Epsil2);
#endif

   TopoDS_Shape shape_orig = shape->getShape ();
   BRepBuilderAPI_Transform builder (shape_orig, transfo, Standard_True);
   TopoDS_Shape result = builder.Shape();

   geo_builder.Add (geo_compound, result);

   int  subid  = curr_subid;
   curr_subid += 2;                       // PROVISOIRE
   return subid;
}
// ====================================================== translateShape
int NewShape::translateShape (double dir[], SubShape* shape)
{
   gp_Trsf transfo;
   gp_Vec  vecteur        (dir [dir_x], dir [dir_y], dir [dir_z]);
   transfo.SetTranslation (vecteur);

   TopoDS_Shape shape_orig = shape->getShape ();
   BRepBuilderAPI_Transform builder (shape_orig, transfo, Standard_True);
   TopoDS_Shape result = builder.Shape();

   geo_builder.Add (geo_compound, result);

   int  subid  = curr_subid;
   curr_subid += 2;                       // PROVISOIRE
   return subid;
}
// ====================================================== addAssociation
void NewShape::addAssociation (Vertex* vertex, int subid, double param)
{
   if (vertex==NULL)
      return;

   asso_vertex.push_back (vertex);
   asso_param .push_back (param);
   asso_vxid  .push_back (subid);
}
// ====================================================== addAssociation
void NewShape::addAssociation (Edge* edge, int subid, double pmin, double pmax)
{
   if (edge==NULL)
      return;

   if (db0) 
      {
      cout << edge->getName() 
           << " = (" << edge->getVertex(0)->getName() 
           << ", "   << edge->getVertex(1)->getName() 
           << ") --> subid= " << subid << " [ " 
           << pmin << ", " << pmax << "]" << endl;
      }

   asso_edge.push_back (edge);
   asso_pmin.push_back (pmin);
   asso_pmax.push_back (pmax);
   asso_edid.push_back (subid);
}
// ====================================================== addAssociation
void NewShape::addAssociation (Quad* quad, int subid)
{
   if (quad==NULL)
      return;

   asso_quad.push_back (quad);
   asso_quid.push_back (subid);
}
// ====================================================== closeShape
void NewShape::closeShape ()
{
   geo_shape     = geo_compound;
   shape_defined = true;
   if (db)
   cout <<  " ------------------- closeShape " << el_name << endl;
   parseShape (geo_shape);

   int nombre = asso_edge.size ();
   for (int nro = 0 ; nro < nombre ; ++nro)
       asso_edge[nro]->addAssociation (this, asso_edid[nro], asso_pmin[nro],
                                                             asso_pmax[nro]);
   nombre = asso_quad.size ();
   for (int nro = 0 ; nro < nombre ; ++nro)
       asso_quad[nro]->addAssociation (this, asso_quid[nro]);

   Real3 point;
   nombre = asso_vertex.size ();
   for (int nro = 0 ; nro < nombre ; ++nro)
       {
       EdgeShape* line   = findEdge (asso_vxid[nro]);
       Vertex*    vertex = asso_vertex[nro];
       if (line!=NULL && vertex!=NULL && NOT vertex->isAssociated())
          {
          line->getPoint (asso_param[nro], point);
          asso_vertex[nro]->setAssociation (point);
          }
       }

}
//--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8
// ====================================================== findVertex
VertexShape* NewShape::findVertex (int shid)
{
   int nbre = tab_vertex.size ();
   for (int nro=0 ; nro < nbre ; nro++)
       {
       VertexShape* shape = tab_vertex [nro];
       if (shape->getIdent() == shid)
          return shape;
       }
   return NULL;
}
// ====================================================== findVertex
VertexShape* NewShape::findVertex (double point[])
{
   int nbre = tab_vertex.size ();
   for (int nro=0 ; nro < nbre ; nro++)
       {
       VertexShape* shape = tab_vertex [nro];
       if (shape->definedBy (point))
          return shape;
       }
   return NULL;
}
// ====================================================== findEdge
EdgeShape* NewShape::findEdge (int shid)
{
   int nbre = tab_edge.size ();
   for (int nro=0 ; nro < nbre ; nro++)
       {
       EdgeShape* shape = tab_edge [nro];
       if (shape->getIdent() == shid)
          return shape;
       }
   return NULL;
}
// ====================================================== findEdge
EdgeShape* NewShape::findEdge (double p1[], double p2[])
{
   int nbre = tab_edge.size ();
   for (int nro=0 ; nro < nbre ; nro++)
       {
       EdgeShape* shape = tab_edge [nro];
       if (shape->definedBy (p1, p2))
          return shape;
       }
   return NULL;
}
// ====================================================== findFace
FaceShape* NewShape::findFace (int shid)
{
   int nbre = tab_face.size ();
   for (int nro=0 ; nro < nbre ; nro++)
       {
       FaceShape* shape = tab_face [nro];
       if (shape->getIdent() == shid)
          return shape;
       }
   return NULL;
}
// ====================================================== findSubShape
SubShape* NewShape::findSubShape (int shid)
{
   SubShape* shape = findEdge (shid);
   if (shape==NULL)
       shape = findVertex (shid);
   if (shape==NULL)
       shape = findFace (shid);

   return shape;
}
// ====================================================== getVertex
int NewShape::getVertex (int nro)
{
   if (nro <0 || nro >= (int) tab_vertex.size())
      return 0;

   SubShape* shape = tab_vertex [nro];
   return shape->getIdent() ;
}
// ====================================================== getEdge
int NewShape::getEdge   (int nro)
{
   if (nro <0 || nro >= (int) tab_edge.size())
      return 0;

   SubShape* shape = tab_edge [nro];
   return shape->getIdent() ;
}
// ====================================================== getFace
int NewShape::getFace   (int nro)
{
   if (nro <0 || nro >= (int) tab_face.size())
      return 0;

   SubShape* shape = tab_face [nro];
   return shape->getIdent() ;
}

// ====================================================== getNameVertex
cpchar NewShape::getNameVertex  (int nro)
{
   if (nro <0 || nro >= (int) tab_vertex.size())
      return "?";

   SubShape* shape = tab_vertex [nro];
   return shape->getName() ;
}
// ====================================================== getNameEdge
cpchar NewShape::getNameEdge    (int nro)
{
   if (nro <0 || nro >= (int) tab_edge.size())
      return "?";

   SubShape* shape = tab_edge [nro];
   return shape->getName() ;
}
// ====================================================== getNameFace
cpchar NewShape::getNameFace    (int nro)
{
   if (nro <0 || nro >= (int) tab_face.size())
      return "?";

   SubShape* shape = tab_face [nro];
   return shape->getName() ;
}
// ====================================================== getFaceShape
FaceShape* NewShape::getFaceShape (int nro)
{
   if (nro <0 || nro >= (int) tab_face.size())
      return 0;

   FaceShape* shape = tab_face [nro];
   return shape;
}
// ====================================================== getEdgeShape
EdgeShape* NewShape::getEdgeShape (int nro)
{
   if (nro <0 || nro >= (int) tab_edge.size())
      return 0;

   EdgeShape* shape = tab_edge [nro];
   return shape;
}
// ====================================================== getVertexShape
VertexShape* NewShape::getVertexShape (int nro)
{
   if (nro <0 || nro >= (int) tab_vertex.size())
      return 0;

   VertexShape* shape = tab_vertex [nro];
   return shape;
}
// ====================================================== addPoint
int NewShape::addPoint (double* point)
{
   char suffix [32];
   int  subid = tab_vertex.size() + 2;

   sprintf (suffix, ":vertex_%02d", subid);
   string name = el_name + suffix;

   VertexShape* sub_shape = new VertexShape (this, subid, point);
   sub_shape->setName (name);
   tab_vertex.push_back (sub_shape);
   return subid;
}
// ===================================================== saveBrep
int NewShape::saveBrep ()
{
   string filename = el_name + ".brep";

   int ier   = updateBrep ();
   if (ier != HOK)
      return ier;

   pfile fic = fopen (filename.c_str(), "w");
   if (fic==NULL)
      return HERR;

   fprintf (fic, "%s\n", geo_brep.c_str());
   fclose  (fic);
   return HOK;
}
// ===================================================== getBrep
cpchar NewShape::getBrep ()
{
   int ier   = updateBrep ();
   if (ier != HOK)
      return NULL;
   return geo_brep.c_str();
}
// ====================================================== saveXml
void NewShape::saveXml (XmlWriter* xml)
{
   if (sh_origin==SH_CLOUD)
      {
      int nbre = tab_vertex.size();
      xml->addMark ("Cloud");
      for (int nro=0 ; nro<nbre ; nro++)
          tab_vertex[nro]->saveXml (xml);
      xml->closeMark ();
      return;
      }

   char buffer [32];
   int  ya1brep = updateBrep ();

   xml->openMark     ("Shape");
   xml->addAttribute ("id",    getName (buffer));
   xml->addAttribute ("type",  sh_origin);
   if (el_name!=buffer)
      xml->addAttribute ("name", el_name);

   if (ya1brep == HOK)
      {
      clean_brep (geo_brep);
      xml->addAttribute ("brep",  geo_brep.c_str());
      }
   xml->closeMark ();
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// ===================================================== parseShape
void NewShape::parseShape (const TopoDS_Shape& shape)
{
   push_level ();
   addSubShape (shape);

   TopoDS_Iterator iter (shape);
   for (; iter.More(); iter.Next())
       {
       TopoDS_Shape sub_shape = iter.Value();
       parseShape (sub_shape);
       }

   pop_level ();
}
// ===================================================== addSubShape
void NewShape::addSubShape (const TopoDS_Shape& shape)
{
   char chnum [32];
   string name;
   sh_indice ++;

   int subid = map_shape.Add (shape);
   int type  = shape.ShapeType();

   switch (type)
      {
      case TopAbs_FACE :
           sprintf (chnum, ":face_%02d", subid);
           name = el_name + chnum;
           if (subid > sh_face_max)
              {
              FaceShape* sub_shape = new FaceShape (this, subid);
              sub_shape->setName (name);
              tab_face.push_back (sub_shape);
              sh_face_max = subid;
              }
           break;
      case TopAbs_EDGE  :
           sprintf (chnum, ":edge_%02d", subid);
           name = el_name + chnum;
           if (subid > sh_edge_max)
              {
              EdgeShape* sub_shape = new EdgeShape (this, subid);
              sub_shape->setName (name);
              tab_edge.push_back (sub_shape);
              sh_edge_max = subid;
              }
           break;
      case TopAbs_VERTEX :
           sprintf (chnum, ":vertex_%02d", subid);
           name = el_name + chnum;
           if (subid > sh_vertex_max)
              {
              VertexShape* sub_shape = new VertexShape (this, subid);
              sub_shape->setName (name);
              tab_vertex.push_back (sub_shape);
              sh_vertex_max = subid;
              }
           break;
      default : ;
           sprintf (chnum, "type=%d, subid=%02d", type, subid);
           name   = chnum;
      }

   if (db0)
      {
      sprintf (chnum, "%4d : ", sh_indice);
      cout <<  chnum;
      for (int nro=1; nro<sh_level; nro++) cout << " | ";
      cout <<  name;

      if (type == TopAbs_VERTEX)
         {
         TopoDS_Vertex gver = TopoDS::Vertex (shape);
         gp_Pnt      gpoint = BRep_Tool::Pnt (gver);

         cout << " = (" << gpoint.X()
              << ","    << gpoint.Y()
              << ","    << gpoint.Z() << ")";
         }
      cout << endl;
      }
}
// ===================================================== updateShape
int NewShape::updateShape ()
{
   if (shape_defined)
      return HOK;

   if (NOT brep_defined)
      return HERR;

   shape_defined = true;
   geom_brep2shape (geo_brep, geo_shape);
   cout <<  " ------------------- updateShape " << el_name << endl;
   parseShape (geo_shape);
   return HOK;
}
// ===================================================== updateBrep
int NewShape::updateBrep ()
{
   if (brep_defined)
      return HOK;

   if (NOT shape_defined)
      return HERR;

   brep_defined = true;
   geom_make_brep (geo_shape, geo_brep);
   return HOK;
}
END_NAMESPACE_HEXA
