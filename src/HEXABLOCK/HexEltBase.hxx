
// Class : Element de base des Vertex/Edge/Quad, etc...

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

#ifndef __ELT_BASE_H
#define __ELT_BASE_H

#include "hexa_base.hxx"
#include <vector>

#define HexDump(x) {printf(#x " = "); if (x) x->dump(); else printf("NULL\n");}
#define PrintName(x) if (x) x->printName(); else printf ("NULL, ")
#define PutName(x) { printf(#x " = "); if (x) x->printName("\n"); else printf("NULL\n"); }

#define GetClone(elt) ((elt)==NULL ? NULL : elt->getClone())
#define BadElement(elt) (elt)==NULL || (elt)->isBad()
#define EltIsValid(elt)    (elt)!=NULL && (elt)->isValid()

#if defined WIN32
#pragma warning ( disable: 4251 )
#endif

BEGIN_NAMESPACE_HEXA

class HexaExport EltBase
{
public :
   virtual int     countHexa   ()  { return 0; }
   virtual int     countQuad   ()  { return 0; }
   virtual int     countEdge   ()  { return 0; }
   virtual int     countVertex ()  { return 0; }

   virtual void    setError    (int kod=HERR)  { el_status = kod; }
   virtual int     getError    ()              { return el_status; }
   virtual bool    isValid     ()              { return el_status==HOK; }
   virtual bool    isBad       ()              { return el_status!=HOK; }

   virtual void    duplicate ()                   {}
   virtual void    clearAssociation ()            {}

   // void copyAssociation    (EltBase* orig);
   // void replaceAssociation (EltBase* orig);

public :
   virtual void replaceEdge   (Edge* old, Edge* nouveau)
                               { printf ("rep-edge\n") ; }
   virtual void replaceVertex (Vertex* old, Vertex* nouveau)
                               { printf ("rep-vertex\n") ; }

   EltBase (Document* doc, EnumElt type=EL_NONE);
   EltBase (EnumElt type=EL_NONE);

   virtual ~EltBase ();
   virtual  void remove ();
   virtual  void suppress ();
   virtual  void dump ();
   virtual  void saveXml (XmlWriter* xml)   {}
   virtual  void majReferences () { }
   virtual  char*  makeVarName (char* nom);

   static   char* makeName  (int type, int id, char* name);

   EltBase*  next ()                        { return el_next; }
   void      setNext (EltBase* suivant)     { el_next = suivant; }
   int       getId ()                       { return el_id; }
   void      setId (int ln);
   Document* dad ()                         { return el_root; }
   EnumElt   getType ()                     { return el_type; }
   bool      isHere ()                      { return el_type!=EL_REMOVED; }
   bool      isDeleted ()                   { return el_type==EL_REMOVED; }

                 // On s'occupe des parents

   void  razReferences ()         { el_parent.clear() ; }
   void  addParent (EltBase* dad) { if (dad) el_parent.push_back(dad) ; }
   int   getNbrParents ()         { return el_parent.size() ; }
   bool  hasParents ();
   EltBase* getFather  (int nro);

   int   getMark ()                   { return el_mark; }
   void  setMark (int ln)             { el_mark = ln ; }
   char* getName   (pchar nom);
   void  printName (cpchar sep=" ");
   void  dumpRef ();

   cpchar getName ();
   void   setName (const string& nom) { el_name = nom ; }
   void   setName (cpchar nom)        { el_name = nom ; }

   bool    debug (int niv=0);
   bool    isAssociated ()            { return is_associated ; }
   string  getNextName ();

protected :
   EnumElt   el_type;
   EltBase*  el_next;
   int       el_id;
   Document* el_root;
   string    el_name;

   int       el_status;
   int       el_mark;
   bool      is_associated;
   std::vector <EltBase*> el_parent;
};
// ========================================================= dump
inline void EltBase::dump ()
{
   printf ("Elt%d  Nro=%04d", el_type, el_id);
   dumpRef() ;
}

// ========================================================= getFather
inline EltBase* EltBase::getFather  (int nro)
{
   EltBase* elt = NULL;
   if (nro >= 0 && nro < (int) el_parent.size() && isHere()
                && el_parent[nro]->isHere())
      elt = el_parent[nro];

   return elt;
}
// ========================================================= hasParents
inline bool EltBase::hasParents ()
{
   int nbp = el_parent.size();
   for (int nro=0 ; nro<nbp ; nro++)
       if (el_parent[nro]!=NULL && el_parent[nro]->isHere())
          return true;

   return false;
}
END_NAMESPACE_HEXA
#endif
