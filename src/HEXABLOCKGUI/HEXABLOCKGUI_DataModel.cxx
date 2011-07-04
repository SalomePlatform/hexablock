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

//#define _DEVDEBUG_
#include "HEXABLOCKGUI_Trace.hxx"

#include "HEXABLOCKGUI_DataModel.hxx"
#include "HEXABLOCKGUI_DataObject.hxx"

// #include <LightApp_Study.h>
#include <SalomeApp_Study.h>
#include <SalomeApp_Module.h>
#include <SUIT_Tools.h>
#include <SUIT_DataObjectIterator.h>
#include <SUIT_DataBrowser.h>

#include <CAM_Module.h>
#include <CAM_Application.h>

#include <CAM_Module.h>
#include <CAM_Application.h>

// #include <LightApp_Application.h>
#include <SalomeApp_Application.h>

#include <qstring.h>
#include <qfile.h>
#ifndef QT_NO_DOM
#include <qdom.h>
#endif

#include <vector>
#include <string>

// const QString doc_name     = "Atomic";
// const QString doc_tag      = "document";
// const QString molecule_tag = "molecule";
// const QString atom_tag     = "atom";
// const QString name_attr    = "name";
// const QString x_attr       = "x";
// const QString y_attr       = "y";
// const QString z_attr       = "z";

/*! Constructor */
HEXABLOCKGUI_DataModel::HEXABLOCKGUI_DataModel( CAM_Module* module )
//   : LightApp_DataModel( module ),
  : SalomeApp_DataModel( module ),
    myStudyURL( "" )
{
}

/*! Destructor */
HEXABLOCKGUI_DataModel::~HEXABLOCKGUI_DataModel()
{
}


// bool HEXABLOCKGUI_DataModel::createDocument( HEXA_NS::Document* doc, QWidget* viewWindow )
// {
//   HEXABLOCKGUI_Document docModel( doc->getFile(),  reinterpret_cast<intptr_t>(doc) );
//   QString docEntry = QString("HEXA_ENTRY:%1").arg( QString::number( docModel.id() ) );
//   myViews[ docEntry ]   = viewWindow;
//   myEntry[ viewWindow ] = docEntry;
// //   // temporary code to add a few atoms to a molecule..
// //   mol.addAtom( "atom_1", 0, 0, 0 );
// //   mol.addAtom( "atom_2", 0, 0, 0 );
// //   mol.addAtom( "atom_3", 0, 0, 0 );
// //   // end of temporary code
//   myDocuments.append( docModel );
// //   _createDocument ();
// 
//   update();
// 
//   return true;
// }


