
// C++ : Gestion des noeuds

#include "HexVertex.hxx"
#include "HexEdge.hxx"
#include "HexVector.hxx"

#include "HexXmlWriter.hxx"
#include "HexShape.hxx"

BEGIN_NAMESPACE_HEXA

// ====================================================== Constructeur
Vertex::Vertex  (Document* doc, double x, double y, double z)
      : EltBase (doc, EL_VERTEX)
{
   v_x = x;
   v_y = y;
   v_z = z;
   v_scalar = 0;
}
// ========================================================= getParent 
Edge* Vertex::getParent  (int nro)
{
   return static_cast <Edge*> (getFather (nro));
}
// ========================================================= saveXml 
void Vertex::saveXml  (XmlWriter& xml)
{
   char buffer[12], coord[80];

   sprintf (coord, "%g %g %g", v_x,v_y,v_z);

   xml.openMark     ("Vertex");
   xml.addAttribute ("id",    getName (buffer));
   xml.addAttribute ("coord", coord);
   if (el_assoc!=NULL)
      xml.addAttribute ("shape", el_assoc->getBrep().c_str());
   xml.closeMark ();
}
// ========================================================= translate 
void  Vertex::translate  (Vector* vecteur, double fact)
{
    v_x += fact*vecteur->getDx ();
    v_y += fact*vecteur->getDy ();
    v_z += fact*vecteur->getDz ();
}
// ========================================================= createMiddle 
Vertex* Vertex::createMiddle  (Vertex* left, Vertex* right)
{
    Vertex* milieu = new Vertex (left);

    milieu->v_x = (left->v_x + right->v_x) / 2;
    milieu->v_y = (left->v_y + right->v_y) / 2;
    milieu->v_z = (left->v_z + right->v_z) / 2;

    return milieu;
}
END_NAMESPACE_HEXA
