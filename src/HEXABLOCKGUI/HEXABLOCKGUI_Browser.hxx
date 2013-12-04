// Copyright (C) 2009-2013  CEA/DEN, EDF R&D
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

#ifndef _HEXABLOCKGUI_BROWSER_HXX_
#define _HEXABLOCKGUI_BROWSER_HXX_

//--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8

#include "hexa_base.hxx"
#include "HEXABLOCKGUI_Export.hxx"

using namespace Hex;

class HEXABLOCKGUI;
class SalModel;

class HEXABLOCK_EXPORT HEXABLOCKGUI_Browser 
{
public :
    HEXABLOCKGUI_Browser (HEXABLOCKGUI* dad);
    void rebuildTree ();


private :
    Hex::Hex*     hexa_root;
    SalModel*     data_model; 
    HEXABLOCKGUI* hexa_gui; 
};
#endif
