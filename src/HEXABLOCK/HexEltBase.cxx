
// C++ : Element de base

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
#include "HexEltBase.hxx"
#include "HexDocument.hxx"
#include "HexDiagnostics.hxx"

BEGIN_NAMESPACE_HEXA
// =================================================== Constructeur
EltBase::EltBase (Document* doc, EnumElt type)
{
   el_root   = doc;
   el_type   = type;
   el_id     = 0;

   el_next   = NULL;
   el_status = HOK;
   el_mark   = 0;
   is_associated = false;

   // EL_NONE, EL_VERTEX, EL_EDGE, EL_QUAD, EL_HEXA, EL_REMOVED

   if (el_root==NULL)
      {
      el_name  = "NoValid";
      setError ();
      return;
      }

   el_id    = el_root->doc_nbr_elt [el_type];
   el_root->doc_nbr_elt  [el_type] ++;
   el_root->doc_last_elt [el_type] -> el_next = this;
   el_root->doc_last_elt [el_type] = this;
   el_root->setDeprecated (1);

   char buffer [16];
   el_name = getName (buffer);
}
// =================================================== Destructeur
EltBase::~EltBase ()
{
   //  printf (" delete ");
   //  dump ();
}
// =================================================== Constructeur Bis
// Utilise uniquement dans le constructeur de Document
// Creation d'un premier element fictif pour accelerer les chainages
EltBase::EltBase (EnumElt type)
{
   el_root   = NULL;
   el_type   = type;
   el_id     = -1;

   el_next   = NULL;
   el_status = HOK;
   el_mark   = 0;
}
// =================================================== remove
void EltBase::remove ()
{
   if (el_type == EL_REMOVED)
      return;

   el_root->setDeprecated (2);
   el_type   = EL_REMOVED;
   el_status = HERR;
   int nbp = el_parent.size();
   for (int nro=0 ; nro<nbp ; nro++)
       {
       EltBase* elt = el_parent[nro];
       if (elt != NULL && elt->isHere())
           elt->remove ();
       }
}
// =================================================== suppress
void EltBase::suppress ()
{
   if (el_type == EL_REMOVED)
      return;

   el_root->setDeprecated (2);
   el_type   = EL_REMOVED;
   el_status = HERR;
}
// ========================================================= getName
cpchar EltBase::getName  ()
{
   return el_name.c_str() ;
}
// ========================================================= getName
char* EltBase::getName  (pchar buffer)
{
   return makeName (el_type, el_id, buffer);
}
// ========================================================= makeName
char* EltBase::makeName  (int type, int id, char* name)
{
// EL_NONE, EL_VERTEX, EL_EDGE, EL_QUAD, EL_HEXA, EL_REMOVED
   sprintf (name, "%c%04d", ABR_TYPES[type], id);
   return   name;
}

// ========================================================= printName
void EltBase::printName  (cpchar sep)
{
   char nom[12];

   printf ("%s%s", getName(nom), sep);
}
// ========================================================= dumpRef
void EltBase::dumpRef ()
{
   int  nbp   = el_parent.size();
   bool prems = true;

   for (int nro=0 ; nro<nbp ; nro++)
       {
       if (el_parent[nro]->isHere ())
          {
          if (prems)
              printf ("\t isin ");
          prems = false;
          el_parent[nro]->printName(", ");
          }
       }

   printf ("\n");
}
// ========================================================= setId
void EltBase::setId (int ln)
{
   char buffer [16];
   bool defname = el_name == getName (buffer);

   el_id     = ln;
   int maxid = std::max (el_root->doc_nbr_elt[el_type], ln+1);

   el_root->doc_nbr_elt[el_type] = maxid;
   if (defname)
      el_name = getName (buffer);
}
// ========================================================= makeVarName
char* EltBase::makeVarName (char* nom)
{
   static cpchar PREFIX [EL_MAXI] = {"undef", "ver",   "edge",  "quad",  "hexa",
                                     "vect",  "grid",  "cyl",  "pipe", "group",
                                     "law", "shape", "subsh", "prop",
                                     "doc",  "Xxxx" };
   sprintf (nom, "%s%d", PREFIX[el_type], el_id);
   return   nom;
}
// ========================================================= debug
bool EltBase::debug (int niv)
{
   return el_root != NULL && el_root->getLevel() > niv ;
}
// ========================================================= getNextName
string EltBase::getNextName ()
{
   if (el_root != NULL)
      return el_root->getNextName (el_type);

   char name [16];
   makeName  (el_type, 0, name);
   return string (name);
}
END_NAMESPACE_HEXA

