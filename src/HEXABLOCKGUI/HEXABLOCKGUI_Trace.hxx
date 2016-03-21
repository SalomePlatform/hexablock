// Copyright (C) 2009-2016  CEA/DEN, EDF R&D
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

#ifndef __HEXABLOCKGUI_TRACE_HXX__
#define __HEXABLOCKGUI_TRACE_HXX__

#include <iostream>
#include <sstream>

#include "HEXABLOCKGUI_Export.hxx"
#include "HEXABLOCKGUI_Exception.hxx"

#ifdef _DEVDEBUG_
#define DEBTRACE(msg) {std::cerr<<std::flush<<__FILE__<<" ["<<__LINE__<<"] : "<<msg<<std::endl<<std::flush;}
#else
#define DEBTRACE(msg)
#endif

namespace HEXABLOCK {
  namespace GUI {
    extern HEXABLOCK_EXPORT int traceLevel;
  }
}

//! HEXABLOCKTRACE macro for dynamic trace: print only if HEXABLOCK_TRACELEVEL environment variable is set and level is less than  its value
#define HEXABLOCKTRACE(level,msg) {if(HEXABLOCK::GUI::traceLevel >=level)std::cerr<<__FILE__<<" ["<<__LINE__<<"] : "<<msg<<std::endl;}

//! HASSERT macro is always defined, used like assert, but throw a HEXABLOCKGUI::GUI::Exception instead of abort

#define HASSERT(val) {if(!(val)){std::stringstream mess; mess<<__FILE__<<" ["<<__LINE__<<"] : assertion "<<#val<<" failed"; throw HEXABLOCK::GUI::Exception(mess.str());}}

void AttachDebugger();

#endif
