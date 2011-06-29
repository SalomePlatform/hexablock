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

// HEXABLOCK HEXABLOCKGUI : Displayer for HEXABLOCK module
// File   : HEXABLOCKGUI_Displayer.cxx
// Author : C-S
// HEXABLOCK includes
//

#include <sstream>

#include "HEXABLOCKGUI_Displayer.hxx"
#include "HEXABLOCKGUI_DocumentGraphicView.hxx"


// SALOME GUI includes
#include <SalomeApp_Study.h>
#include <SalomeApp_Application.h>
#include <SUIT_ViewManager.h>
#include <SVTK_ViewModel.h>
#include <SVTK_ViewWindow.h>
#include <SVTK_Prs.h>
#include <SALOME_Actor.h>

// VTK includes
#include <vtkIdList.h>
#include <vtkCellArray.h>
#include <vtkUnsignedCharArray.h>
#include <vtkUnstructuredGrid.h>
#include <vtkDataSetMapper.h>
#include <vtkProperty.h>
#include <vtkPoints.h>


using namespace HEXABLOCK::GUI;

HEXABLOCKGUI_Displayer::HEXABLOCKGUI_Displayer(SalomeApp_Application* app, DocumentGraphicView* graphicView )
: LightApp_Displayer(),
  myApp( app )
{
    myGraphicView = graphicView;
}

HEXABLOCKGUI_Displayer::~HEXABLOCKGUI_Displayer()
{
    myApp = NULL;
    myGraphicView = NULL;
}

SALOME_Prs* HEXABLOCKGUI_Displayer::buildPresentation( const QString& entry, SALOME_View* theViewFrame)
{
    SALOME_Prs* prs = 0;

    SALOME_View* aViewFrame = theViewFrame ? theViewFrame : GetActiveView();

    if ( aViewFrame )
    {
        SVTK_Viewer* vtk_viewer = dynamic_cast<SVTK_Viewer*>( aViewFrame );
        if (vtk_viewer)
        {
            SUIT_ViewWindow* wnd = vtk_viewer->getViewManager()->getActiveView();
            SALOME_Actor* anActor = myGraphicView->FindActorByEntry( wnd, entry.toLatin1().data() );
            if (!anActor)
            {
//                anActor = myGraphicView->CreateActor( study()->studyDS(), entry.toLatin1().data(), true );
                anActor = myGraphicView->CreateActor(entry.toLatin1().data());
            }
            if (anActor)
            {
                // Display actor :
                SVTK_ViewWindow* vtkWnd = dynamic_cast<SVTK_ViewWindow*> (wnd);
                if (vtkWnd != NULL)
                {
                    vtkWnd->AddActor(anActor);
                    vtkWnd->Repaint();
                    prs = LightApp_Displayer::buildPresentation(entry.toLatin1().data(), aViewFrame);
                }
            }
            if (prs)
            {
                UpdatePrs(prs);
            }
            else if (anActor)
            {
                //SMESH::RemoveActor( vtk_viewer->getViewManager()->getActiveView(), anActor );
                std::cout << "Remove Actor" << std::endl;
            }
        }
    }

    return prs;
}

SalomeApp_Study* HEXABLOCKGUI_Displayer::study() const
{
  return dynamic_cast<SalomeApp_Study*>( myApp->activeStudy() );
}

bool HEXABLOCKGUI_Displayer::canBeDisplayed( const QString& /*entry*/, const QString& viewer_type ) const
{
  return viewer_type==SVTK_Viewer::Type();
}
