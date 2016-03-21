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


#ifndef MYGEOM_MYDLGREF_HXX
#define MYGEOM_MYDLGREF_HXX

#if defined WIN32
#  if defined MYDLGREF_EXPORTS || defined MyDlgRef_EXPORTS
#    define MYDLGREF_EXPORT __declspec( dllexport )
#  else
#    define MYDLGREF_EXPORT __declspec( dllimport )
#  endif
#else
#  define MYDLGREF_EXPORT
#endif


//////////////////////////////////////////
// DlgRef_1Sel2Spin
//////////////////////////////////////////

#include "ui_DlgRef_1Sel2Spin_QTD.h"

class MYDLGREF_EXPORT MyDlgRef_1Sel2Spin : public QWidget, 
                                       public Ui::MyDlgRef_1Sel2Spin_QTD
{
  Q_OBJECT

public:
  MyDlgRef_1Sel2Spin( QWidget* = 0, Qt::WindowFlags = 0 );
  ~MyDlgRef_1Sel2Spin();
};


//////////////////////////////////////////
// DlgRef_1Sel3Spin
//////////////////////////////////////////

#include "ui_DlgRef_1Sel3Spin_QTD.h"

class MYDLGREF_EXPORT MyDlgRef_1Sel3Spin : public QWidget, 
                                       public Ui::MyDlgRef_1Sel3Spin_QTD
{
  Q_OBJECT

public:
  MyDlgRef_1Sel3Spin( QWidget* = 0, Qt::WindowFlags = 0 );
  ~MyDlgRef_1Sel3Spin();
};

//////////////////////////////////////////
// DlgRef_2Sel1Spin
//////////////////////////////////////////

#include "ui_DlgRef_2Sel1Spin_QTD.h"

class MYDLGREF_EXPORT MyDlgRef_2Sel1Spin : public QWidget, 
                                       public Ui::MyDlgRef_2Sel1Spin_QTD
{
  Q_OBJECT

public:
 MyDlgRef_2Sel1Spin( QWidget* = 0, Qt::WindowFlags = 0 );
  ~MyDlgRef_2Sel1Spin();
};

//////////////////////////////////////////
// DlgRef_2Sel
//////////////////////////////////////////

#include "ui_DlgRef_2Sel_QTD.h"

class MYDLGREF_EXPORT MyDlgRef_2Sel : public QWidget, 
                                  public Ui::MyDlgRef_2Sel_QTD
{
  Q_OBJECT

public:
  MyDlgRef_2Sel( QWidget* = 0, Qt::WindowFlags = 0 );
  ~MyDlgRef_2Sel();
};

//////////////////////////////////////////
// DlgRef_3Spin
//////////////////////////////////////////

#include "ui_DlgRef_3Spin_QTD.h"

class MYDLGREF_EXPORT MyDlgRef_3Spin : public QWidget, 
                                   public Ui::MyDlgRef_3Spin_QTD
{
  Q_OBJECT

public:
  MyDlgRef_3Spin( QWidget* = 0, Qt::WindowFlags = 0 );
  ~MyDlgRef_3Spin();
};

//////////////////////////////////////////
// DlgRef_Skeleton
//////////////////////////////////////////

#include "ui_DlgRef_Skeleton_QTD.h"

class MYDLGREF_EXPORT MyDlgRef_Skeleton : public QWidget, 
                                      public Ui::MyDlgRef_Skeleton_QTD
{
  Q_OBJECT

public:
  MyDlgRef_Skeleton( QWidget* = 0, Qt::WindowFlags = 0 );
  ~MyDlgRef_Skeleton();
};

//////////////////////////////////////////
// Utility functions
//////////////////////////////////////////

namespace MyDlgRef
{
  MYDLGREF_EXPORT QString PrintDoubleValue( double, int = 16 );
};

#endif // GEOM_MYDLGREF_H
