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


#include "HEXABLOCK.hxx"

#include "SALOME_NamingService.hxx"

#include CORBA_CLIENT_HEADER(SALOME_ModuleCatalog)

#include "utilities.h"
#include "Utils_ExceptHandlers.hxx"

#include <TCollection_AsciiString.hxx>

static int MYDEBUG = 1;


using namespace HEXABLOCK_ORB;

//=============================================================================
/*!
 *  SMESH_Gen_i::CanPublishInStudy
 *
 *  Returns true if object can be published in the study
 */
//=============================================================================
bool HEXABLOCK_Gen_i::CanPublishInStudy(CORBA::Object_ptr theIOR)
{
  if(CORBA::is_nil(myCurrentStudy))
    return false;
  
  Document_var aDoc       = Document::_narrow(theIOR);
  if( !aDoc->_is_nil() )
    return true;


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

  return SO._retn();
}


//=======================================================================
//function : setName
//purpose  : 
//=======================================================================
std::string HEXABLOCK_Gen_i::SetName( SALOMEDS::SObject_ptr theSObject,
                         const char*           theName,
                         const char*           theDefaultName )
{
  std::string attrName;
  if (theSObject->_is_nil() ) 
     return attrName;
     
  SALOMEDS::StudyBuilder_var aStudyBuilder = theSObject->GetStudy()->NewBuilder();
  SALOMEDS::GenericAttribute_var anAttr =
      aStudyBuilder->FindOrCreateAttribute( theSObject, "AttributeName" );
  SALOMEDS::AttributeName_var aNameAttr = SALOMEDS::AttributeName::_narrow( anAttr );
    if ( theName && strlen( theName ) != 0 )
      attrName = theName;
    else {
      CORBA::String_var curName = CORBA::string_dup( aNameAttr->Value() );
      if ( strlen( curName ) == 0 ) {
        TCollection_AsciiString aName( (char*) theDefaultName );
        attrName = aName.ToCString();
      }
    }

  aNameAttr->SetValue( attrName.c_str() );
  return attrName;
}

// =======================================================================
// function : PublishComponent
// purpose  : 
// =======================================================================
SALOMEDS::SComponent_ptr HEXABLOCK_Gen_i::PublishComponent(SALOMEDS::Study_ptr theStudy)
{
  if ( CORBA::is_nil( theStudy ))
    return SALOMEDS::SComponent::_nil();

  SALOMEDS::SComponent_var father =
    SALOMEDS::SComponent::_narrow( theStudy->FindComponent( ComponentDataType() ) );
  if ( !CORBA::is_nil( father ) )
    return father._retn();

  SALOME_ModuleCatalog::ModuleCatalog_var aCat =
    SALOME_ModuleCatalog::ModuleCatalog::_narrow( GetNS()->Resolve("/Kernel/ModulCatalog") );
  if ( CORBA::is_nil( aCat ) )
    return father._retn();

  SALOME_ModuleCatalog::Acomponent_var aComp = aCat->GetComponent( ComponentDataType() );
  if ( CORBA::is_nil( aComp ) )
    return father._retn();

  SALOMEDS::StudyBuilder_var     aStudyBuilder = theStudy->NewBuilder(); 
  SALOMEDS::GenericAttribute_var anAttr;
  SALOMEDS::AttributePixMap_var  aPixmap;

  father  = aStudyBuilder->NewComponent( ComponentDataType() );
  aStudyBuilder->DefineComponentInstance( father, HEXABLOCK_Gen_i::_this() );


  anAttr  = aStudyBuilder->FindOrCreateAttribute( father, "AttributePixMap" );
  aPixmap = SALOMEDS::AttributePixMap::_narrow( anAttr );
  aPixmap ->SetPixMap( "ICON_OBJBROWSER_HEXABLOCK" );
  SetName( father, aComp->componentusername(), "HEXABLOCK" );

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

  // find or publish a mesh

  SALOMEDS::SObject_var aDocSO = ObjectToSObject( theStudy, theDoc );
  if ( aDocSO->_is_nil() )
  {
    SALOMEDS::SComponent_var father = PublishComponent( theStudy );
    if ( father->_is_nil() )
      return aDocSO._retn();

    // Find correct free tag
    long aTag = findMaxChildTag( father.in() );
    aTag++;

    aDocSO = publish (theStudy, theDoc, father, aTag);
    if ( aDocSO->_is_nil() )
      return aDocSO._retn();
  }
  std::string docName = SetName( aDocSO, theName, theDoc->getName()); // Bph 

  // Add shape reference

  return aDocSO._retn();
}

