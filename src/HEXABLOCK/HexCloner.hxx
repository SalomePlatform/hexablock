
// class : Le Reproducteur de blocks

#ifndef __CLONER_H
#define __CLONER_H

#include "hexa_base.hxx"

#include <map>

BEGIN_NAMESPACE_HEXA

class Cloner 
{
public:
    Cloner (Matrix* mat);

    void erase ();

    Hexa*     clonerHexa     (Hexa*     orig);
    Quad*     clonerQuad     (Quad*     orig);
    Edge*     clonerEdge     (Edge*     orig);
    Vertex*   clonerVertex   (Vertex*   orig);
    Elements* clonerElements (Elements* orig);

private:
    Matrix* matrice;
    std::map <Hexa*,   Hexa*>   clone_hexa;
    std::map <Quad*,   Quad*>   clone_quad;
    std::map <Edge*,   Edge*>   clone_edge;
    std::map <Vertex*, Vertex*> clone_vertex;
};
END_NAMESPACE_HEXA
#endif
