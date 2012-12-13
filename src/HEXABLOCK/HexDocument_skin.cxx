
// C++ : Reordonnancement des faces

#include "HexDocument.hxx"

#include "HexVertex.hxx"
#include "HexEdge.hxx"
#include "HexQuad.hxx"
#include "HexHexa.hxx"

#include <queue>

BEGIN_NAMESPACE_HEXA
// ----------------------------------------------------------------------
struct OrientedQuad 
     {
     Quad* quad;
     Vertex* v1;
     Vertex* v2;
     };

void propagateOrientation (queue <OrientedQuad> &queue_quads, Quad* orig);
void makeSkin (Quad* orig);

// ========================================================= reorderFaces
// ==== Ordonner les faces externes
void Document::reorderQuads ()
{
   majReferences ();
   EltBase* elt = NULL;

   for (elt = doc_first_elt[EL_QUAD]->next (); elt!=NULL; elt = elt->next())
       if (elt!=NULL && elt->isHere())
          {
          Quad* quad = static_cast <Quad*> (elt);
          if (quad->getNbrParents()==1) quad->setOrientation (Q_UNDEFINED);
          else                          quad->setOrientation (Q_INSIDE);
          }

   for (elt = doc_first_elt[EL_HEXA]->next (); elt!=NULL; elt = elt->next())
       if (elt!=NULL && elt->isHere())
          {
          Hexa* hexa = static_cast <Hexa*> (elt);
          for (int nq=0; nq<HQ_MAXI ; nq++)
              {
              Quad* quad = hexa->getQuad (nq);
              if (quad->getOrientation()==Q_UNDEFINED)
                 makeSkin (quad);
              }
          }
}
// ========================================================= makeSkin
void makeSkin (Quad* orig)
{
   OrientedQuad triplet;
   queue <OrientedQuad> queue_quads;

   orig->setOrientation();  // Q_DIRECT=1, Q_INVERSE=2 
   propagateOrientation (queue_quads, orig);
   
   while (NOT queue_quads.empty ())
      {
      triplet = queue_quads.front ();
      queue_quads.pop ();
      int n1 = triplet.quad->indexVertex (triplet.v1);
      int n2 = triplet.quad->indexVertex (triplet.v2);
      int sens = Q_WAITING;
      if (n2 == (n1+1) MODULO QUAD4)
          sens = Q_DIRECT;
      else if (n1 == (n2+1) MODULO QUAD4)
          sens = Q_INVERSE;
      else 
          printf (" **************** Orientation face impossible\n");

      triplet.quad->setOrientation (sens);
      propagateOrientation (queue_quads, triplet.quad);
      }
}
// ==================================================== propagateOrientation
void propagateOrientation (queue <OrientedQuad> &queue_quads, Quad* orig)
{
   OrientedQuad triplet;
                // Q_DIRECT : le sens des vertex est l'exterieur 
   int sens = orig->getOrientation();  // Q_DIRECT=1, Q_INVERSE=2 

   for (int ned=0; ned<QUAD4 ; ned++)
       {
       triplet.v1 = orig->getVertex ( ned );
       triplet.v2 = orig->getVertex ((ned+1) MODULO QUAD4);
       if (sens==Q_DIRECT)
          {
          Vertex* v3 = triplet.v1;
          triplet.v1 = triplet.v2;
          triplet.v2 = v3;
          }
       Edge* edge = orig->getEdge (ned);
       int nbp    = edge->getNbrParents ();
       for (int np=0; np<nbp ; np++)
           {
           triplet.quad = edge->getParent (np); 
           if (triplet.quad->getOrientation ()==Q_UNDEFINED)
              {
              triplet.quad->setOrientation (Q_WAITING);
              queue_quads.push (triplet);
              }
           }
       }
}
END_NAMESPACE_HEXA