bool HEXABLOCKGUI_DataModel::createDocument( HEXA_NS::Document* doc, QWidget* viewWindow , QString docIOR )
{
  DEBTRACE("HEXABLOCKGUI_DataModel::createDocument : docIOR =>" << docIOR );
  SalomeApp_ModuleObject* aRoot = dynamic_cast<SalomeApp_ModuleObject*>(root());
  if (!aRoot)
  {
      DEBTRACE("SalomeApp_ModuleObject* aRoot =0");
      return false;
  }
  DEBTRACE("aRoot OK");
  _PTR(SComponent)         aSComp(aRoot->object());
  _PTR(Study)              aStudy = getStudy()->studyDS();
  _PTR(StudyBuilder)       aBuilder(aStudy->NewBuilder());
  _PTR(GenericAttribute)   anAttr;
  _PTR(AttributeName)      aName;
  _PTR(AttributePixMap)    aPixmap;
  _PTR(AttributeParameter) aType;
  _PTR(AttributeIOR)       aIOR;

  // --- create a new document SObject
  _PTR(SObject) aSObj;
  aSObj = aBuilder->NewObject(aSComp);

  anAttr =  aBuilder->FindOrCreateAttribute(aSObj, "AttributeName");
  aName = _PTR(AttributeName)(anAttr);
//   aName->SetValue(QFileInfo(schemaName).baseName().toStdString());
  aName->SetValue("totoooo");

//   anAttr = aBuilder->FindOrCreateAttribute(aSObj, "AttributePixMap");
//   aPixmap = _PTR(AttributePixMap)(anAttr);
//   aPixmap->SetPixMap("schema.png");


  anAttr = aBuilder->FindOrCreateAttribute(aSObj, "AttributeIOR");
  aIOR = _PTR(AttributeIOR)(anAttr);
  aIOR->SetValue(docIOR.toStdString());

//     CORBA::String_var objStr = HEXABLOCK_Gen_i::GetORB()->object_to_string( theIOR );
  

//   anAttr = aBuilder->FindOrCreateAttribute(aSObj, "AttributeParameter");
//   aType = _PTR(AttributeParameter)(anAttr);
//   aType->SetInt("ObjectType", SchemaObject);
//   string filePath = schemaName.toStdString();
//   aType->SetString("FilePath", filePath.c_str());
//   DEBTRACE("DataModel : FilePath = " << filePath);

//   _viewEntryMap[viewWindow] = aSObj->GetID();
//   DEBTRACE(viewWindow);
//   _entryViewMap[aSObj->GetID()] = viewWindow;
//   DEBTRACE("--- " << viewWindow << " "<< aSObj->GetID());
  SalomeApp_Module *mod = dynamic_cast<SalomeApp_Module*>(module());
  if (mod) mod->updateObjBrowser();

//   QxScene_ViewWindow *swv = dynamic_cast<QxScene_ViewWindow*>(viewWindow);
//   if (!swv) return;
//   QString tabName = QFileInfo(schemaName).baseName();
//   swv->getViewManager()->setTitle(tabName);
  return true;
}

/*! Open Data Model.  Build data structure from the given list of files. */
bool HEXABLOCKGUI_DataModel::open( const QString& URL, CAM_Study* study, QStringList listOfFiles )
{
//   LightApp_Study* aDoc = dynamic_cast<LightApp_Study*>( study );
//   if ( !aDoc )
//     return false;
// 
//   LightApp_DataModel::open( URL, aDoc, listOfFiles );
// 
//   // The first list item contains path to a temporary
//   // directory, where the persistent files was placed
//   if ( listOfFiles.count() > 0 ) {
//     QString aTmpDir ( listOfFiles[0] );
// 
//     // This module operates with a single persistent file
//     if ( listOfFiles.size() == 2 ) {
//       myStudyURL = URL;
//       QString aFullPath = SUIT_Tools::addSlash( aTmpDir ) + listOfFiles[1];
//       return importFile( aFullPath, aDoc );
//     }
//   }

  return false;
}

/*! Save Data Model.  Export data structure to temprorary files and return the list of files. */
bool HEXABLOCKGUI_DataModel::save( QStringList& listOfFiles )
{
//   bool isMultiFile = false; // TODO: decide, how to access this parameter
// 
//   LightApp_DataModel::save( listOfFiles );
// 
//   LightApp_Study* study = dynamic_cast<LightApp_Study*>( module()->application()->activeStudy() );
// 
//   QString aTmpDir(study->GetTmpDir( myStudyURL.toLatin1(), isMultiFile ).c_str());
// 
//   QString aFileName = SUIT_Tools::file( myStudyURL, false ) + "_HEXABLOCKGUI.xml";
//   QString aFullPath = aTmpDir + aFileName;
//   bool ok = exportFile( aFullPath );
// 
//   listOfFiles.append( aTmpDir );
//   listOfFiles.append( aFileName );
// 
//   printf( " \n saved to %s\n ", aFullPath.toLatin1().data() );
// 
//   return ok;
  return false;
}

/*! Save Data Model under given name.  Export data structure to temprorary files and return the list of files. */
bool HEXABLOCKGUI_DataModel::saveAs ( const QString& URL, CAM_Study* study, QStringList& listOfFiles )
{
//   myStudyURL = URL;
//   return save( listOfFiles );
  return false;
}

