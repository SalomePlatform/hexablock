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

#include "HEXABLOCKGUI_Data.hxx"
#include <qobject.h>

int HEXABLOCKGUI_Document::myMaxId       = 0;
// int HEXABLOCKGUI_Document::Atom::myMaxId = 0;

/*! Constructor of Atom */
// HEXABLOCKGUI_Document::Atom::Atom()
//   : myName( QString::null ), myX( 0. ), myY( 0. ), myZ( 0. ) 
// {
//   myId = ++myMaxId;
// }

/*! Constructor of Atom */
// HEXABLOCKGUI_Document::Atom::Atom( const QString& name, 
// 				  const double x, 
// 				  const double y, 
// 				  const double z )
//   : myName( name ), myX( x ), myY( y ), myZ( z )
// {
//   myId = ++myMaxId;
// }

/*! Constructor of Molecule */
HEXABLOCKGUI_Document::HEXABLOCKGUI_Document( const QString& name, int id )
{
//   myId = ++myMaxId;
  myId = id ;
  myName = name.isEmpty() ? QObject::tr( "HEXABLOCKGUI_NONAME" ) : name;
}

/*! Destructor of Molecule */
HEXABLOCKGUI_Document::~HEXABLOCKGUI_Document()
{
}

/*! Adds an atom to the molecule */
// void HEXABLOCKGUI_Document::addAtom( const QString& atom, 
// 				    const double   x, 
// 				    const double   y, 
// 				    const double   z )
// {
//   myAtoms.append( Atom( atom, x, y, z ) ); 
// }

/*! Removes an atom from molecule by index */
// void HEXABLOCKGUI_Document::deleteAtom( const int index )
// {
//   if ( index >= 0 and index < myAtoms.count() )
//     myAtoms.removeAt( index );
// }

/*! Gets an atom id by index */
// int HEXABLOCKGUI_Document::atomId( const int index ) const
// {
//   if ( index >= 0 and index < myAtoms.count() )
//     return myAtoms[ index ].id();
//   return 0;
// }

/*! Gets an atom name by index */
// QString HEXABLOCKGUI_Document::atomName( const int index ) const
// {
//   QString n;
//   if ( index >= 0 and index < myAtoms.count() )
//     n = myAtoms[ index ].name();
//   return n;
// }

/*! Gets an atom x coordinate by index */
// double HEXABLOCKGUI_Document::atomX( const int index ) const
// {
//   if ( index >= 0 and index < myAtoms.count() )
//     return myAtoms[ index ].x();
//   return 0;
// }

/*!  Gets an atom y coordinate by index */
// double HEXABLOCKGUI_Document::atomY( const int index ) const
// {
//   if ( index >= 0 and index < myAtoms.count() )
//     return myAtoms[ index ].y();
//   return 0;
// }

/*! Gets an atom z coordinate by index */
// double HEXABLOCKGUI_Document::atomZ( const int index ) const
// {
//   if ( index >= 0 and index < myAtoms.count() )
//     return myAtoms[ index ].z();
//   return 0;
// }

/*! Sets a name to the molecule or atom */
void HEXABLOCKGUI_Document::setName( const QString& name, const int index )
{
//   if ( index < 0 )
    myName = name;
//   else if ( index >= 0 and index < myAtoms.count() )
//     myAtoms[ index ].myName = name;
}
