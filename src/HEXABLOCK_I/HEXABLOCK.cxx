using namespace std;

#include <TCollection_AsciiString.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>


#include <Geom_Curve.hxx>

#include <gp_Pnt.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>

#include "SALOME_NamingService.hxx"
#include "SALOME_LifeCycleCORBA.hxx"
#include "Utils_SINGLETON.hxx"

#include "HexElements_grid.cxx"


#include "HEXABLOCK.hxx"
#include "utilities.h"

#include "HexDocument_impl.hxx"
#include <string>

using namespace HEXABLOCK_ORB;


// Static variables definition
PortableServer::POA_var HEXABLOCK::_poa;
HEXABLOCK*                   HEXABLOCK::_HEXABLOCKGen = NULL;
SALOME_LifeCycleCORBA*  HEXABLOCK::_lcc = NULL;
GEOM::GEOM_Gen_var      HEXABLOCK::_geomGen = GEOM::GEOM_Gen::_nil();
// GEOM_Client*            HEXABLOCK::_geomClient = NULL;
CORBA::ORB_var          HEXABLOCK::_orb;
SALOME_NamingService*   HEXABLOCK::_ns = NULL;

//=============================================================================
/*!
 *  standard constructor
 */
//=============================================================================

HEXABLOCK::HEXABLOCK(CORBA::ORB_ptr orb,
	PortableServer::POA_ptr poa,
	PortableServer::ObjectId * contId, 
	const char *instanceName, 
	const char *interfaceName) :
  Engines_Component_i(orb, poa, contId, instanceName, interfaceName)
{
  MESSAGE("activate object");
  _thisObj = this ;
  _poa = PortableServer::POA::_duplicate(poa);
  _orb = CORBA::ORB::_duplicate(orb);
  _id = _poa->activate_object(_thisObj);
  _engine_cpp = new HEXA_NS::Hex;
  _geomClient = NULL;
  _HEXABLOCKGen = this;
  MESSAGE("ALRRRRRIGHTTTTT");
}

//=============================================================================
/*!
 *  GetServant [ static ]
 *
 *  Get servant of the CORBA object
 */
//=============================================================================

PortableServer::ServantBase_var HEXABLOCK::GetServant( CORBA::Object_ptr theObject )
{
  if( CORBA::is_nil( theObject ) || CORBA::is_nil( GetPOA() ) )
    return NULL;
  try {
    PortableServer::Servant aServant = GetPOA()->reference_to_servant( theObject );
    return aServant;
  } 
  catch (...) {
    INFOS( "GetServant - Unknown exception was caught!!!" ); 
    return NULL;
  }
}



//=============================================================================
//  destructor
//=============================================================================
HEXABLOCK::~HEXABLOCK()
{
}


void HEXABLOCK::test()
{
  MESSAGE("HEEEEEEEEEEEEEEYYYYYYYYYYY");
}

CORBA::Long HEXABLOCK::countDocument()
{
    CORBA::Long nbDocument = _engine_cpp->countDocument();
//     CORBA::Long nbDocument = 333;
    return nbDocument;
}

Document_ptr HEXABLOCK::getDocument(CORBA::Long i)
{
    HEXA_NS::Document *doc=_engine_cpp->getDocument(i);
    Document_impl *servantCorba=new Document_impl(doc);
    return servantCorba->_this();
}

void HEXABLOCK::removeDocument(HEXABLOCK_ORB::Document_ptr docIn)
{
  Document_impl* docServant = ::DownCast<Document_impl*>( docIn );
  ASSERT( docServant );
  if ( docServant ) {
    HEXA_NS::Document* doc = docServant->GetImpl();
    _engine_cpp->removeDocument( doc );
  }
}


Document_ptr HEXABLOCK::addDocument()
{
    MESSAGE("ADD1");
    HEXA_NS::Document *doc=_engine_cpp->addDocument();
    MESSAGE("ADD2");
    Document_impl *servantCorba=new Document_impl(doc);
    MESSAGE("ADD3");
    return servantCorba->_this();
}



Document_ptr HEXABLOCK::loadDocument(const char* xmlFilename)
{
    HEXA_NS::Document *doc=_engine_cpp->loadDocument(xmlFilename);
    Document_impl *servantCorba=new Document_impl(doc);
    return servantCorba->_this();
}

GEOM_Client* HEXABLOCK::getGeomClient()
{
  // create shape reader if necessary
  if ( _geomClient == NULL ) 
    _geomClient = new GEOM_Client(GetContainerRef());
  ASSERT( _geomClient);
  return _geomClient;
}