/*! Called on Study closure */
bool HEXABLOCKGUI_DataModel::close()
{
  myStudyURL = "";
  myDocuments.clear();
  return LightApp_DataModel::close();
}

/*! Called on Study creation */
bool HEXABLOCKGUI_DataModel::create( CAM_Study* study )
{
  return true;
}

/*! Default implementation, always returns false so as not to mask study's isModified() */
bool HEXABLOCKGUI_DataModel::isModified() const
{
  return false;
}

/*! Default implementation, always returns true so as not to mask study's isSaved() */
bool HEXABLOCKGUI_DataModel::isSaved() const
{
  return true;
}

/*! Called on update of the structure of Data Objects */
void HEXABLOCKGUI_DataModel::build()
{
//   HEXABLOCKGUI_ModuleObject* modelRoot = dynamic_cast<HEXABLOCKGUI_ModuleObject*>( root() );
//   if( !modelRoot )  {  // root is not set yet
// //     modelRoot = new HEXABLOCKGUI_ModuleObject( 0 );//this, 0 );
//     modelRoot = new HEXABLOCKGUI_ModuleObject( this, 0 );
// //     modelRoot->setDataModel( this );
//     setRoot( modelRoot );
//   }
// 
// //   HEXABLOCKGUI_ModuleObject* modelRoot = dynamic_cast<HEXABLOCKGUI_ModuleObject*>( root() );
// //   if( !modelRoot )  {  // root is not set yet
// //   HEXABLOCKGUI_ModuleObject*  modelRoot = new HEXABLOCKGUI_ModuleObject( this, root()  );
// //     modelRoot->setDataModel( this );
// //     setRoot( modelRoot );
// //   }
// 
// //   LightApp_RootObject* r = dynamic_cast<LightApp_RootObject*>( root() );
// //   r->setStudy( (LightApp_Study*) module()->application()->activeStudy());
// 
// //   create 'molecule' objects under model root object and 'atom' objects under 'molecule'-s
//   for ( int i = 0; i < myDocuments.count(); i++ ){
//     std::cout << " myDocuments => " << i <<std::endl;
//     HEXABLOCKGUI_DataObject* molObj = new HEXABLOCKGUI_DataObject( modelRoot, &myDocuments[i] );
// //     for ( int j = 0; j < myDocuments[ i ].count(); j++ ) {
// //       /*HEXABLOCKGUI_DataObject* atomObj = */new HEXABLOCKGUI_DataObject ( molObj, &myDocuments[i], j );
// //     }
//   }
// //   if ( myDocuments.count()> 0 )
// //     modelRoot->appendChild( new HEXABLOCKGUI_DataObject ( modelRoot, &myDocuments[0] ) );
// 
// // if ( modelRoot )
// //       txt.trimmed().isEmpty() ? new LIGHTGUI_DataObject( generateId(), txt, modelRoot ) : 
// 
//   root()->dump();

}

