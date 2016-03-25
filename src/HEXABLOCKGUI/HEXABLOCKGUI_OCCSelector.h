// Copyright (C) 2013-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
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

// GEOM GEOMGUI : GUI for Geometry component
// File   : HEXABLOCKGUI_OCCSelector.h
//

#ifndef HEXABLOCKGUI_OCCSELECTOR_H
#define HEXABLOCKGUI_OCCSELECTOR_H


#include <LightApp_OCCSelector.h>
#include "hexa_base.hxx"
#include "HEXABLOCKGUI_Export.hxx"

class HEXABLOCK_EXPORT HEXABLOCKGUI_OCCSelector : public LightApp_OCCSelector
{
public:
    HEXABLOCKGUI_OCCSelector( OCCViewer_Viewer*, SUIT_SelectionMgr* );
    virtual ~HEXABLOCKGUI_OCCSelector();

protected:
    virtual void      getSelection( SUIT_DataOwnerPtrList& ) const;
    virtual void      setSelection( const SUIT_DataOwnerPtrList& );

};

#endif // HEXABLOCKGUI_OCCSELECTOR_H