TopoDS_Shape HEXABLOCK::geomObjectToShape(GEOM::GEOM_Object_ptr theGeomObject)
{
  TopoDS_Shape S;
  if ( !theGeomObject->_is_nil() ){
    _geomClient = getGeomClient();

    if(CORBA::is_nil(_geomGen)) {
      Engines::Component_ptr temp=GetLCC()->FindOrLoad_Component("FactoryServer","GEOM");
      _geomGen=GEOM::GEOM_Gen::_narrow(temp);
    }
    if ( _geomClient != NULL  && !_geomGen->_is_nil () )
      S = _geomClient->GetShape( _geomGen, theGeomObject );
  }
  return S;
}





// HEXABLOCK_ORB::GEOM_Object_ptr HEXABLOCK::shapeToGeomObject (const TopoDS_Shape& theShape )
// {
//   HEXABLOCK_ORB::GEOM_Object_var aShapeObj;
//   if ( !theShape.IsNull() ) {
//     _geomClient = getGeomClient();//new GEOM_Client(GetContainerRef());
//     TCollection_AsciiString IOR;
//     bool found = _geomClient->Find(theShape, IOR);
//     if (found){
//       CORBA::Object_var obj = _orb->string_to_object( IOR.ToCString() );
//       aShapeObj = HEXABLOCK_ORB::GEOM_Object::_narrow ( obj );
//     }
//   }
//   return aShapeObj._retn();
// }



// GEOM::GEOM_Object_ptr HEXABLOCK::_makeFace( const TopoDS_Shape& theShape )
// {
//   ASSERT ( theShape.ShapeType() == TopAbs_FACE );
// 
//   int theStudyID = 0;
//   GEOM::GEOM_Object_var result; 
//   
// // PrimOp   = self.GetI3DPrimOperations 
// // GEOM_IShapesOperations
//   return result._retn();
// }

// GEOM::GEOM_Object_ptr HEXABLOCK::_makeEdge( const TopoDS_Shape& theShape )
// {
//   ASSERT ( theShape.ShapeType() == TopAbs_EDGE );
//   int theStudyID = 0;
//   GEOM::GEOM_Object_var result;
//   GEOM::GEOM_Object_var p1;
//   GEOM::GEOM_Object_var p2;
// 
//   GEOM::GEOM_IBasicOperations_var     aBasicOp  = _geomGen->GetIBasicOperations(theStudyID);
//   GEOM::GEOM_IShapesOperations_var    aShapesOp = _geomGen->GetIShapesOperations(theStudyID);
// 
//   TopoDS_Edge anEdge = TopoDS::Edge( theShape );
//   double f, l;
//   Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, f, l);
// 
//   gp_Pnt aCurveStart = aCurve->Value( f );
//   gp_Pnt aCurveEnd   = aCurve->Value( l );
// 
//   double x1, y1, z1;
//   double x2, y2, z2;
// 
//   x1 = aCurveStart.X(); y1 = aCurveStart.Y(); z1 = aCurveStart.Z();
//   x2 = aCurveEnd.X();   y2 = aCurveEnd.Y(); z2 = aCurveEnd.Z();
//   p1= aBasicOp->MakePointXYZ(x1, y1, z1);
//   p2= aBasicOp->MakePointXYZ(x2, y2, z2);
//   result = aShapesOp->MakeEdge(p1, p2);
// 
//   return result._retn();
// }

GEOM::GEOM_Object_ptr HEXABLOCK::_makeVertex( const TopoDS_Shape& theShape )
{
  ASSERT ( theShape.ShapeType() == TopAbs_VERTEX );

  int theStudyID = 0;
  GEOM::GEOM_Object_var result;

  TopoDS_Vertex aPoint;
  aPoint      = TopoDS::Vertex( theShape );
  gp_Pnt aPnt = BRep_Tool::Pnt( aPoint );
  double x = aPnt.X();
  double y = aPnt.Y();
  double z = aPnt.Z();

  GEOM::GEOM_IBasicOperations_var aBasicOp = _geomGen->GetIBasicOperations(theStudyID);
  result = aBasicOp->MakePointXYZ(x, y, z);

  return result._retn();
}


GEOM::GEOM_Object_ptr HEXABLOCK::shapeToGeomObject (const TopoDS_Shape& theShape )
{
  GEOM::GEOM_Object_var aShapeObj;
  if ( !theShape.IsNull() ) {
    switch ( theShape.ShapeType() ){
      case TopAbs_VERTEX: aShapeObj = _makeVertex( theShape ); break;
//       case TopAbs_FACE:   aShapeObj = _makeFace( theShape ); break;
//       case TopAbs_EDGE:   aShapeObj = _makeEdge( theShape ); break;
      default: throw (SALOME_Exception(LOCALIZED("shapeToGeomObject : shape type not supported")));
    }
  }
  return aShapeObj._retn();
}