/*! Loads data from the XML file. */
bool HEXABLOCKGUI_DataModel::importFile( const QString& fileName, CAM_Study* study )
{
  bool res = false;
// 
// #ifndef QT_NO_DOM
// 
//   QFile file( fileName );
//   if ( !file.open( QIODevice::ReadOnly ) )
//     return false;
// 
//   QDomDocument doc;
// 
//   res = doc.setContent( &file );
//   file.close();
// 
//   if ( !res )
//     return false;
// 
//   QDomElement root = doc.documentElement();
//   if ( root.isNull() || root.tagName() != doc_tag )
//     return false;
// 
//   QDomNode molNode = root.firstChild();
//   while ( res && !molNode.isNull() ) {
//     res = molNode.isElement();
//     if ( res ) {
//       QDomElement molElem = molNode.toElement();
//       if ( molElem.tagName() == molecule_tag && molElem.hasAttribute( name_attr ) ) {
// 	HEXABLOCKGUI_AtomicMolecule aMol( molElem.attribute( name_attr ) );
//         QDomNode atomNode = molNode.firstChild();
//         while ( res && !atomNode.isNull() ) {
//           res = atomNode.isElement();
//           if ( res ) {
//             QDomElement atomElem = atomNode.toElement();
//             if ( atomElem.tagName() == atom_tag &&
//                  atomElem.hasAttribute( name_attr ) && 
// 		 atomElem.hasAttribute( x_attr ) &&
// 		 atomElem.hasAttribute( y_attr ) &&
// 		 atomElem.hasAttribute( z_attr ) ) {
// 	      aMol.addAtom( atomElem.attribute( name_attr ),
// 			     atomElem.attribute( x_attr ).toDouble(),
// 			     atomElem.attribute( y_attr ).toDouble(),
// 			     atomElem.attribute( z_attr ).toDouble() );
//             }
//             else
//               res = false;
//           }
// 	  else
// 	    res = atomNode.isComment();
// 
//           atomNode = atomNode.nextSibling();
//         }
// 	myDocuments.append( aMol );
//       }
//       else
//         res = false;
//     }
//     else
//       res = molNode.isComment();
// 
//     molNode = molNode.nextSibling();
//   }
// 
// #endif
// 
  return res;
}

/*! Saves data to the XML file */
bool HEXABLOCKGUI_DataModel::exportFile( const QString& fileName )
{
  bool res = false;

// #ifndef QT_NO_DOM
// 
//   QFile file( fileName );
//   if ( !file.open( QIODevice::WriteOnly ) )
//     return false;
// 
//   QDomDocument doc( doc_name );
//   QDomElement root = doc.createElement( doc_tag );
//   doc.appendChild( root );
// 
//   for ( int i = 0; i < myDocuments.count(); i++ ) {
//     QDomElement molecule = doc.createElement( molecule_tag );
//     molecule.setAttribute( name_attr, myDocuments[ i ].name() );
//     root.appendChild( molecule );
//     for ( int j = 0; j < myDocuments[ i ].count(); j++ ) {
//       QDomElement atom = doc.createElement( atom_tag );
//       atom.setAttribute( name_attr, myDocuments[ i ].atomName( j ) );
//       atom.setAttribute( x_attr, myDocuments[ i ].atomX( j ) );
//       atom.setAttribute( y_attr, myDocuments[ i ].atomY( j ) );
//       atom.setAttribute( z_attr, myDocuments[ i ].atomZ( j ) );
//       molecule.appendChild( atom );
//     }
//   }
// 
//   QString docStr = doc.toString();
//   res = file.write( docStr.toLatin1(), docStr.length() ) == (int)docStr.length();
//   file.close();
// 
// #endif

  return res;
}



