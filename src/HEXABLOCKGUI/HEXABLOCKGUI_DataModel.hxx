//  Copyright (C) 2006-2010  CEA/DEN, EDF R&D
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

#ifndef _HEXABLOCKGUI_DATAMODEL_HXX_
#define _HEXABLOCKGUI_DATAMODEL_HXX_

#include "HEXABLOCKGUI_Data.hxx"
// #include <LightApp_DataModel.h>
#include <SalomeApp_DataModel.h>
#include <qlist.h>
#include "HexDocument.hxx"

class CAM_Module;
class LightApp_Study;
class LightApp_DataObject;
class HEXABLOCKGUI_DataObject;

/*!
 * Class       : HEXABLOCKGUI_DataModel
 * Description : Data Model of HEXABLOCK component
 */
class HEXABLOCKGUI_DataModel : public LightApp_DataModel
// class HEXABLOCKGUI_DataModel : public SalomeApp_DataModel
{
//   Q_OBJECT

public:
  HEXABLOCKGUI_DataModel ( CAM_Module* );
  virtual ~HEXABLOCKGUI_DataModel();

  virtual bool          open   ( const QString&, CAM_Study*, QStringList );
  virtual bool          save   ( QStringList& );
  virtual bool          saveAs ( const QString&, CAM_Study*, QStringList& );
  virtual bool          close  ();
  virtual bool          create ( CAM_Study* );

  virtual bool          isModified () const;
  virtual bool          isSaved    () const;

  bool                  importFile ( const QString&, CAM_Study* = 0 );
  bool                  exportFile ( const QString& = QString::null );
  bool                  createDocument (HEXA_NS::Document* doc);
//   bool                  addAtom    ( const QString&, const QString&, 
// 				     const double, const double, const double );
  bool                  renameObj  ( const QString&, const QString& );
  bool                  deleteObjs ( const QStringList& );

  HEXABLOCKGUI_DataObject* findObject   ( const QString& ); 
  HEXABLOCKGUI_DataObject* findDocument ( const QString& ); 

protected:
  virtual void          build();

private:
  void _createDocument ();
  QString                              myStudyURL;
//   QList<HEXABLOCKGUI_AtomicMolecule>      myMolecules;
  QList<HEXABLOCKGUI_Document>      myDocuments;
};

#endif //_HEXABLOCKGUI_DATAMODEL_HXX_

