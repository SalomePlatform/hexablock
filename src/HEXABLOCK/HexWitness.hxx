
// Class : Enregistrement des dianostics

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

#ifndef __WITNESS_H_
#define __WITNESS_H_

#define  Mess   el_root->glob->mess << Begin
#define  Follow el_root->glob->mess

#include "hexa_base.hxx"

#if defined WIN32
#pragma warning ( disable: 4251 )
#endif

BEGIN_NAMESPACE_HEXA

const std::string Begin (" **** ");

class DumpStudy;
class HexaExport Witness
{
public :
   Witness ();
   void     setDump (DumpStudy* dd)      { dump_study = dd                ; }
   void     raz ()                       { mess_state = NoMessage         ; }
   bool     hasMessage ()                { return mess_state != NoMessage ; }
   TabText& getMessage ();
   int      sizeofMessage  ();
   cpchar   getLine (int nro);
   void     printMessage ();

   Witness& operator << (int      val);
   Witness& operator << (double   val);
   Witness& operator << (cpchar   val);
   Witness& operator << (rcstring val);
   Witness& operator << (EltBase* val);

private :
   void openMessage  ();
   void closeMessage () {};
   void pushLine     ();

private :
   TabText      bla_bla;
   std::string  curr_line; 
   int          w_status;
   int          nbr_lines;
   enum EnumMessage { NoMessage, MessOpen, MessClosed }  mess_state;
   bool         line_open;
   DumpStudy*   dump_study;
};
END_NAMESPACE_HEXA
#endif
