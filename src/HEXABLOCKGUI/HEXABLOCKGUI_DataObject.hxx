//  Copyright (C) 2009-2011  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#if !defined(HEXABLOCKGUI_DATAOBJECT_HXX)
#define HEXABLOCKGUI_DATAOBJECT_HXX

// #include <LightApp_DataObject.h>
#include <SalomeApp_DataObject.h>


#include <qstring.h>

class HEXABLOCKGUI_Document;

/*!
 * Class       : HEXABLOCKGUI_DataObject
 * Description : Data Object for data of ATOMIC component
 *               Using the same Data Object for Atoms and Molecules
 *               If it is an Atom then myIndex field is >= 0
 *               For Molecule myIndex equals to -1.
 */
class HEXABLOCKGUI_DataObject : public virtual LightApp_DataObject
// class HEXABLOCKGUI_DataObject : public virtual SalomeApp_DataObject
{
public:
  HEXABLOCKGUI_DataObject ( SUIT_DataObject*, HEXABLOCKGUI_Document* = 0, int = -1 );
  virtual ~HEXABLOCKGUI_DataObject();
    
  virtual QString entry() const;

  QString    name()    const;
  QPixmap    icon(const int = NameId)    const;
  QString    toolTip(const int = NameId) const;

  HEXABLOCKGUI_Document* document() const { return myDocument;  }
//   int        atomIndex()  const { return myIndex; }

  bool       isDocument() const;
//   bool       isAtom() const;

private:
  HEXABLOCKGUI_Document*  myDocument;
//   int                     myIndex;
};

class HEXABLOCKGUI_ModuleObject : public HEXABLOCKGUI_DataObject,
                                  public LightApp_ModuleObject
//                                   public SalomeApp_ModuleObject
{
public:
  HEXABLOCKGUI_ModuleObject ( CAM_DataModel*, SUIT_DataObject* = 0 );
//   HEXABLOCKGUI_ModuleObject ( SUIT_DataObject*  );

  virtual QString name()    const;
  QPixmap         icon(const int = NameId)    const;
  QString         toolTip(const int = NameId) const;
};

#endif // HEXABLOCKGUI_DATAOBJECT_H
