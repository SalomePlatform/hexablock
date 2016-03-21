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


#ifndef MYBASICGUI_POINTDLG_H
#define MYBASICGUI_POINTDLG_H

#include "HEXABLOCKGUI_Export.hxx"

#include "MyGEOMBase_Skeleton.hxx"
#include <QMap>

class MyDlgRef_2Sel1Spin;
class MyDlgRef_3Spin;
class MyDlgRef_2Sel;
class MyDlgRef_1Sel3Spin;
class MyDlgRef_1Sel2Spin;

class QLineEdit;
class QGroupBox;
class QButtonGroup;
class QMenu;
class QAction;

class gp_Pnt;

#include <QItemSelection>
#include <SUIT_ViewManager.h>
#include <SUIT_Session.h>
#include <OCCViewer_ViewWindow.h>
#include <OCCViewer_ViewManager.h>

#include "SVTK_Selection.h"
#include <SVTK_ViewModel.h>
namespace HEXABLOCK
{
  namespace GUI
  {
    class DocumentModel;
    class PatternDataSelectionModel;
//   }
// }


//=================================================================================
// class    : MyBasicGUI_PointDlg
// purpose  :
//=================================================================================
typedef class HEXABLOCK_EXPORT MyBasicGUI_PointDlg : public MyGEOMBase_Skeleton
{
  Q_OBJECT

public:
  MyBasicGUI_PointDlg( QWidget* = 0, Qt::WindowFlags = 0 );
  ~MyBasicGUI_PointDlg();

  bool                               acceptMouseEvent() const { return ( getConstructorId() == 0 );  }
  void                               OnPointSelected( const gp_Pnt& );

private:
  double                             getParameter() const;
  double                             getUParameter() const;
  double                             getVParameter() const;
  void                               updateParamCoord(bool theIsUpdate);
  bool                               myBusy;

  MyDlgRef_3Spin*                      GroupXYZ;
  MyDlgRef_1Sel3Spin*                  GroupRefPoint;
  MyDlgRef_2Sel1Spin*                  GroupOnCurve;
  MyDlgRef_2Sel*                       GroupLineIntersection;
  MyDlgRef_1Sel2Spin*                  GroupOnSurface;

  QGroupBox*                         myCoordGrp;
  QLineEdit*                         myX;
  QLineEdit*                         myY;
  QLineEdit*                         myZ;



  QGroupBox*                         myParamGroup;
  QButtonGroup*                      myParamCoord;

  QMenu*                             myBtnPopup;
  QMenu*                             myBtnPopup2;
  QAction*                           action_line1_edge;
  QAction*                           action_line1_wire;
  QAction*                           action_line2_edge;
  QAction*                           action_line2_wire;

private slots:
  void                               ConstructorsClicked( int );
  void                               SetDoubleSpinBoxStep( double );
  void                               ClickParamCoord( int );
  void                               onBtnPopup( QAction* );
  void                               updateSize();
 // void                               onRefPointChanged(const QString& pointName);
 // void                               onStartPointSelected(const QString& pointName);
  void                               onParamValueChanged(double newValue);
  void                               onXCoordChanged(double newValue);
  void                               onYCoordChanged(double newValue);
  void                               onZCoordChanged(double newValue);
  void                               onUpdateResults(const QString& data);

// HEXABLOCK
public:
  void clear();
  TopoDS_Vertex computeGeomVertex(bool preview=false);
  TopoDS_Vertex computeGeomPointXYZ();
  TopoDS_Vertex computeGeomPointRef();
  TopoDS_Vertex computeGeomPointEdge();
  TopoDS_Vertex computeGeomPointInt();
  TopoDS_Vertex computeGeomPointSurf();

protected:
  virtual void showEvent ( QShowEvent * event );

  virtual bool apply(QModelIndex& result);
  virtual void _initInputWidget( Mode editmode );
  void updateInputs(const int constructorId);

  TopoDS_Vertex currentVertex;
  QMap<int, double> paramInputValue;
  QMap<int, QVector3D*> coordsInputValue;

protected slots:
  void onWindowActivated( SUIT_ViewManager* vm );
  void updateHelpFileName();
  virtual void onSelectionChanged(  const QItemSelection& sel, const QItemSelection& unsel );

} VertexAssocDialog;

  }
}

#endif // MYBASICGUI_POINTDLG_H
