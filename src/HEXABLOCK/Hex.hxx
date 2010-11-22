
// class : La clase principale de Hexa

#ifndef __HEX_H
#define __HEX_H

#include "hexa_base.hxx"

BEGIN_NAMESPACE_HEXA

class Hex
{
public:
   int       countDocument ();
   Document* getDocument (int nro);
   void      removeDocument (Document* doc);

   Document* addDocument ();
   Document* loadDocument (const char* filename);

             //  Constructeur + destructeur
    Hex ();
   ~Hex ();

private:
   vector <Document*> liste_documents;
};

END_NAMESPACE_HEXA
#endif
