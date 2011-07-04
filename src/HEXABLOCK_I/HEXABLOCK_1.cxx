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


#include "HEXABLOCK.hxx"
// #include "SMESH_Mesh_i.hxx"
// #include "SMESH_Hypothesis_i.hxx"
// #include "SMESH_Algo_i.hxx"
// #include "SMESH_Group_i.hxx"
// #include "SMESH_subMesh_i.hxx"

#include "SALOME_NamingService.hxx"

#include CORBA_CLIENT_HEADER(SALOME_ModuleCatalog)

#include "utilities.h"
#include "Utils_ExceptHandlers.hxx"

#include <TCollection_AsciiString.hxx>

// #ifdef _DEBUG_
// static int MYDEBUG = 0;
// static int VARIABLE_DEBUG = 0;
// #else
// static int MYDEBUG = 0;
// static int VARIABLE_DEBUG = 0;
// #endif

static int MYDEBUG = 1;


using namespace HEXABLOCK_ORB;


//=============================================================================
/*!
 *  Get...Tag [ static ]
 *
 *  Methods which determine SMESH data model structure
 */
//=============================================================================

// long SMESH_Gen_i::GetHypothesisRootTag()
// {
//   return SMESH::Tag_HypothesisRoot;
// }
// 
// long SMESH_Gen_i::GetAlgorithmsRootTag()
// {
//   return SMESH::Tag_AlgorithmsRoot;
// }
// 
// long SMESH_Gen_i::GetRefOnShapeTag()
// {
//   return SMESH::Tag_RefOnShape;
// }
// 
// long SMESH_Gen_i::GetRefOnAppliedHypothesisTag()
// {
//   return SMESH::Tag_RefOnAppliedHypothesis;
// }
// 
// long SMESH_Gen_i::GetRefOnAppliedAlgorithmsTag()
// {
//   return SMESH::Tag_RefOnAppliedAlgorithms;
// }
// 
// long SMESH_Gen_i::GetSubMeshOnVertexTag()
// {
//   return SMESH::Tag_SubMeshOnVertex;
// }
// 
// long SMESH_Gen_i::GetSubMeshOnEdgeTag()
// {
//   return SMESH::Tag_SubMeshOnEdge;
// }
// 
// long SMESH_Gen_i::GetSubMeshOnFaceTag()
// {
//   return SMESH::Tag_SubMeshOnFace;
// }
// 
// long SMESH_Gen_i::GetSubMeshOnSolidTag()
// {
//   return SMESH::Tag_SubMeshOnSolid;
// }
// 
// long SMESH_Gen_i::GetSubMeshOnCompoundTag()
// {
//   return SMESH::Tag_SubMeshOnCompound;
// }
// 
// long SMESH_Gen_i::GetSubMeshOnWireTag()
// {
//   return SMESH::Tag_SubMeshOnWire;
// }
// 
// long SMESH_Gen_i::GetSubMeshOnShellTag()
// {
//   return SMESH::Tag_SubMeshOnShell;
// }
// 
// long SMESH_Gen_i::GetNodeGroupsTag()
// {
//   return SMESH::Tag_NodeGroups;
// }
// 
// long SMESH_Gen_i::GetEdgeGroupsTag()
// {
//   return SMESH::Tag_EdgeGroups;
// }
// 
// long SMESH_Gen_i::GetFaceGroupsTag()
// {
//   return SMESH::Tag_FaceGroups;
// }
// 
// long SMESH_Gen_i::GetVolumeGroupsTag()
// {
//   return SMESH::Tag_VolumeGroups;
// }
// 
// long SMESH_Gen_i::Get0DElementsGroupsTag()
// {
//   return SMESH::Tag_0DElementsGroups;
// }


//=============================================================================
/*!
 *  SMESH_Gen_i::CanPublishInStudy
 *
 *  Returns true if object can be published in the study
 */
//=============================================================================
bool HEXABLOCK_Gen_i::CanPublishInStudy(CORBA::Object_ptr theIOR)
{
  if(MYDEBUG) MESSAGE("CanPublishInStudy - "<<!CORBA::is_nil(myCurrentStudy));
  if(CORBA::is_nil(myCurrentStudy))
    return false;
  
  Document_var aDoc       = Document::_narrow(theIOR);
  if( !aDoc->_is_nil() )
    return true;

//   SMESH::SMESH_subMesh_var aSubMesh = SMESH::SMESH_subMesh::_narrow(theIOR);
//   if( !aSubMesh->_is_nil() )
//     return true;
// 
//   SMESH::SMESH_Hypothesis_var aHyp  = SMESH::SMESH_Hypothesis::_narrow(theIOR);
//   if( !aHyp->_is_nil() )
//     return true;
// 
//   SMESH::SMESH_GroupBase_var aGroup = SMESH::SMESH_GroupBase::_narrow(theIOR);
//   if( !aGroup->_is_nil() )
//     return true;

  if(MYDEBUG) MESSAGE("CanPublishInStudy--CANT");
  return false;
}

//=======================================================================
//function : ObjectToSObject
//purpose  : 
//=======================================================================

SALOMEDS::SObject_ptr HEXABLOCK_Gen_i::ObjectToSObject( SALOMEDS::Study_ptr theStudy,
                                                  CORBA::Object_ptr   theObject )
{
  SALOMEDS::SObject_var aSO;
  if ( !CORBA::is_nil( theStudy ) && !CORBA::is_nil( theObject ))
  {
    CORBA::String_var objStr = HEXABLOCK_Gen_i::GetORB()->object_to_string( theObject );
    aSO = theStudy->FindObjectIOR( objStr.in() );
  }
  return aSO._retn();
}

//=======================================================================
//function : objectToServant
//purpose  : 
//=======================================================================

// template<typename T> static inline T* objectToServant( CORBA::Object_ptr theIOR )
// {
//   return dynamic_cast<T*>( SMESH_Gen_i::GetServant( theIOR ).in() );
// }

//=======================================================================
//function : ShapeToGeomObject
//purpose  : 
//=======================================================================

// GEOM::GEOM_Object_ptr SMESH_Gen_i::ShapeToGeomObject (const TopoDS_Shape& theShape )
// {
//   GEOM::GEOM_Object_var aShapeObj;
//   if ( !theShape.IsNull() ) {
//     GEOM_Client* aClient = GetShapeReader();
//     TCollection_AsciiString IOR;
//     if ( aClient && aClient->Find( theShape, IOR ))
//     {
//       CORBA::Object_var obj = GetORB()->string_to_object( IOR.ToCString() );
//       aShapeObj = GEOM::GEOM_Object::_narrow ( obj );
//     }
//   }
//   return aShapeObj._retn();
// }

//=======================================================================
//function : GeomObjectToShape
//purpose  : 
//=======================================================================

