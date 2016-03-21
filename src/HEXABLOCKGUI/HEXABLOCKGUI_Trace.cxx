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

#include <cstdlib>
#include <iostream>
#include <sstream>

#include "HEXABLOCKGUI_Trace.hxx"


#ifdef WNT
#include <process.h>
#define getpid _getpid
#else                         // Modification INCKA AD
#include<stdio.h>
#include<sys/types.h>
#include <unistd.h>
#endif

namespace HEXABLOCK{
  namespace GUI{
    int traceLevel=0;
  }
}

void AttachDebugger()
{
  if(getenv ("HEXABLOCKDEBUGGER"))
    {
      std::stringstream exec;
      exec << "$HEXABLOCKDEBUGGER " << getpid() << "&";
      std::cerr << exec.str() << std::endl;
      system(exec.str().c_str());
      while(1);
    }
}
