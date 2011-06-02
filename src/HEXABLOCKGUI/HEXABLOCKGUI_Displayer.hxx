// Copyright (C) 2009-2011  CEA/DEN, EDF R&D
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

// HEXABLOCK HEXABLOCKGUI : Displayer for HEXABLOCK module
// File   : HEXABLOCKGUI_Displayer.h
// Author : C-S
//
#ifndef HEXABLOCKGUI_DISPLAYER_H
#define HEXABLOCKGUI_DISPLAYER_H

// HEXABLOCK includes
//#include "HEXABLOCK_HEXABLOCKGUI.hxx"
#include "HEXABLOCKGUI_Export.hxx"


// SALOME GUI includes
#include <LightApp_Displayer.h>

class SalomeApp_Application;
class SalomeApp_Study;


namespace HEXABLOCK
{
    namespace GUI
    {
        class DocumentGraphicView;
        
        class HEXABLOCKGUI_EXPORT HEXABLOCKGUI_Displayer : public LightApp_Displayer
        {
        public:
            HEXABLOCKGUI_Displayer( SalomeApp_Application*, DocumentGraphicView* graphicView);
            ~HEXABLOCKGUI_Displayer();

            virtual SALOME_Prs*      buildPresentation( const QString&, SALOME_View* = 0 );
            virtual bool             canBeDisplayed( const QString&, const QString& ) const;

        protected:
            SalomeApp_Study*         study() const;

        private:
            SalomeApp_Application*   myApp;
            DocumentGraphicView* myGraphicView;

        };

    }
}
#endif // HEXABLOCKGUI_DISPLAYER_H