// TopoDS_Shape SMESH_Gen_i::GeomObjectToShape(GEOM::GEOM_Object_ptr theGeomObject)
// {
//   TopoDS_Shape S;
//   if ( !theGeomObject->_is_nil() ) {
//     GEOM_Client* aClient = GetShapeReader();
//     GEOM::GEOM_Gen_ptr aGeomEngine = GetGeomEngine();
//     if ( aClient && !aGeomEngine->_is_nil () )
//       S = aClient->GetShape( aGeomEngine, theGeomObject );
//   }
//   return S;
// }















//=======================================================================
//function : publish
//purpose  : 
//=======================================================================

static SALOMEDS::SObject_ptr publish(SALOMEDS::Study_ptr   theStudy,
                                     CORBA::Object_ptr     theIOR,
                                     SALOMEDS::SObject_ptr theFatherObject,
                                     const int             theTag = 0,
                                     const char*           thePixMap = 0,
                                     const bool            theSelectable = true)
{
  SALOMEDS::SObject_var SO = HEXABLOCK_Gen_i::ObjectToSObject( theStudy, theIOR );
  SALOMEDS::StudyBuilder_var aStudyBuilder = theStudy->NewBuilder();
  if ( SO->_is_nil() ) {
    if ( theTag == 0 )
      SO = aStudyBuilder->NewObject( theFatherObject );
    else if ( !theFatherObject->FindSubObject( theTag, SO ))
      SO = aStudyBuilder->NewObjectToTag( theFatherObject, theTag );
  }

  SALOMEDS::GenericAttribute_var anAttr;
  if ( !CORBA::is_nil( theIOR )) {
    anAttr = aStudyBuilder->FindOrCreateAttribute( SO, "AttributeIOR" );
    CORBA::String_var objStr = HEXABLOCK_Gen_i::GetORB()->object_to_string( theIOR );
    SALOMEDS::AttributeIOR::_narrow(anAttr)->SetValue( objStr.in() );
  }

  if ( thePixMap ) {
    anAttr  = aStudyBuilder->FindOrCreateAttribute( SO, "AttributePixMap" );
    SALOMEDS::AttributePixMap_var pm = SALOMEDS::AttributePixMap::_narrow( anAttr );
    pm->SetPixMap( thePixMap );
  }

//   if ( !theSelectable ) {
//     anAttr   = aStudyBuilder->FindOrCreateAttribute( SO, "AttributeSelectable" );
//     SALOMEDS::AttributeSelectable::_narrow( anAttr )->SetSelectable( false );
//   }
  return SO._retn();
}


//=======================================================================
//function : setName
//purpose  : 
//=======================================================================
void HEXABLOCK_Gen_i::SetName( SALOMEDS::SObject_ptr theSObject,
                         const char*           theName,
                         const char*           theDefaultName )
{
  if ( !theSObject->_is_nil() ) {
    SALOMEDS::StudyBuilder_var aStudyBuilder = theSObject->GetStudy()->NewBuilder();
    SALOMEDS::GenericAttribute_var anAttr =
      aStudyBuilder->FindOrCreateAttribute( theSObject, "AttributeName" );
    SALOMEDS::AttributeName_var aNameAttr = SALOMEDS::AttributeName::_narrow( anAttr );
    if ( theName && strlen( theName ) != 0 )
      aNameAttr->SetValue( theName );
    else {
      CORBA::String_var curName = CORBA::string_dup( aNameAttr->Value() );
      if ( strlen( curName ) == 0 ) {
        TCollection_AsciiString aName( (char*) theDefaultName );
        aName += TCollection_AsciiString("_") + TCollection_AsciiString( theSObject->Tag() );
        aNameAttr->SetValue( aName.ToCString() );
      }
    }
  }
}

//=======================================================================
//function : SetPixMap
//purpose  : 
//=======================================================================
// void SMESH_Gen_i::SetPixMap(SALOMEDS::SObject_ptr theSObject,
//                             const char*           thePixMap)
// {
//   if ( !theSObject->_is_nil() && thePixMap && strlen( thePixMap ))
//   {
//     SALOMEDS::Study_var aStudy = theSObject->GetStudy();
//     SALOMEDS::StudyBuilder_var aStudyBuilder = aStudy->NewBuilder();
//     SALOMEDS::GenericAttribute_var anAttr =
//       aStudyBuilder->FindOrCreateAttribute( theSObject, "AttributePixMap" );
//     SALOMEDS::AttributePixMap_var aPMAttr = SALOMEDS::AttributePixMap::_narrow( anAttr );
//     aPMAttr->SetPixMap( thePixMap );
//   }
// }

//=======================================================================
//function : addReference
//purpose  : 
//=======================================================================

// static void addReference (SALOMEDS::Study_ptr   theStudy,
//                           SALOMEDS::SObject_ptr theSObject,
//                           CORBA::Object_ptr     theToObject,
//                           int                   theTag = 0)
// {
//   SALOMEDS::SObject_var aToObjSO = SMESH_Gen_i::ObjectToSObject( theStudy, theToObject );
//   if ( !aToObjSO->_is_nil() && !theSObject->_is_nil() ) {
//     SALOMEDS::StudyBuilder_var aStudyBuilder = theStudy->NewBuilder();
//     SALOMEDS::SObject_var aReferenceSO;
//     if ( !theTag ) {
//       // check if the reference to theToObject already exists
//       // and find a free label for the reference object
//       bool isReferred = false;
//       int tag = 1;
//       SALOMEDS::ChildIterator_var anIter = theStudy->NewChildIterator( theSObject );
//       for ( ; !isReferred && anIter->More(); anIter->Next(), ++tag ) {
//         if ( anIter->Value()->ReferencedObject( aReferenceSO )) {
//           if ( strcmp( aReferenceSO->GetID(), aToObjSO->GetID() ) == 0 )
//             isReferred = true;
//         }
//         else if ( !theTag ) {
//           SALOMEDS::GenericAttribute_var anAttr;
//           if ( !anIter->Value()->FindAttribute( anAttr, "AttributeIOR" ))
//             theTag = tag;
//         }
//       }
//       if ( isReferred )
//         return;
//       if ( !theTag )
//         theTag = tag;
//     }
//     if ( !theSObject->FindSubObject( theTag, aReferenceSO ))
//       aReferenceSO = aStudyBuilder->NewObjectToTag( theSObject, theTag );
//     aStudyBuilder->Addreference( aReferenceSO, aToObjSO );
//   }
// }

//=============================================================================
/*!
 *  SMESH_Gen_i::PublishInStudy
 *
 *  Publish object in the study
 */
//=============================================================================

