
// Class : Ecriture d'un dump python

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

#ifndef __DUMP_STUDY_H_
#define __DUMP_STUDY_H_

#include "hexa_base.hxx"
#include <map>

#if defined WIN32
#pragma warning ( disable: 4251 )
#endif

BEGIN_NAMESPACE_HEXA

#define DumpStart0(p) bool actif=el_root->glob->dump.start(this, p)
#define DumpStart(p,args) bool actif=el_root->glob->dump.start(this, p); if (actif)  el_root->glob->dump << args
#define DumpEnd       el_root->glob->dump.close (actif)
#define DumpReturn(v) el_root->glob->dump.close (actif,v)

#define DumpLock    bool actif=el_root->glob->dump.lock ()
#define DumpRestore            el_root->glob->dump.restore (actif)

class Witness;
class HexaExport DumpStudy
{
public :
    DumpStudy ();
    void setWitness (Witness* temoin)       { witness = temoin ; }

    DumpStudy& operator << (int      val);
    DumpStudy& operator << (double   val);
    DumpStudy& operator << (cpchar   val);
    DumpStudy& operator << (EltBase* elt);
    DumpStudy& operator << (Edges&   elt);
    DumpStudy& operator << (Quads&   elt);
    DumpStudy& operator << (Hexas&   elt);
    DumpStudy& operator << (RealVector& elt);

    bool start (EltBase* obj, cpchar method);
    bool start (cpchar   obj, cpchar method, bool razmess=true);
    void close (bool reactive);
    void close (bool reactive, EltBase* retour);
    void close (bool reactive, int      retour);
    void close (bool reactive, double   retour);

    bool lock ();
    void restore  (bool reactive);
    void getBegin (string& begin);

private :
   cpchar findName   (EltBase* elt);
   void addArgVector (EnumElt type, TabElts& table);
   void addArgument  (cpchar  arg);
   void addArgument  (string& arg) { addArgument (arg.c_str()) ; }
   void declareVectors ();
   void addVector (cpchar name);
   void majVector (cpchar value);
   void closeVector ();

private :
   std::map    <EltBase*, std::string> map_name;
   std::vector <std::string>           tab_declar;

   int    tab_count [EL_MAXI];
   string curr_vector;
   int    nbr_values;

   FILE*    fic_dump;
   string   this_name;
   string   right_part;
   bool     is_open;
   int      nbr_nulls;
   int      nbr_args;
   Witness* witness;
};
END_NAMESPACE_HEXA
#endif
