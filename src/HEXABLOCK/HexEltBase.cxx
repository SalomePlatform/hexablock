
// C++ : Element de base 

//  Copyright (C) 2009-2011  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
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
   el_id     = el_root->doc_nbr_elt [type];

   el_next   = NULL;
   el_assoc  = NULL;
   el_status = HOK;
   el_mark   = 0;

   el_root->doc_nbr_elt  [type] ++;
   el_root->doc_last_elt [type] -> el_next = this;
   el_root->doc_last_elt [type] = this;
   el_root->setDeprecated (1);

   // EL_NONE, EL_VERTEX, EL_EDGE, EL_QUAD, EL_HEXA, EL_REMOVED
   char buffer [16];
   sprintf (buffer, "%c%04d", ABR_TYPES[el_type], el_id);
   el_name = buffer;
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
   el_type = EL_REMOVED;
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
   el_type = EL_REMOVED;
}
// ========================================================= replaceAssociation 
void EltBase::replaceAssociation (EltBase* orig)
{
   if (   orig == NULL || orig->el_assoc == NULL
       || orig == this || orig->el_assoc == el_assoc)
      return;

   if (el_assoc==NULL)
      el_assoc = orig->el_assoc;
   else 
      el_root->hputError (W_REPL_ASSOC, this, orig);
}
// ========================================================= copyAssociation 
void EltBase::copyAssociation (EltBase* orig)
{
   if (   orig == NULL || orig->el_assoc == NULL
       || orig == this || orig->el_assoc == el_assoc)
      return;

   el_assoc = orig->el_assoc;
   el_root->hputError (W_DISCO_ASSOC, orig);
}
// ========================================================= getName 
cpchar EltBase::getName  ()
{
   return el_name.c_str() ;
}
// ========================================================= getName 
char* EltBase::getName  (pchar buffer)
{
// EL_NONE, EL_VERTEX, EL_EDGE, EL_QUAD, EL_HEXA, EL_REMOVED
   sprintf (buffer, "%c%04d", ABR_TYPES[el_type], el_id);
   return   buffer;
}
// ========================================================= printName 
void EltBase::printName  (cpchar sep)
{
   char nom[12];

   printf ("%s%s", getName(nom), sep);
}
// ========================================================= setAssociation
void EltBase::setAssociation (Shape* forme)
{
   el_assoc = forme;

   if (el_root->debug (2))
       cout << "  Vertex " << el_name << " : setAssociation" << endl;
}
// ========================================================= dumpRef 
void EltBase::dumpRef ()
{
   int nbp = el_parent.size();
   bool prems = true;

   if (nbp==0)
      {
      printf ("\n");
      }

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

END_NAMESPACE_HEXA