// SALOMEDS::SObject_ptr HEXABLOCK_Gen_i::PublishInStudy( SALOMEDS::Study_ptr   theStudy,
//                                                  SALOMEDS::SObject_ptr theSObject,
//                                                  CORBA::Object_ptr     theIOR,
//                                                  const char*           theName )
//      throw (SALOME::SALOME_Exception)
// {
//   Unexpect aCatch(SALOME_SalomeException);
//   SALOMEDS::SObject_var aSO;
//   if ( CORBA::is_nil( theStudy ) || CORBA::is_nil( theIOR ))
//     return aSO._retn();
//   if(MYDEBUG) MESSAGE("PublishInStudy");
// 
//   // Publishing a mesh
// //   SMESH::SMESH_Mesh_var aMesh = SMESH::SMESH_Mesh::_narrow( theIOR );
//   HEXABLOCK_Gen_i::Document_var aDoc = HEXABLOCK_Gen_i::Document::_narrow( theIOR );
//   if( !aDoc->_is_nil() )
//     aSO = PublishDoc( theStudy, aDoc, theName );
// 
// //   // Publishing a sub-mesh
// //   SMESH::SMESH_subMesh_var aSubMesh = SMESH::SMESH_subMesh::_narrow( theIOR );
// //   if( aSO->_is_nil() && !aSubMesh->_is_nil() ) {
// //     GEOM::GEOM_Object_var aShapeObject = aSubMesh->GetSubShape();
// //     aMesh = aSubMesh->GetFather();
// //     aSO = PublishSubMesh( theStudy, aMesh, aSubMesh, aShapeObject, theName );
// //   }
// // 
// //   // Publishing a hypothesis or algorithm
// //   SMESH::SMESH_Hypothesis_var aHyp = SMESH::SMESH_Hypothesis::_narrow( theIOR );
// //   if ( aSO->_is_nil() && !aHyp->_is_nil() )
// //     aSO = PublishHypothesis( theStudy, aHyp );
// // 
// //   // Publishing a group
// //   SMESH::SMESH_GroupBase_var aGroup = SMESH::SMESH_GroupBase::_narrow(theIOR);
// //   if ( aSO->_is_nil() && !aGroup->_is_nil() ) {
// //     GEOM::GEOM_Object_var aShapeObject;
// //     aMesh = aGroup->GetMesh();
// //     aSO = PublishGroup( theStudy, aMesh, aGroup, aShapeObject, theName );
// //   }
//   if(MYDEBUG) MESSAGE("PublishInStudy_END");
// 
//   return aSO._retn();
// }

// =======================================================================
// function : PublishComponent
// purpose  : 
// =======================================================================
SALOMEDS::SComponent_ptr HEXABLOCK_Gen_i::PublishComponent(SALOMEDS::Study_ptr theStudy)
{
  if ( CORBA::is_nil( theStudy ))
    return SALOMEDS::SComponent::_nil();
  if(MYDEBUG) MESSAGE("PublishComponent");

  SALOMEDS::SComponent_var father =
    SALOMEDS::SComponent::_narrow( theStudy->FindComponent( ComponentDataType() ) );
  if ( !CORBA::is_nil( father ) )
    return father._retn();
  MESSAGE("AAAAAAAAAAA");

  SALOME_ModuleCatalog::ModuleCatalog_var aCat =
    SALOME_ModuleCatalog::ModuleCatalog::_narrow( GetNS()->Resolve("/Kernel/ModulCatalog") );
  if ( CORBA::is_nil( aCat ) )
    return father._retn();
  MESSAGE("BBBBBBBBBBB");

  SALOME_ModuleCatalog::Acomponent_var aComp = aCat->GetComponent( ComponentDataType() );
  if ( CORBA::is_nil( aComp ) )
    return father._retn();
  MESSAGE("CCCCCCCCCCC");

  SALOMEDS::StudyBuilder_var     aStudyBuilder = theStudy->NewBuilder(); 
  SALOMEDS::GenericAttribute_var anAttr;
  SALOMEDS::AttributePixMap_var  aPixmap;

  father  = aStudyBuilder->NewComponent( ComponentDataType() );
  aStudyBuilder->DefineComponentInstance( father, HEXABLOCK_Gen_i::_this() );
//   aStudyBuilder->DefineComponentInstance( father, _orb->object_to_string( HEXABLOCK_Gen_i::_this() ) );

//   MESSAGE("PublishComponent-- _orb->object_to_string( this ) =>"<< _orb->object_to_string( this ) );
  MESSAGE("PublishComponent-- HEXABLOCK_Gen_i::_this() =>"<< HEXABLOCK_Gen_i::_this() );

  anAttr  = aStudyBuilder->FindOrCreateAttribute( father, "AttributePixMap" );
  aPixmap = SALOMEDS::AttributePixMap::_narrow( anAttr );
  aPixmap ->SetPixMap( "ICON_OBJBROWSER_HEXABLOCK" );
  SetName( father, aComp->componentusername(), "HEXABLOCK" );
  if(MYDEBUG) MESSAGE("PublishComponent--END");

  return father._retn();
}






// bool HEXABLOCKGUI::createSComponent() //addComponent
// {
//   DEBTRACE("HEXABLOCKGUI::createSComponent");
//   _PTR(Study)            aStudy = (( SalomeApp_Study* )(getApp()->activeStudy()))->studyDS();
//   _PTR(StudyBuilder)     aBuilder (aStudy->NewBuilder());
//   _PTR(GenericAttribute) anAttr;
//   _PTR(AttributeName)    aName;
// 
//   // --- Find or create "HEXABLOCK" SComponent in the study
//   _PTR(SComponent) aComponent = aStudy->FindComponent("HEXABLOCK");
//   if ( !aComponent )
//     {
//       aComponent = aBuilder->NewComponent("HEXABLOCK");
//       anAttr = aBuilder->FindOrCreateAttribute(aComponent, "AttributeName");
//       aName = _PTR(AttributeName) (anAttr);
//       aName->SetValue(getApp()->moduleTitle("HEXABLOCK").toStdString());
//       
//       anAttr = aBuilder->FindOrCreateAttribute(aComponent, "AttributePixMap");
//       _PTR(AttributePixMap) aPixmap(anAttr);
//       aPixmap->SetPixMap("share/salome/resources/hexablock/ModuleHexablock.png");
//       
//       aBuilder->DefineComponentInstance(aComponent, getApp()->defaultEngineIOR().toStdString());
// 
//       DEBTRACE("HEXABLOCKGUI::createSComponent defaultEngineIOR=>"<<getApp()->defaultEngineIOR().toStdString());
// //       SalomeApp_DataModel::synchronize( aComponent, HEXABLOCKGUI::activeStudy() );
//       return true;
//     }
//   return false;
// }

















