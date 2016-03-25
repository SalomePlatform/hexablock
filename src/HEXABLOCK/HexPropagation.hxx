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

// class : Ensembles de propagation

#ifndef __PROPAGATION_H
#define __PROPAGATION_H

#include "HexEltBase.hxx"

#include "HexDocument.hxx"
#include "HexEdge.hxx"
#include "HexXmlWriter.hxx"
#include "HexLaw.hxx"
#include "HexGlobale.hxx"

BEGIN_NAMESPACE_HEXA

class HexaExport Propagation : public EltBase 
{
public:
   const Edges& getEdges ()        { return prop_edges; }
   Law*  getLaw ();
   bool  getWay ()                 { return prop_way; }

   void  setWay (bool sens)        { prop_way = sens; }
   int   setLaw (Law* loi);

public:
    Propagation  (Document*  doc);
    void saveXml (XmlWriter* xml);
    void addEdge (Edge* arete);
    void majLaw  ();

private:
    Edges prop_edges;
    Law*  prop_law;
    bool  prop_way;
};
// =========================================================== Constructeur
inline Propagation::Propagation (Document* doc)
                  : EltBase (doc, EL_PROPAGATION)
{
    prop_law = NULL;
    prop_way = true;
}
// =========================================================== getLaw
inline Law* Propagation::getLaw ()
{
    return prop_law; 
/* **********************************  Modif Abu du 18/10/2010
    if (prop_law != NULL) 
       return prop_law; 

    if (prop_edges.size()==0) 
       return NULL;

    Document* root = prop_edges[0]->dad();
    return root->getLaw (0);
   ********************************** */
}
// =========================================================== setLaw
inline int Propagation::setLaw (Law* loi)
{
   prop_law = loi; 
   majLaw ();

   DumpStart  ("setLaw", loi);
   DumpEnd;
   return HOK;
}
// =========================================================== majLaw
inline void Propagation::majLaw ()
{
   int nbre = prop_edges.size();
   for (int nro=0 ; nro < nbre ; nro++)
        prop_edges[0]->setLaw (prop_law);
}
// =========================================================== addEdge
inline void Propagation::addEdge (Edge* arete)
{
   prop_edges.push_back (arete);

   if (prop_law == NULL)
       prop_law =  arete->getLaw();
}
// =========================================================== saveXml
inline void Propagation::saveXml (XmlWriter* xml)
{
    char   buffer[12];
    cpchar booleen [] = { "true", "false" };
    cpchar law = prop_law == NULL ? "default" : prop_law->getName();

    xml->openMark     ("Propagation");
    xml->addAttribute ("edge", prop_edges[0]->getName (buffer));
    xml->addAttribute ("law",  law);
    xml->addAttribute ("way",  booleen [prop_way]);
    xml->closeMark ();
}
END_NAMESPACE_HEXA

#endif
