// Copyright (C) 2009-2012  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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
#ifndef _HEXABLOCK_HXX_
#define _HEXABLOCK_HXX_

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(HEXABLOCK_Gen)
#include "SALOME_Component_i.hxx"
//  #include "SALOMEDSImpl_TMPFile.hxx"
//  #include "SALOMEDSImpl_SComponent.hxx"



#include <sstream>


#include <TopoDS_Shape.hxx>
#include "GEOM_Client.hxx"

#include "hexa_base.hxx"
#include "Hex.hxx"

using namespace HEXABLOCK_ORB;

class SALOME_LifeCycleCORBA;
class SALOME_NamingService;





// ===========================================================
// Study context - stores study-connected objects references
// ==========================================================
class StudyContext
{
public:
  // constructor
  StudyContext() {}
  // destructor
  ~StudyContext()
  {
    mapIdToIOR.clear();
    mapIdToId.clear();
  }
  // register object in the internal map and return its id
  int addObject( string theIOR )
  {
    int nextId = getNextId();
    mapIdToIOR[ nextId ]  = theIOR;
    return nextId;
  }
  // find the object id in the internal map by the IOR
  int findId( string theIOR )
  {
    map<int, string>::iterator imap;
    for ( imap = mapIdToIOR.begin(); imap != mapIdToIOR.end(); ++imap ) {
      if ( imap->second == theIOR )
        return imap->first;
    }
    return 0;
  }
  // get object's IOR by id
  string getIORbyId( const int theId )
  {
    if ( mapIdToIOR.find( theId ) != mapIdToIOR.end() )
      return mapIdToIOR[ theId ];
    return string( "" );
  }
  // get object's IOR by old id
  string getIORbyOldId( const int theOldId )
  {
    if ( mapIdToId.find( theOldId ) != mapIdToId.end() )
      return getIORbyId( mapIdToId[ theOldId ] );
    return string( "" );
  }
  // maps old object id to the new one (used when restoring data)
  void mapOldToNew( const int oldId, const int newId ) {
    mapIdToId[ oldId ] = newId;
  }
  // get old id by a new one
  int getOldId( const int newId ) {
    map<int, int>::iterator imap;
    for ( imap = mapIdToId.begin(); imap != mapIdToId.end(); ++imap ) {
      if ( imap->second == newId )
        return imap->first;
    }
    return 0;
  }

private:
  // get next free object identifier
  int getNextId()
  {
    int id = 1;
    while( mapIdToIOR.find( id ) != mapIdToIOR.end() )
      id++;
    return id;
  }

  map<int, string> mapIdToIOR;      // persistent-to-transient map
  map<int, int>    mapIdToId;       // used to translate object from persistent to transient form
};