// SALOMEDS::SObject_ptr GEOM_Gen_i::PublishInStudy(SALOMEDS::Study_ptr theStudy,
//                                                  SALOMEDS::SObject_ptr theSObject,
//                                                  CORBA::Object_ptr theObject,
//                                                  const char* theName) throw (SALOME::SALOME_Exception)
// {
//   Unexpect aCatch(SALOME_SalomeException);
//   SALOMEDS::SObject_var aResultSO;
//   if(CORBA::is_nil(theObject) || theStudy->_is_nil()) return aResultSO;
// 
// 
//   GEOM::GEOM_Object_var aShape = GEOM::GEOM_Object::_narrow(theObject);
//   if(aShape->_is_nil()) return aResultSO;
// 
//   SALOMEDS::GenericAttribute_var anAttr;
//   SALOMEDS::StudyBuilder_var     aStudyBuilder = theStudy->NewBuilder();
// 
//   SALOMEDS::SComponent_var       aFather = theStudy->FindComponent("GEOM");
//   if (aFather->_is_nil()) {
//     aFather = aStudyBuilder->NewComponent("GEOM");
//     anAttr = aStudyBuilder->FindOrCreateAttribute(aFather, "AttributeName");
//     SALOMEDS::AttributeName_var aName = SALOMEDS::AttributeName::_narrow(anAttr);
//     aName->SetValue("Geometry");
//     aName->UnRegister();
//     anAttr = aStudyBuilder->FindOrCreateAttribute(aFather, "AttributePixMap");
//     SALOMEDS::AttributePixMap_var aPixMap=SALOMEDS::AttributePixMap::_narrow(anAttr);
//     aPixMap->SetPixMap("ICON_OBJBROWSER_Geometry");
//     aPixMap->UnRegister();
//     aStudyBuilder->DefineComponentInstance(aFather, (GEOM::GEOM_Gen_var)GEOM_Gen::_this());
//   }
//   if (aFather->_is_nil()) return aResultSO;
// 
//   if (CORBA::is_nil(theSObject)) {
//     aResultSO = aStudyBuilder->NewObject(aFather);
//   } else {
//     if (!theSObject->ReferencedObject(aResultSO))
//       aResultSO = SALOMEDS::SObject::_duplicate(theSObject); //SRN: Added Aug 24,2004 : for  the method AddInStudy with theFather argumenet != NULL
//       //THROW_SALOME_CORBA_EXCEPTION("Publish in study supervision graph error",SALOME::BAD_PARAM);
//   }
//   CORBA::String_var aGeomObjIOR = _orb->object_to_string(theObject);
//   aResultSO->SetAttrString("AttributeIOR",aGeomObjIOR);
// 
//   TCollection_AsciiString aShapeName("Shape_");
// 
//   CORBA::Long mytype=aShape->GetType();
//   if ( mytype == GEOM_GROUP ) {
//     GEOM::GEOM_IGroupOperations_var anOp = GetIGroupOperations( theStudy->StudyId() );
//     switch ( (TopAbs_ShapeEnum)anOp->GetType( aShape ) ) {
//     case TopAbs_VERTEX:
//       aResultSO->SetAttrString("AttributePixMap","ICON_OBJBROWSER_GROUP_PNT" );
//       aShapeName = "Group_Of_Vertices_";
//       break;
//     case TopAbs_EDGE:
//       aResultSO->SetAttrString("AttributePixMap","ICON_OBJBROWSER_GROUP_EDGE");
//       aShapeName = "Group_Of_Edges_";
//       break;
//     case TopAbs_FACE:
//       aResultSO->SetAttrString("AttributePixMap","ICON_OBJBROWSER_GROUP_FACE");
//       aShapeName = "Group_Of_Faces_";
//       break;
//     case TopAbs_SOLID:
//       aResultSO->SetAttrString("AttributePixMap","ICON_OBJBROWSER_GROUP_SOLID");
//       aShapeName = "Group_Of_Solids_";
//       break;
//     }
//   } else if ( mytype == GEOM_MARKER ) {
//     aResultSO->SetAttrString("AttributePixMap","ICON_OBJBROWSER_LCS");
//     aShapeName = "LocalCS_";
//   } else if ( mytype > ADVANCED_BASE ) {
//     char buf[20];
//     sprintf( buf, "%d", aShape->GetType() );
//     std::string advId = "ICON_OBJBROWSER_ADVANCED_"; advId += buf;
//     aResultSO->SetAttrString("AttributePixMap",advId.c_str());
//     aShapeName = "Advanced_";
//   } else {
//     GEOM::shape_type myshapetype=aShape->GetShapeType();
//     if ( myshapetype == GEOM::COMPOUND ) {
//     aResultSO->SetAttrString("AttributePixMap","ICON_OBJBROWSER_COMPOUND" );
//     aShapeName = "Compound_";
//   } else if ( myshapetype == GEOM::COMPSOLID ) {
//     aResultSO->SetAttrString("AttributePixMap","ICON_OBJBROWSER_COMPSOLID");
//     aShapeName = "Compsolid_";
//   } else if ( myshapetype == GEOM::SOLID ) {
//     aResultSO->SetAttrString("AttributePixMap","ICON_OBJBROWSER_SOLID");
//     aShapeName = "Solid_";
//   } else if ( myshapetype == GEOM::SHELL ) {
//     aResultSO->SetAttrString("AttributePixMap","ICON_OBJBROWSER_SHELL");
//     aShapeName = "Shell_";
//   } else if ( myshapetype == GEOM::FACE ) {
//     aResultSO->SetAttrString("AttributePixMap","ICON_OBJBROWSER_FACE");
//     aShapeName = "Face_";
//   } else if ( myshapetype == GEOM::WIRE ) {
//     aResultSO->SetAttrString("AttributePixMap","ICON_OBJBROWSER_WIRE");
//     aShapeName = "Wire_";
//   } else if ( myshapetype == GEOM::EDGE ) {
//     aResultSO->SetAttrString("AttributePixMap", "ICON_OBJBROWSER_EDGE");
//     aShapeName = "Edge_";
//   } else if ( myshapetype == GEOM::VERTEX ) {
//     aResultSO->SetAttrString("AttributePixMap","ICON_OBJBROWSER_VERTEX" );
//     aShapeName = "Vertex_";
//   }
//   }
//   //if (strlen(theName) == 0) aShapeName += TCollection_AsciiString(aResultSO->Tag());
//   //else aShapeName = TCollection_AsciiString(CORBA::string_dup(theName));
// 
//   // BEGIN: try to find existed name for current shape
//   bool HasName = false;
//   // recieve current TopoDS shape
//   CORBA::String_var entry = aShape->GetEntry();
//   Handle(GEOM_Object) aGShape = _impl->GetObject(aShape->GetStudyID(), entry);
//   TopoDS_Shape TopoSh = aGShape->GetValue();
//   // find label of main shape
//   GEOM::GEOM_Object_var aMainSh = aShape;
//   while( !aMainSh->IsMainShape() ) {
//     aMainSh = aMainSh->GetMainShape();
//   }
//   entry = aMainSh->GetEntry();
//   Handle(GEOM_Object) anObj = _impl->GetObject(aMainSh->GetStudyID(), entry);
//   TDF_Label aMainLbl = anObj->GetFunction(1)->GetNamingEntry();
// 
//   // check all named shapes using iterator
//   TDF_ChildIDIterator anIt (aMainLbl, TNaming_NamedShape::GetID(), Standard_True);
// 
//   for (; anIt.More() && !HasName; anIt.Next()) {
//     Handle(TNaming_NamedShape) anAttr =
//       Handle(TNaming_NamedShape)::DownCast(anIt.Value());
//     if (anAttr.IsNull()) continue;
//     TopoDS_Shape S = anAttr->Get();
//     if (S.IsEqual(TopoSh)) {
//       TDF_Label L = anAttr->Label();
//       Handle(TDataStd_Name) aName;
//       if (L.FindAttribute(TDataStd_Name::GetID(), aName)) {
//         aShapeName = aName->Get();
//         HasName = true;
//       }
//     }
//   }
//   // END: try to find existed name for current shape
// 
//   
//   aResultSO->SetAttrString("AttributeString",aVars.ToCString());
// 
//   aFather->UnRegister();
// 
//   //Set a name of the GEOM object
//   aShape->SetName(aShapeName.ToCString());
// 
//   return aResultSO._retn();
// }