// GEOM::GEOM_Object_ptr HEXABLOCK::shapeToGeomObject (const TopoDS_Shape& theShape )
// {
//   GEOM::GEOM_Object_var aShapeObj;
//   std::cout<<"shapeToGeomObject "<<std::endl;
//   if ( !theShape.IsNull() ) {
//     std::cout<<"shapeToGeomObject !theShape.IsNull()"<<std::endl;
//     _geomClient = getGeomClient();//new GEOM_Client(GetContainerRef());
//     TCollection_AsciiString IOR;
//     bool found = _geomClient->Find(theShape, IOR);
//     std::cout<<"shapeToGeomObject found"<<found<<std::endl;
//     if (found){
//       CORBA::Object_var obj = _orb->string_to_object( IOR.ToCString() );
//       aShapeObj = GEOM::GEOM_Object::_narrow ( obj );
//     }
//   }
//   return aShapeObj._retn();
// }








SALOME_LifeCycleCORBA*  HEXABLOCK::GetLCC() {
  if ( _lcc == NULL ) {
    _lcc = new SALOME_LifeCycleCORBA( GetNS() );
  }
  return _lcc;
}

SALOME_NamingService* HEXABLOCK::GetNS()
{
  if ( _ns == NULL ) {
    _ns = SINGLETON_<SALOME_NamingService>::Instance();
    ASSERT(SINGLETON_<SALOME_NamingService>::IsAlreadyExisting());
    _ns->init_orb( _orb );
  }
  return _ns;
}



TopoDS_Shape string2shape( const string& brep )
{
  TopoDS_Shape shape;

//   istringstream streamBrep(brep.c_str());
  istringstream streamBrep(brep);
//   char* chaine = new char[brep.size()];
//   strcpy(chaine, brep.c_str()); 
//   istringstream streamBrep( chaine );

  BRep_Builder aBuilder;
  BRepTools::Read(shape, streamBrep, aBuilder);
//   BRepTools::Read(shape, brep, aBuilder);
  return shape;
}

// TopoDS_Shape STRING_TO_SHAPE( char* buf, int sizebuf )
// {
//   TopoDS_Shape shape;
//   std::istrstream streamBrep(buf,sizebuf);
//   BRep_Builder aBuilder;
//   BRepTools::Read(shape, streamBrep, aBuilder);
//   return(shape);
// }




string shape2string( const TopoDS_Shape& aShape )
{
  ostringstream streamShape;
//   string  strShape;
  BRepTools::Write(aShape, streamShape);
//   BRepTools::Write(aShape, strShape);

  return streamShape.str();
}


extern "C"
{
  PortableServer::ObjectId * HEXABLOCKEngine_factory(
			       CORBA::ORB_ptr orb,
			       PortableServer::POA_ptr poa, 
			       PortableServer::ObjectId * contId,
			       const char *instanceName, 
		       	       const char *interfaceName)
  {
    MESSAGE("PortableServer::ObjectId * HEXABLOCKEngine_factory()");
    SCRUTE(interfaceName);
    HEXABLOCK * myHEXABLOCK 
      = new HEXABLOCK(orb, poa, contId, instanceName, interfaceName);
    return myHEXABLOCK->getId() ;
  }

}

// //================= Primitives Construction : BasicOperations =================
// //=============================================================================
// //  makeBanner: renvoie un message
// //=============================================================================
// 
// 
// #include "Vertex_impl.hxx"
// #include "Edge_impl.hh"
// char* HEXABLOCK::makeBanner(const char* name)
// {
//     string banner="Pourquoi?, ";
//     banner+=name;
//     return CORBA::string_dup(banner.c_str());
// }
// 
// //=============================================================================
// //  makeVertex: construit un sommet
// //=============================================================================
// 
// HEXABLOCK_ORB::Vertex_ptr HEXABLOCK::makeVertex(CORBA::Double x, CORBA::Double y, CORBA::Double z)
// {
//   Vertex *vert=_engine_cpp->makeVertex(x,y,z);
//   Vertex_impl *servantCorba=new Vertex_impl(vert);
//   return servantCorba->_this();
// }
// 
// //=============================================================================
// //  makeEdge: construit une arrête
// //=============================================================================
// 
// HEXABLOCK_ORB::Edge_ptr HEXABLOCK::makeEdge(HEXABLOCK_ORB::Vertex_ptr va, HEXABLOCK_ORB::Vertex_ptr vb)
// {
//   Edge *edg=_engine_cpp->makeEdge(va,vb);
//   Edge_impl *servantCorba=new Edge_impl(edg);
//   return servantCorba->_this();
// }
// //=============================================================================
// // 
// //=============================================================================

