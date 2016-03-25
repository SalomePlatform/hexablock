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


#include "MyDlgRef.hxx"


//////////////////////////////////////////
// MyDlgRef_1Sel2Spin
//////////////////////////////////////////

MyDlgRef_1Sel2Spin::MyDlgRef_1Sel2Spin( QWidget* parent, Qt::WindowFlags f )
: QWidget( parent, f )
{
  setupUi( this );
}

MyDlgRef_1Sel2Spin::~MyDlgRef_1Sel2Spin()
{
}

//////////////////////////////////////////
// MyDlgRef_1Sel3Spin
//////////////////////////////////////////

MyDlgRef_1Sel3Spin::MyDlgRef_1Sel3Spin( QWidget* parent, Qt::WindowFlags f )
: QWidget( parent, f )
{
  setupUi( this );
}

MyDlgRef_1Sel3Spin::~MyDlgRef_1Sel3Spin()
{
}

//////////////////////////////////////////
// MyDlgRef_2Sel1Spin
//////////////////////////////////////////

MyDlgRef_2Sel1Spin::MyDlgRef_2Sel1Spin( QWidget* parent, Qt::WindowFlags f )
: QWidget( parent, f )
{
  setupUi( this );
}

MyDlgRef_2Sel1Spin::~MyDlgRef_2Sel1Spin()
{
}

//////////////////////////////////////////
// MyDlgRef_2Sel
//////////////////////////////////////////

MyDlgRef_2Sel::MyDlgRef_2Sel( QWidget* parent, Qt::WindowFlags f )
: QWidget( parent, f )
{
  setupUi( this );
}

MyDlgRef_2Sel::~MyDlgRef_2Sel()
{
}

//////////////////////////////////////////
// MyDlgRef_3Spin
//////////////////////////////////////////

MyDlgRef_3Spin::MyDlgRef_3Spin( QWidget* parent, Qt::WindowFlags f )
: QWidget( parent, f )
{
  setupUi( this );
}

MyDlgRef_3Spin::~MyDlgRef_3Spin()
{
}

//////////////////////////////////////////
// MyDlgRef_Skeleton
//////////////////////////////////////////

MyDlgRef_Skeleton::MyDlgRef_Skeleton( QWidget* parent, Qt::WindowFlags f )
: QWidget( parent, f )
{
  setupUi( this );
}

MyDlgRef_Skeleton::~MyDlgRef_Skeleton()
{
}

//////////////////////////////////////////
// Utility functions
//////////////////////////////////////////

QString MyDlgRef::PrintDoubleValue( double theValue, int thePrecision )
{
  const double prec = 1e-12;
  
  if ( qAbs(theValue) < prec )
    return "0";

  QString aRes = QLocale().toString( theValue, thePrecision >= 0 ? 'f' : 'g', qAbs( thePrecision ) );

  if ( prec > 0 ) {
    int p = 0;
    while ( p < thePrecision ) {
      QString aRes = QLocale().toString( theValue, thePrecision >= 0 ? 'f' : 'g', qAbs( p++ ) );
      double v = aRes.toDouble();
      double err = qAbs( theValue - v );
      if ( err > 0 && err <= prec )
        break;
    }
  }

  // remove trailing zeroes

  QRegExp expre( QString( "(%1|%2)[+-]?[0-9]+$" ).arg( QLocale().exponential().toLower(), 
						       QLocale().exponential().toUpper() ) );

  int idx = aRes.indexOf( expre );
  QString aResExp = "";
  if ( idx >= 0 ) {
    aResExp = aRes.mid( idx );
    aRes = aRes.left( idx );
  }

  if ( aRes.contains( QLocale().decimalPoint() ) )
    aRes.remove( QRegExp( QString( "(\\%1|0)0*$" ).arg( QLocale().decimalPoint() ) ) );

  return aRes == "-0" ? QString( "0" ) : aRes + aResExp;
}