//=============================================================================
/*!
 *  findMaxChildTag [ static internal ]
 *
 *  Finds maximum child tag for the given object
 */
//=============================================================================

static long findMaxChildTag( SALOMEDS::SObject_ptr theSObject )
{
  long aTag = 0;
  if ( !theSObject->_is_nil() ) {
    SALOMEDS::Study_var aStudy = theSObject->GetStudy();
    if ( !aStudy->_is_nil() ) {
      SALOMEDS::ChildIterator_var anIter = aStudy->NewChildIterator( theSObject );
      for ( ; anIter->More(); anIter->Next() ) {
        long nTag = anIter->Value()->Tag();
        if ( nTag > aTag )
          aTag = nTag;
      }
    }
  }
  return aTag;
}

//=======================================================================
//function : PublishMesh
//purpose  : 
//=======================================================================
SALOMEDS::SObject_ptr HEXABLOCK_Gen_i::PublishDoc ( SALOMEDS::Study_ptr     theStudy,
                                              Document_ptr theDoc,
                                              const char*             theName)
{
  if ( CORBA::is_nil( theStudy ) ||
       CORBA::is_nil( theDoc ))
    return SALOMEDS::SComponent::_nil();
  if(MYDEBUG) MESSAGE("PublishMesh--IN");

  // find or publish a mesh

  SALOMEDS::SObject_var aDocSO = ObjectToSObject( theStudy, theDoc );
  if ( aDocSO->_is_nil() )
  {
    SALOMEDS::SComponent_var father = PublishComponent( theStudy );
    if ( father->_is_nil() )
      return aDocSO._retn();

    // Find correct free tag
    long aTag = findMaxChildTag( father.in() );
//     if ( aTag <= GetAlgorithmsRootTag() )
//       aTag = GetAlgorithmsRootTag() + 1;
//     else
//       aTag++;
    aTag++;

    aDocSO = publish (theStudy, theDoc, father, aTag);//, "ICON_DOC_TREE_MESH_WARN" );
    if ( aDocSO->_is_nil() )
      return aDocSO._retn();
  }
  SetName( aDocSO, theName, "Doc" );

  // Add shape reference
//   GEOM::GEOM_Object_var aShapeObject = theMesh->GetShapeToMesh();
//   if ( !CORBA::is_nil( aShapeObject )) {
//     addReference( theStudy, aMeshSO, aShapeObject, GetRefOnShapeTag() );
// 
//     // Publish global hypotheses
// 
//     SMESH::ListOfHypothesis_var hypList = theMesh->GetHypothesisList( aShapeObject );
//     for ( int i = 0; i < hypList->length(); i++ ) {
//       SMESH::SMESH_Hypothesis_var aHyp = SMESH::SMESH_Hypothesis::_narrow( hypList[ i ]);
//       PublishHypothesis( theStudy, aHyp );
//       AddHypothesisToShape( theStudy, theMesh, aShapeObject, aHyp );
//     }
//   }
// 
//   // Publish submeshes
// 
//   SMESH_Mesh_i* mesh_i = objectToServant<SMESH_Mesh_i>( theMesh );
//   if ( !mesh_i )
//     return aMeshSO._retn();
//   map<int, SMESH_subMesh_i*>& subMap = mesh_i->_mapSubMesh_i;
//   map<int, SMESH_subMesh_i*>::iterator subIt = subMap.begin();
//   for ( ; subIt != subMap.end(); subIt++ ) {
//     SMESH::SMESH_subMesh_ptr aSubMesh = (*subIt).second->_this();
//     if ( !CORBA::is_nil( aSubMesh )) {
//       aShapeObject = aSubMesh->GetSubShape();
//       PublishSubMesh( theStudy, theMesh, aSubMesh, aShapeObject );
//     }
//   }
// 
//   // Publish groups
//   const map<int, SMESH::SMESH_GroupBase_ptr>& grMap = mesh_i->getGroups();
//   map<int, SMESH::SMESH_GroupBase_ptr>::const_iterator it = grMap.begin();
//   for ( ; it != grMap.end(); it++ )
//   {
//     SMESH::SMESH_GroupBase_ptr aGroup = (*it).second;
//     if ( !aGroup->_is_nil() ) {
//       GEOM::GEOM_Object_var  aShapeObj;
//       SMESH::SMESH_GroupOnGeom_var aGeomGroup =
//         SMESH::SMESH_GroupOnGeom::_narrow( aGroup );
//       if ( !aGeomGroup->_is_nil() )
//         aShapeObj = aGeomGroup->GetShape();
//       PublishGroup( theStudy, theMesh, aGroup, aShapeObj );
//     }
//   }

  if(MYDEBUG) MESSAGE("PublishDoc_END");
  return aDocSO._retn();
}

//=======================================================================
//function : PublishSubMesh
//purpose  : 
//=======================================================================

