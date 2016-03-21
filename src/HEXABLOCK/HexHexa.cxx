
// C++ : Gestion des Hexaedres

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
#include "HexHexa.hxx"
#include "HexQuad.hxx"

#include "HexVertex.hxx"
#include "HexDocument.hxx"
#include "HexEdge.hxx"

#include "HexGlobale.hxx"
#include "HexXmlWriter.hxx"
#include "HexDiagnostics.hxx"
#include "HexGlobale.hxx"
#include "HexMatrix.hxx"
#include "HexElements.hxx"

BEGIN_NAMESPACE_HEXA

// ======================================================== Constructeur
Hexa::Hexa (Vertex* va, Vertex* vb, Vertex* vc, Vertex* vd,
            Vertex* ve, Vertex* vf, Vertex* vg, Vertex* vh)
    : EltBase (va->dad(), EL_HEXA)
{
   h_vertex [V_ACE] = va;
   h_vertex [V_ACF] = vb;   // = vc ; Modif Abu 30/08/2011
   h_vertex [V_ADE] = vc;   // = vb ; Modif Abu 30/08/2011
   h_vertex [V_ADF] = vd;

   h_vertex [V_BCE] = ve;
   h_vertex [V_BCF] = vf;   // = vg ; Modif Abu 30/08/2011
   h_vertex [V_BDE] = vg;   // = vf ; Modif Abu 30/08/2011
   h_vertex [V_BDF] = vh;

   h_clone          = NULL;

   controlerSommets ();

   Globale* glob = Globale::getInstance ();

   for (int nro=0 ; nro<HE_MAXI ; nro++)
       {
       h_edge[nro] = new Edge (h_vertex[glob->EdgeVertex (nro, V_AMONT)],
                               h_vertex[glob->EdgeVertex (nro, V_AVAL)]);
       }

   for (int nro=0 ; nro<HQ_MAXI ; nro++)
       {
       h_quad[nro] = new Quad (h_edge [glob->QuadEdge (nro, E_A)],
                               h_edge [glob->QuadEdge (nro, E_B)],
                               h_edge [glob->QuadEdge (nro, E_C)],
                               h_edge [glob->QuadEdge (nro, E_D)]);

       h_quad[nro] -> addParent (this);
       }
   majReferences ();
   if (el_root != NULL && el_status==HOK)
       el_root->addHexa (this);
}
// ======================================================== Constructeur 2
Hexa::Hexa (Quad* qa, Quad* qb, Quad* qc, Quad* qd, Quad* qe, Quad* qf)
    : EltBase (qa->dad(), EL_HEXA)
{
   h_quad [Q_A] = qa;
   h_quad [Q_B] = qb;
   h_quad [Q_C] = qc;
   h_quad [Q_D] = qd;
   h_quad [Q_E] = qe;
   h_quad [Q_F] = qf;
   h_clone      = NULL;

   for (int nb=0 ; nb<HE_MAXI ; nb++) h_edge   [nb] = NULL;
   for (int nb=0 ; nb<HV_MAXI ; nb++) h_vertex [nb] = NULL;

   controlerFaces  ();

   verifierAretes  ();
   verifierSommets ();
   majReferences   ();

   if (el_status != HOK)
      {
      printf (" +++++++++++++++++++++++++++++++++++++++++++ \n");
      printf (" +++ Heaedre impossible \n");
      printf (" +++++++++++++++++++++++++++++++++++++++++++ \n");
      // el_root->dump ();
      // printf (" +++++++++++++++++++++++++++++++++++++++++++ \n");
      dumpFull ();
      for (int nro=0; nro<HQ_MAXI; nro++) HexDump (h_quad[nro]);
      for (int nro=0; nro<HE_MAXI; nro++) HexDump (h_edge[nro]);
      for (int nro=0; nro<HV_MAXI; nro++) HexDump (h_vertex[nro]);
      printf (" +++++++++++++++++++++++++++++++++++++++++++ \n");
      // exit (1);
      }
   else
       el_root->addHexa (this);
}
// =========================================================  Constructeur 3
// === a utiliser uniquement pour le clonage
Hexa::Hexa (Hexa* other)
    : EltBase (other->el_root, EL_HEXA)
{
   for (int nro=0 ; nro<HQ_MAXI ; nro++) h_quad   [nro] = NULL;
   for (int nro=0 ; nro<HE_MAXI ; nro++) h_edge   [nro] = NULL;
   for (int nro=0 ; nro<HV_MAXI ; nro++) h_vertex [nro] = NULL;
   h_clone = NULL;

   if (el_root != NULL && el_status==HOK)
       el_root->addHexa (this);
}
// ======================================================== majReferences
void Hexa::majReferences  ()
{
   for (int nro=0 ; nro<HQ_MAXI ; nro++)
       h_quad[nro]->addParent (this);
}
// ======================================================== controlerArete
void Hexa::controlerArete  (int arete, int face1, int face2)
{
   h_edge [arete] = h_quad[face1]->commonEdge (h_quad[face2]);
   if (h_edge [arete]==NULL)
       {
       el_root->putError (W_H_BAD_EDGE,
                          el_root->glob->namofHexaEdge (arete));
       el_status = 888;
       }
}
// ======================================================== controlerSomet
void Hexa::controlerSommet  (int node, int ne1, int ne2, int ne3)
{
    if (h_edge[ne1] == NULL || h_edge[ne2] == NULL || h_edge[ne3] == NULL)
       return;

    Vertex* hv = h_edge[ne1]->commonVertex (h_edge[ne2]);
    h_vertex [node] = hv;
    if (hv == NULL)
       {
       el_root->putError (W_H_BAD_VERTEX,
                          el_root->glob->namofHexaVertex(node));
       el_status = 888;
       }
    else if (hv == NULL || h_edge[ne3]->index (hv)<0)
       {
       char b[10];
       el_root->putError (W_H_BAD_VERTEX,
                          el_root->glob->namofHexaVertex(node));
       el_status = 888;
       printf ("%s = %s\n", el_root->glob->namofHexaVertex(node),
                            hv->getName(b));
       printf ("%s = %s\n", el_root->glob->namofHexaEdge(ne1),
                            h_edge[ne1]->getName(b));
       printf ("%s = %s\n", el_root->glob->namofHexaEdge(ne2),
                            h_edge[ne2]->getName(b));
       printf ("%s = %s\n", el_root->glob->namofHexaEdge(ne3),
                            h_edge[ne3]->getName(b));
       }
}
// ======================================================== controlerFaces
void Hexa::controlerFaces  ()
{
   for (int n1=0 ; n1<HQ_MAXI ; n1++)
       {
       if (BadElement (h_quad [n1]))
          {
          el_root->putError (W_H_NULL_QUAD,
                             el_root->glob->namofHexaQuad (n1));
          setError (886);
          return;
          }
       for (int n2=n1+1 ; n2<HQ_MAXI ; n2++)
           if (h_quad [n1] == h_quad[n2])
              {
              el_root->putError (W_H_EQ_QUAD,
                         el_root->glob->namofHexaQuad (n1),
                         el_root->glob->namofHexaQuad (n2));
              setError (888);
              }
       }
   for (int dd=0 ; dd<DIM3 ; dd++)
       {
       Quad* qa  = h_quad [2*dd];
       Quad* qb  = h_quad [2*dd+1];
       Edge* cut = qa->inter (qb);
       if (cut != NULL)
          {
          bool more = true;
          for (int nc=2*dd+2 ; more && nc<HQ_MAXI ; nc++)
              {
              Edge* cut = qa->inter (h_quad[nc]);
              if (cut==NULL)
                 {
                 more = false;
                 // cout << " ... le quad oppose au quad " << 2*dd
                      // << " est le " << nc << endl;
                 qb             = h_quad[nc];
                 h_quad[nc]     = h_quad [2*dd+1];
                 h_quad[2*dd+1] = qb;
                 }
              }
          if (more)
             {
             char num [10];
             sprintf (num, "%d", 2*dd+1);
             el_root->putError ("addHexa : the %sth quad has no opposed quad",
                                 num);
             setError (886);
             return ;
             }
          }
       }
}
// ======================================================== controlerSommets
void Hexa::controlerSommets  ()
{
   for (int n1=0 ; n1<HV_MAXI ; n1++)
       {
       if (BadElement (h_vertex [n1]))
          {
          el_root->putError (W_H_NULL_QUAD,
                             el_root->glob->namofHexaVertex (n1));
          setError (886);
          return;
          }
       for (int n2=n1+1 ; n2<HQ_MAXI ; n2++)
           if (h_vertex [n1] == h_vertex[n2])
              {
              el_root->putError (W_H_EQ_QUAD,
                         el_root->glob->namofHexaVertex (n1),
                         el_root->glob->namofHexaVertex (n2));
              setError (888);
              }
       }
}
// ======================================================== verifierAretes
void Hexa::verifierAretes  ()
{
   for (int nro=0 ; nro<HE_MAXI ; nro++) h_edge [nro] = NULL;

   controlerArete ( E_AC, Q_A, Q_C);
   controlerArete ( E_AF, Q_A, Q_F);
   controlerArete ( E_AD, Q_A, Q_D);
   controlerArete ( E_AE, Q_A, Q_E);

   controlerArete ( E_BC, Q_B, Q_C);
   controlerArete ( E_BF, Q_B, Q_F);
   controlerArete ( E_BD, Q_B, Q_D);
   controlerArete ( E_BE, Q_B, Q_E);

   controlerArete ( E_CE, Q_C, Q_E);
   controlerArete ( E_CF, Q_C, Q_F);
   controlerArete ( E_DF, Q_D, Q_F);
   controlerArete ( E_DE, Q_D, Q_E);
}
// ======================================================== verifierSommets
void Hexa::verifierSommets  ()
{
   for (int nro=0 ; nro<HV_MAXI ; nro++) h_vertex [nro] = NULL;

   controlerSommet (V_ACE, E_AC, E_AE, E_CE);
   controlerSommet (V_ACF, E_AC, E_AF, E_CF);
   controlerSommet (V_ADF, E_AD, E_AF, E_DF);
   controlerSommet (V_ADE, E_AD, E_AE, E_DE);

   controlerSommet (V_BCE, E_BC, E_BE, E_CE);
   controlerSommet (V_BCF, E_BC, E_BF, E_CF);
   controlerSommet (V_BDF, E_BD, E_BF, E_DF);
   controlerSommet (V_BDE, E_BD, E_BE, E_DE);
}
// ======================================================== Inter
Quad* Hexa::Inter (Hexa* other)
{
   for (int nf1=0 ; nf1 < HQ_MAXI ; nf1++)
       for (int nf2=0 ; nf2 < HQ_MAXI ; nf2++)
           if (h_quad[nf1] == other->h_quad[nf2])
              return h_quad[nf1];

   return NULL;
}
// -------------------------------------------------------------------
//                         Debug
// -------------------------------------------------------------------
// ======================================================= razNodes
void Hexa::razNodes ()
{
   for (int nb=0 ; nb<HV_MAXI ; nb++)
       h_vertex[nb]->setMark (NO_COUNTED);
}
// ======================================================= countNodes
int Hexa::countNodes ()
{
   int nombre = 0;
   for (int nb=0 ; nb<HV_MAXI ; nb++)
       if (h_vertex[nb]->getMark () == NO_COUNTED)
          {
          h_vertex[nb]->setMark (NO_USED);
          nombre ++;
          }

   return nombre;
}
// ======================================================= printNodes
void Hexa::printNodes (pfile vtk, int& compteur)
{
   const double minvtk = 1e-30;
   Real3 koord;

   for (int nb=0 ; nb<HV_MAXI ; nb++)
       if (h_vertex[nb]->getMark()==NO_USED)
          {
          h_vertex[nb]->getPoint (koord);
          for (int nc=dir_x ; nc<=dir_z ; nc++)
              if (koord [nc] < minvtk &&  koord [nc] > -minvtk)
                  koord[nc] = 0.0;

          fprintf (vtk, "%g %g %g\n", koord[dir_x], koord[dir_y], koord[dir_z]);
          h_vertex[nb]->setMark (compteur);
          compteur ++;
          }
}
// ======================================================= colorNodes
void Hexa::colorNodes (pfile vtk)
{
   for (int nb=0 ; nb<HV_MAXI ; nb++)
       if (h_vertex[nb]->getMark()>=0)
          {
          double color = 100*(h_vertex[nb]->getScalar()+1);
          fprintf (vtk, "%g\n", color);
          h_vertex[nb]->setMark (NO_COUNTED);
          }
}
// ======================================================= moveNodes
void Hexa::moveNodes (Matrix* matrice)
{
   for (int nb=0 ; nb<HV_MAXI ; nb++)
       if (h_vertex[nb]->getMark()!=IS_USED)
          {
          matrice->perform (h_vertex[nb]);
          h_vertex[nb]->setMark (IS_USED);
          }
}
// ======================================================= transform
void Hexa::transform (Matrix* matrice)
{
   for (int nb=0 ; nb<HV_MAXI ; nb++)
       matrice->perform (h_vertex[nb]);
}
// ======================================================= printHexa
void Hexa::printHexa  (pfile vtk)
{
   fprintf (vtk, "%d", HV_MAXI);

   fprintf (vtk, " %d", h_vertex[V_ACE]->getMark ());
   fprintf (vtk, " %d", h_vertex[V_ACF]->getMark ());
   fprintf (vtk, " %d", h_vertex[V_ADF]->getMark ());
   fprintf (vtk, " %d", h_vertex[V_ADE]->getMark ());

   fprintf (vtk, " %d", h_vertex[V_BCE]->getMark ());
   fprintf (vtk, " %d", h_vertex[V_BCF]->getMark ());
   fprintf (vtk, " %d", h_vertex[V_BDF]->getMark ());
   fprintf (vtk, " %d", h_vertex[V_BDE]->getMark ());

   fprintf (vtk, "\n");
}
// ======================================================= printHexaVtk
void Hexa::printHexaVtk (pfile vtk)
{
   fprintf (vtk, "%d", HV_MAXI);

   fprintf (vtk, " %d", h_vertex[V_ACE]->getId ());
   fprintf (vtk, " %d", h_vertex[V_ACF]->getId ());
   fprintf (vtk, " %d", h_vertex[V_ADF]->getId ());
   fprintf (vtk, " %d", h_vertex[V_ADE]->getId ());

   fprintf (vtk, " %d", h_vertex[V_BCE]->getId ());
   fprintf (vtk, " %d", h_vertex[V_BCF]->getId ());
   fprintf (vtk, " %d", h_vertex[V_BDF]->getId ());
   fprintf (vtk, " %d", h_vertex[V_BDE]->getId ());

   fprintf (vtk, "\n");
}
// ======================================================== hasFreEdges
bool Hexa::hasFreeEdges ()
{
   if (isDeleted())
       return false;

   for (int nro=0; nro<HE_MAXI ; nro++)
       if (h_edge[nro]->getMark()<0)
          return true;

   return false;
}
// ======================================================== propager
void Hexa::propager (Propagation* prop, int nro)
{
   if (isDeleted())
       return;

   for (int nro=0; nro<HE_MAXI ; nro++)
       if (h_edge[nro]->getMark()<0)
           h_edge[nro]->propager (prop, nro);
}
// ========================================================= saveXml
void Hexa::saveXml (XmlWriter* xml)
{
   char ident[12];
   string quads;

   for (int nro=0 ; nro<HQ_MAXI ; nro++)
       {
       if (nro>0) quads += " ";
       quads += h_quad[nro]->getName(ident);
       }

   getName (ident);
   xml->openMark     ("Hexa");
   xml->addAttribute ("id",    ident);
   xml->addAttribute ("quads", quads);
   if (el_name!=ident)
       xml->addAttribute ("name", el_name);
   xml->closeMark ();
}
// ========================================================= findQuad
int Hexa::findQuad (Quad* element)
{
   for (int nro=0 ; nro<HQ_MAXI ; nro++)
       if (h_quad[nro]==element)
          return nro;

   return NOTHING;
}
// ========================================================= findEdge
int Hexa::findEdge (Edge* element)
{
   for (int nro=0 ; nro<HE_MAXI ; nro++)
       if (h_edge[nro]==element)
          return nro;

   return NOTHING;
}
// ========================================================= findVertex
int Hexa::findVertex (Vertex* element)
{
   for (int nro=0 ; nro<HV_MAXI ; nro++)
       if (h_vertex[nro]==element)
          return nro;

   return NOTHING;
}
// ========================================================= disconnectQuad
Elements* Hexa::disconnectQuad (Quad* quad)
{
   if (quad==NULL)
      return NULL;

   if (debug())
      {
      printf (" ... Avant disconnectQuad, quad=");
      quad->printName ("\n");
      dumpFull ();
      }

   int nface = findQuad (quad);
   if (nface==NOTHING)
      return NULL;
                                       // Face opposee : replace
   // int nfopp = (nface MODULO 2==0) ? nface+1 : nface-1;

   int  ind_edge  [QUAD4], ind_opp_quad [QUAD4];
   bool make_quad [QUAD4], make_edge [QUAD4];

   for (int nro=0 ; nro<QUAD4 ; nro++)
       make_quad [nro] = make_edge[nro] = false;

   for (int nro=0 ; nro<QUAD4 ; nro++)
       {
       int nro1  = (nro+1) MODULO QUAD4;
       int pedge = findEdge   (quad->getEdge   (nro));
       int pnode = findVertex (quad->getVertex (nro));
       int oppq  = findOpposedQuad (quad, quad->getEdge (nro));

       ind_edge [nro]     = pedge;
       ind_opp_quad [nro] = oppq;

       if (pedge==NOTHING || pnode==NOTHING || oppq==NOTHING)
          return NULL;

       make_quad [nro]  = h_quad[oppq]->getNbrParents() == 2;
       make_edge [nro ] = make_edge [nro ] || make_quad [nro];
       make_edge [nro1] = make_edge [nro1] || make_quad [nro];

       if (debug())
          {
          printf (" Sommet nro %d : ", nro);
          quad->getVertex(nro)->printName (", ");
          printf (" edge = ");
          quad->getEdge(nro)->printName (", ");
          printf (" quad oppose = ");
          h_quad[oppq]->printName("");
          if (make_quad [nro])
             printf (" a dissocier\n");
          else
             printf ("\n");

          }
       }

   Vertex* new_node     [QUAD4];
   Edge*   new_opp_edge [QUAD4];
   Edge*   old_opp_edge [QUAD4];

   for (int nro=0 ; nro<QUAD4 ; nro++)
       {
       old_opp_edge [nro] = NULL;
       new_opp_edge [nro] = NULL;
       Vertex*  o_v0  = quad->getVertex (nro);
       new_node [nro] = new Vertex (o_v0);
       if (debug())
          {
          printf (" quad.vertex [%d] = ", nro);
          quad->getVertex (nro)->printName (" --> ");
          new_node [nro]->printName ("\n");
          }

       if (make_edge[nro])
          {
          Quad*   pface  = h_quad [ind_opp_quad [nro]];
          int     bid;
          int     ncut = pface->inter (quad, bid);
          Edge*   ecut = pface->getEdge ((ncut+1) MODULO QUAD4);
          Vertex* vopp = ecut->getVertex(V_AMONT);
          if (vopp==o_v0)
              vopp = ecut->getVertex (V_AVAL);
          else if (o_v0 != ecut->getVertex (V_AVAL));
              {
              ecut = pface->getEdge ((ncut+3) MODULO QUAD4);
              vopp = ecut->getVertex(V_AMONT);
              if (vopp==o_v0)
                  vopp = ecut->getVertex (V_AVAL);
              else if (o_v0 != ecut->getVertex (V_AVAL))
                  return NULL;
              }

          old_opp_edge [nro] = ecut;
          new_opp_edge [nro] = new Edge (new_node[nro], vopp);
          if (debug())
             {
             printf (" quad.opp_edge [%d] = ", nro);
             old_opp_edge [nro]->printName (" --> ");
             new_opp_edge [nro]->printName ("\n");
             }
          }
       }

   Quad* new_quad = new Quad (new_node[0], new_node[1], new_node[2],
                                                        new_node[3]);

   Quad* new_opp_quad [QUAD4];
   Quad* old_opp_quad [QUAD4];
   for (int nro=0 ; nro<QUAD4 ; nro++)
       {
       old_opp_quad [nro] = NULL;
       new_opp_quad [nro] = NULL;
       if (make_quad[nro])
          {
          int nro1 = (nro+1) MODULO QUAD4;

          Edge* n_edge0 = new_quad->getEdge (nro);
          Edge* n_edge1 = new_opp_edge [nro];
          Edge* n_edge3 = new_opp_edge [nro1];

          int iv1 = n_edge1->inter (n_edge0);
          int iv3 = n_edge3->inter (n_edge0);
          if (iv1 <0 || iv3 <0)
             return NULL;

          Quad* o_face = h_quad [ind_opp_quad [nro]];
          Edge* edge2  = o_face->findEdge (n_edge1->getVertex (1-iv1),
                                           n_edge3->getVertex (1-iv3));
          if (edge2==NULL)
             return NULL;
          // Edge* o_edge0 = h_edge [ind_edge     [nro]];
          // int sens = 1;
          // Edge* edge2   = o_face->getOpposEdge (o_edge0, sens);

          old_opp_quad [nro] = o_face;
          if (debug())
             printf (" -------- Quad oppose nro %d\n", nro);
          new_opp_quad [nro] = new Quad (n_edge0, n_edge1, edge2, n_edge3);
          }
       }

   replaceQuad (quad, new_quad);
   for (int nro=0 ; nro<QUAD4 ; nro++)
       if (make_quad[nro])
          replaceQuad (old_opp_quad [nro], new_opp_quad [nro]);

   for (int nro=0 ; nro<QUAD4 ; nro++)
       {
       replaceEdge (h_edge[ind_edge[nro]], new_quad->getEdge (nro));
       if (make_edge[nro])
          replaceEdge (old_opp_edge [nro], new_opp_edge [nro]);
       }

   for (int nro=0 ; nro<QUAD4 ; nro++)
       {
       replaceVertex (quad->getVertex(nro), new_node[nro]);
       }


   h_quad [nface] = new_quad;
   if (debug())
      {
      printf (" ... Apres disconnectQuad, new_quad=");
      new_quad->printName ("\n");
      dumpFull ();
      }

   Elements* nouveaux  = new Elements (el_root);
   nouveaux->addQuad   (new_quad);
   for (int nro=0 ; nro<QUAD4 ; nro++)
       {
       nouveaux->addEdge   (new_quad->getEdge   (nro));
       nouveaux->addVertex (new_quad->getVertex (nro));
       if (make_edge[nro])
          nouveaux->addEdge (new_opp_edge [nro]);
       if (make_quad[nro])
          nouveaux->addQuad (new_opp_quad [nro]);
       }
   nouveaux->moveDisco (this);
   return nouveaux;
}
// ========================================================= disconnectEdge
Elements* Hexa::disconnectEdge (Edge* arete)
{
   int nedge  = findEdge   (arete);
   int namont = findVertex (arete->getVertex(V_AMONT));
   int naval  = findVertex (arete->getVertex(V_AVAL ));

   if (nedge==NOTHING || namont==NOTHING || naval==NOTHING)
      return NULL;

   if (debug())
      {
      printf (" ... Avant disconnectEdge, arete=");
      arete->printName ("\n");
      dumpFull ();
      }

   Edge*   n_edge   [HE_MAXI];
   Quad*   n_quad   [HQ_MAXI];
   Vertex* n_vertex [HV_MAXI];

   for (int nro=0 ; nro<HQ_MAXI ; nro++) n_quad [nro]   = NULL;
   for (int nro=0 ; nro<HE_MAXI ; nro++) n_edge [nro]   = NULL;
   for (int nro=0 ; nro<HV_MAXI ; nro++) n_vertex [nro] = NULL;

   Vertex* old_amont = arete->getVertex (V_AMONT);
   Vertex* old_aval  = arete->getVertex (V_AVAL );
   Vertex* new_amont = n_vertex [namont] = new Vertex (old_amont);
   Vertex* new_aval  = n_vertex [naval]  = new Vertex (old_aval);
   n_edge [nedge]    = new Edge   (new_amont, new_aval);

   // Un edge non remplace, qui contient un vertex remplace
   //         commun a plus de 2 faces (donc appartenant a un autre hexa)
   //         doit etre duplique

   for (int nro=0 ; nro<HE_MAXI ; nro++)
       {
       if (   n_edge[nro]==NULL && h_edge[nro] != NULL
           && h_edge[nro]->getNbrParents()>2)
          {
          Vertex* va =  h_edge[nro]->getVertex (V_AMONT);
          Vertex* vb =  h_edge[nro]->getVertex (V_AVAL);

          if (va==old_amont)
             n_edge [nro] = new Edge (new_amont, vb);
          else if (va==old_aval)
             n_edge [nro] = new Edge (new_aval,  vb);
          else if (vb==old_amont)
             n_edge [nro] = new Edge (va, new_amont);
          else if (vb==old_aval)
             n_edge [nro] = new Edge (va, new_aval);
          }
       }

   // Un quad non remplace, qui contient un edge remplace
   //         commun a plus de 2 Hexas
   //         doit etre duplique

   Globale* glob = Globale::getInstance();
   for (int nro=0 ; nro<HQ_MAXI ; nro++)
       if (   n_quad[nro]==NULL && h_quad[nro] != NULL
           && h_quad[nro]->getNbrParents()>1)
          {
          Edge* qedge[QUAD4];
          bool  duplic = false;
          for (int ned=0 ; ned<QUAD4 ; ned++)
              {
              int ndup = glob->QuadEdge (nro, (EnumQuad)ned);
              if (n_edge [ndup] ==NULL)
                 qedge [ned] = h_edge[ndup];
              else
                 {
                 qedge [ned] = n_edge[ndup];
                 duplic = true;
                 }
              }
          if (duplic)
             n_quad[nro] = new Quad (qedge[Q_A], qedge[Q_B],
                                     qedge[Q_C], qedge[Q_D]);
          }

   Elements* nouveaux  = new Elements (el_root);
   for (int nro=0 ; nro<HQ_MAXI ; nro++)
       if (n_quad[nro]!=NULL)
          {
          replaceQuad (h_quad[nro], n_quad[nro]);
          nouveaux->addQuad  (n_quad[nro]);
          }

   for (int nro=0 ; nro<HE_MAXI ; nro++)
       if (n_edge[nro]!=NULL)
          {
          replaceEdge (h_edge[nro], n_edge[nro]);
          nouveaux->addEdge  (n_edge[nro]);
          }

   for (int nro=0 ; nro<HV_MAXI ; nro++)
       if (n_vertex[nro]!=NULL)
          {
          replaceVertex (h_vertex[nro], n_vertex[nro]);
          nouveaux->addVertex  (n_vertex[nro]);
          }

   if (debug())
      {
      printf (" ... Apres disconnectEdge\n");
      dumpFull ();
      }

   nouveaux->moveDisco (this);
   return nouveaux;
}
// ========================================================= disconnectVertex
Elements* Hexa::disconnectVertex (Vertex* noeud)
{
   if (debug())
      {
      printf (" ... Avant disconnectVertex, vertex=");
      noeud->printName ("\n");
      dumpFull ();
      }

   int node = findVertex (noeud);
   if (node==NOTHING)
      return NULL;

   Vertex* new_node = new Vertex (noeud);
   Quad*   new_quad [HQ_MAXI];
   Edge*   new_edge [HE_MAXI];

   for (int nro=0 ; nro<HE_MAXI ; nro++) new_edge [nro] = NULL;
   for (int nro=0 ; nro<HQ_MAXI ; nro++)
       {
       new_quad [nro] = NULL;
            // Cete face contient le sommet et est commune a 2 hexas
       if (   h_quad[nro]->indexVertex(noeud) >= 0
           && h_quad[nro]->getNbrParents  ()  >= 2)
           {
           int nbmod = 0;
           Edge* tedge [QUAD4];
           for (int qed=0 ; qed<QUAD4 ; qed++)
               {
               Edge* arete = tedge[qed] = h_quad[nro]->getEdge (qed);
               int   indv  = arete->index (noeud);
               if (indv>=0)
                  {
                  nbmod++;
                  int hed = findEdge (arete);
                  if (hed<0)
                     return NULL;
                  if (new_edge [hed]==NULL)
                      new_edge [hed] = new Edge (new_node,
                                                 arete->getVertex(1-indv));
                  tedge [qed] = new_edge [hed];
                  }
               }
           if (nbmod!=2)
              return NULL;
           new_quad [nro] = new Quad (tedge[0], tedge[1], tedge[2], tedge[3]);
           }
       }

   Elements* nouveaux  = new Elements (el_root);

   for (int nro=0 ; nro<HQ_MAXI ; nro++)
       if (new_quad [nro] != NULL)
          {
          replaceQuad (h_quad [nro], new_quad [nro]);
          nouveaux->addQuad (new_quad[nro]);
          }

   for (int nro=0 ; nro<HE_MAXI ; nro++)
       if (new_edge [nro] != NULL)
          {
          replaceEdge (h_edge [nro], new_edge [nro]);
          nouveaux->addEdge (new_edge[nro]);
          }

   replaceVertex (noeud, new_node);
   nouveaux->addVertex (new_node);


   if (debug())
      {
      printf (" ... Apres disconnectVertex\n");
      dumpFull ();
      }

   nouveaux->moveDisco (this);
   return nouveaux;
}
// ========================================================= getBase
int Hexa::getBase (Vertex* orig, Edge* normale)
{
   for (int nq=0 ; nq<HQ_MAXI ; nq++)
       {
       if (   h_quad[nq]->indexVertex(orig)    >= 0
           && h_quad[nq]->indexEdge  (normale) < 0)
           return nq;
       }
   return NOTHING;
}
// ======================================================== replaceQuad
void Hexa::replaceQuad (Quad* old, Quad* par)
{
   for (int nro=0 ; nro<HQ_MAXI ; nro++)
       {
       if (h_quad[nro]==old)
           {
           h_quad[nro] = par;
           if (debug())
              {
              printf (" Dans ");
              printName ();
              printf (" [%d], ", nro);
              old->printName (" est remplace par ");
              par->printName ("\n");
              }
           }
       }

}
// ======================================================== replaceEdge
void Hexa::replaceEdge (Edge* old, Edge* par)
{
   for (int nro=0 ; nro<HE_MAXI ; nro++)
       {
       if (h_edge[nro]==old)
           {
           h_edge[nro] = par;
           if (debug())
              {
              printf (" Dans ");
              printName ();
              printf (" [%d], ", nro);
              old->printName (" est remplace par ");
              par->printName ("\n");
              }
           }
       }

   for (int nro=0 ; nro<HQ_MAXI ; nro++)
       {
       h_quad[nro]->replaceEdge (old, par);
       }
}
// ======================================================== replaceVertex
void Hexa::replaceVertex (Vertex* old, Vertex* par)
{
   for (int nro=0 ; nro<HV_MAXI ; nro++)
       {
       if (h_vertex [nro]==old)
           {
           h_vertex [nro] = par;
           if (debug())
              {
              printf (" Dans ");
              printName ();
              printf (" [%d], ", nro);
              old->printName (" est remplace par ");
              par->printName ("\n");
              }
           }
       }

   for (int nro=0 ; nro<HE_MAXI ; nro++)
       {
       h_edge[nro]->replaceVertex (old, par);
       }

   for (int nro=0 ; nro<HQ_MAXI ; nro++)
       {
       h_quad[nro]->replaceVertex (old, par);
       }
}
// ======================================================== removeConnected
void Hexa::removeConnected ()
{

   if (el_type == EL_REMOVED)
      return;

   remove();

   for (int nro=0 ; nro<HQ_MAXI ; nro++)
       {
       Quad*  face = h_quad [nro];
       int nbhexas = face->getNbrParents ();

       for (int nc=0 ; nc<nbhexas ; nc++)
           {
           Hexa* cell = face->getParent(nc);
           if (cell!=NULL && cell->isValid ())
              cell->removeConnected ();
           }
       }

   for (int nro=0 ; nro<HQ_MAXI ; nro++)
       h_quad [nro]->remove();
   for (int nro=0 ; nro<HE_MAXI ; nro++)
       h_edge [nro]->remove();
   for (int nro=0 ; nro<HV_MAXI ; nro++)
       h_vertex [nro]->remove();
}
// ======================================================== findOpposedQuad
int Hexa::findOpposedQuad (Quad* face, Edge* arete)
{
   for (int nro=0 ; nro<HQ_MAXI ; nro++)
       {
       Quad*  quad = h_quad [nro];
       if (quad!=face && quad->indexEdge (arete) >=0)
          return nro;
       }

   return NOTHING;
}
// ========================================================= dump
void Hexa::dump ()
{
   printName(" = (");
   if (NOT isHere ())
      {
      printf ("*** deleted ***)\n");
      return;
      }

   for (int nro=0; nro<HQ_MAXI ; nro++)
        PrintName (h_quad[nro]);
   printf (")\n");

   printf ("      = (");

   for (int nro=0; nro<HE_MAXI ; nro++)
        {
        PrintName (h_edge[nro]);
        if (nro==3 || nro ==7)
           printf ("\n         ");
        }
   printf (")\n");

   printf ("      = (");
   for (int nro=0; nro<HV_MAXI ; nro++)
        PrintName (h_vertex[nro]);
   printf (")\n");
   Real3 cg;
   getCenter (cg);
   printf ("cg    = (%g, %g, %g)\n", cg[0], cg[1], cg[2]);

}
// ======================================================== dumpPlus
void Hexa::dumpPlus ()
{
   dump ();
   for (int nro=0 ; nro < HV_MAXI ; nro++)
       {
       Vertex* pv = h_vertex[nro];
       printf ( "    ");
       if (pv!=NULL)
          {
          pv->printName ("");
          printf ( " (%g, %g, %g)\n", pv->getX(),  pv->getY(),  pv->getZ());
          }
       else
          {
          printf ( "NULL\n");
          }
       }
}
// ======================================================== dumpFull
void Hexa::dumpFull ()
{
   dump ();
   Globale* glob = Globale::getInstance ();

   printf ("\n");
   for (int nro=0; nro<HQ_MAXI ; nro++)
       {
       printf (" quad(%s) = ", glob->namofHexaQuad(nro));
       if (h_quad[nro] ==NULL)
           printf (" NULL\n");
       else
           {
           h_quad[nro]->printName (" = (");
           for (int nc=0; nc<QUAD4 ; nc++)
                h_quad[nro]->getEdge(nc)->printName ();
           printf (")\n");
           printf ("                   = (");
           for (int nc=0; nc<QUAD4 ; nc++)
                h_quad[nro]->getVertex(nc)->printName ();
           printf (")\n");
           }
       }

   printf ("\n");
   for (int nro=0; nro<HE_MAXI ; nro++)
       {
       printf (" edge(%s) = ", glob->namofHexaEdge(nro));
       if (h_edge[nro] ==NULL)
           printf (" NULL\n");
       else
           {
           h_edge[nro]->printName (" = (");
           for (int nc=0; nc<V_TWO ; nc++)
                h_edge[nro]->getVertex(nc)->printName ();
           printf (")\n");
           }
       }
   printf ("\n");

   for (int nro=0; nro<HV_MAXI ; nro++)
       {
       Vertex* pv = h_vertex[nro];
       printf (" vertex(%s) = ", glob->namofHexaVertex(nro));
       if (pv ==NULL)
           printf (" NULL");
       else
           {
           pv->printName (" = (");
           printf ("%g, %g, %g)\n", pv->getX(), pv->getY(), pv->getZ());
           }
       }
   printf ("\n");
}
// ======================================================== getOpposedQuad
Quad* Hexa::getOpposedQuad (Quad* face)
{
   if      (face == h_quad [Q_A]) return h_quad [Q_B];
   else if (face == h_quad [Q_B]) return h_quad [Q_A];
   else if (face == h_quad [Q_C]) return h_quad [Q_D];
   else if (face == h_quad [Q_D]) return h_quad [Q_C];
   else if (face == h_quad [Q_E]) return h_quad [Q_F];
   else if (face == h_quad [Q_F]) return h_quad [Q_F];
   else                           return NULL;
}
// ========================================================= findQuad
Quad* Hexa::findQuad (Edge* ed1, Edge* ed2)
{
   for (int nro=0 ; nro<HQ_MAXI ; nro++)
       {
       if (   h_quad[nro]->indexEdge (ed1) >= 0
           && h_quad[nro]->indexEdge (ed2) >= 0)
          return h_quad [nro];
       }

   return NULL;
}
// ========================================================= findEdge
Edge* Hexa::findEdge (Vertex* v1, Vertex* v2)
{
   for (int nro=0 ; nro<HE_MAXI ; nro++)
       {
       if (   h_edge[nro]->index (v1) >= 0
           && h_edge[nro]->index (v2) >= 0)
          return h_edge [nro];
       }

   return NULL;
}
// ====================================================== opposedVertex
Vertex* Hexa::opposedVertex (Quad* quad, Vertex* vertex)
{
   if (quad==NULL || vertex==NULL)
      return NULL;

   int nv = quad->indexVertex (vertex);
   int nq = findQuad (quad);
   if (nv<0 || nq<0)
      return NULL;

   for (int nro=0 ; nro<HE_MAXI ; nro++)
       {
       Edge* edge = h_edge [nro];
       int   nv   = edge->index (vertex);
       if (nv>=0 && quad->indexEdge(edge) <0)
          return edge->getVertex (1-nv);
       }

   return NULL;
}
// ====================================================== perpendicularEdge
Edge* Hexa::perpendicularEdge (Quad* quad, Vertex* vertex)
{
   if (quad==NULL || vertex==NULL)
      return NULL;

   int nv = quad->indexVertex (vertex);
   int nq = findQuad (quad);
   if (nv<0 || nq<0)
      return NULL;

   for (int nro=0 ; nro<HE_MAXI ; nro++)
       {
       if (quad->indexEdge (h_edge[nro])<0 && h_edge[nro]->index(vertex)>=0)
          return h_edge [nro];
       }

   return NULL;
}
// ====================================================== perpendicularQuad
Quad* Hexa::perpendicularQuad (Quad* quad, Edge* edge)
{
   if (BadElement (quad) || BadElement (edge))
      return NULL;

   int qed = quad->indexEdge (edge);
   int ned = findEdge (edge);
   int nq  = findQuad (quad);
   if (qed <0 || ned<0 || nq<0)
      return NULL;

   for (int nro=0 ; nro<HQ_MAXI ; nro++)
       {
       if (nro != nq)
          {   
          Quad* face = h_quad[nro];
          if (EltIsValid(face) && face->indexEdge (edge)>=0)
             return face;
          }
       }
   return NULL;
}
// ============================================================  getQuad
Quad* Hexa::getQuad (int nro)
{
   Quad* elt = NULL;
   if (nro >=0 && nro < HQ_MAXI && el_status == HOK && h_quad [nro]->isValid())
      elt = h_quad [nro];

   return elt;
}
// ============================================================  getEdge
Edge* Hexa::getEdge (int nro)
{
   Edge* elt = NULL;
   if (nro >=0 && nro < HE_MAXI && el_status == HOK && h_edge [nro]->isValid())
      elt = h_edge [nro];

   return elt;
}
// ============================================================  getVertex
Vertex* Hexa::getVertex (int nro)
{
   Vertex* elt = NULL;
   if (nro >=0 && nro <  HV_MAXI && el_status == HOK && h_vertex [nro]->isValid())
      elt = h_vertex [nro];

   return elt;
}
// ============================================================  getCenter
double* Hexa::getCenter (double centre[])
{
   centre [dir_x] = centre [dir_y] = centre [dir_z] = 0;

   for (int nv=0 ; nv<HV_MAXI ; nv++)
       {
       centre [dir_x] += h_vertex[nv]->getX ();
       centre [dir_y] += h_vertex[nv]->getY ();
       centre [dir_z] += h_vertex[nv]->getZ ();
       }

   centre [dir_x] /= HV_MAXI;
   centre [dir_y] /= HV_MAXI;
   centre [dir_z] /= HV_MAXI;
   return centre;
}
// =============================================================== definedBy
bool Hexa::definedBy  (Vertex* v1, Vertex* v2)
{
   for (int n1=0 ; n1< HV_MAXI ; n1++)
       {
//              (   Diagonale        )  Dessus
       int n2 = (n1 + 2) MODULO HV_MAXI + HV_MAXI;
       if (   (v1 == h_vertex[n1] && v2 == h_vertex[n2])
           || (v1 == h_vertex[n2] && v2 == h_vertex[n1])) return true;
       }
   return false;
}
// =============================================================== definedBy
bool Hexa::definedBy  (Quad* qa, Quad* qb)
{
   if (qa==qb || BadElement (qa) || BadElement (qb))
       return false;

   bool p1 = false, p2 = false;
   for (int nq=0 ; nq< HQ_MAXI ; nq++)
       {
       if (qa == h_quad[nq])
          p1 = true;
       else if (qb == h_quad[nq])
          p2 = true;
       }
   return p1 && p2;
}
// =============================================================== setColor
void Hexa::setColor  (double val)
{
   for (int nc=0 ; nc< HV_MAXI ; nc++)
       h_vertex[nc] -> setColor (val);
}
// ============================================================== markElements
void Hexa::markElements  (int marque)
{
   for (int nc=0 ; nc< HQ_MAXI ; nc++) h_quad  [nc] -> setMark (marque);
   for (int nc=0 ; nc< HE_MAXI ; nc++) h_edge  [nc] -> setMark (marque);
   for (int nc=0 ; nc< HV_MAXI ; nc++) h_vertex[nc] -> setMark (marque);
}
// =============================================================== duplicate
void Hexa::duplicate  ()
{
   h_clone = new Hexa (GetClone (h_quad [Q_A]),
                       GetClone (h_quad [Q_B]),
                       GetClone (h_quad [Q_C]),
                       GetClone (h_quad [Q_D]),
                       GetClone (h_quad [Q_E]),
                       GetClone (h_quad [Q_F]));
}
END_NAMESPACE_HEXA
