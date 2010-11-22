
// Class : EltBase de base des Vertex/Edge/Quad, etc... 

#ifndef __ELT_BASE_H
#define __ELT_BASE_H

#include "hexa_base.hxx"
#include "HexDocument.hxx"
#include <vector>

#define HexDump(x) {printf(#x " = "); if (x) x->dump(); else printf("NULL\n");}
#define PrintName(x) if (x) x->printName(); else printf ("NULL, ")
#define PutName(x) { printf(#x " = "); if (x) x->printName("\n"); else printf("NULL\n"); }

BEGIN_NAMESPACE_HEXA

class EltBase 
{
public :
   virtual Hexa*   getHexa   (int nro)  { return NULL; }
   virtual Quad*   getQuad   (int nro)  { return NULL; }
   virtual Edge*   getEdge   (int nro)  { return NULL; }
   virtual Vertex* getVertex (int nro)  { return NULL; }

   virtual int     countHexa   ()  { return 0; }
   virtual int     countQuad   ()  { return 0; }
   virtual int     countEdge   ()  { return 0; }
   virtual int     countVertex ()  { return 0; }

   virtual int     getError    ()  { return el_status; }
   virtual bool    isValid     ()  { return el_status==HOK; }

   virtual void    setAssociation (Shape* forme)  { el_assoc = forme ; }
   virtual void    removeAssociation ()           { el_assoc = NULL  ; }
   virtual Shape*  getAssociation ()              { return el_assoc  ; }

public :
   // virtual void replaceHexa   (Hexa* old, Hexa* nouveau) {}
   // virtual void replaceQuad   (Quad* old, Quad* nouveau) {}
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
   virtual  void saveXml (XmlWriter& xml)   {}
   virtual  void majReferences () { }

   EltBase*  next ()                        { return el_next; }
   void      setNext (EltBase* suivant)     { el_next = suivant; }
   int       getId ()                       { return el_id; }
   void      setId (int ln)                 { el_id = ln; }
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

   int   getMark ()         { return el_mark; }
   void  setMark (int ln)   { el_mark = ln ; }
   char* getName  (pchar nom);
   void  printName (cpchar sep=" ");
   void  dumpRef ();

   bool   debug (int niv=0) { return el_root->getLevel() > niv ; } 

protected :
   EnumElt   el_type;
   EltBase*  el_next;
   int       el_id;
   Document* el_root;
   Shape*    el_assoc;

   int       el_status;
   int       el_mark;
   std::vector <EltBase*> el_parent;
};
// ========================================================= getName 
inline char* EltBase::getName  (pchar nom)
{
// EL_NONE, EL_VERTEX, EL_EDGE, EL_QUAD, EL_HEXA, EL_REMOVED
   cpchar nm_type = "xveqh????";

   sprintf (nom, "%c%04d", nm_type[el_type], el_id);
   return nom;
}
// ========================================================= printName 
inline void EltBase::printName  (cpchar sep)
{
   char nom[12];

   printf ("%s%s", getName(nom), sep);
}
// ========================================================= dump
inline void EltBase::dump ()
{
   printf ("Elt%d  Nro=%04d", el_type, el_id);
   dumpRef() ; 
}
// ========================================================= dumpRef 
inline void EltBase::dumpRef ()
{
   int nbp = el_parent.size();

   if (nbp>0)
      {
      printf ("\t isin ");
      for (int nro=0 ; nro<nbp ; nro++)	      
          el_parent[nro]->printName(", ");
      }
    printf ("\n");
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
// ========================================================= getFather 
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