// SALOMEDS::SObject_ptr SMESH_Gen_i::PublishSubMesh (SALOMEDS::Study_ptr      theStudy,
//                                                    SMESH::SMESH_Mesh_ptr    theMesh,
//                                                    SMESH::SMESH_subMesh_ptr theSubMesh,
//                                                    GEOM::GEOM_Object_ptr    theShapeObject,
//                                                    const char*              theName)
// {
//   if (theStudy->_is_nil() || theMesh->_is_nil() ||
//       theSubMesh->_is_nil() || theShapeObject->_is_nil() )
//     return SALOMEDS::SObject::_nil();
// 
//   SALOMEDS::SObject_var aSubMeshSO = ObjectToSObject( theStudy, theSubMesh );
//   if ( aSubMeshSO->_is_nil() )
//   {
//     SALOMEDS::SObject_var aMeshSO = ObjectToSObject( theStudy, theMesh );
//     if ( aMeshSO->_is_nil() ) {
//       aMeshSO = PublishMesh( theStudy, theMesh );
//       if ( aMeshSO->_is_nil())
//         return SALOMEDS::SObject::_nil();
//     }
//     // Find submesh sub-tree tag
//     long aRootTag;
//     const char* aRootName = "";
//     switch ( theShapeObject->GetShapeType() ) {
//     case GEOM::VERTEX:
//       aRootTag  = GetSubMeshOnVertexTag();
//       aRootName = "SubMeshes on Vertex";
//       break;
//     case GEOM::EDGE:
//       aRootTag  = GetSubMeshOnEdgeTag();
//       aRootName = "SubMeshes on Edge";
//       break;
//     case GEOM::WIRE:
//       aRootTag  = GetSubMeshOnWireTag();
//       aRootName = "SubMeshes on Wire";
//       break;
//     case GEOM::FACE:
//       aRootTag  = GetSubMeshOnFaceTag();
//       aRootName = "SubMeshes on Face";    
//       break;
//     case GEOM::SHELL:
//       aRootTag  = GetSubMeshOnShellTag();
//       aRootName = "SubMeshes on Shell";   
//       break;
//     case GEOM::SOLID:
//       aRootTag  = GetSubMeshOnSolidTag();
//       aRootName = "SubMeshes on Solid";
//       break;
//     default:
//       aRootTag  = GetSubMeshOnCompoundTag();
//       aRootName = "SubMeshes on Compound";
//       break;
//     }
// 
//     // Find or create submesh root
//     SALOMEDS::SObject_var aRootSO = publish (theStudy, CORBA::Object::_nil(),
//                                              aMeshSO, aRootTag, 0, false );
//     SetName( aRootSO, aRootName );
// 
//     // Add new submesh to corresponding sub-tree
//     aSubMeshSO = publish (theStudy, theSubMesh, aRootSO, 0, "ICON_SMESH_TREE_MESH_WARN");
//     if ( aSubMeshSO->_is_nil() )
//       return aSubMeshSO._retn();
//   }
//   SetName( aSubMeshSO, theName, "SubMesh" );
// 
//   // Add reference to theShapeObject
// 
//   addReference( theStudy, aSubMeshSO, theShapeObject, 1 );
// 
//   // Publish hypothesis
// 
//   SMESH::ListOfHypothesis * hypList = theMesh->GetHypothesisList( theShapeObject );
//   if ( hypList )
//     for ( int i = 0; i < hypList->length(); i++ ) {
//       SMESH::SMESH_Hypothesis_var aHyp = SMESH::SMESH_Hypothesis::_narrow( (*hypList)[ i ]);
//       PublishHypothesis( theStudy, aHyp );
//       AddHypothesisToShape( theStudy, theMesh, theShapeObject, aHyp );
//     }
// 
//   return aSubMeshSO._retn();
// }

//=======================================================================
//function : PublishGroup
//purpose  : 
//=======================================================================

// SALOMEDS::SObject_ptr SMESH_Gen_i::PublishGroup (SALOMEDS::Study_ptr    theStudy,
//                                                  SMESH::SMESH_Mesh_ptr  theMesh,
//                                                  SMESH::SMESH_GroupBase_ptr theGroup,
//                                                  GEOM::GEOM_Object_ptr  theShapeObject,
//                                                  const char*            theName)
// {
//   if (theStudy->_is_nil() || theMesh->_is_nil() || theGroup->_is_nil() )
//     return SALOMEDS::SObject::_nil();
// 
//   SALOMEDS::SObject_var aGroupSO = ObjectToSObject( theStudy, theGroup );
//   if ( aGroupSO->_is_nil() )
//   {
//     SALOMEDS::SObject_var aMeshSO = ObjectToSObject( theStudy, theMesh );
//     if ( aMeshSO->_is_nil() ) {
//       aMeshSO = PublishInStudy( theStudy, SALOMEDS::SObject::_nil(), theMesh, "");
//       if ( aMeshSO->_is_nil())
//         return SALOMEDS::SObject::_nil();
//     }
//     int aType = (int)theGroup->GetType();
//     const char* aRootNames[] = {
//       "Compound Groups", "Groups of Nodes", "Groups of Edges",
//       "Groups of Faces", "Groups of Volumes", "Groups of 0D Elements" };
// 
//     // Currently, groups with heterogenous content are not supported
//     if ( aType != SMESH::ALL ) {
//       long aRootTag = GetNodeGroupsTag() + aType - 1;
// 
//       // Find or create groups root
//       SALOMEDS::SObject_var aRootSO = publish (theStudy, CORBA::Object::_nil(),
//                                                aMeshSO, aRootTag, 0, false );
//       if ( aType < 6 )
//         SetName( aRootSO, aRootNames[aType] );
// 
//       // Add new group to corresponding sub-tree
//       aGroupSO = publish (theStudy, theGroup, aRootSO, 0, "ICON_SMESH_TREE_GROUP" );
//     }
//     if ( aGroupSO->_is_nil() )
//       return aGroupSO._retn();
//   }
// 
//   SetName( aGroupSO, theName, "Group" );
// 
//   //Add reference to geometry
//   if ( !theShapeObject->_is_nil() )
//     addReference( theStudy, aGroupSO, theShapeObject, 1 );
// 
//   return aGroupSO._retn();
// }

//=======================================================================
//function : PublishHypothesis
//purpose  : 
//=======================================================================

// SALOMEDS::SObject_ptr
//   SMESH_Gen_i::PublishHypothesis (SALOMEDS::Study_ptr         theStudy,
//                                   SMESH::SMESH_Hypothesis_ptr theHyp,
//                                   const char*                 theName)
// {
//   if(MYDEBUG) MESSAGE("PublishHypothesis")
//   if (theStudy->_is_nil() || theHyp->_is_nil())
//     return SALOMEDS::SObject::_nil();
// 
//   SALOMEDS::SObject_var aHypSO = ObjectToSObject( theStudy, theHyp );
//   if ( aHypSO->_is_nil() )
//   {
//     SALOMEDS::SComponent_var father = PublishComponent( theStudy );
//     if ( father->_is_nil() )
//       return aHypSO._retn();
// 
//     //Find or Create Hypothesis root
//     bool isAlgo = ( !SMESH::SMESH_Algo::_narrow( theHyp )->_is_nil() );
//     int aRootTag = isAlgo ? GetAlgorithmsRootTag() : GetHypothesisRootTag();
//     SALOMEDS::SObject_var aRootSO =
//       publish (theStudy, CORBA::Object::_nil(),father, aRootTag,
//                isAlgo ? "ICON_SMESH_TREE_ALGO" : "ICON_SMESH_TREE_HYPO", false);
//     SetName( aRootSO, isAlgo ?  "Algorithms" : "Hypotheses" );
// 
//     // Add New Hypothesis
//     string aPmName = isAlgo ? "ICON_SMESH_TREE_ALGO_" : "ICON_SMESH_TREE_HYPO_";
//     aPmName += theHyp->GetName();
//     // prepend plugin name to pixmap name
//     string pluginName = myHypCreatorMap[string(theHyp->GetName())]->GetModuleName();
//     if ( pluginName != "StdMeshers" )
//       aPmName = pluginName + "::" + aPmName;
//     aHypSO = publish( theStudy, theHyp, aRootSO, 0, aPmName.c_str() );
//   }
// 
//   if ( !aHypSO->_is_nil() ) {
//     CORBA::String_var aHypName = CORBA::string_dup( theHyp->GetName() );
//     SetName( aHypSO, theName, aHypName );
//   }
// 
//   if(MYDEBUG) MESSAGE("PublishHypothesis--END")
//   return aHypSO._retn();
// }