void HEXABLOCKGUI_DataModel::_createDocument () // full component
{
// //   DEBTRACE("HEXABLOCKGUI_DataModel::_createDocument : ";
//   SalomeApp_ModuleObject* aRoot = dynamic_cast<SalomeApp_ModuleObject*>(root());
//   if (!aRoot)
//     {
//       DEBTRACE("SalomeApp_ModuleObject* aRoot =0");
//       return;
//     }
//   DEBTRACE("aRoot OK");
//   _PTR(SComponent)         aSComp(aRoot->object());
//   _PTR(Study)              aStudy = getStudy()->studyDS();
//   _PTR(StudyBuilder)       aBuilder(aStudy->NewBuilder());
//   _PTR(GenericAttribute)   anAttr;
//   _PTR(AttributeName)      aName;
//   _PTR(AttributePixMap)    aPixmap;
//   _PTR(AttributeParameter) aType;
// 
//   // --- create a new schema SObject
// 
//   _PTR(SObject) aSObj;
//   aSObj = aBuilder->NewObject(aSComp);
// 
//   anAttr =  aBuilder->FindOrCreateAttribute(aSObj, "AttributeName");
//   aName = _PTR(AttributeName)(anAttr);
// //   aName->SetValue(QFileInfo(schemaName).baseName().toStdString());
//   aName->SetValue("titi");
// 
//   anAttr = aBuilder->FindOrCreateAttribute(aSObj, "AttributePixMap");
//   aPixmap = _PTR(AttributePixMap)(anAttr);
//   aPixmap->SetPixMap("schema.png");
// 
// //   anAttr = aBuilder->FindOrCreateAttribute(aSObj, "AttributeParameter");
// //   aType = _PTR(AttributeParameter)(anAttr);
// //   aType->SetInt("ObjectType", SchemaObject);
// //   string filePath = schemaName.toStdString();
// //   aType->SetString("FilePath", filePath.c_str());
// //   DEBTRACE("DataModel : FilePath = " << filePath);
// 
// //   _viewEntryMap[viewWindow] = aSObj->GetID();
// //   DEBTRACE(viewWindow);
// //   _entryViewMap[aSObj->GetID()] = viewWindow;
// //   DEBTRACE("--- " << viewWindow << " "<< aSObj->GetID());
//   SalomeApp_Module *mod = dynamic_cast<SalomeApp_Module*>(module());
//   if (mod) mod->updateObjBrowser();
// 
// //   QxScene_ViewWindow *swv = dynamic_cast<QxScene_ViewWindow*>(viewWindow);
// //   if (!swv) return;
// //   QString tabName = QFileInfo(schemaName).baseName();
// //   swv->getViewManager()->setTitle(tabName);
// 
//   root()->dump();
}

/*! Adds a new molecule to the data structure */
// bool HEXABLOCKGUI_DataModel::createDocument( HEXA_NS::Document* doc, QWidget* viewWindow )
// {
//   HEXABLOCKGUI_Document docModel( doc->getFile(),  reinterpret_cast<intptr_t>(doc) );
//   QString docEntry = QString("HEXA_ENTRY:%1").arg( QString::number( docModel.id() ) );
//   myViews[ docEntry ]   = viewWindow;
//   myEntry[ viewWindow ] = docEntry;
// //   // temporary code to add a few atoms to a molecule..
// //   mol.addAtom( "atom_1", 0, 0, 0 );
// //   mol.addAtom( "atom_2", 0, 0, 0 );
// //   mol.addAtom( "atom_3", 0, 0, 0 );
// //   // end of temporary code
//   myDocuments.append( docModel );
// //   _createDocument ();
// 
//   update();
// 
//   return true;
// }


/*! Adds a new atom to the given molecule */
// bool HEXABLOCKGUI_DataModel::addAtom( const QString& entry, const QString& atom, 
// 				   const double x, const double y, const double z )
// {
//   HEXABLOCKGUI_DataObject* obj = findMolecule( entry );
//   if ( obj && obj->isMolecule() ) {
//     obj->molecule()->addAtom( atom, x,HEXABLOCKGUI_DataModel::build() y, z );
//   }HEXABLOCKGUI_DataModel::build()
//   return true;
// }


/*! Rename the given (by entry) object */
bool HEXABLOCKGUI_DataModel::renameObj( const QString& entry, const QString& name )
{
  HEXABLOCKGUI_DataObject* obj = findObject( entry );
  if ( obj ) {
//     if ( obj->isMolecule() || obj->isAtom() ) {
//       obj->molecule()->setName( name, obj->atomIndex() );
//       return true;
//     }
    if ( obj->isDocument() ) {
      obj->document()->setName( name );//, obj->docIndex() );
      return true;
    }
  }
  return false;
}

