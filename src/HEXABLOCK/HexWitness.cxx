
// C++ : Dump python

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
#include "HexWitness.hxx"
#include "HexDumpStudy.hxx"
#include "HexEltBase.hxx"
#include "HexDocument.hxx"

BEGIN_NAMESPACE_HEXA

// =================================================== Constructeur
Witness::Witness ()
{
   mess_state = NoMessage; 
   dump_study = NULL; 
   line_open  = false;;
   nbr_lines  = 0;
}
// =================================================== operator << (int)
Witness& Witness::operator << (int val)
{
   openMessage ();

   char     valeur [20];
   sprintf (valeur, "%d", val);

   curr_line += valeur;
   line_open  = true;
   return *this;
}
// =================================================== operator << (double)
Witness& Witness::operator << (double val)
{
   openMessage ();

   char valeur [20];
   sprintf (valeur, "%g", val);

   curr_line += valeur;
   line_open  = true;
   return *this;
}
// =================================================== operator << (cpchar)
Witness& Witness::operator << (cpchar val)
{
   openMessage ();
   curr_line += val;
   line_open  = true;
   return *this;
}
// =================================================== operator << (string)
Witness& Witness::operator << (rcstring val)
{
   openMessage ();
   if (val==Begin)
       pushLine ();

   curr_line += val;
   line_open  = true;
   return *this;
}
// =================================================== operator << (elt)
Witness& Witness::operator << (EltBase* elt)
{
   openMessage ();
   cpchar name = elt==NULL ? "<null>" : elt->getName();
   curr_line += name;
   return *this;
}
// =================================================== printMessage
void Witness::printMessage ()
{
   if (mess_state==NoMessage)
       return;

   if (line_open) 
       pushLine ();

   int nbre = bla_bla.size();
   if (nbre ==0)
       return;

   cout << endl 
        << " ... In function :" << endl;
   cout << bla_bla [0] << endl << endl;
   for (int nro=1 ; nro<nbre ; nro++)
       cout << bla_bla [nro] << endl;
   cout << endl;
}
// =================================================== getMessage
TabText& Witness::getMessage ()
{
   if (line_open) 
       pushLine ();

   return bla_bla;
}
// =================================================== sizeofMessage
int Witness::sizeofMessage ()
{
   if (line_open) 
       pushLine ();

   return bla_bla.size();
}
// =================================================== getLine
cpchar Witness::getLine (int nro)
{
   if (line_open) 
       pushLine ();

   if (nro < 0 || nro >= nbr_lines)
      return "";

   return bla_bla[nro].c_str();
}

// -------------------------------------------------------------
// ------------ Private
// ------------ For members only
// -------------------------------------------------------------
// =================================================== openMessage 
void Witness::openMessage  ()
{
   if (mess_state!=NoMessage)
       return;

   mess_state = MessOpen;
   bla_bla.clear ();
   if (dump_study == NULL)
      {
      curr_line  = "";
      return;
      }

   nbr_lines = 0;
   dump_study->getBegin (curr_line);
   curr_line += " :" ;
}
// =================================================== pushLine  
void Witness::pushLine  ()
{
   if (mess_state != MessOpen)
      {
      nbr_lines = 0;
      return;
      }

   bla_bla.push_back (curr_line);
   curr_line = "";
   line_open = false;
   nbr_lines = bla_bla.size ();
}
END_NAMESPACE_HEXA
