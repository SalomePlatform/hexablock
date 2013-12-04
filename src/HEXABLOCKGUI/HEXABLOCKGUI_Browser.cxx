// Copyright (C) 2009-2013  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#include "HEXABLOCKGUI_Browser.hxx"
#include "HEXABLOCKGUI.hxx"

#include "Hex.hxx"
#include "HexDocument.hxx"

#include <LightApp_Module.h>
#include <LightApp_Study.h>
#include <LightApp_Application.h>
#include <LightApp_DataModel.h>
#include <LightApp_DataObject.h>

// --------------------------------------------------- classe SaLObject
class SalObject : public LightApp_DataObject 
{
public :
    SalObject (cpchar nom="") : LightApp_DataObject (NULL) 
              { obj_name = nom ; obj_entry = "6.6.66" ; }

    virtual ~SalObject     ()       { cout << "SalObject::delete" << endl ; }
    virtual  QString name  () const { return obj_name ; } 
    virtual  QString entry () const { return obj_entry ; } 
    void  rename (cpchar nom)       { obj_name = nom ; }

private :
    QString obj_name;
    QString obj_entry;
};
//--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8
// --------------------------------------------------- classe SaLModel
class SalModel : public LightApp_DataModel
{
public :
   SalModel (HEXABLOCKGUI* guy);
   void clear   ();
   void addItem (cpchar fils);

private :
   CAM_ModuleObject* obj_root;
};

// =================================================== Constructeur SalModel
SalModel::SalModel (HEXABLOCKGUI* guy)
        : LightApp_DataModel (guy)
{
/* **********************************************
   LightApp_Study* study    = (LightApp_Study*) guy->activeStudy();
   obj_root = createModuleObject (study->root());

   obj_root ->setDataModel (this);
   setRoot (obj_root);

   SalObject* obj1 = new  SalObject ("Document1");
   SalObject* obj2 = new  SalObject ("Document2");

   PutData (obj1->name ().toStdString());
   PutData (obj2->name ().toStdString());

   obj_root->appendChild (obj1);
   obj_root->appendChild (obj2);
   PutData (obj_root->childCount());

   ********************************************** */
}
// ================================================================ clear
void SalModel::clear ()
{
   const bool nodel = false;
   while (true)
         {
         SUIT_DataObject* obj = obj_root->firstChild ();
         if (obj==NULL)
            return;
         obj_root->removeChild (obj, nodel);
         cout << " Delete object " << obj->name ().toStdString()
              << endl;
         } 
}
// ================================================================ addItem
void SalModel::addItem (cpchar name)
{
   SalObject* obj1 = new  SalObject (name);
   obj_root->appendChild (obj1);
}
//--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8
// =================================================== Constructeur H.G.Browser
HEXABLOCKGUI_Browser::HEXABLOCKGUI_Browser (HEXABLOCKGUI* guy)
{
    hexa_gui   = guy;
    hexa_root  = HEXA_NS::Hex::getInstance ();
    data_model = NULL;
}

// =============================================================== rebuildTree
void HEXABLOCKGUI_Browser::rebuildTree ()
{
   if (data_model==NULL) 
      data_model = new SalModel (hexa_gui);

    data_model->clear ();

    int nbre = hexa_root->countDocument ();
    for (int nro=0 ; nro<nbre ; nro++)
        {
        Document* doc = hexa_root->getDocument (nro);
        cpchar    nom = doc->getName ();
        data_model->addItem (nom);
        }

    hexa_gui->updateObjBrowser( true );
}