/*! Delete the given objects (list of entries) */
bool HEXABLOCKGUI_DataModel::deleteObjs( const QStringList& entries )
{
  QMap<QString, HEXABLOCKGUI_DataObject*> cmap;
  // first find all molecules
  for ( int i = 0; i < entries.count(); i++ ) {
    HEXABLOCKGUI_DataObject* o = findObject( entries[i] );
    if ( o && o->isDocument() )
      cmap[ entries[i] ] = o;
  }

//   // then find all atoms
//   typedef QList<int> IntList;
//   QMap<HEXABLOCKGUI_DataObject*, IntList> amap;
//   for ( int i = 0; i < entries.count(); i++ ) {
//     HEXABLOCKGUI_DataObject* o = findObject( entries[i] );
//     if ( o && o->isAtom() ) {
//       HEXABLOCKGUI_DataObject* c = dynamic_cast<HEXABLOCKGUI_DataObject*>( o->parent() );
//       if ( !c || cmap.find( c->entry() ) == cmap.end() )
// 	amap[ c ].append( o->atomIndex() );
//     }
//   }

  // now perform deleting
  int removed = 0;
  QMap<QString, HEXABLOCKGUI_DataObject*>::Iterator it;
  for ( it = cmap.begin(); it != cmap.end(); ++it ) {
    for ( int i = 0; i < myDocuments.count(); i++ ) {
//       if ( &myDocuments[ i ] == it.value()->molecule() ) {
      if ( &myDocuments[ i ] == it.value()->document() ) {
	myDocuments.removeAt( i );
	removed++;
	break;
      }
    }
  }
//   QMap<HEXABLOCKGUI_DataObject*, IntList>::Iterator it1;
//   for ( it1 = amap.begin(); it1 != amap.end(); ++it1 ) {
//     IntList indices = it1.value();
//     qSort( indices );
//     for ( int i = indices.count() - 1; i >= 0; i-- ) {
//       it1.key()->molecule()->deleteAtom( indices[i] );
//       removed++;
//     }
//   }
  return removed;
}

/*! Returns the Data Object by entry */
HEXABLOCKGUI_DataObject* HEXABLOCKGUI_DataModel::findObject( const QString& entry )
{
  for ( SUIT_DataObjectIterator it( root(), SUIT_DataObjectIterator::DepthLeft ); it.current(); ++it ) {
    HEXABLOCKGUI_DataObject* obj = dynamic_cast<HEXABLOCKGUI_DataObject*>( it.current() );
    if ( obj && obj->entry() == entry )
      return obj;
  }
  return 0;
}

/*! Returns the Data Object by entry.  If object is an Atom, its parent Molecule is returned. */
HEXABLOCKGUI_DataObject* HEXABLOCKGUI_DataModel::findDocument( const QString& entry )
{
  for ( SUIT_DataObjectIterator it( root(), SUIT_DataObjectIterator::DepthLeft ); it.current(); ++it ) {
    HEXABLOCKGUI_DataObject* obj = dynamic_cast<HEXABLOCKGUI_DataObject*>( it.current() );
    if ( obj && obj->entry() == entry ) {
      if ( obj->isDocument() )
	return obj;
//       else if ( obj->isAtom() )
// 	return dynamic_cast<HEXABLOCKGUI_DataObject*>( obj->parent() );
      else
	return 0;
    }
  }
  return 0;
}

QWidget* HEXABLOCKGUI_DataModel::getViewWindow( const QString& entry )
{
  DEBTRACE("HEXABLOCKGUI_DataModel::getViewWindow "<<entry);
  if ( !myViews.count(entry) ) return 0;
  DEBTRACE("HEXABLOCKGUI_DataModel::found  "<<entry);
  return myViews[entry];
}

void HEXABLOCKGUI_DataModel::setSelected(QWidget* viewWindow)
{
  DEBTRACE("HEXABLOCKGUI_DataModel::setSelected"<<viewWindow);
  if (!myEntry.count(viewWindow)) return;
  DEBTRACE("HEXABLOCKGUI_DataModel::setSelected found "<<viewWindow);
  QString entry = myEntry[viewWindow];
  SUIT_DataObject* item = findDocument(entry);
  if(item) getModule()->getApp()->objectBrowser()->setSelected(item);
}