//=======================================================================
//function : GetMeshOrSubmeshByShape
//purpose  : 
//=======================================================================

// SALOMEDS::SObject_ptr
//   SMESH_Gen_i::GetMeshOrSubmeshByShape (SALOMEDS::Study_ptr   theStudy,
//                                         SMESH::SMESH_Mesh_ptr theMesh,
//                                         GEOM::GEOM_Object_ptr theShape)
// {
//   if(MYDEBUG) MESSAGE("GetMeshOrSubmeshByShape")
//   SALOMEDS::SObject_var aMeshOrSubMesh;
//   if (theMesh->_is_nil() || ( theShape->_is_nil() && theMesh->HasShapeToMesh()))
//     return aMeshOrSubMesh._retn();
//   
//   TopoDS_Shape aShape;
//   if(theMesh->HasShapeToMesh())
//     aShape = GeomObjectToShape( theShape );
//   else
//     aShape = SMESH_Mesh::PseudoShape();
// 
//   SMESH_Mesh_i* mesh_i = objectToServant<SMESH_Mesh_i>( theMesh );
// 
//   if ( !aShape.IsNull() && mesh_i && mesh_i->GetImpl().GetMeshDS() ) {
//     SMESHDS_Mesh* meshDS = mesh_i->GetImpl().GetMeshDS();
//     if ( aShape.IsSame( meshDS->ShapeToMesh() ))
//       aMeshOrSubMesh = ObjectToSObject( theStudy, theMesh );
//     else {
//       int shapeID = meshDS->ShapeToIndex( aShape );
//       SMESH::SMESH_subMesh_var aSubMesh = mesh_i->getSubMesh(shapeID);
//       if ( !aSubMesh->_is_nil() )
//         aMeshOrSubMesh = ObjectToSObject( theStudy, aSubMesh );
//     }
//   }
//   if(MYDEBUG) MESSAGE("GetMeshOrSubmeshByShape--END")
//   return aMeshOrSubMesh._retn();
// }

//=======================================================================
//function : AddHypothesisToShape
//purpose  : 
//=======================================================================

// bool SMESH_Gen_i::AddHypothesisToShape(SALOMEDS::Study_ptr         theStudy,
//                                        SMESH::SMESH_Mesh_ptr       theMesh,
//                                        GEOM::GEOM_Object_ptr       theShape,
//                                        SMESH::SMESH_Hypothesis_ptr theHyp)
// {
//   if(MYDEBUG) MESSAGE("AddHypothesisToShape")
//   if (theStudy->_is_nil() || theMesh->_is_nil() ||
//       theHyp->_is_nil() || (theShape->_is_nil()
//                             && theMesh->HasShapeToMesh()) )
//     return false;
// 
//   SALOMEDS::SObject_var aMeshSO = ObjectToSObject( theStudy, theMesh );
//   if ( aMeshSO->_is_nil() )
//     aMeshSO = PublishMesh( theStudy, theMesh );
//   SALOMEDS::SObject_var aHypSO = PublishHypothesis( theStudy, theHyp );
//   if ( aMeshSO->_is_nil() || aHypSO->_is_nil())
//     return false;
// 
//   // Find a mesh or submesh refering to theShape
//   SALOMEDS::SObject_var aMeshOrSubMesh =
//     GetMeshOrSubmeshByShape( theStudy, theMesh, theShape );
//   if ( aMeshOrSubMesh->_is_nil() )
//   {
//     // publish submesh
//     TopoDS_Shape aShape = GeomObjectToShape( theShape );
//     SMESH_Mesh_i* mesh_i = objectToServant<SMESH_Mesh_i>( theMesh );
//     if ( !aShape.IsNull() && mesh_i && mesh_i->GetImpl().GetMeshDS() ) {
//       SMESHDS_Mesh* meshDS = mesh_i->GetImpl().GetMeshDS();
//       int shapeID = meshDS->ShapeToIndex( aShape );
//       SMESH::SMESH_subMesh_var aSubMesh = mesh_i->getSubMesh(shapeID);
//       aMeshOrSubMesh = PublishSubMesh( theStudy, theMesh, aSubMesh, theShape );
//     }
//     if ( aMeshOrSubMesh->_is_nil() )
//       return false;
//   }
// 
//   //Find or Create Applied Hypothesis root
//   bool aIsAlgo = !SMESH::SMESH_Algo::_narrow( theHyp )->_is_nil();
//   SALOMEDS::SObject_var AHR =
//     publish (theStudy, CORBA::Object::_nil(), aMeshOrSubMesh,
//              aIsAlgo ? GetRefOnAppliedAlgorithmsTag() : GetRefOnAppliedHypothesisTag(),
//              aIsAlgo ? "ICON_SMESH_TREE_ALGO" : "ICON_SMESH_TREE_HYPO", false);
//   SetName( AHR, aIsAlgo ? "Applied algorithms" : "Applied hypotheses" );
//   if ( AHR->_is_nil() )
//     return false;
// 
//   addReference( theStudy, AHR, theHyp );
//   if(MYDEBUG) MESSAGE("AddHypothesisToShape--END")
//   return true;
// }

//=======================================================================
//function : RemoveHypothesisFromShape
//purpose  : 
//=======================================================================

