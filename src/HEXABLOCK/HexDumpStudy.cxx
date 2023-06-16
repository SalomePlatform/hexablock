
// C++ : Dump python

// Copyright (C) 2009-2023  CEA/DEN, EDF R&D
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
#include "HexDumpStudy.hxx"
#include "HexWitness.hxx"
#include "HexEltBase.hxx"
#include "HexDocument.hxx"

BEGIN_NAMESPACE_HEXA

enum {EL_REAL=EL_NONE};

static cpchar 
    tab_name [EL_MAXI] = {"TReals", "TNodes", "TEdges", "TQuads", "THexas", 
                      "TxxxA", "TxxxB",  "TxxxC", "TxxxD", "TxxxE", "TxxxF" };

// =================================================== Constructeur
DumpStudy::DumpStudy ()
{
   map_name [NULL] = "None";
   is_open   = NOT DumpActif;
   nbr_nulls = 0;
   fic_dump  = stdout;
   witness   = NULL;

   for (int nc=0 ; nc<EL_MAXI ; nc++) tab_count [nc] = 0;
   if (is_open)
      return;

   fic_dump = fopen ("tmp_dump.py", "w");
   if (fic_dump==NULL)
       fic_dump = stdout;

   std::string buff;
   cpchar when = get_time (buff);

   fprintf (fic_dump, "\n");
   fprintf (fic_dump, "# Dump generated by HEXABLOCK at %s\n", when);
   fprintf (fic_dump, "\n");
   fprintf (fic_dump, "import hexablock\n");
   fprintf (fic_dump, "\n");

}
// =================================================== start (EltBase*)
bool DumpStudy::start (EltBase* obj, cpchar method)
{
   if (is_open || obj==NULL)
      return false;

   cpchar name = findName (obj);
   return start (name, method);
}
// =================================================== start (name)
bool DumpStudy::start (cpchar obj, cpchar method, bool raz)
{
   if (is_open || obj==NULL)
      return false;

   tab_declar.clear ();

   right_part  = obj;
   right_part += ".";
   right_part += method;
   right_part += " (";

   nbr_args = 0;
   is_open  = true;
   if (raz) 
      witness->raz();
   return is_open;
}
// =================================================== operator << (int)
DumpStudy& DumpStudy::operator << (int val)
{
   if (NOT is_open)
      return *this; 

   char     valeur [20];
   sprintf (valeur, "%d", val);

   addArgument (valeur);
   return *this;
}
// =================================================== operator << (double)
DumpStudy& DumpStudy::operator << (double val)
{
   if (NOT is_open)
      return *this; 

   char valeur [20];
   sprintf (valeur, "%g", val);

   addArgument (valeur);
   return *this;
}
// =================================================== operator << (cpchar)
DumpStudy& DumpStudy::operator << (cpchar val)
{
   if (NOT is_open)
      return *this; 

   std::string valeur ("'");
   valeur += val;
   valeur += "'";

   addArgument (valeur);
   return *this;
}
// =================================================== operator << (elt)
DumpStudy& DumpStudy::operator << (EltBase* elt)
{
   if (NOT is_open)
      return *this; 

   cpchar name = findName (elt);
   addArgument (name);
   return *this;
}
// =================================================== operator << (Quads)
DumpStudy& DumpStudy::operator << (Quads& tab)
{
   if (NOT is_open)
      return *this; 

   TabElts&  tabelt = (TabElts&) tab;
   addArgVector (EL_QUAD, tabelt) ;
   return *this; 
}
// =================================================== operator << (Edges)
DumpStudy& DumpStudy::operator << (Edges& tab)
{
   if (NOT is_open)
      return *this; 

   TabElts&  tabelt = (TabElts&) tab;
   addArgVector (EL_EDGE, tabelt) ;
   return *this; 
}
// =================================================== operator << (Hexas)
DumpStudy& DumpStudy::operator << (Hexas& tab)
{
   if (NOT is_open)
      return *this; 

   TabElts&  tabelt = (TabElts&) tab;
   addArgVector (EL_HEXA, tabelt) ;
   return *this; 
}
// =================================================== operator << (Reals)
DumpStudy& DumpStudy::operator << (RealVector& tab)
{
   if (NOT is_open)
      return *this; 

   char name [20], valeur [30];

   sprintf (name, "%s%d", tab_name[EL_REAL], ++tab_count[EL_REAL]);
   addVector (name);

   int lg = tab.size ();
   for (int nv=0 ; nv<lg ; nv++)
       { 
       sprintf   (valeur, "%g", tab[nv]);
       majVector (valeur);
       } 

   closeVector ();
   return *this; 
}
// =================================================== close + return
void DumpStudy::close (bool reactive, EltBase* result)
{
   if (reactive)
      is_open = false;
   else
      return;

   char name [32];
   if (result==NULL)
      {
      nbr_nulls ++;
      sprintf (name, "null%03d", nbr_nulls); 
      }
   else
      {
      map_name [result] = result->makeVarName (name);
      }

   declareVectors ();
   right_part += ")";
   fprintf (fic_dump, "%s = %s\n", name, right_part.c_str());
}
// =================================================== close + return int
void DumpStudy::close (bool reactive, int result)
{
   static int nbr_values = 0;
   if (reactive)
      is_open = false;
   else
      return;

   char name [32];
   nbr_values ++;
   sprintf (name, "rep%03d", nbr_values); 

   declareVectors ();
   right_part += ")";
   fprintf (fic_dump, "%s = %s\n", name, right_part.c_str());
}
// =================================================== close + return double
void DumpStudy::close (bool reactive, double result)
{
   static int nbr_values = 0;
   if (reactive)
      is_open = false;
   else
      return;

   char name [32];
   nbr_values ++;
   sprintf (name, "val%03d", nbr_values); 

   declareVectors ();
   right_part += ")";
   fprintf (fic_dump, "%s = %s\n", name, right_part.c_str());
}
// =================================================== close 
void DumpStudy::close (bool reactive)
{
   if (reactive)
      is_open = false;
   else
      return;

   declareVectors ();
   right_part += ")";
   fprintf (fic_dump, "%s\n", right_part.c_str());
}
// =================================================== lock
bool DumpStudy::lock ()
{
   if (is_open)
      return false;

   is_open = true;
   return is_open;
}
// =================================================== restore 
void DumpStudy::restore (bool reactive)
{
   if (reactive)
      is_open = false;
}
// =================================================== getBegin 
void DumpStudy::getBegin (std::string& begin)
{
   begin  = right_part;
   begin += ")";
}
// -------------------------------------------------------------
// ------------ Private
// ------------ For members only
// -------------------------------------------------------------
// =================================================== addArgument
void DumpStudy::addArgument (cpchar arg)
{
   nbr_args ++;
   if (nbr_args>1)
       right_part += ", ";

   right_part += arg;
}
// =================================================== addArgVector
void DumpStudy::addArgVector (EnumElt type, TabElts& table)
{
   char name [20];
   sprintf (name, "%s%d", tab_name[type], ++tab_count[type]);

   addVector (name);

   int lg = table.size ();

   for (int nv=0 ; nv<lg ; nv++)
       { 
       EltBase* elt = table[nv];
       cpchar nom = findName (elt);
       majVector (nom);
       } 

   closeVector ();
}
// =================================================== declareVectors
void DumpStudy::declareVectors ()
{
   int lg = tab_declar.size();
   if (lg==0)
      return;

   for (int nd=0 ; nd<lg ; nd++)
       {
       fprintf (fic_dump, "%s\n", tab_declar[nd].c_str());
       }
}
// =================================================== addVector
void DumpStudy::addVector (cpchar name)
{
   addArgument (name);

   nbr_values   = 0;
   curr_vector  = name;
   curr_vector += " = ";
}
// =================================================== majVector
void DumpStudy::majVector (cpchar value)
{
   if (nbr_values == 0)
       curr_vector += "[";
   else if ((nbr_values MODULO 7) == 0)
       curr_vector += ",\n           ";
   else
       curr_vector += ", ";

   curr_vector += value;
   nbr_values ++;
}
// =================================================== closeVector
void DumpStudy::closeVector ()
{
   curr_vector += "]";
   tab_declar.push_back (curr_vector);
}
// =================================================== findName
cpchar DumpStudy::findName (EltBase* elt)
{
   cpchar name = "Unknown";
   std::map <EltBase*, std::string> :: iterator iter = map_name.find (elt);
   if (iter != map_name.end())
      name = iter->second.c_str();
   else
      name = "Unknown";

   return name;
}
END_NAMESPACE_HEXA
