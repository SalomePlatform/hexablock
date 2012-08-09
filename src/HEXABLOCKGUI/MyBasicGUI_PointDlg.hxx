// Copyright (C) 2009-2012  CEA/DEN, EDF R&D
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


#ifndef MYBASICGUI_POINTDLG_H
#define MYBASICGUI_POINTDLG_H




#include "MyGEOMBase_Skeleton.hxx"
#include "GEOM_GenericObjPtr.h"
#include <QMap>

class DlgRef_2Sel1Spin;
class DlgRef_3Spin;
class DlgRef_2Sel;
class DlgRef_1Sel3Spin;
class DlgRef_1Sel2Spin;

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
// #include <VTKViewer_ViewModel.h>



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
typedef class MyBasicGUI_PointDlg : public MyGEOMBase_Skeleton
{ 
  Q_OBJECT

public:
  MyBasicGUI_PointDlg( GeometryGUI*, QWidget* = 0, bool = false, Qt::WindowFlags = 0 );
  ~MyBasicGUI_PointDlg();

  bool                               acceptMouseEvent() const { return ( getConstructorId() == 0 );  }
  void                               OnPointSelected( const gp_Pnt& ); // called by BasicGUI::OnMousePress()

protected:
  // redefined from GEOMBase_Helper

  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                       isValid( QString& );
  virtual bool                       execute( ObjectList& );
  virtual void                       addSubshapesToStudy();

private:
  void                               Init();
  void                               enterEvent( QEvent* );
  double                             getParameter() const;
  double                             getUParameter() const;
  double                             getVParameter() const;
  void                               updateParamCoord(bool theIsUpdate);

private:
  GEOM::GeomObjPtr                   myEdge;
  GEOM::GeomObjPtr                   myFace;
  GEOM::GeomObjPtr                   myRefPoint; 
  GEOM::GeomObjPtr                   myLine1; 
  GEOM::GeomObjPtr                   myLine2;

  bool                               myBusy;

  DlgRef_3Spin*                      GroupXYZ;
  DlgRef_1Sel3Spin*                  GroupRefPoint;
  DlgRef_2Sel1Spin*                  GroupOnCurve;
  DlgRef_2Sel*                       GroupLineIntersection;
  DlgRef_1Sel2Spin*                  GroupOnSurface;
  
  QGroupBox*                         myCoordGrp;
  QLineEdit*                         myX;
  QLineEdit*                         myY;
  QLineEdit*                         myZ;

  

  QGroupBox*                         myParamGroup;
  QButtonGroup*                      myParamCoord;

  QMenu*                             myBtnPopup;
  QMap<QAction*, int>                myActions;

  TopAbs_ShapeEnum                   myNeedType;
  
private slots:
  void                               ClickOnOk();
  bool                               ClickOnApply();
  void                               ActivateThisDialog();
  void                               DeactivateActiveDialog();
  void                               SelectionIntoArgument();
  void                               SetEditCurrentArgument();
  void                               ConstructorsClicked( int );
  void                               ValueChangedInSpinBox( double );
  void                               SetDoubleSpinBoxStep( double );
  void                               ClickParamCoord( int );
  void                               CheckBoxClicked( int );
  void                               onBtnPopup( QAction* );
  void                               updateSize();




// HEXABLOCK
public:
  void setDocumentModel( HEXABLOCK::GUI::DocumentModel* m );
  void setPatternDataSelectionModel( HEXABLOCK::GUI::PatternDataSelectionModel* s );

  bool onAccept( const bool publish = true, const bool useTransaction = true );


protected:
  virtual bool eventFilter( QObject *obj, QEvent *event );
  virtual void hideEvent ( QHideEvent * event );
  virtual void showEvent ( QShowEvent * event );
  void _initInputWidget();
  void _initViewManager();

protected slots:
  void onSelectionChanged(  const QItemSelection& sel, const QItemSelection& unsel );
  void onWindowActivated( SUIT_ViewManager* vm );


private:
  HEXABLOCK::GUI::DocumentModel*              _documentModel;
  HEXABLOCK::GUI::PatternDataSelectionModel*  _patternDataSelectionModel;
  LightApp_SelectionMgr*                      _mgr;
  SUIT_ViewManager*                           _vtkVm;
  SUIT_ViewManager*                           _occVm;
//   QLineEdit*                                  _vertex_le;

  QObject*                                    _currentObj;
  bool                                        _selectionMutex;


} VertexAssocDialog;

  }
}

#endif // MYBASICGUI_POINTDLG_H
