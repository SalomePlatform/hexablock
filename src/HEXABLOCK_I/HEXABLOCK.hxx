#ifndef _HEXABLOCK_HXX_
#define _HEXABLOCK_HXX_

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(HEXABLOCK_Gen)
#include "SALOME_Component_i.hxx"


#include <sstream>
// #include <strstream.h>
// #include <iostream.h>


#include <TopoDS_Shape.hxx>
#include "GEOM_Client.hxx"

#include "hexa_base.hxx"
#include "Hex.hxx"

using namespace HEXABLOCK_ORB;

class SALOME_LifeCycleCORBA;
class SALOME_NamingService;


class HEXABLOCK:
  public POA_HEXABLOCK_ORB::HEXABLOCK_Gen,
  public Engines_Component_i 
{

public:
    HEXABLOCK( CORBA::ORB_ptr orb,
	    PortableServer::POA_ptr poa,
	    PortableServer::ObjectId * contId, 
	    const char *instanceName, 
	    const char *interfaceName);
  // Get object of the CORBA reference
    static PortableServer::POA_var GetPOA() { return _poa;}
    static PortableServer::ServantBase_var GetServant( CORBA::Object_ptr theObject );

    virtual ~HEXABLOCK();

    void test();
    ::CORBA::Long countDocument();
    Document_ptr getDocument(::CORBA::Long i);
    void removeDocument(Document_ptr d); //CS_TODO
    Document_ptr addDocument();
    Document_ptr loadDocument(const char* xmlFilename);

    GEOM_Client* getGeomClient();
    TopoDS_Shape geomObjectToShape(GEOM::GEOM_Object_ptr theGeomObject);
    GEOM::GEOM_Object_ptr shapeToGeomObject(const TopoDS_Shape& theShape );
    static SALOME_LifeCycleCORBA*  GetLCC();
    static SALOME_NamingService* GetNS();
    static HEXABLOCK* GetHEXABLOCKGen() { return _HEXABLOCKGen;}


    virtual Engines::TMPFile* DumpPython(CORBA::Object_ptr theStudy,
				         CORBA::Boolean isPublished,
				         CORBA::Boolean& isValidScript);

private:
   HEXA_NS::Hex* _engine_cpp;
   GEOM_Client*  _geomClient;


  static PortableServer::POA_var _poa;// POA reference
  static GEOM::GEOM_Gen_var      _geomGen;
  static SALOME_LifeCycleCORBA*  _lcc;
  static CORBA::ORB_var          _orb;
  static SALOME_NamingService*   _ns;
  static HEXABLOCK*                   _HEXABLOCKGen;

  GEOM::GEOM_Object_ptr _makeVertex( const TopoDS_Shape& theShape );

};

extern "C"
    PortableServer::ObjectId * HEXABLOCKEngine_factory(
      CORBA::ORB_ptr orb,
      PortableServer::POA_ptr poa,
      PortableServer::ObjectId * contId,
      const char *instanceName,
      const char *interfaceName
    );


template<class T>
  T
  DownCast(CORBA::Object_ptr theArg)
  {
    return dynamic_cast<T>(HEXABLOCK::GetServant(theArg).in());
  }





TopoDS_Shape string2shape( const string& brep );


string shape2string( const TopoDS_Shape& aShape );




#endif


// 1) Séquence de code convertissant une chaîne au format BREP vers un TopoDS_Shape
// 
// TopoDS_Shape STRING_TO_SHAPE( char* buf, int sizebuf )
// {
//   TopoDS_Shape shape;
//   std::istrstream streamBrep(buf,sizebuf);
//   BRep_Builder aBuilder;
//   BRepTools::Read(shape, streamBrep, aBuilder);
//   return(shape);
// }
// 
// 2) Séquence de code convertissant un TopoDS_Shape en une chaîne au format BREP
// 
// char* SHAPE_TO_STRING( TopoDS_Shape aShape, int& sizebuf )
// {
//  ostrstream streamShape;
//  BRepTools::Write(aShape, streamShape);
//  sizebuf = streamShape.pcount();
//  char* buf = new char [sizebuf];
//  char* valueOfStream = streamShape.str();
//  memcpy(buf, valueOfStream, size);
//  return buf;
// }
// 
// 3) Séquence de code accédant aux coordonnées d'un TopoDS_Shape qui est un Vertex
// 
// bool SHAPE_TO_COORD(TopoDS_Shape aShape, double& x, double& y, double& z)
// {
//   if ( aShape.ShapeType() == TopAbs_VERTEX ) {
//       TopoDS_Vertex aPoint;
//        aPoint = TopoDS::Vertex( aShape );
//       gp_Pnt aPnt = BRep_Tool::Pnt( aPoint );
//       x = aPnt.X();
//       y = aPnt.Y();
//       z = aPnt.Z();
//        return(1);
//    } else {
//        return(0);
//   };
// }
// 
// 4) Séquence de code créant un TopoDS_Shape à partir de coordonnées
// 
// TopoDS_Shape COORD_TO_SHAPE(double x, double y, double z)
// {
//       gp_Pnt aPnt(x, y, z);
//       BRepBuilderAPI_MakeVertex mkVertex (aPnt);
//      TopoDS_Vertex aVertex = TopoDS::Vertex(mkVertex.Shape());
//       return aVertex;
// }