class HEXABLOCK_Gen_i: public POA_HEXABLOCK_ORB::HEXABLOCK_Gen,
                       public Engines_Component_i 
{
public:
    HEXABLOCK_Gen_i( CORBA::ORB_ptr orb,
            PortableServer::POA_ptr poa,
            PortableServer::ObjectId * contId, 
            const char *instanceName, 
            const char *interfaceName);
  // Get object of the CORBA reference
    static CORBA::ORB_var GetORB() { return _orb;}
    static PortableServer::POA_var GetPOA() { return _poa;}
    static PortableServer::ServantBase_var GetServant( CORBA::Object_ptr theObject );

    virtual ~HEXABLOCK_Gen_i();

    void test();
    ::CORBA::Long countDocument();
    Document_ptr getDocument(::CORBA::Long i);
    void removeDocument(Document_ptr d); //CS_TODO
    Document_ptr addDocument  (const char* name);
    Document_ptr loadDocument (const char* xmlFilename);

    GEOM_Client* getGeomClient();
    TopoDS_Shape geomObjectToShape(GEOM::GEOM_Object_ptr theGeomObject);
//     GEOM::GEOM_Object_ptr shapeToGeomObject(const TopoDS_Shape& theShape );
    GEOM::GEOM_Object_ptr brepToGeomObject(const std::string& theBrep);
    static SALOME_LifeCycleCORBA*  GetLCC();
    static SALOME_NamingService* GetNS();
    static HEXABLOCK_Gen_i* GetHEXABLOCKGen() { return _HEXABLOCKGen;}


    virtual Engines::TMPFile* DumpPython(CORBA::Object_ptr theStudy,
                                         CORBA::Boolean isPublished,
                                         CORBA::Boolean isMultiFile,
                                         CORBA::Boolean& isValidScript);

  void SetCurrentStudy( SALOMEDS::Study_ptr theStudy );
  int GetCurrentStudyID();
  StudyContext* GetCurrentStudyContext();
  std::string /*void */SetName( SALOMEDS::SObject_ptr theSObject,
                const char*           theName,
                const char*           theDefaultName );
  int   RegisterObject(CORBA::Object_ptr theObject);
//   SMESH::SMESH_Mesh_ptr SMESH_Gen_i::createMesh() throw ( SALOME::SALOME_Exception )
  char* ComponentDataType();
  

  bool CanPublishInStudy(CORBA::Object_ptr theIOR);
  SALOMEDS::SComponent_ptr PublishComponent(SALOMEDS::Study_ptr theStudy);
  static SALOMEDS::SObject_ptr ObjectToSObject(SALOMEDS::Study_ptr theStudy, CORBA::Object_ptr   theObject);
  
  SALOMEDS::SObject_ptr PublishDoc( SALOMEDS::Study_ptr   theStudy,
                                    Document_ptr          theDoc,
                                    const char*           theName = 0);



  Document_ptr createDoc(const char* name) 
               throw ( SALOME::SALOME_Exception );
  Document_ptr createDocInStudy(const char* name)
               throw ( SALOME::SALOME_Exception );

  //-----------------------------------------------------------------------//
  // Inherited methods from SALOMEDS::Driver                               //
  //-----------------------------------------------------------------------//

public :
  SALOMEDS::TMPFile* Save(SALOMEDS::SComponent_ptr theComponent,
                          const char* theURL,
                          bool isMultiFile);

  SALOMEDS::TMPFile* SaveASCII(SALOMEDS::SComponent_ptr theComponent,
                               const char* theURL,
                               bool isMultiFile);

  CORBA::Boolean Load(SALOMEDS::SComponent_ptr theComponent,
                      const SALOMEDS::TMPFile& theStream,
                      const char* theURL,
                      bool isMultiFile);

  CORBA::Boolean LoadASCII(SALOMEDS::SComponent_ptr theComponent,
                           const SALOMEDS::TMPFile& theStream,
                           const char* theURL,
                           bool isMultiFile);

  void Close(SALOMEDS::SComponent_ptr theComponent);
  // char* ComponentDataType();  Deja la 

  CORBA::Boolean CanCopy  (SALOMEDS::SObject_ptr theObject);
  CORBA::Boolean CanPaste (const char* theComponentName, 
                           CORBA::Long theObjectID);

  SALOMEDS::TMPFile* CopyFrom(SALOMEDS::SObject_ptr theObject, 
                              CORBA::Long& theObjectID);

  SALOMEDS::SObject_ptr PasteInto(const SALOMEDS::TMPFile& theStream,
                                  CORBA::Long theObjectID,
                                  SALOMEDS::SObject_ptr theObject);
  SALOMEDS::SObject_ptr PublishInStudy(SALOMEDS::Study_ptr theStudy,
                                       SALOMEDS::SObject_ptr theSObject,
                                       CORBA::Object_ptr theObject,
                                       const char* theName) throw (SALOME::SALOME_Exception) ;


  char* IORToLocalPersistentID(SALOMEDS::SObject_ptr theSObject,
                               const char* IORString,
                               CORBA::Boolean isMultiFile,
                               CORBA::Boolean isASCII);
  char* LocalPersistentIDToIOR(SALOMEDS::SObject_ptr theSObject,
                               const char* aLocalPersistentID,
                               CORBA::Boolean isMultiFile,
                               CORBA::Boolean isASCII);

  char* getVersion();

#if 0
  virtual std::string GetIOR();

  /*******************
  virtual SALOMEDSImpl_TMPFile* Save(const SALOMEDSImpl_SComponent& compo,
                                     const std::string& theURL,
                                     long& theStreamLength,
                                     bool isMultiFile);

  virtual SALOMEDSImpl_TMPFile* SaveASCII(const SALOMEDSImpl_SComponent& compo,
                                          const std::string& theURL,
                                          long& theStreamLength,
                                          bool isMultiFile);
  *********************/
  
  virtual SALOMEDS::TMPFile* Save(SALOMEDS::SComponent_ptr theComponent,
                                  const char* theURL,
                                  bool isMultiFile);


  virtual SALOMEDS::TMPFile* SaveASCII(SALOMEDS::SComponent_ptr theComponent,
                               const char* theURL,
                               bool isMultiFile);


  
  virtual bool Load(const SALOMEDSImpl_SComponent& compo,
                    const unsigned char* theStream,
                    const long theStreamLength,
                    const std::string& theURL,
                    bool isMultiFile);

  virtual bool LoadASCII(const SALOMEDSImpl_SComponent& theComponent,
                         const unsigned char* theStream,
                         const long theStreamLength,
                         const std::string& theURL,
                         bool isMultiFile);

  virtual void Close(const SALOMEDSImpl_SComponent& theComponent);
 
  // virtual std::string ComponentDataType();

  virtual std::string IORToLocalPersistentID(const SALOMEDSImpl_SObject& theSObject,
                                             const std::string& IORString,
                                             bool isMultiFile,
                                             bool isASCII);

  virtual std::string LocalPersistentIDToIOR(const SALOMEDSImpl_SObject& theSObject,
                                             const std::string& aLocalPersistentID,
                                             bool isMultiFile,
                                             bool isASCII);

  virtual bool CanCopy(const SALOMEDSImpl_SObject& theObject);

  virtual SALOMEDSImpl_TMPFile* CopyFrom(const SALOMEDSImpl_SObject& theObject, 
                                         int& theObjectID,
                                         long& theStreamLength);
  
  virtual bool CanPaste(const std::string& theComponentName, int theObjectID);

  virtual std::string PasteInto(const unsigned char* theStream,
                                const long theStreamLength,
                                int theObjectID,
                                const SALOMEDSImpl_SObject& theObject);

  virtual SALOMEDSImpl_TMPFile* DumpPython(SALOMEDSImpl_Study* theStudy, 
                                           bool isPublished, 
                                           bool isMultiFile,
                                           bool& isValidScript,
                                           long& theStreamLength);
#endif
private :
   HEXA_NS::Hex* _engine_cpp;
   GEOM_Client*  _geomClient;


  static PortableServer::POA_var _poa;// POA reference
  static GEOM::GEOM_Gen_var      _geomGen;
  static SALOME_LifeCycleCORBA*  _lcc;
  static CORBA::ORB_var          _orb;
  static SALOME_NamingService*   _ns;
  static HEXABLOCK_Gen_i*        _HEXABLOCKGen;


  GEOM::GEOM_Object_ptr _makeVertex( const TopoDS_Shape& theShape );
  GEOM::GEOM_Object_ptr _makeEdge( const TopoDS_Shape& theShape );

  map<int, StudyContext*>   myStudyContextMap;  // Map of study context objects
  SALOMEDS::Study_var       myCurrentStudy;     // Current study

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
    return dynamic_cast<T>(HEXABLOCK_Gen_i::GetServant(theArg).in());
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
