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
using namespace std;






// #include <Basics_Utils.hxx>
// 
// #include <TDF_Tool.hxx>
// #include <TDF_Data.hxx>
// #include <TDF_Reference.hxx>
// #include <TDF_LabelSequence.hxx>
// #include <TDataStd_Integer.hxx>
// #include <TDataStd_ChildNodeIterator.hxx>
// #include <TFunction_Driver.hxx>
// #include <TFunction_DriverTable.hxx>
// #include <TDataStd_HArray1OfByte.hxx>
// #include <TDataStd_ByteArray.hxx>
// #include <TDataStd_UAttribute.hxx>
// #include <TDF_ChildIterator.hxx>
// #include <TDataStd_Comment.hxx>
// #include <Resource_DataMapIteratorOfDataMapOfAsciiStringAsciiString.hxx>

////////////






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
#include "Utils_ExceptHandlers.hxx"
#include "HexElements_grid.cxx"


#include "HEXABLOCK.hxx"
#include "utilities.h"

#include "HexDocument_impl.hxx"
#include <string>

using namespace HEXABLOCK_ORB;


// Static variables definition
PortableServer::POA_var HEXABLOCK_Gen_i::_poa;
HEXABLOCK_Gen_i*        HEXABLOCK_Gen_i::_HEXABLOCKGen = NULL;
SALOME_LifeCycleCORBA*  HEXABLOCK_Gen_i::_lcc = NULL;
GEOM::GEOM_Gen_var      HEXABLOCK_Gen_i::_geomGen = GEOM::GEOM_Gen::_nil();
// GEOM_Client*            HEXABLOCK_Gen_i::_geomClient = NULL;
CORBA::ORB_var          HEXABLOCK_Gen_i::_orb;
SALOME_NamingService*   HEXABLOCK_Gen_i::_ns = NULL;

//=============================================================================
/*!
 *  standard constructor
 */
//=============================================================================

