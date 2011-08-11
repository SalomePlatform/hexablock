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

#include <set>


#include <BRepTools.hxx>


// VTK includes
#include <vtkRenderer.h>
#include <vtkActorCollection.h>
#include <vtkUnstructuredGrid.h>



#include <SUIT_Session.h>

#include <SalomeApp_Study.h>
#include <SalomeApp_Application.h>
// #include <SALOME_Actor.h>
// #include <SALOME_ListIO.hxx>
// #include <SALOME_ListIteratorOfListIO.hxx>





// #include <SALOMEconfig.h>
// #include "SALOME_Component_i.hxx"
// #include <omniORB4/CORBA.h>


#include <SVTK_ViewManager.h>
#include <SVTK_ViewModel.h>
#include <SVTK_ViewWindow.h>
#include <SVTK_Prs.h>
#include <SALOME_Actor.h>
#include <VTKViewer_Algorithm.h>


#include "HEXABLOCKGUI_SalomeTools.hxx"


//#define _DEVDEBUG_
using namespace std;
// using namespace HEXABLOCK::GUI;



namespace HEXABLOCK{

  namespace GUI{

SUIT_Study* GetActiveStudy()
{
  SUIT_Application* app = SUIT_Session::session()->activeApplication();
  if (app)
    return app->activeStudy();
  else
    return NULL;
}


_PTR(Study) GetActiveStudyDocument()
{
  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>(GetActiveStudy());
  if (aStudy)
    return aStudy->studyDS();
  else
    return _PTR(Study)();
}


CORBA::Object_var corbaObj( _PTR(SObject) theSO )
{
  std::cout<< "corbaObj( _PTR(SObject) theSO )" << std::endl;
  CORBA::Object_var aCorbaObj = CORBA::Object::_nil();
  if ( theSO ) {
    std::cout<< "theSO" << std::endl;
    std::string aValue = theSO->GetIOR();
    std::cout<< "aValue" << std::endl;
    if (strcmp(aValue.c_str(), "") != 0) {
      CORBA::ORB_ptr anORB = SalomeApp_Application::orb();
      std::cout<< "anORB" << anORB << std::endl;
      aCorbaObj = anORB->string_to_object(aValue.c_str());
      std::cout<< "aCorbaObj" << aCorbaObj << std::endl;
//         anDoc = Document::_narrow(aCorbaObj);
    }
  }
  return aCorbaObj._retn();
}


CORBA::Object_var corbaObj( const Handle(SALOME_InteractiveObject)& theIO )
{
  CORBA::Object_var aCorbaObj = CORBA::Object::_nil();

  if ( !theIO.IsNull() && theIO->hasEntry() ){
    _PTR(Study)   aStudy = GetActiveStudyDocument();
    _PTR(SObject) aSObj  = aStudy->FindObjectID(theIO->getEntry());
    aCorbaObj = corbaObj(aSObj);
  }
  return aCorbaObj._retn();
}

/*
std::string name( _PTR(SObject) theSO ) 
{
  std::cout << "name( _PTR(SObject) theSO )"<< theSO;
  std::string aResName;
  if ( theSO )
  {
    _PTR(GenericAttribute) anAttr;
    _PTR(AttributeName)    aNameAttr;
    if ( theSO->FindAttribute( anAttr, "AttributeName" ) )
    {
      std::cout << "FindAttribute";
      aNameAttr = anAttr;
      std::cout << "aNameAttr = anAttr";
      aResName = aNameAttr->Value().c_str();
    }
  }
  return aResName;
}


std::string name( const std::string& entry )
{
  std::cout << "name( const std::string& entry )"<< entry;
  std::string aResName;

  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );
  if ( appStudy ){
    std::cout << "appStudy => "<< appStudy ;
    _PTR(Study) aStudy = appStudy->studyDS();
    std::cout << "aStudy=> "<< appStudy ;
    _PTR(SObject) obj( aStudy->FindObjectID( entry ) );
    std::cout << "obj=> "<< obj;
    aResName = name( obj );
  }

  return aResName;
}
*/



SALOME_Actor* findActorByEntry( SVTK_ViewWindow *theVtkViewWindow, const char* theEntry)
{
//     SVTK_ViewWindow* aViewWindow = dynamic_cast<SVTK_ViewWindow*>(theWindow);
  SALOME_Actor *foundActor = NULL;
  vtkActor     *aVTKActor  = NULL; 
  Handle(SALOME_InteractiveObject) anIO;
 
  vtkRenderer *aRenderer = theVtkViewWindow->getRenderer();
  VTK::ActorCollectionCopy aCopy(aRenderer->GetActors());
  vtkActorCollection *aCollection = aCopy.GetActors();
  aCollection->InitTraversal();
  while( aVTKActor = aCollection->GetNextActor() ){
//         if ( anAct->IsA("GEOM_Actor") ) std::cout<<"is an actor"<< std::endl;
    foundActor = dynamic_cast<SALOME_Actor*>( aVTKActor );
    if ( foundActor && foundActor->hasIO() ){
        anIO = foundActor->getIO();
        if( anIO->hasEntry() && strcmp(anIO->getEntry(), theEntry) == 0 )
          return foundActor;
    }
  }

  return NULL; // no actor found
}




/*
SVTK_ViewWindow* GetActiveVTKViewWindow()
{
    SVTK_ViewWindow* aVtkView = NULL;
    SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>
        ( SUIT_Session::session()->activeApplication() );
    if (anApp)
      aVtkView = dynamic_cast<SVTK_ViewWindow*>(anApp->desktop()->activeWindow());
    return aVtkView;
}

//       SUIT_ViewManager* aViewManager =
//         anApp->getViewManager(SVTK_Viewer::Type(), createIfNotFound);
//       if (aViewManager) {
//         if (SUIT_ViewWindow* aViewWindow = aViewManager->getActiveView()) {
//           if (SVTK_ViewWindow* aView = dynamic_cast<SVTK_ViewWindow*>(aViewWindow)) {
//             aViewWindow->raise();
//             aViewWindow->setFocus();
//             return aView;
//           }
//         }
//       }


SOCC_ViewWindow* GetActiveOCCViewWindow()
{
  SOCC_ViewWindow* anOccView = NULL;
    SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>
        (SUIT_Session::session()->activeApplication());
    if (anApp)
      anOccView = dynamic_cast<SOCC_ViewWindow*>(anApp->desktop()->activeWindow());


  OCCViewer_ViewWindow* aOCCFrame = dynamic_cast<OCCViewer_ViewWindow*>( anApp->desktop()->activeWindow() );
  std::cout << "aOCCFrame => "<< aOCCFrame;

    return anOccView ;
}


OCCViewer_ViewWindow* GetActiveOCCViewerWindow()
{
  OCCViewer_ViewWindow* aOCCFrame = NULL;
  SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>
    (SUIT_Session::session()->activeApplication());
  if (anApp) aOCCFrame = dynamic_cast<OCCViewer_ViewWindow*>( anApp->desktop()->activeWindow() );
  std::cout << "aOCCFrame => "<< aOCCFrame;
  return aOCCFrame;
}



int GetNameOfSelectedNodes( SVTK_ViewWindow *theWindow,
                            const Handle(SALOME_InteractiveObject)& theIO,
                            QString& theName )
{
    SVTK_Selector* theSelector = theWindow->GetSelector();
    theName = "";
    TColStd_IndexedMapOfInteger aMapIndex;
    theSelector->GetIndex(theIO,aMapIndex);

    for( int i = 1; i <= aMapIndex.Extent(); i++ )
      theName += QString(" %1").arg(aMapIndex(i));

    return aMapIndex.Extent();
}
*/


int GetNameOfSelectedElements( SVTK_ViewWindow *theWindow,
                               const Handle(SALOME_InteractiveObject)& theIO,
                               QString& theName )
{
    SVTK_Selector* theSelector = theWindow->GetSelector();
    theName = "";
    TColStd_IndexedMapOfInteger aMapIndex;
    theSelector->GetIndex(theIO,aMapIndex);

    typedef std::set<int> TIdContainer;
    std::set<int> anIdContainer;
    for( int i = 1; i <= aMapIndex.Extent(); i++)
      anIdContainer.insert(aMapIndex(i));

    std::set<int>::const_iterator anIter = anIdContainer.begin();
    for( ; anIter != anIdContainer.end(); anIter++)
      theName += QString(" %1").arg(*anIter);

    std::cout << "GetNameOfSelectedElements name =>" << theName.toStdString() << std::endl;
    return aMapIndex.Extent();
}

string shape2string( const TopoDS_Shape& aShape )
{
  ostringstream streamShape;
//   string  strShape;
  BRepTools::Write(aShape, streamShape);
//   BRepTools::Write(aShape, strShape);

  return streamShape.str();
}



MyGEOMBase_Helper::MyGEOMBase_Helper( SUIT_Desktop* p ):
GEOMBase_Helper(p)
{
}

MyGEOMBase_Helper::~MyGEOMBase_Helper()
{
}

GEOM_Displayer*  MyGEOMBase_Helper::getDisplayer()
{
  return GEOMBase_Helper::getDisplayer();
}

bool MyGEOMBase_Helper::selectObjects( ObjectList& objects )
{
  return GEOMBase_Helper::selectObjects( objects );
}

void MyGEOMBase_Helper::display( const ObjectList& objects , const bool o )
{
  return GEOMBase_Helper::display( objects, o );
}

void MyGEOMBase_Helper::redisplay( const ObjectList& objList,
                const bool withChildren,
                const bool updateView )
{
  return GEOMBase_Helper::redisplay( objList, withChildren, updateView );
}

void MyGEOMBase_Helper::displayPreview( GEOM::GEOM_Object_ptr object,
                      const bool   append,
                      const bool   activate, 
                      const bool   update,
                      const double lineWidth,
                      const int    displayMode,
                      const int    color )
{
  return GEOMBase_Helper::displayPreview( object,
                                    append,
                                    activate,
                                    update,
                                    lineWidth,
                                    displayMode,
                                    color );
}

void MyGEOMBase_Helper::erasePreview( const bool b )
{
  return GEOMBase_Helper::erasePreview(b);
}

GEOM::GEOM_IOperations_ptr MyGEOMBase_Helper::createOperation()
{
//   return getGeomEngine()->GetITransformOperations(getStudyId());
  return getGeomEngine()->GetIBasicOperations(getStudyId());
}



MyBasicGUI_PointDlg::MyBasicGUI_PointDlg( GeometryGUI* g, QWidget* w, bool b, Qt::WindowFlags f ):
  BasicGUI_PointDlg( g, w, b, f)
{
}

MyBasicGUI_PointDlg::~MyBasicGUI_PointDlg()
{
}

QPushButton* MyBasicGUI_PointDlg::buttonCancel() const
{ 
  return BasicGUI_PointDlg::buttonCancel();
}

QPushButton* MyBasicGUI_PointDlg::buttonOk() const
{ 
  return BasicGUI_PointDlg::buttonOk();
}

QPushButton* MyBasicGUI_PointDlg::buttonApply() const
{ 
  return BasicGUI_PointDlg::buttonApply();
}












} //namespace GUI{

}//namespace HEXABLOCK{





