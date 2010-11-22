
// Class : Implementation provisoire et symbolique de la classe "shape"
//
#ifndef __GROUPS_H
#define __GROUPS_H

#include "hexa_base.hxx"

BEGIN_NAMESPACE_HEXA

class Group 
{
public :
   Group (cpchar nom, EnumGroup grp);
  ~Group ()                   {}

   void      setName (cpchar nom)          { grp_name = nom ; }
   cpchar    getName ()                    { return grp_name.c_str () ; }
   EnumGroup getKind ()                    { return grp_kind ; }
   int       addElement    (EltBase* elt);
   int       removeElement (int nro);
   EltBase*  getElement    (int nro)       { return grp_table [nro] ; }
   int       countElement  ()              { return grp_table.size () ; }
   void      clearElement  ()              { grp_table.clear () ; }

private :
   EnumGroup   grp_kind;
   EnumElt     grp_typelt;
   std::string grp_name;
   std::vector <EltBase*> grp_table;
};
// ======================================================== Constructeur
inline Group::Group (cpchar nom, EnumGroup grp)  
{
   grp_name = nom ; 
   grp_kind = grp;

   switch (grp_kind)
      {
      case HexaCell : case HexaNode :
           grp_typelt = EL_HEXA;
           break;

      case QuadCell : case QuadNode :
           grp_typelt = EL_QUAD;
           break;

      case EdgeNode : case EdgeCell :
           grp_typelt = EL_EDGE;
           break;

      case Vertex_Node : default :
           grp_typelt = EL_VERTEX;
      }
}
// ======================================================== addElement
inline int Group::addElement (EltBase* elt)
{
   if (grp_typelt != elt->getType())
      return HERR;
 
   grp_table.push_back (elt);
   return HOK;
}
// ======================================================== removeElement
inline int Group::removeElement (int nro)
{
   int nbelts = grp_table.size ();

   if (nro<0 || nro >= nbelts)
      return HERR;
 
   grp_table.erase (grp_table.begin() + nro);
   return HOK;
}
END_NAMESPACE_HEXA
#endif
