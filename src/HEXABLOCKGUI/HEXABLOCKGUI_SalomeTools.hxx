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

#ifndef _HEXABLOCKGUI_SALOMETOOLS_HXX_
#define _HEXABLOCKGUI_SALOMETOOLS_HXX_


#include <SalomeApp_Application.h>
#include <SALOME_Actor.h>
#include <SALOME_ListIO.hxx>
#include <SALOME_ListIteratorOfListIO.hxx>

#include "BasicGUI_PointDlg.h"
#include "GEOMBase_Helper.h"

#include <TopoDS_Shape.hxx>
#include <SALOME_Actor.h>
#include <SVTK_ViewWindow.h>



namespace HEXABLOCK
{
  namespace GUI
  {
  // Sélection v,e,f GEOM ( entry )
  // Highlight v,e,f GEOM ( entry )
  // Création pt GEOM from ( Brep, Par ) + option: add to Study; 
  // creation vertex sur edge
  //                 sur face

  SALOME_Actor* findActorByEntry( SVTK_ViewWindow *theVtkViewWindow, const char* theEntry );
  _PTR(Study)   GetActiveStudyDocument();
  CORBA::Object_var corbaObj( _PTR(SObject) theSO );
  CORBA::Object_var corbaObj( const Handle(SALOME_InteractiveObject)& theIO );
  int GetNameOfSelectedElements( SVTK_ViewWindow *theWindow,/* SVTK_Selector* theSelector,*/
                                 const Handle(SALOME_InteractiveObject)& theIO,
                                 QString& theName );
  std::string shape2string( const TopoDS_Shape& aShape );




  class MyGEOMBase_Helper : public GEOMBase_Helper
  {
    public:
      MyGEOMBase_Helper( SUIT_Desktop* p );
      virtual ~MyGEOMBase_Helper();

      GEOM_Displayer*  getDisplayer();
      bool selectObjects( ObjectList& objects );
      void display( const ObjectList& objects , const bool o = true );
      void redisplay( const ObjectList& objList,
                      const bool withChildren = true,
                      const bool updateView = true );
      void displayPreview(  GEOM::GEOM_Object_ptr object,
                            const bool   append = false, 
                            const bool   activate = false, 
                            const bool   update = true,
                            const double lineWidth = -1, 
                            const int    displayMode = -1,
                            const int    color  = -1 );
      void erasePreview( const bool b = true );
      GEOM::GEOM_IOperations_ptr createOperation();
  };



  class MyBasicGUI_PointDlg : public BasicGUI_PointDlg
  {
    public:
      MyBasicGUI_PointDlg( GeometryGUI* g, QWidget* w = 0, bool b= false, Qt::WindowFlags f= 0 );
      virtual ~MyBasicGUI_PointDlg();
  
      QPushButton* buttonCancel() const;
      QPushButton* buttonOk() const;
      QPushButton* buttonApply() const;
    };



  }
}

#endif
