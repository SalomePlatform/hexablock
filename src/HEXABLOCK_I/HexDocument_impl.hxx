//  Copyright (C) 2009-2011  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#ifndef __HexDocument_impl__
#define __HexDocument_impl__

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(Document)
#include "SALOME_GenericObj_i.hh"

#include "hexa_base.hxx"
#include "HexDocument.hxx"


// #define HEXA_NS Hex
using namespace HEXABLOCK_ORB;

class Document_impl:
  public POA_HEXABLOCK_ORB::Document,
  public SALOME::GenericObj_i
{

public:
//   Document_impl(HEXA_NS::Document *ptrCpp);
  Document_impl( PortableServer::POA_ptr thePOA, HEXA_NS::Document *ptrCpp);

  HEXA_NS::Document* GetImpl() throw (SALOME::SALOME_Exception);

  void purge() throw (SALOME::SALOME_Exception);
  void dump()  throw (SALOME::SALOME_Exception);

  ::CORBA::Long saveVtk (const char* fname)    throw (SALOME::SALOME_Exception);
  ::CORBA::Long save (const char* fileName)    throw (SALOME::SALOME_Exception);
  ::CORBA::Long setName (const char* fileName) throw (SALOME::SALOME_Exception);
   char* getName ()                      throw (SALOME::SALOME_Exception);


  void setTolerance(::CORBA::Double tol) throw (SALOME::SALOME_Exception);
  ::CORBA::Double getTolerance() throw (SALOME::SALOME_Exception);
  Vertex_ptr addVertex(::CORBA::Double x, ::CORBA::Double y, ::CORBA::Double z) throw (SALOME::SALOME_Exception);
  ::CORBA::Long countVertex() throw (SALOME::SALOME_Exception);
  Vertex_ptr getVertex(::CORBA::Long i) throw (SALOME::SALOME_Exception);
  Vertex_ptr findVertex(::CORBA::Double x, ::CORBA::Double y, ::CORBA::Double z) throw (SALOME::SALOME_Exception);
  Edge_ptr addEdge(Vertex_ptr v0, Vertex_ptr v1) throw (SALOME::SALOME_Exception);
  ::CORBA::Long countEdge() throw (SALOME::SALOME_Exception);
  Edge_ptr getEdge(::CORBA::Long i) throw (SALOME::SALOME_Exception);
  Edge_ptr findEdge(Vertex_ptr p1, Vertex_ptr p2) throw (SALOME::SALOME_Exception);
  Quad_ptr addQuad(Edge_ptr e0, Edge_ptr e1, Edge_ptr e2, Edge_ptr e3) throw (SALOME::SALOME_Exception);
  Quad_ptr addQuadVertices(Vertex_ptr v0, Vertex_ptr v1, Vertex_ptr v2, Vertex_ptr v3) throw (SALOME::SALOME_Exception);
  ::CORBA::Long countQuad() throw (SALOME::SALOME_Exception);
  Quad_ptr getQuad(::CORBA::Long i) throw (SALOME::SALOME_Exception);
  Quad_ptr findQuad(Vertex_ptr v1, Vertex_ptr v2) throw (SALOME::SALOME_Exception);
  Hexa_ptr addHexa(Quad_ptr q0, Quad_ptr q1, Quad_ptr q2, Quad_ptr q3, Quad_ptr q4, Quad_ptr q5) throw (SALOME::SALOME_Exception);
  Hexa_ptr addHexaVertices(Vertex_ptr v0, Vertex_ptr v1, Vertex_ptr v2, Vertex_ptr v3, Vertex_ptr v4, Vertex_ptr v5, Vertex_ptr v6, Vertex_ptr v7) throw (SALOME::SALOME_Exception);
  ::CORBA::Long countHexa() throw (SALOME::SALOME_Exception);
  Hexa_ptr getHexa(::CORBA::Long i) throw (SALOME::SALOME_Exception);
  Hexa_ptr findHexa(Vertex_ptr p1, Vertex_ptr p2) throw (SALOME::SALOME_Exception);
  Vector_ptr addVector(::CORBA::Double dx, ::CORBA::Double dy, ::CORBA::Double dz) throw (SALOME::SALOME_Exception);
  Vector_ptr addVectorVertices(Vertex_ptr v1, Vertex_ptr v2) throw (SALOME::SALOME_Exception);
//   ::CORBA::Long countVector() throw (SALOME::SALOME_Exception);
//   Vector_ptr getVector(::CORBA::Long i) throw (SALOME::SALOME_Exception);
  Cylinder_ptr addCylinder(Vertex_ptr base, Vector_ptr direction, ::CORBA::Double radius, ::CORBA::Double height) throw (SALOME::SALOME_Exception);
//   ::CORBA::Long countCylinder() throw (SALOME::SALOME_Exception);
//   Cylinder_ptr getCylinder(::CORBA::Long i) throw (SALOME::SALOME_Exception);
  Pipe_ptr addPipe(Vertex_ptr base, Vector_ptr direction, ::CORBA::Double int_radius, ::CORBA::Double ext_radius, ::CORBA::Double height) throw (SALOME::SALOME_Exception);
//   ::CORBA::Long countPipe() throw (SALOME::SALOME_Exception);
//   Pipe_ptr getPipe(::CORBA::Long i) throw (SALOME::SALOME_Exception);
  ::CORBA::Boolean removeHexa(Hexa_ptr h) throw (SALOME::SALOME_Exception);
  ::CORBA::Boolean removeConnectedHexa(Hexa_ptr h) throw (SALOME::SALOME_Exception);
  Elements_ptr makeCartesian(Vertex_ptr pt, Vector_ptr vx, Vector_ptr vy, Vector_ptr vz, ::CORBA::Long nx, ::CORBA::Long ny, ::CORBA::Long nz) throw (SALOME::SALOME_Exception);
  Elements_ptr makeCartesian1(Vertex_ptr v, Vector_ptr v1, ::CORBA::Long px, ::CORBA::Long py, ::CORBA::Long pz, ::CORBA::Long mx, ::CORBA::Long my, ::CORBA::Long mz) throw (SALOME::SALOME_Exception);

  Elements_ptr makeCylindrical(Vertex_ptr pt, Vector_ptr vex, Vector_ptr vez, ::CORBA::Double dr, ::CORBA::Double da, ::CORBA::Double dl, ::CORBA::Long nr, ::CORBA::Long na, ::CORBA::Long nl, ::CORBA::Boolean fill) throw (SALOME::SALOME_Exception);

  Elements_ptr makeCylindricals (Vertex_ptr pt, Vector_ptr vex, Vector_ptr vez,
        const RealVector& tdr, const RealVector& tda, const RealVector& tdl,
       	::CORBA::Boolean fill)
        throw (SALOME::SALOME_Exception);

  Elements_ptr makeSpherical( Vertex_ptr ptIn,
    ::CORBA::Double rayon, 
    ::CORBA::Long n,
    ::CORBA::Double k ) throw (SALOME::SALOME_Exception);

  Elements_ptr makeCylinder(Cylinder_ptr cyl, Vector_ptr vr, ::CORBA::Long nr, ::CORBA::Long na, ::CORBA::Long nl) throw (SALOME::SALOME_Exception);
  Elements_ptr makePipe(Pipe_ptr p, Vector_ptr vr, ::CORBA::Long nr, 
                                 ::CORBA::Long na, ::CORBA::Long nl) 
                throw (SALOME::SALOME_Exception);

//   Elements_ptr makeCylinders(Cylinder_ptr c1, Cylinder_ptr c2) throw (SALOME::SALOME_Exception);
    CrossElements_ptr makeCylinders(Cylinder_ptr cyl1, Cylinder_ptr cyl2) throw (SALOME::SALOME_Exception);

   Elements_ptr makePipes(Pipe_ptr p1, Pipe_ptr p2) throw (SALOME::SALOME_Exception);

  Elements_ptr prismQuad(Quad_ptr qd, Vector_ptr v, ::CORBA::Long nb) throw (SALOME::SALOME_Exception);
  Elements_ptr prismQuads(const Quads& qds, Vector_ptr v, ::CORBA::Long nb) throw (SALOME::SALOME_Exception);

  Elements_ptr joinQuad(Quad_ptr qa, Quad_ptr qb, Vertex_ptr va1, Vertex_ptr vb1, Vertex_ptr va2, Vertex_ptr vb2, ::CORBA::Long nb) throw (SALOME::SALOME_Exception);
  Elements_ptr joinQuads(const Quads& qds, Quad_ptr qb, Vertex_ptr va1, Vertex_ptr vb1, Vertex_ptr va2, Vertex_ptr vb2, ::CORBA::Long nb) throw (SALOME::SALOME_Exception);

//CS_NOT_SPEC
//   Elements_ptr mergeQuads(Quad_ptr qa, Quad_ptr qb, Vertex_ptr va1, Vertex_ptr vb1, Vertex_ptr va2, Vertex_ptr vb2) throw (SALOME::SALOME_Exception);
//   Elements_ptr mergeEdges(Edge_ptr e1, Edge_ptr e2, Vertex_ptr v1, Vertex_ptr v2) throw (SALOME::SALOME_Exception);
//   Elements_ptr mergeVertices(Vertex_ptr v1, Vertex_ptr v2) throw (SALOME::SALOME_Exception);


  ::CORBA::Long mergeQuads(Quad_ptr qa, Quad_ptr qb, Vertex_ptr va1, Vertex_ptr vb1, Vertex_ptr va2, Vertex_ptr vb2) throw (SALOME::SALOME_Exception);
  ::CORBA::Long mergeEdges(Edge_ptr e1, Edge_ptr e2, Vertex_ptr v1, Vertex_ptr v2) throw (SALOME::SALOME_Exception);
  ::CORBA::Long mergeVertices(Vertex_ptr v1, Vertex_ptr v2) throw (SALOME::SALOME_Exception);

  Elements_ptr disconnectQuad(Hexa_ptr h, Quad_ptr q) throw (SALOME::SALOME_Exception);
//   Elements_ptr disconnectEdge(Hexa_ptr h, Edge_ptr e) throw (SALOME::SALOME_Exception);
//   Elements_ptr disconnectVertex(Hexa_ptr h, Vertex_ptr v) throw (SALOME::SALOME_Exception);
//   ::CORBA::Long disconnectQuad(Hexa_ptr h, Quad_ptr q) throw (SALOME::SALOME_Exception);
//   Quad_ptr disconnectQuad(Hexa_ptr h, Quad_ptr q) throw (SALOME::SALOME_Exception);
//   ::CORBA::Long disconnectEdge(Hexa_ptr h, Edge_ptr e) throw (SALOME::SALOME_Exception);
//   Edge_ptr disconnectEdge(Hexa_ptr h, Edge_ptr e) throw (SALOME::SALOME_Exception);
  Elements_ptr disconnectEdge(Hexa_ptr h, Edge_ptr e) throw (SALOME::SALOME_Exception);
//   ::CORBA::Long disconnectVertex(Hexa_ptr h, Vertex_ptr v) throw (SALOME::SALOME_Exception);
  Elements_ptr disconnectVertex(Hexa_ptr h, Vertex_ptr v) throw (SALOME::SALOME_Exception);
//   Vertex_ptr disconnectVertex(Hexa_ptr h, Vertex_ptr v) throw (SALOME::SALOME_Exception);

  Elements_ptr cut(Edge_ptr e, ::CORBA::Long nb_of_cuts) throw (SALOME::SALOME_Exception);
//   Elements_ptr makeTranslation(Element_ptr e, Vector_ptr vec) throw (SALOME::SALOME_Exception);//CS_Bruno
  Elements_ptr makeTranslation(Elements_ptr e, Vector_ptr vec) throw (SALOME::SALOME_Exception);

Elements_ptr makeRotation(Elements_ptr l, Vertex_ptr ver, Vector_ptr vec, 
                          ::CORBA::Double angle) 
             throw (SALOME::SALOME_Exception);

Elements_ptr makeScale (Elements_ptr e, Vertex_ptr ver, ::CORBA::Double k) 
             throw (SALOME::SALOME_Exception);
Elements_ptr makeSymmetryPoint (Elements_ptr e, Vertex_ptr ver) 
             throw (SALOME::SALOME_Exception);
Elements_ptr makeSymmetryLine  (Elements_ptr e, Vertex_ptr ver, Vector_ptr vec) 
             throw (SALOME::SALOME_Exception);
Elements_ptr makeSymmetryPlane (Elements_ptr e, Vertex_ptr ver, Vector_ptr vec)
             throw (SALOME::SALOME_Exception);

//   void performTranslation(Element_ptr e, Vector_ptr vec) throw (SALOME::SALOME_Exception);
  void performTranslation(Elements_ptr l, Vector_ptr vec) throw (SALOME::SALOME_Exception);

void performRotation(Elements_ptr l, Vertex_ptr ver, Vector_ptr vec, ::CORBA::Double angle) throw (SALOME::SALOME_Exception);

void performScale (Elements_ptr e, Vertex_ptr ver, ::CORBA::Double k) 
     throw (SALOME::SALOME_Exception);
void performSymmetryPoint (Elements_ptr e, Vertex_ptr ver) 
     throw (SALOME::SALOME_Exception);
void performSymmetryLine  (Elements_ptr e, Vertex_ptr ver, Vector_ptr vec) 
     throw (SALOME::SALOME_Exception);
void performSymmetryPlane (Elements_ptr e, Vertex_ptr ver, Vector_ptr vec) 
     throw (SALOME::SALOME_Exception);

  Group_ptr addHexaGroup(const char* name) throw (SALOME::SALOME_Exception);
  Group_ptr addQuadGroup(const char* name) throw (SALOME::SALOME_Exception);
  Group_ptr addEdgeGroup(const char* name) throw (SALOME::SALOME_Exception);
  Group_ptr addHexaNodeGroup(const char* name) throw (SALOME::SALOME_Exception);
  Group_ptr addQuadNodeGroup(const char* name) throw (SALOME::SALOME_Exception);
  Group_ptr addEdgeNodeGroup(const char* name) throw (SALOME::SALOME_Exception);
  Group_ptr addVertexNodeGroup(const char* name) throw (SALOME::SALOME_Exception);
  ::CORBA::Long removeGroup(Group_ptr g) throw (SALOME::SALOME_Exception);
  ::CORBA::Long countGroup() throw (SALOME::SALOME_Exception);
  Group_ptr getGroup(::CORBA::Long i) throw (SALOME::SALOME_Exception);
  Group_ptr findGroup(const char* name) throw (SALOME::SALOME_Exception);

  Law_ptr addLaw(const char* name, ::CORBA::Long nb_nodes) throw (SALOME::SALOME_Exception);
  ::CORBA::Long countLaw() throw (SALOME::SALOME_Exception);
  Law_ptr getLaw(::CORBA::Long i) throw (SALOME::SALOME_Exception);
  void removeLaw(Law_ptr l) throw (SALOME::SALOME_Exception);
  Law_ptr findLaw(const char* name) throw (SALOME::SALOME_Exception);
  ::CORBA::Long countPropagation() throw (SALOME::SALOME_Exception);
  Propagation_ptr getPropagation(::CORBA::Long i) throw (SALOME::SALOME_Exception);
  Propagation_ptr findPropagation(Edge_ptr e) throw (SALOME::SALOME_Exception);

          // Evols 13/12/2010

 ::CORBA::Long associateOpenedLine (Edge_ptr        mstart, 
                            const Edges&            mline, 
                            GEOM::GEOM_Object_ptr   gstart, 
                            ::CORBA::Double         pstart, 
                            const Shapes&           gline, 
                            ::CORBA::Double pend) 
                      throw (SALOME::SALOME_Exception);
 ::CORBA::Long associateClosedLine (Vertex_ptr      mfirst, 
                            Edge_ptr                mstart, 
                            const Edges&            mline, 
                            GEOM::GEOM_Object_ptr   gstart, 
                            ::CORBA::Double         pstart, 
                            ::CORBA::Boolean        inv,
                            const Shapes&           gline)
                      throw (SALOME::SALOME_Exception);

void setShape (GEOM::GEOM_Object_ptr geom_object)
                      throw (SALOME::SALOME_Exception);
GEOM::GEOM_Object_ptr getShape ()
                      throw (SALOME::SALOME_Exception);
char*  getBrep ()     throw (SALOME::SALOME_Exception);

          // Evols 05/09/2011
	  //
  ::CORBA::Long countUsedVertex () throw (SALOME::SALOME_Exception);
  ::CORBA::Long countUsedEdge   () throw (SALOME::SALOME_Exception);
  ::CORBA::Long countUsedQuad   () throw (SALOME::SALOME_Exception);
  ::CORBA::Long countUsedHexa   () throw (SALOME::SALOME_Exception);

  Vertex_ptr getUsedVertex (::CORBA::Long i) throw (SALOME::SALOME_Exception);
  Edge_ptr    getUsedEdge   (::CORBA::Long i) throw (SALOME::SALOME_Exception);
  Quad_ptr    getUsedQuad   (::CORBA::Long i) throw (SALOME::SALOME_Exception);
  Hexa_ptr    getUsedHexa   (::CORBA::Long i) throw (SALOME::SALOME_Exception);

  Hexa_ptr addHexa5Quads (Quad_ptr q1, Quad_ptr q2, Quad_ptr q3, Quad_ptr q4, 
                          Quad_ptr q5)
           throw (SALOME::SALOME_Exception);
  Hexa_ptr addHexa4Quads (Quad_ptr q1, Quad_ptr q2, Quad_ptr q3, Quad_ptr q4)
           throw (SALOME::SALOME_Exception);
  Hexa_ptr addHexa3Quads (Quad_ptr q1, Quad_ptr q2, Quad_ptr q3)
           throw (SALOME::SALOME_Exception);
  Hexa_ptr addHexa2Quads (Quad_ptr q1, Quad_ptr q2)
           throw (SALOME::SALOME_Exception);

  Elements_ptr revolutionQuads (const Quads& start, Vertex_ptr center, 
                                Vector_ptr axis, const RealVector &angles)
           throw (SALOME::SALOME_Exception);

  Elements_ptr replace (const Quads& pattern, Vertex_ptr p1, Vertex_ptr c1, 
                Vertex_ptr p2, Vertex_ptr c2, Vertex_ptr p3, Vertex_ptr c3)
           throw (SALOME::SALOME_Exception);

  Elements_ptr makeSphere (Vertex_ptr center, Vector_ptr vx, Vector_ptr vz, 
                         ::CORBA::Double radius, ::CORBA::Double radhole,
                         Vertex_ptr plorig, 
                 ::CORBA::Long nrad, ::CORBA::Long nang, ::CORBA::Long nhaut)
           throw (SALOME::SALOME_Exception);

  Elements_ptr makePartSphere (Vertex_ptr center, Vector_ptr vx, Vector_ptr vz, 
                             ::CORBA::Double  radius, ::CORBA::Double radhole,
                             Vertex_ptr plorig, ::CORBA::Double angle,
                 ::CORBA::Long nrad, ::CORBA::Long nang, ::CORBA::Long nhaut)
           throw (SALOME::SALOME_Exception);

  Elements_ptr makeRind (Vertex_ptr center, Vector_ptr vx, Vector_ptr vz, 
                       ::CORBA::Double  radext, ::CORBA::Double radint, 
                       ::CORBA::Double radhole, Vertex_ptr plorig, 
                 ::CORBA::Long nrad, ::CORBA::Long nang, ::CORBA::Long nhaut)
           throw (SALOME::SALOME_Exception);

  Elements_ptr makePartRind (Vertex_ptr center, Vector_ptr vx, Vector_ptr vz, 
                       ::CORBA::Double radext, ::CORBA::Double radint, 
                       ::CORBA::Double radhole,
                       Vertex_ptr plorig, ::CORBA::Double angle,
                 ::CORBA::Long nrad, ::CORBA::Long nang, ::CORBA::Long nhaut)
           throw (SALOME::SALOME_Exception);
public:


  ::CORBA::Boolean removeQuad (Quad_ptr quad) 
           throw (SALOME::SALOME_Exception);
  ::CORBA::Boolean removeElements (Elements_ptr bloc) 
           throw (SALOME::SALOME_Exception);

private:
  HEXA_NS::Document *_document_cpp;

};

#endif
