
// C++ : Gestion des noeuds

#include "HexEltBase.hxx"
#include "HexDocument.hxx"

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
END_NAMESPACE_HEXA