//////////////////////////////////////////////////////////////////////////


//   SUIT_Study* GetActiveStudy();
//   std::string name( _PTR(SObject) theSO );
//   std::string name( const std::string& entry );
//   SVTK_ViewWindow* GetActiveVTKViewWindow();
//   SOCC_ViewWindow* GetActiveOCCViewWindow();
//   OCCViewer_ViewWindow* GetActiveOCCViewerWindow();
//   int GetNameOfSelectedNodes( SVTK_ViewWindow *theWindow,
//                               const Handle(SALOME_InteractiveObject)& theIO,
//                               QString& theName );

//   QString addInStudy   ( GEOM::GEOM_Object_ptr o, const char* theName )
//   {
//     QString res;
// 
//     std::cout << "getStudyId()  => " << getStudyId() << std::endl;
//     std::cout << "getStudy()  => "   << getStudy() << std::endl;
// 
//     openCommand();
//     res = GEOMBase_Helper::addInStudy(o, theName);
//     std::cout << "addInStudy => " << res.toStdString() << std::endl;
//     commitCommand();
//     return res; 
//   }
// 
// 
// // displayPreview( obj, true, activate, false, lineWidth, displayMode, color );
//   void displayPreview( GEOM::GEOM_Object_ptr obj, 
//                                  const bool   append = false, 
//                                  const bool   activate = false, 
//                                  const bool   update = true,
//                                  const double lineWidth = -1, 
//                                  const int    displayMode = -1,
//                                  const int    color  = -1 )
//   {
//     std::cout << "AAAAAAAAAA => "     <<  std::endl;
// //     GEOM::GEOM_ITransformOperations_var anOp =
// //         getGeomEngine()->GetITransformOperations(getStudyId());
// //     GEOM::GEOM_Object_ptr obj2 = anOp->TranslateDXDYDZ (obj, 100, 100, 100);
// 
//     GEOM::GEOM_IBasicOperations_var anOp =
//         getGeomEngine()->GetIBasicOperations(getStudyId());
//       std::cout << "BBBBBBBBBB => "     <<  std::endl;
//     GEOM::GEOM_Object_ptr obj2 = anOp->MakePointXYZ (100, 125, 150);
// 
//     std::cout << "obj2->GetEntry() => "     << obj2->GetEntry()<< std::endl;
//     std::cout << "obj2->GetStudyID() => "   << obj2->GetStudyID()<< std::endl;
//     std::cout << "obj2->GetType() => "      << obj2->GetType()<< std::endl;
//     std::cout << "obj2->GetShapeType() => " << obj2->GetShapeType()<< std::endl;
// 
//     QString res = addInStudy   ( obj2, "trans");
//     std::cout << "trans  => "   << res.toStdString() << std::endl;
// 
//     globalSelection(); // close local contexts, if any
//     localSelection( GEOM::GEOM_Object::_nil(), TopAbs_VERTEX );
//     GEOMBase_Helper::activate( 39);//GEOM_MARKER );
// 
// //     getDisplayer()->SetColor( Quantity_NOC_RED );
// //     getDisplayer()->SetColor( Quantity_NOC_CYAN1 );
// 
//     // set width of displayed shape
// //     getDisplayer()->SetWidth( (lineWidth == -1)?8:lineWidth );
// 
//     GEOMBase_Helper::displayPreview( obj2,
//                                      true, //append,
//                                      true, //activate,
//                                      update,
//                                      8,//lineWidth,
//                                      displayMode,
//                                      Quantity_NOC_CYAN1);//Quantity_NOC_RED);//color );
//     
// 
//   }
// 
//   void erase( GEOM::GEOM_Object_ptr obj, const bool d = true)
//   {
//     GEOMBase_Helper::erase( obj, d);
//   }



// TopoDS_Shape GEOMBase::GetShapeFromIOR(QString IOR)
// {
//   TopoDS_Shape result;
//   if(IOR.trimmed().isEmpty())
//     return result;
// 
//   CORBA::Object_var obj = SalomeApp_Application::orb()->string_to_object(IOR.toLatin1().data());
//   if(CORBA::is_nil(obj))
//     return result;
//   GEOM::GEOM_Object_var GeomObject = GEOM::GEOM_Object::_narrow( obj );
//   if (GeomObject->_is_nil())
//     return result;
// 
//   result = GEOM_Client().GetShape(GeometryGUI::GetGeomGen(), GeomObject);
//   return result;
// }