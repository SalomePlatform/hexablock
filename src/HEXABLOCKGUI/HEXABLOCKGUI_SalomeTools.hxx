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


#include "GEOM_Client.hxx"
// #include "GEOMBase_Helper.h"
#include "GEOM_Displayer.h"



#include <TopoDS_Shape.hxx>
#include <SALOME_Actor.h>
#include <SVTK_ViewWindow.h>



namespace HEXABLOCK
{
  namespace GUI
  {

  SALOME_Actor* findActorByEntry( SVTK_ViewWindow *theVtkViewWindow, const char* theEntry );
  _PTR(Study)   GetActiveStudyDocument();
  CORBA::Object_var corbaObj( _PTR(SObject) theSO );
  CORBA::Object_var corbaObj( const Handle(SALOME_InteractiveObject)& theIO );
  int GetNameOfSelectedElements( SVTK_ViewWindow *theWindow,/* SVTK_Selector* theSelector,*/
                                 const Handle(SALOME_InteractiveObject)& theIO,
                                 QString& theName );
  std::string shape2string( const TopoDS_Shape& aShape );


  
  class MyGEOM_Displayer : public GEOM_Displayer
  {
    public:
      MyGEOM_Displayer( SalomeApp_Study* app );
      virtual ~MyGEOM_Displayer();


    SALOME_Prs* BuildPrs( GEOM::GEOM_Object_ptr theObj );

//   int aPrevDispMode = getDisplayer()->SetDisplayMode( displayMode );
//   getDisplayer()->SetToActivate( activate );
//   getDisplayer()->SetName( objStr.in() );
//   SALOME_Prs* aPrs = getDisplayer()->BuildPrs( object );




  };






  }
}

#endif