// bool SMESH_Gen_i::RemoveHypothesisFromShape(SALOMEDS::Study_ptr         theStudy,
//                                             SMESH::SMESH_Mesh_ptr       theMesh,
//                                             GEOM::GEOM_Object_ptr       theShape,
//                                             SMESH::SMESH_Hypothesis_ptr theHyp)
// {
//   if (theStudy->_is_nil() || theMesh->_is_nil() ||
//       theHyp->_is_nil() || (theShape->_is_nil()
//                             && theMesh->HasShapeToMesh()))
//     return false;
// 
//   SALOMEDS::SObject_var aHypSO = ObjectToSObject( theStudy, theHyp );
//   if ( aHypSO->_is_nil() )
//     return false;
// 
//   // Find a mesh or submesh refering to theShape
//   SALOMEDS::SObject_var aMeshOrSubMesh =
//     GetMeshOrSubmeshByShape( theStudy, theMesh, theShape );
//   if ( aMeshOrSubMesh->_is_nil() )
//     return false;
// 
//   // Find and remove a reference to aHypSO
//   SALOMEDS::SObject_var aRef, anObj;
//   CORBA::String_var     anID = CORBA::string_dup( aHypSO->GetID() );
//   SALOMEDS::ChildIterator_var it = theStudy->NewChildIterator( aMeshOrSubMesh );
//   for ( it->InitEx( true ); it->More(); it->Next() ) {
//     anObj = it->Value();
//     if (anObj->ReferencedObject( aRef ) && strcmp( aRef->GetID(), anID ) == 0 ) {
//       theStudy->NewBuilder()->RemoveObject( anObj );
//       break;
//     }
//   }
//   return true;
// }

//=======================================================================
//function : UpdateParameters
//purpose  : 
//=======================================================================
// void SMESH_Gen_i::UpdateParameters(CORBA::Object_ptr theObject, const char* theParameters)
// {
// 
//   if(VARIABLE_DEBUG)
//     cout<<"UpdateParameters : "<<theParameters<<endl;
//   SALOMEDS::Study_ptr aStudy = GetCurrentStudy();
//   if(aStudy->_is_nil() || CORBA::is_nil(theObject)) 
//     return;
// 
//   SALOMEDS::SObject_var aSObj =  ObjectToSObject(aStudy,theObject);
//   if(aSObj->_is_nil())  
//     return;
// 
//   SALOMEDS::StudyBuilder_var aStudyBuilder = aStudy->NewBuilder();
// 
//   SALOMEDS::GenericAttribute_var aFindAttr;
//   bool hasAttr = aSObj->FindAttribute(aFindAttr, "AttributeString");
//   if(VARIABLE_DEBUG)
//     cout<<"Find Attribute "<<hasAttr<<endl;
// 
//   SALOMEDS::GenericAttribute_var anAttr;
//   anAttr = aStudyBuilder->FindOrCreateAttribute( aSObj, "AttributeString");
//   SALOMEDS::AttributeString_var aStringAttr = SALOMEDS::AttributeString::_narrow(anAttr);
// 
//   CORBA::String_var oldparVar = aStringAttr->Value();
//   CORBA::String_var inpparVar = ParseParameters(theParameters);
//   TCollection_AsciiString aNewParams;
//   TCollection_AsciiString aOldParameters(oldparVar.inout());
//   TCollection_AsciiString anInputParams(inpparVar.inout());
//   if(!hasAttr)
//     aNewParams = anInputParams;
//   else 
//     {
//       int pos = aOldParameters.SearchFromEnd("|");
//       if(pos==-1) pos = 0;
//       TCollection_AsciiString previousParamFull(aOldParameters.Split(pos));
//       TCollection_AsciiString previousParam(previousParamFull);
//       TCollection_AsciiString theRepet("1");
//       pos = previousParam.SearchFromEnd(";*=");
//       if(pos >= 0)
//         {
//           theRepet = previousParam.Split(pos+2);
//           pos = pos-1;
//           if(pos==-1) pos = 0;
//           previousParam.Split(pos);
//         }
//       if(previousParam == anInputParams)
//         {
//           theRepet = theRepet.IntegerValue()+1;
//           aNewParams = aOldParameters + previousParam + ";*=" + theRepet;
//         }
//       else
//         {
//           aNewParams = aOldParameters + previousParamFull + "|" + anInputParams;
//         }
//     }
// 
//   if(VARIABLE_DEBUG)
//   {
//     cout<<"Input Parameters : "<<anInputParams<<endl;
//     cout<<"Old Parameters : "<<aOldParameters<<endl;
//     cout<<"New Parameters : "<<aNewParams<<endl;
//   }
// 
//   aStringAttr->SetValue( aNewParams.ToCString() );
// }

//=======================================================================
//function : ParseParameters
//purpose  : 
//=======================================================================
// char* SMESH_Gen_i::ParseParameters(const char* theParameters)
// {
//   //const char* aParameters = theParameters;
// //   const char* aParameters = CORBA::string_dup(theParameters);
//   TCollection_AsciiString anInputParams;
//   SALOMEDS::Study_var aStudy = GetCurrentStudy();
//   if( !aStudy->_is_nil() ) {
// //     SALOMEDS::ListOfListOfStrings_var aSections = aStudy->ParseVariables(theParameters);
// //     for(int j=0;j<aSections->length();j++) {
// //       SALOMEDS::ListOfStrings aVars= aSections[j];
// //       for(int i=0;i<aVars.length();i++ ) {
// //         anInputParams += aStudy->IsVariable(aVars[i].in()) ? 
// //           TCollection_AsciiString(aVars[i].in()) : TCollection_AsciiString("");
// //         if(i != aVars.length()-1)
// //           anInputParams+=":";
// //       }
// //       if(j!=aSections->length()-1)
// //         anInputParams+="|";
// //     }
//     TCollection_AsciiString paramStr( theParameters );
//     static TCollection_AsciiString separators(":|");
//     int beg = 0, end;
//     char sep, *pParams = (char*)paramStr.ToCString();
//     while ( beg < paramStr.Length() )
//     {
//       end = beg-1;
//       while ( ++end < paramStr.Length() )
//         if ( pParams[end] == ':' || pParams[end] == '|')
//           break;
//       if ( end < paramStr.Length())
//       {
//         sep = pParams[end];
//         pParams[end] = '\0';
//       }
//       if ( aStudy->IsVariable( pParams+beg ))
//         anInputParams += pParams+beg;
//       if ( end < paramStr.Length() )
//         anInputParams += sep;
//       else
//         break;
//       beg = end + 1;
//     }
//   }
//   return CORBA::string_dup(anInputParams.ToCString());
// }

//=======================================================================
//function : GetParameters
//purpose  : 
//=======================================================================
// char* SMESH_Gen_i::GetParameters(CORBA::Object_ptr theObject)
// {
//   TCollection_AsciiString aResult;
// 
//   SALOMEDS::Study_ptr aStudy = GetCurrentStudy();
//   SALOMEDS::SObject_var aSObj =  ObjectToSObject(aStudy,theObject);
// 
//   if(!aStudy->_is_nil() && 
//      !CORBA::is_nil(theObject) && 
//      !aSObj->_is_nil()){
//     
//     SALOMEDS::GenericAttribute_var anAttr;
//     if ( aSObj->FindAttribute(anAttr, "AttributeString")) {
//       aResult = TCollection_AsciiString(SALOMEDS::AttributeString::_narrow(anAttr)->Value());
//     }
//   }
//   
//   return CORBA::string_dup( aResult.ToCString() );
// }
