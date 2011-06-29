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

#include "HEXABLOCKGUI_DataObject.hxx"
#include "HEXABLOCKGUI_Data.hxx"

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <CAM_DataModel.h>
#include <CAM_Module.h>

/*! Constructor */
HEXABLOCKGUI_DataObject::HEXABLOCKGUI_DataObject( SUIT_DataObject* parent,
					    HEXABLOCKGUI_Document* doc, 
					    int idx )
  : //SalomeApp_DataObject( parent ),
    LightApp_DataObject( parent ),
    CAM_DataObject( parent),
    myDocument( doc )//,
//     myIndex( idx )
{
}

/*! Destructor */
HEXABLOCKGUI_DataObject::~HEXABLOCKGUI_DataObject()
{
}

/*! Returns unique object ID */
//  QString id = "root";
//   if ( myMolecule )
//     id = QString::number( myMolecule->id() ); 
//   if ( myIndex >= 0 )
//     id += QString( "_%1" ).arg( QString::number( myMolecule->atomId( myIndex ) ) ); 
//   return QString( "ATOMICGUI_%1" ).arg( id );

QString HEXABLOCKGUI_DataObject::entry() const
{
  QString id = "HEXA_ENTRY";
//   QString id = dynamic_cast<LightApp_DataObject*>( parent() )->entry();
//   if ( myDocument )
//     id = "0:4:3:1";
  if ( myDocument )
//     id = "0:4:3:1";
    id += QString( ":%1" ).arg( QString::number(myDocument->id()) );
//     id += QString( "0:%1" ).arg( QString::number( myDocument->id() ) );
//       id = "0:1:1:1";
//   else 
//       id = "0:1:1";
//     id = QString::number( myDocument->id() );

//   if ( myIndex >= 0 )
//     id += QString( "_%1" ).arg( QString::number( myMolecule->atomId( myIndex ) ) ); 
//   return QString( "HEXABLOCKGUI_%1" ).arg( id );
//   return QString( "%1" ).arg( id );
  return id;

//   QString entry = "0:1:1";
//   if ( myDocument )
//     entry = "0:1:1:1";
//   return entry;
}

/*! Returns a name of the Data Object. */
QString HEXABLOCKGUI_DataObject::name() const
{
  QString n;
  if ( myDocument )
    return myDocument->name();
//     n = myIndex < 0 ? myMolecule->name() 
//                     : myMolecule->atomName( myIndex );
//   return n;
}

/*! Returns an icon of the Data Object. */
QPixmap HEXABLOCKGUI_DataObject::icon(const int column) const
{
  static QPixmap pxdoc = SUIT_Session::session()->resourceMgr()->loadPixmap( "HEXABLOCK", QObject::tr( "ICON_DOCUMENT" ), false );
  static QPixmap pxatom = SUIT_Session::session()->resourceMgr()->loadPixmap( "HEXABLOCK", QObject::tr( "ICON_ATOM" ), false );
  static QPixmap nullpx;
//   return column == NameId ? ( myIndex < 0 ? pxmole : pxatom ) : nullpx;
  return column == NameId ? pxdoc : nullpx;
}

/*! Returns a tooltip for the object (to be displayed in the Object Browser). */
QString HEXABLOCKGUI_DataObject::toolTip(const int) const
{
  QString tt;
//   if ( myDocument )
//     tt = myIndex < 0 ? QString( QObject::tr( "ATOMIC_MOLECULE" ) + ": %1" ).arg( myMolecule->name() )
//       : QString( QObject::tr( "ATOMIC_ATOM" ) + ": %1: %2,%3,%4" ).
//       arg( myMolecule->atomName( myIndex ) ).
//       arg( myMolecule->atomX( myIndex ) ).
//       arg( myMolecule->atomY( myIndex ) ).
//       arg( myMolecule->atomZ( myIndex ) );
  return tt;
}

/*! Returns true if the Data Object corresponds to a Molecule. */
bool HEXABLOCKGUI_DataObject::isDocument() const
{
//   return myMolecule && myIndex < 0;
  return true;
}

/*! Returns true if the Data Object corresponds to an Atom. */
// bool HEXABLOCKGUI_DataObject::isAtom() const
// {
//   return myMolecule && myIndex >= 0;
// }

/*! Constructor */
HEXABLOCKGUI_ModuleObject::HEXABLOCKGUI_ModuleObject( CAM_DataModel* dm, SUIT_DataObject* parent )
// HEXABLOCKGUI_ModuleObject::HEXABLOCKGUI_ModuleObject( SUIT_DataObject* parent )
//   :SalomeApp_ModuleObject( parent )
: HEXABLOCKGUI_DataObject( parent ),
  LightApp_ModuleObject( dm, parent ),
  CAM_DataObject( parent )
{
}

/*! Returns a name of the root object. */
QString HEXABLOCKGUI_ModuleObject::name() const
{
  return CAM_ModuleObject::name();
}

/*! Returns an icon of the root object. */
QPixmap HEXABLOCKGUI_ModuleObject::icon(const int column) const
{
  QPixmap px;
  if ( column == NameId && dataModel() ) {
    QString anIconName = dataModel()->module()->iconName();
    if ( !anIconName.isEmpty() )
      px = SUIT_Session::session()->resourceMgr()->loadPixmap( "HEXABLOCK", anIconName, false );
  }
  return px;
}

/*! Returns a tooltip of the root object. */
QString HEXABLOCKGUI_ModuleObject::toolTip(const int) const
{
  return QObject::tr( "HEXABLOCK_ROOT_TOOLTIP" );
}
