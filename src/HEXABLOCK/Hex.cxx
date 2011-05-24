
// C++ : La clase principale de Hexa

#include "Hex.hxx"

#include "HexDocument.hxx"
#include <clocale>

BEGIN_NAMESPACE_HEXA

// ======================================================== Constructeur
Hex::Hex ()
{
   setlocale (LC_NUMERIC, "C");
}
// ======================================================== Destructeur
Hex::~Hex ()
{
   int nbre = liste_documents.size();
   for (int nd=0 ; nd<nbre ; nd++) 
       delete liste_documents [nd];
}
// ======================================================== countDocument
int Hex::countDocument ()
{
   return liste_documents.size();
}
// ======================================================== getDocument
Document* Hex::getDocument (int nro)
{
   if (nro<0 || nro>=(int)liste_documents.size())
      return NULL;
   
   return liste_documents [nro];
}
// ======================================================== removeDocument
void Hex::removeDocument (Document* doc)
{
   int nbre = liste_documents.size();
   for (int nd=0 ; nd<nbre ; nd++) 
       {
       if (doc == liste_documents [nd])
          {
          liste_documents.erase (liste_documents.begin()+nd);
          delete doc;
          return;
          }
       }
                      // Pas trouve dans la liste. On detruit quand meme
    delete doc;
}
// ======================================================== addDocument
Document* Hex::addDocument ()
{
   char   un_nom [40];
   cpchar nomdoc = get_temp_name ("HexaTempo%02d.dat", un_nom);

   Document* doc = new Document (nomdoc);
   
   liste_documents.push_back (doc);
   return doc;
}
// ======================================================== loadDocument
Document* Hex::loadDocument (const char* filename)
{
   Document* doc = new Document (filename);
   
   liste_documents.push_back (doc);
   doc->loadXml ();
   return doc;
}
END_NAMESPACE_HEXA
