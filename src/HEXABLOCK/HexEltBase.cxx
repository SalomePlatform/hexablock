
// C++ : Gestion des noeuds

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
END_NAMESPACE_HEXA