HEXABLOCK_Gen_i::HEXABLOCK_Gen_i(CORBA::ORB_ptr orb,
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

PortableServer::ServantBase_var HEXABLOCK_Gen_i::GetServant( CORBA::Object_ptr theObject )
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
HEXABLOCK_Gen_i::~HEXABLOCK_Gen_i()
{
}




//=============================================================================
/*!
 *  DumpPython
 */
//=============================================================================
Engines::TMPFile* HEXABLOCK_Gen_i::DumpPython(CORBA::Object_ptr theStudy,
					      CORBA::Boolean /*isPublished*/,
					      CORBA::Boolean /*isMultiFile*/,
					      CORBA::Boolean& isValidScript)
{
  MESSAGE("DumpPython DumpPython DumpPython DumpPython");
  std::cout << "DumpPython DumpPython DumpPython DumpPython" << std::endl;
  isValidScript = true;  // to avoid dump python operation failing
  Engines::TMPFile_var aStreamFile = new Engines::TMPFile(0);
  return aStreamFile._retn(); 
}
//   SALOMEDS::Study_var aStudy = SALOMEDS::Study::_narrow(theStudy);
//   if(CORBA::is_nil(aStudy))
//     return new Engines::TMPFile(0);   
// 
//   SALOMEDS::SObject_var aSO = aStudy->FindComponent(ComponentDataType());
//   if(CORBA::is_nil(aSO))
//     return new Engines::TMPFile(0);  
// 
//   TObjectData objData;
//   std::vector<TObjectData> objectDataVec;
// 
//   TVariablesList aVariableMap;
// 
//   SALOMEDS::ChildIterator_var Itr = aStudy->NewChildIterator(aSO);
//   for(Itr->InitEx(true); Itr->More(); Itr->Next()) {
//     SALOMEDS::SObject_var aValue = Itr->Value();
//     CORBA::String_var IOR = aValue->GetIOR();
//     if(strlen(IOR.in()) > 0) {
//       CORBA::Object_var obj = _orb->string_to_object(IOR);
//       GEOM::GEOM_Object_var GO = GEOM::GEOM_Object::_narrow(obj);
//       if(!CORBA::is_nil(GO)) {
//         CORBA::String_var aName       = aValue->GetName();
//         CORBA::String_var anEntry     = GO->GetEntry();
//         CORBA::String_var aStudyEntry = aValue->GetID();
//         objData._name       = aName.in();
//         objData._entry      = anEntry.in();
//         objData._studyEntry = aStudyEntry.in();
// 	objectDataVec.push_back( objData );
// 
// 	//Find attribute with list of used notebook variables
// 	SALOMEDS::GenericAttribute_var anAttr;
// 	SALOMEDS::AttributeString_var anAttrStr;
// 	if(aValue->FindAttribute(anAttr,"AttributeString")){
// 	  anAttrStr = SALOMEDS::AttributeString::_narrow(anAttr);
// 	  SALOMEDS::ListOfListOfStrings_var aSections = aStudy->ParseVariables(anAttrStr->Value());
// 	  ObjectStates* aStates = new ObjectStates();
// 	  for(int i = 0; i < aSections->length(); i++) {
// 	    TState aState;
// 	    SALOMEDS::ListOfStrings aListOfVars = aSections[i];
// 	    for(int j = 0; j < aListOfVars.length(); j++) {
// 	      bool isVar = aStudy->IsVariable(aListOfVars[j].in());
// 	      TVariable aVar = TVariable( (char*)aListOfVars[j].in(), isVar );
// 	      aState.push_back(aVar);
// 	    }
// 	    aStates->AddState(aState);
// 	  }
// 	  aVariableMap.insert(std::make_pair(TCollection_AsciiString(anEntry),aStates));
// 	}
//       }
//     }
//   }
//   
//   TCollection_AsciiString aScript;
//   aScript += _impl->DumpPython(aStudy->StudyId(), objectDataVec, aVariableMap, isPublished, isValidScript);
// 
//   if (isPublished)
//   {
//     //Output the script that sets up the visual parameters.
//     char* script = aStudy->GetDefaultScript(ComponentDataType(), "\t");
//     if (script && strlen(script) > 0) {
//       aScript += "\n\t### Store presentation parameters of displayed objects\n";
//       aScript += script;
//       CORBA::string_free(script);
//     }
//   }
// 
//   aScript += "\n\tpass\n";
// 
//   int aLen = aScript.Length(); 
//   unsigned char* aBuffer = new unsigned char[aLen+1];
//   strcpy((char*)aBuffer, aScript.ToCString());
// 
//   CORBA::Octet* anOctetBuf =  (CORBA::Octet*)aBuffer;
//   Engines::TMPFile_var aStreamFile = new Engines::TMPFile(aLen+1, aLen+1, anOctetBuf, 1); 
// 
//   return aStreamFile._retn(); 
// }




void HEXABLOCK_Gen_i::test()
{
  MESSAGE("HEEEEEEEEEEEEEEYYYYYYYYYYY");
}

CORBA::Long HEXABLOCK_Gen_i::countDocument()
{
    CORBA::Long nbDocument = _engine_cpp->countDocument();
//     CORBA::Long nbDocument = 333;
    return nbDocument;
}

Document_ptr HEXABLOCK_Gen_i::getDocument(CORBA::Long i)
{
    HEXA_NS::Document *doc=_engine_cpp->getDocument(i);
    if ( doc == NULL ) return Document::_nil();
    Document_impl *servantCorba = new Document_impl( GetPOA(), doc );
    return servantCorba->_this();
}

void HEXABLOCK_Gen_i::removeDocument(HEXABLOCK_ORB::Document_ptr docIn)
{
  Document_impl* docServant = ::DownCast<Document_impl*>( docIn );
  ASSERT( docServant );
  if ( docServant ) {
    HEXA_NS::Document* doc = docServant->GetImpl();
    _engine_cpp->removeDocument( doc );
  }
}


Document_ptr HEXABLOCK_Gen_i::addDocument(const char* name)
{
   // HEXA_NS::Document *doc=_engine_cpp->addDocument (name);
   // Document_impl *servantCorba=new Document_impl( GetPOA(), doc);
   // return servantCorba->_this();

   return createDocInStudy(name);
}



Document_ptr HEXABLOCK_Gen_i::loadDocument(const char* xmlFilename)
{
    HEXA_NS::Document *doc=_engine_cpp->loadDocument(xmlFilename);
    Document_impl *servantCorba=new Document_impl( GetPOA(), doc );
    return servantCorba->_this();
}

GEOM_Client* HEXABLOCK_Gen_i::getGeomClient()
{
  // create shape reader if necessary
  if ( _geomClient == NULL ) 
    _geomClient = new GEOM_Client(GetContainerRef());
  ASSERT( _geomClient);
  return _geomClient;
}


TopoDS_Shape HEXABLOCK_Gen_i::geomObjectToShape(GEOM::GEOM_Object_ptr theGeomObject)
{
  TopoDS_Shape S;
  if ( !theGeomObject->_is_nil() ){
    _geomClient = getGeomClient();

    if(CORBA::is_nil(_geomGen)) {
      Engines::EngineComponent_ptr temp=GetLCC()->FindOrLoad_Component("FactoryServer","GEOM");
      _geomGen=GEOM::GEOM_Gen::_narrow(temp);
    }
    if ( _geomClient != NULL  && !_geomGen->_is_nil () )
      S = _geomClient->GetShape( _geomGen, theGeomObject );
  }
  return S;
}





// HEXABLOCK_ORB::GEOM_Object_ptr HEXABLOCK_Gen_i::shapeToGeomObject (const TopoDS_Shape& theShape )
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



// GEOM::GEOM_Object_ptr HEXABLOCK_Gen_i::_makeFace( const TopoDS_Shape& theShape )
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

// GEOM::GEOM_Object_ptr HEXABLOCK_Gen_i::_makeEdge( const TopoDS_Shape& theShape )
// {
//   ASSERT ( theShape.ShapeType() == TopAbs_EDGE );
//   int theStudyID = 0;
//   GEOM::GEOM_Object_var result;
//   GEOM::GEOM_Object_var p1;
//   GEOM::GEOM_Object_var p2;
// 
//   if ( CORBA::is_nil(_geomGen) ) { //_geomGen->_is_nil() 
//     Engines::EngineComponent_ptr temp=GetLCC()->FindOrLoad_Component("FactoryServer","GEOM");
//     _geomGen=GEOM::GEOM_Gen::_narrow(temp);
//   }
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

// GEOM::GEOM_Object_ptr HEXABLOCK_Gen_i::_makeVertex( const TopoDS_Shape& theShape )
// {
//   ASSERT ( theShape.ShapeType() == TopAbs_VERTEX );
// 
//   int theStudyID = 0;
//   GEOM::GEOM_Object_var result;
// 
//   TopoDS_Vertex aPoint;
//   aPoint      = TopoDS::Vertex( theShape );
//   gp_Pnt aPnt = BRep_Tool::Pnt( aPoint );
//   double x = aPnt.X();
//   double y = aPnt.Y();
//   double z = aPnt.Z();
// 
//   GEOM::GEOM_IBasicOperations_var aBasicOp = _geomGen->GetIBasicOperations(theStudyID);
//   result = aBasicOp->MakePointXYZ(x, y, z);
// 
//   return result._retn();
// }


// GEOM::GEOM_Object_ptr HEXABLOCK_Gen_i::shapeToGeomObject (const TopoDS_Shape& theShape )
// {
//   std::cout << "XXXXXXXX shapeToGeomObject "<< std::endl;
//   GEOM::GEOM_Object_var aShapeObj;
//   if ( !theShape.IsNull() ) {
//     switch ( theShape.ShapeType() ){
//       case TopAbs_VERTEX: aShapeObj = _makeVertex( theShape ); break;
// //       case TopAbs_FACE:   aShapeObj = _makeFace( theShape ); break; //CS_TODO
//       case TopAbs_EDGE:   aShapeObj = _makeEdge( theShape ); break;
//       default: throw (SALOME_Exception(LOCALIZED("shapeToGeomObject : shape type not supported")));
//     }
//   }
//   return aShapeObj._retn();
// }




GEOM::GEOM_Object_ptr HEXABLOCK_Gen_i::brepToGeomObject(const std::string& theBrep)
{
  int theStudyID = 0; //CS_TODO
  GEOM::GEOM_Object_var aShapeObj;

  if ( CORBA::is_nil(_geomGen) ){
    Engines::EngineComponent_ptr temp=GetLCC()->FindOrLoad_Component("FactoryServer","GEOM");
    _geomGen=GEOM::GEOM_Gen::_narrow(temp);
  }
  GEOM::GEOM_IInsertOperations_var aInsOp = _geomGen->GetIInsertOperations(theStudyID);

  char *tmpname = strdup("HexaAssocXXXXXX");
  int fd = mkstemp( tmpname );

  if ( fd != -1 ){
    ofstream tmpfile( tmpname );
    tmpfile << theBrep;
    tmpfile.close();
    aShapeObj = aInsOp->ImportFile( tmpname, "BREP" );
    close(fd);
    unlink(tmpname);
  }

  return aShapeObj._retn();
}






// GEOM::GEOM_Object_ptr HEXABLOCK_Gen_i::shapeToGeomObject (const TopoDS_Shape& theShape )
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


StudyContext* HEXABLOCK_Gen_i::GetCurrentStudyContext()
{
  if ( !CORBA::is_nil( myCurrentStudy ) &&
      myStudyContextMap.find( GetCurrentStudyID() ) != myStudyContextMap.end() )
    return myStudyContextMap[ myCurrentStudy->StudyId() ];
  else
    return 0;
}

int HEXABLOCK_Gen_i::RegisterObject(CORBA::Object_ptr theObject)
{
  StudyContext* myStudyContext = GetCurrentStudyContext();
  if ( myStudyContext && !CORBA::is_nil( theObject )) {
    CORBA::String_var iorString = GetORB()->object_to_string( theObject );
    return myStudyContext->addObject( string( iorString.in() ) );
  }
  return 0;
}


// SMESH::SMESH_Mesh_ptr SMESH_Gen_i::createMesh()
//      throw ( SALOME::SALOME_Exception )


char* HEXABLOCK_Gen_i::ComponentDataType()
{
//   if(MYDEBUG) MESSAGE( "HEXABLOCK_Gen_i::ComponentDataType" );
  return CORBA::string_dup( "HEXABLOCK" );
}



// SMESH_Mesh* SMESH_Gen::CreateMesh(int theStudyId, bool theIsEmbeddedMode)
//   throw(SALOME_Exception)
// {
//   Unexpect aCatch(SalomeException);
//   MESSAGE("SMESH_Gen::CreateMesh");
// 
//   // Get studyContext, create it if it does'nt exist, with a SMESHDS_Document
//   StudyContextStruct *aStudyContext = GetStudyContext(theStudyId);
// 
//   // create a new SMESH_mesh object
//   SMESH_Mesh *aMesh = new SMESH_Mesh(_localId++,
//                                      theStudyId,
//                                      this,
//                                      theIsEmbeddedMode,
//                                      aStudyContext->myDocument);
//   aStudyContext->mapMesh[_localId] = aMesh;
// 
//   return aMesh;
// }




Document_ptr HEXABLOCK_Gen_i::createDoc (const char* name)
                              throw ( SALOME::SALOME_Exception )
{
  Unexpect aCatch(SALOME_SalomeException);
//   if(MYDEBUG) MESSAGE( "SMESH_Gen_i::createMesh" );

  // Get or create the GEOM_Client instance
  try {
    HEXA_NS::Document *d   = _engine_cpp->addDocument (name);
    Document_impl *docImpl = new Document_impl( GetPOA(), d );

    // activate the CORBA servant of Mesh
    Document_var docServant = Document::_narrow( docImpl->_this() );
    int nextId = RegisterObject( docServant );
//     if(MYDEBUG) MESSAGE( "Add mesh to map with id = "<< nextId);
    return docServant._retn();
  }
  catch (SALOME_Exception& S_ex) {
//     THROW_SALOME_CORBA_EXCEPTION( S_ex.what(), SALOME::BAD_PARAM );
  }
  return Document::_nil();
}

Document_ptr HEXABLOCK_Gen_i::createDocInStudy (const char* name)
                              throw ( SALOME::SALOME_Exception )
{
  Unexpect aCatch(SALOME_SalomeException);
//   if(MYDEBUG) MESSAGE( "SMESH_Gen_i::CreateMesh" );
  // create mesh
  Document_var doc = this->createDoc (name);

  // publish mesh in the study
  if ( CanPublishInStudy( doc ) ) {
    SALOMEDS::StudyBuilder_var aStudyBuilder = myCurrentStudy->NewBuilder();
    aStudyBuilder->NewCommand();  // There is a transaction
    SALOMEDS::SObject_var aSO = PublishDoc( myCurrentStudy, doc.in() );
    aStudyBuilder->CommitCommand();
//     if ( !aSO->_is_nil() ) {
//       // Update Python script
//       TPythonDump() << aSO << " = " << this << ".CreateEmptyMesh()";
//     }
  }

  return doc._retn();
}




SALOME_LifeCycleCORBA*  HEXABLOCK_Gen_i::GetLCC() {
  if ( _lcc == NULL ) {
    _lcc = new SALOME_LifeCycleCORBA( GetNS() );
  }
  return _lcc;
}

SALOME_NamingService* HEXABLOCK_Gen_i::GetNS()
{
  if ( _ns == NULL ) {
    _ns = SINGLETON_<SALOME_NamingService>::Instance();
    ASSERT(SINGLETON_<SALOME_NamingService>::IsAlreadyExisting());
    _ns->init_orb( _orb );
  }
  return _ns;
}



int HEXABLOCK_Gen_i::GetCurrentStudyID()
{
  return myCurrentStudy->_is_nil() || myCurrentStudy->_non_existent() ? -1 : myCurrentStudy->StudyId();
}


void HEXABLOCK_Gen_i::SetCurrentStudy( SALOMEDS::Study_ptr theStudy )
{
  int curStudyId = GetCurrentStudyID();
  myCurrentStudy = SALOMEDS::Study::_duplicate( theStudy );
  // create study context, if it doesn't exist and set current study
  int studyId = GetCurrentStudyID();
  if ( myStudyContextMap.find( studyId ) == myStudyContextMap.end() ) {
    myStudyContextMap[ studyId ] = new StudyContext;
  }

//   // myCurrentStudy may be nil
//   if ( !CORBA::is_nil( myCurrentStudy ) ) {
//     SALOMEDS::StudyBuilder_var aStudyBuilder = myCurrentStudy->NewBuilder();
//     if( !myCurrentStudy->FindComponent( "GEOM" )->_is_nil() )
//       aStudyBuilder->LoadWith( myCurrentStudy->FindComponent( "GEOM" ), GetGeomEngine() );
// 
//     // NPAL16168, issue 0020210
//     // Let meshes update their data depending on GEOM groups that could change
//     if ( curStudyId != studyId )
//     {
//       //SALOMEDS::SComponent_var me =  PublishComponent( myCurrentStudy );
//       SALOMEDS::SComponent_var me = SALOMEDS::SComponent::_narrow
//         ( myCurrentStudy->FindComponent( ComponentDataType() ) );
//       if ( !me->_is_nil() ) {
//         SALOMEDS::ChildIterator_var anIter = myCurrentStudy->NewChildIterator( me );
//         for ( ; anIter->More(); anIter->Next() ) {
//           SALOMEDS::SObject_var so = anIter->Value();
//           CORBA::Object_var    ior = SObjectToObject( so );
//           if ( SMESH_Mesh_i*  mesh = SMESH::DownCast<SMESH_Mesh_i*>( ior ))
//             mesh->CheckGeomGroupModif();
//         }
//       }
//     }
//   }
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
    HEXABLOCK_Gen_i *myHEXABLOCK 
      = new HEXABLOCK_Gen_i(orb, poa, contId, instanceName, interfaceName);
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
// char* HEXABLOCK_Gen_i::makeBanner(const char* name)
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
// HEXABLOCK_ORB::Vertex_ptr HEXABLOCK_Gen_i::makeVertex(CORBA::Double x, CORBA::Double y, CORBA::Double z)
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
// HEXABLOCK_ORB::Edge_ptr HEXABLOCK_Gen_i::makeEdge(HEXABLOCK_ORB::Vertex_ptr va, HEXABLOCK_ORB::Vertex_ptr vb)
// {
//   Edge *edg=_engine_cpp->makeEdge(va,vb);
//   Edge_impl *servantCorba=new Edge_impl(edg);
//   return servantCorba->_this();
// }
// //=============================================================================
// // 
// //=============================================================================
