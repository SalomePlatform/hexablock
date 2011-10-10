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
#include "HEXABLOCKGUI_DocumentPanel.hxx"
#include "HEXABLOCKGUI_DocumentGraphicView.hxx"
#include "HEXABLOCKGUI_SalomeTools.hxx"
#include "HEXABLOCKGUI.hxx"


#include <iostream>
#include <QtGui>
#include <QFlags>

#include <SalomeApp_Application.h>
#include <PyConsole_Console.h>


#include <Standard_GUID.hxx>
#include <TDF_Label.hxx>

#include <OCCViewer_ViewManager.h>
#include <OCCViewer_ViewModel.h>
// #include <OCCViewer_ViewWindow.h>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>


#include <SUIT_Session.h>
#include <SUIT_Desktop.h>
#include <SUIT_OverrideCursor.h>
#include <SUIT_MessageBox.h>
#include <SUIT_Session.h>
#include "SVTK_Selection.h"
#include <SVTK_ViewModel.h>

#include <GEOMBase.h>
#include <GEOMImpl_Types.hxx>
#include <BasicGUI_PointDlg.h>




#define VERTEX_COORD_MIN -1000000
#define VERTEX_COORD_MAX  1000000


using namespace std;
using namespace HEXABLOCK::GUI;




HexaBaseDialog::HexaBaseDialog( QWidget * parent, Qt::WindowFlags f ):
//   _model(0),
//   _selectionModel(0),
  QDialog(parent, f),
  _documentModel(0),
//   _patternDataModel(0),
//   _patternBuilderModel(0),
  _patternDataSelectionModel(0),
  _patternBuilderSelectionModel(0),
  _groupsSelectionModel(0),
  _meshSelectionModel(0),
  _currentObj(0),
  _expectedSelection(-1)
{
}

HexaBaseDialog::~HexaBaseDialog()
{
}

void HexaBaseDialog::_allowSelection()
{
  std::cout << "_allowSelection() _allowSelection() _allowSelection() "<< std::endl;
  std::cout << "_documentModel => "<< _documentModel << std::endl;
  if ( _documentModel ){
    _documentModel->disallowEdition();
  }
}

void HexaBaseDialog::_disallowSelection()
{
  std::cout << "_disallowSelection() _disallowSelection() _disallowSelection() "<< std::endl;
  std::cout << "_documentModel => "<< _documentModel << std::endl;
  if ( _documentModel ){
    _documentModel->allowEdition();
  }
  if ( _patternDataSelectionModel ){
    _patternDataSelectionModel->setAllSelection();
  }
//   if ( _patternBuilderSelectionModel ){
//     _patternBuilderSelectionModel->setAllSelection();
//   }
//   if ( _meshSelectionModel ){
//     _meshSelectionModel->setAllSelection();
//   }
}

void HexaBaseDialog::_setHexaSelectionOnly()
{
  std::cout << "_setHexaSelectionOnly() _setHexaSelectionOnly() _setHexaSelectionOnly() "<< std::endl;
  _allowSelection();
  if ( _patternDataSelectionModel ){
    _patternDataSelectionModel->setHexaSelection();
  }
  _expectedSelection = HEXA_TREE;
// //     _patternDataSelectionModel->clearFilters();
}


void HexaBaseDialog::_setQuadSelectionOnly()
{
  std::cout << "_setQuadSelectionOnly() _setQuadSelectionOnly() _setQuadSelectionOnly() "<< std::endl;
  _allowSelection();
  if ( _patternDataSelectionModel ){
    _patternDataSelectionModel->setQuadSelection();
  }
  _expectedSelection = QUAD_TREE;
}

void HexaBaseDialog::_setEdgeSelectionOnly()
{
  std::cout << "_setEdgeSelectionOnly() _setEdgeSelectionOnly() _setEdgeSelectionOnly() "<< std::endl;
  _allowSelection();
  if ( _patternDataSelectionModel ){
    _patternDataSelectionModel->setEdgeSelection();
  }
  _expectedSelection = EDGE_TREE;
}


void HexaBaseDialog::_setVertexSelectionOnly()
{
  std::cout << "_setVertexSelectionOnly() _setVertexSelectionOnly() _setVertexSelectionOnly() "<< std::endl;
  _allowSelection();
  if ( _patternDataSelectionModel ){
    std::cout << "_patternDataSelectionModel "<< std::endl;
    _patternDataSelectionModel->setVertexSelection();
  }
  _expectedSelection = VERTEX_TREE;
}



void HexaBaseDialog::_setElementsSelectionOnly()
{
// std::cout << "_setElementsSelectionOnly() _setElementsSelectionOnly() _setElementsSelectionOnly() "<< std::endl;
  _allowSelection();
  _expectedSelection = ELEMENTS_TREE;
}

void HexaBaseDialog::_setVectorSelectionOnly()
{
//   std::cout << "_setVectorSelectionOnly() _setVectorSelectionOnly() _setVectorSelectionOnly() "<< std::endl;
  _allowSelection();
  _expectedSelection = VECTOR_TREE;
}

void HexaBaseDialog::_setCylinderSelectionOnly()
{
  _allowSelection();
  _expectedSelection = CYLINDER_TREE;
}


void HexaBaseDialog::_setPipeSelectionOnly()
{
  _allowSelection();
  _expectedSelection = PIPE_TREE;
}

void HexaBaseDialog::_setLawSelectionOnly()
{
  _allowSelection();
  _expectedSelection = LAW_TREE;
}


void HexaBaseDialog::setDocumentModel(DocumentModel* m)
{
  _documentModel = m;
}


void HexaBaseDialog::setPatternDataSelectionModel(PatternDataSelectionModel* s)
{
  _patternDataSelectionModel = s;
  _patternDataSelectionModel->clearSelection();
  connect( _patternDataSelectionModel,
           SIGNAL( selectionChanged ( const QItemSelection &, const QItemSelection &) ),
           this,
           SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
//            SLOT( onPatternDataSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
}


void HexaBaseDialog::setPatternBuilderSelectionModel(PatternBuilderSelectionModel* s)
{
  _patternBuilderSelectionModel = s;
  _patternBuilderSelectionModel->clearSelection();

  connect( _patternBuilderSelectionModel,
           SIGNAL( selectionChanged( const QItemSelection &, const QItemSelection &) ),
           this,
           SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
//            SLOT( onPatternBuilderSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
}


void HexaBaseDialog::setMeshSelectionModel(QItemSelectionModel* s)
{
  _meshSelectionModel = s;
  _meshSelectionModel->clearSelection();

  connect( _meshSelectionModel,
           SIGNAL( selectionChanged( const QItemSelection &, const QItemSelection &) ),
           this,
           SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
//            SLOT( onMeshSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
}



void HexaBaseDialog::onSelectionChanged( const QItemSelection& sel, const QItemSelection& unsel )
{
  QModelIndexList l = sel.indexes();

  QLineEdit* currentLineEdit = dynamic_cast<QLineEdit*>(_currentObj);
  if ( !currentLineEdit ) return;

  if ( l.count() > 0 ){
    QModelIndex selected = l[0];
    int hexaType = selected.data(HEXA_TREE_ROLE).toInt();
    if ( _expectedSelection != hexaType ){
      std::cout << "BAD SELECTION!"<< std::endl;
      return;
    }
    currentLineEdit->setText( selected.data().toString() );
    _index[_currentObj] = selected;
  }
}
// QWidget* editor = qobject_cast<QWidget*>(sender());


// void HexaBaseDialog::onPatternDataSelectionChanged( const QItemSelection& sel, const QItemSelection& unsel )
// {
//     QModelIndexList l = _patternDataSelectionModel->selectedIndexes ();
// 
//     if ( l.count() > 0 ){
//       QModelIndex selected = l[0];
//       QLineEdit* currentLineEdit = dynamic_cast<QLineEdit*>(_currentObj);
//       if ( currentLineEdit ){
//         currentLineEdit->setText( selected.data().toString() );
//       }
//       _index[_currentObj] = selected;
//     }
// }


void HexaBaseDialog::onPatternDataSelectionChanged( const QItemSelection& sel, const QItemSelection& unsel )
{
    QModelIndexList l = _patternDataSelectionModel->selectedIndexes ();

    QLineEdit* currentLineEdit = dynamic_cast<QLineEdit*>(_currentObj);
    if ( !currentLineEdit ) return;

    if ( l.count() > 0 ){
      QModelIndex selected = l[0];
      int hexaType = selected.data(HEXA_TREE_ROLE).toInt();
      if ( _expectedSelection != hexaType ){
        std::cout << "BAD SELECTION!"<< std::endl;
        return;
      }
      currentLineEdit->setText( selected.data().toString() );
      _index[_currentObj] = selected;
    }
}

// void HexaBaseDialog::onPatternBuilderSelectionChanged( const QItemSelection& sel, const QItemSelection& unsel )
// {
//   QModelIndexList l = _patternBuilderSelectionModel->selectedIndexes();
//   if ( l.count() > 0 ){
//     QModelIndex selected = l[0];
//     QLineEdit* currentLineEdit = dynamic_cast<QLineEdit*>(_currentObj);
//     if ( currentLineEdit ){
//       currentLineEdit->setText( selected.data().toString() );
//     }
//     _index[_currentObj] = selected;
//   }
// }

void HexaBaseDialog::onPatternBuilderSelectionChanged( const QItemSelection& sel, const QItemSelection& unsel )
{
  QModelIndexList l = _patternBuilderSelectionModel->selectedIndexes();
  if ( l.count() > 0 ){
    QModelIndex selected = l[0];
    QLineEdit* currentLineEdit = dynamic_cast<QLineEdit*>(_currentObj);
    if ( currentLineEdit ){
      currentLineEdit->setText( selected.data().toString() );
    }
    _index[_currentObj] = selected;
  }
}

void HexaBaseDialog::onMeshSelectionChanged( const QItemSelection& sel, const QItemSelection& unsel )
{
  std:cout << " onMeshSelectionChanged onMeshSelectionChanged onMeshSelectionChanged" << std::endl;
  QModelIndexList l = _meshSelectionModel->selectedIndexes();
  if ( l.count() > 0 ){
    QModelIndex selected = l[0];
    QLineEdit* currentLineEdit = dynamic_cast<QLineEdit*>(_currentObj);
    if ( currentLineEdit ){
      currentLineEdit->setText( selected.data().toString() );
    }
    _index[_currentObj] = selected;
  }
}



// void HexaBaseDialog::installEventFilter()
// {
//   foreach(QLineEdit* le,  _hexaLineEdits)
//     le->installEventFilter(this);
//   foreach(QLineEdit* le,  _quadLineEdits)
//     le->installEventFilter(this);
//   foreach(QLineEdit* le,  _edgeLineEdits)
//     le->installEventFilter(this);
//   foreach(QLineEdit* le,  _vertexLineEdits)
//     le->installEventFilter(this);
//   foreach(QLineEdit* le,  _vectorLineEdits)
//     le->installEventFilter(this);
//   foreach(QLineEdit* le,  _cylinderLineEdits)
//     le->installEventFilter(this);
//   foreach(QLineEdit* le,  _pipeLineEdits)
//     le->installEventFilter(this);
//   foreach(QLineEdit* le,  _elementsLineEdits)
//     le->installEventFilter(this);
// }


bool HexaBaseDialog::eventFilter(QObject *obj, QEvent *event)
{
//     std::cout << "$$$$$$$$$$$$$$$$$$$$$ HexaBaseDialog::eventFilter " <<std::endl;
    if ( event->type() == QEvent::FocusIn ){ //QEvent::KeyPress) { 
//         std::cout << "$$$$$$$$$$$$$$$$$$$$$ FocusIn" <<std::endl;
        QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(obj);
        if ( lineEdit ){
//           std::cout << "$$$$$$$$$$$$$$$$$$$$$ lineEdit" <<std::endl;
          if (  _vertexLineEdits.contains( lineEdit ) )
              _setVertexSelectionOnly();
          else if ( _edgeLineEdits.contains( lineEdit ) )
              _setEdgeSelectionOnly();
          else if ( _quadLineEdits.contains( lineEdit ) )
              _setQuadSelectionOnly();
          else if ( _hexaLineEdits.contains( lineEdit ) )
              _setHexaSelectionOnly();
          else if ( _vectorLineEdits.contains( lineEdit ) )
              _setVectorSelectionOnly();
          else if ( _cylinderLineEdits.contains( lineEdit ) )
              _setCylinderSelectionOnly();
          else if ( _pipeLineEdits.contains( lineEdit ) )
              _setPipeSelectionOnly();
          else if ( _elementsLineEdits.contains( lineEdit ) )
              _setElementsSelectionOnly();
          else if ( _lawLineEdits.contains( lineEdit ) )
              _setLawSelectionOnly();
        }
        _currentObj = obj;
        return false;
    } else {
         // standard event processing
         return QObject::eventFilter(obj, event);
    }
}



// bool HexaBaseDialog::eventFilter(QObject *obj, QEvent *event)
// {
// //     std::cout << "HexaBaseDialog::eventFilter " <<std::endl;
//     if ( event->type() == QEvent::FocusIn ){ //QEvent::KeyPress) { 
//         QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(obj);
//         if ( lineEdit ){
//           if (    _vertexLineEdits.contains( lineEdit ) 
//               or  _edgeLineEdits.contains( lineEdit )
//               or  _quadLineEdits.contains( lineEdit )
//               or  _hexaLineEdits.contains( lineEdit )
//               or _vectorLineEdits.contains( lineEdit ) 
//               or _cylinderLineEdits.contains( lineEdit )
//               or _pipeLineEdits.contains( lineEdit )
//               or _elementsLineEdits.contains( lineEdit )
//               or _lawLineEdits.contains( lineEdit ) ){
//             _disallowEdition();
//           }
//         _currentObj = obj;
//         return false;
//       }
//     } else {
//          // standard event processing
//          return QObject::eventFilter(obj, event);
//     }
// }


// ------------------------- VERTEX ----------------------------------
VertexDialog::VertexDialog( QWidget* parent, Qt::WindowFlags f )
: HexaBaseDialog(parent, f),
  _value(0)
{
  setupUi( this );
  x_spb->setRange(VERTEX_COORD_MIN, VERTEX_COORD_MAX);
  y_spb->setRange(VERTEX_COORD_MIN, VERTEX_COORD_MAX);
  z_spb->setRange(VERTEX_COORD_MIN, VERTEX_COORD_MAX);
//   installEventFilter();
}


VertexDialog::~VertexDialog()
{
}


void VertexDialog::setValue(HEXA_NS::Vertex* v)
{
  _value = v;

  x_spb->setValue( v->getX() );
  y_spb->setValue( v->getY() );
  z_spb->setValue( v->getZ() );
}

HEXA_NS::Vertex* VertexDialog::getValue()
{
  return _value;
}


void VertexDialog::setIndex(const QModelIndex& i)
{
  _index = i;
}



void VertexDialog::accept()
{
    std::cout << "VertexDialog::accept " << std::endl;

    double newX = x_spb->value();
    double newY = y_spb->value();
    double newZ = z_spb->value();

    if ( _value ){
      std::cout << "_value " << std::endl;
      _value->setX( newX );
      _value->setY( newY );
      _value->setZ( newZ );
    }

    if ( _documentModel ){
      std::cout << "_documentModel" << std::endl;
      if ( _index.isValid() ){ //EDITION MODE
        std::cout << "_index.isValid()" << std::endl;
        bool ok = _documentModel->updateVertex( _index, newX, newY, newZ );
        std::cout << "ok = " << ok << std::endl;
        if ( ok ){
          //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "VERTEX UPDATED : %1" ).arg(_index.data().toString()) );
//           QDialog::accept();
          std::cout << "AAAAAAAAAAA " << std::endl;
        } else {
          SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT UPDATE VERTEX" ) );
        }
        //emit editingFinished();
      } else { //NEW MODE
        QModelIndex newIndex = _documentModel->addVertex( newX, newY, newZ );
        if ( newIndex.isValid() ){
          _value = newIndex.model()->data(newIndex, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
          //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "VERTEX BUILDED : %1" ).arg(newIndex.data().toString()) );
          QDialog::accept();
          if ( _patternDataSelectionModel ){
            const PatternDataModel* patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
            if ( patternDataModel ){
              newIndex = patternDataModel->mapFromSource(newIndex);
//               _patternDataSelectionModel->select( newIndex, QItemSelectionModel::Clear );
//               _patternDataSelectionModel->select( newIndex, QItemSelectionModel::Select );
              _patternDataSelectionModel->setCurrentIndex ( newIndex, QItemSelectionModel::Clear );
              _patternDataSelectionModel->setCurrentIndex ( newIndex, QItemSelectionModel::Select );
            }
          }
          //emit editingFinished();
        } else {
          SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT BUILD VERTEX" ) );
        }
      }
    }
}






// ------------------------- EDGE ----------------------------------
EdgeDialog::EdgeDialog( QWidget* parent, bool editMode, Qt::WindowFlags f ):
  HexaBaseDialog(parent, f),
  _value(0)
//   _currentObj(0)
{
  setupUi( this );

  _vertexLineEdits << vex_le_rb1 << v0_le_rb0 << v1_le_rb0;
  _vectorLineEdits << vec_le_rb1;
  
  if  ( editMode ){
    foreach(QLineEdit* le,  _vertexLineEdits)
      le->installEventFilter(this);
    foreach(QLineEdit* le,  _vectorLineEdits)
      le->installEventFilter(this);
    // select first Vertex
//     _currentObj = v0_le_rb0;
//     v0_le_rb0->setFocus();
    rb0->setFocusProxy( v0_le_rb0 );
    rb1->setFocusProxy( vex_le_rb1 );
    setFocusProxy( rb0 );

  } else {
    buttonBox->clear();
    rb1->hide();
    v0_le_rb0->setReadOnly(true);
    v1_le_rb0->setReadOnly(true);
  }

  // Default 
  rb0->click();
//   setFocusPolicy(Qt::NoFocus);
  
}


EdgeDialog::~EdgeDialog()
{
}

// void EdgeDialog::showEvent ( QShowEvent * event )
// {
//   // Default 
//   rb0->click();
//   // select first Vertex
//   _currentObj = v0_le_rb0;
//   v0_le_rb0->setFocus();
//   QDialog::showEvent ( event );
// }

void EdgeDialog::setValue(HEXA_NS::Edge* e)
{
  char pName[12];

  HEXA_NS::Vertex* v0 = e->getVertex(0);
  HEXA_NS::Vertex* v1 = e->getVertex(1);

  v0_le_rb0->setText( v0->getName(pName) );
  v1_le_rb0->setText( v1->getName(pName) );

//   buttonBox->clear();
//   rb1->hide();
//   v0_le_rb0->setReadOnly(true);
//   v1_le_rb0->setReadOnly(true);
  _value = e;
}


HEXA_NS::Edge* EdgeDialog::getValue()
{
  return _value;
}

void EdgeDialog::accept()
{
  SUIT_OverrideCursor wc;
  //if ( !_documentModel ) return;
  if ( !_patternDataSelectionModel )    return;
  if ( !_patternBuilderSelectionModel ) return;
  const PatternDataModel*    patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  if ( !patternDataModel ) return;
  if ( !patternBuilderModel ) return;


  QModelIndex iEdge;

  if ( rb0->isChecked() ){
    QModelIndex iv0 = patternDataModel->mapToSource( _index[v0_le_rb0] );
    QModelIndex iv1 = patternDataModel->mapToSource( _index[v1_le_rb0] );
    if ( iv0.isValid()
      && iv1.isValid() ){
      iEdge = _documentModel->addEdgeVertices( iv0, iv1 );
    }
  } else if ( rb1->isChecked() ){
    QModelIndex ivex = patternDataModel->mapToSource( _index[vex_le_rb1] );
    QModelIndex ivec = patternBuilderModel->mapToSource( _index[vec_le_rb1] );
    if ( ivex.isValid()
      && ivec.isValid() ){
      iEdge = _documentModel->addEdgeVector( ivex, ivec );
    }
  }

  if ( iEdge.isValid() ){
    QDialog::accept();
    _disallowSelection();
    iEdge = patternDataModel->mapFromSource(iEdge);
    _patternDataSelectionModel->setCurrentIndex ( iEdge, QItemSelectionModel::Clear );
    _patternDataSelectionModel->setCurrentIndex ( iEdge, QItemSelectionModel::Select );
    //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "EDGE ADDED : %1" ).arg(iEdge.data().toString()) );
    //emit editingFinished();
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "EDGE ADDED" ) );
  }

}


void EdgeDialog::reject()
{
  QDialog::reject();
  _disallowSelection();
}


// ------------------------- QUAD ----------------------------------
QuadDialog::QuadDialog( QWidget* parent, bool editMode, Qt::WindowFlags f ):
  HexaBaseDialog(parent, f),
  _value(0)
{
  setupUi( this );

  _vertexLineEdits << v0_le_rb0 << v1_le_rb0 << v2_le_rb0 << v3_le_rb0;
  _edgeLineEdits   << e0_le_rb1 << e1_le_rb1 << e2_le_rb1 << e3_le_rb1;


  if  ( editMode ){
    foreach(QLineEdit* le,  _vertexLineEdits)
      le->installEventFilter(this);
    foreach(QLineEdit* le,  _edgeLineEdits)
      le->installEventFilter(this);

    rb0->setFocusProxy( v0_le_rb0 );
    rb1->setFocusProxy( e0_le_rb1 );
    setFocusProxy( rb0 );
  } else {
    v0_le_rb0->setReadOnly(true);
    v1_le_rb0->setReadOnly(true);
    v2_le_rb0->setReadOnly(true);
    v3_le_rb0->setReadOnly(true);

    e0_le_rb1->setReadOnly(true);
    e1_le_rb1->setReadOnly(true);
    e2_le_rb1->setReadOnly(true);
    e3_le_rb1->setReadOnly(true);

    buttonBox->clear();
  }

  // Default 
  rb0->click();
}


QuadDialog::~QuadDialog()
{
}


// void QuadDialog::showEvent ( QShowEvent * event )
// {
//   // Default 
//   rb0->click();
//   // select first Vertex
//   _currentObj = v0_le_rb0;
//   v0_le_rb0->setFocus();
//   QDialog::showEvent ( event );
// }




void QuadDialog::setValue(HEXA_NS::Quad* q)
{
  char pName[12];

  Q_ASSERT( q->countEdge() == 4 );
  Q_ASSERT( q->countVertex() == 4 );

  //1) Vertices
  HEXA_NS::Vertex* v0 = q->getVertex(0);
  HEXA_NS::Vertex* v1 = q->getVertex(1);
  HEXA_NS::Vertex* v2 = q->getVertex(2);
  HEXA_NS::Vertex* v3 = q->getVertex(3);

  v0_le_rb0->setText( v0->getName(pName) );
  v1_le_rb0->setText( v1->getName(pName) );
  v2_le_rb0->setText( v2->getName(pName) );
  v3_le_rb0->setText( v3->getName(pName) );
//   v0_le_rb0->setReadOnly(true);
//   v1_le_rb0->setReadOnly(true);
//   v2_le_rb0->setReadOnly(true);
//   v3_le_rb0->setReadOnly(true);


  //2) Edges
//   QList<HEXA_NS::Edge*> edges;
  HEXA_NS::Edge* e0 = q->getEdge(0);
  HEXA_NS::Edge* e1 = q->getEdge(1);
  HEXA_NS::Edge* e2 = q->getEdge(2);
  HEXA_NS::Edge* e3 = q->getEdge(3);
//   edges << e0;
//   edges[0]; 

  e0_le_rb1->setText( e0->getName(pName) );
  e1_le_rb1->setText( e1->getName(pName) );
  e2_le_rb1->setText( e2->getName(pName) );
  e3_le_rb1->setText( e3->getName(pName) );
//   e0_le_rb1->setReadOnly(true);
//   e1_le_rb1->setReadOnly(true);
//   e2_le_rb1->setReadOnly(true);
//   e3_le_rb1->setReadOnly(true);

//   rb0->click();
//   buttonBox->clear();

  _value = q;

}

HEXA_NS::Quad* QuadDialog::getValue()
{
  return _value;
}


void QuadDialog::accept()
{
  SUIT_OverrideCursor wc;
  //if ( !_documentModel ) return;
  if ( !_patternDataSelectionModel ) return;
  const PatternDataModel* patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  if ( !patternDataModel ) return;

  QModelIndex iQuad;

  if ( rb0->isChecked() ){ //vertices
    QModelIndex iv0 = patternDataModel->mapToSource( _index[v0_le_rb0] );
    QModelIndex iv1 = patternDataModel->mapToSource( _index[v1_le_rb0] );
    QModelIndex iv2 = patternDataModel->mapToSource( _index[v2_le_rb0] );
    QModelIndex iv3 = patternDataModel->mapToSource( _index[v3_le_rb0] );

    if ( iv0.isValid()
      && iv1.isValid()
      && iv2.isValid()
      && iv3.isValid() ){
      iQuad = _documentModel->addQuadVertices( iv0, iv1, iv2, iv3 );
    }
  } else if ( rb1->isChecked() ){ //edges
    QModelIndex ie0 = patternDataModel->mapToSource( _index[e0_le_rb1] );
    QModelIndex ie1 = patternDataModel->mapToSource( _index[e1_le_rb1] );
    QModelIndex ie2 = patternDataModel->mapToSource( _index[e2_le_rb1] );
    QModelIndex ie3 = patternDataModel->mapToSource( _index[e3_le_rb1] );

    if ( ie0.isValid()
      && ie1.isValid()
      && ie2.isValid()
      && ie3.isValid() ){
      iQuad = _documentModel->addQuadEdges( ie0, ie1, ie2, ie3 );
    }
  }


  if ( iQuad.isValid() ){
    QDialog::accept();
    _value = iQuad.model()->data(iQuad, HEXA_DATA_ROLE).value<HEXA_NS::Quad *>();
    _disallowSelection();
    //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "QUAD BUILDED : %1" ).arg(iQuad.data().toString()) );
    iQuad = patternDataModel->mapFromSource( iQuad );
    _patternDataSelectionModel->setCurrentIndex ( iQuad, QItemSelectionModel::Clear );
    _patternDataSelectionModel->setCurrentIndex ( iQuad, QItemSelectionModel::Select );
    //emit editingFinished();
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT BUILD QUAD" ) );
  }
}


void QuadDialog::reject()
{
  QDialog::reject();
  _disallowSelection();
}





// ------------------------- HEXA ----------------------------------
HexaDialog::HexaDialog( QWidget* parent, bool editMode, Qt::WindowFlags f ):
  HexaBaseDialog(parent, f),
  _value(0)
{
  setupUi( this );

  _vertexLineEdits << v0_le_rb1 << v1_le_rb1 << v2_le_rb1 << v3_le_rb1 
                   << v4_le_rb1 << v5_le_rb1 << v6_le_rb1 << v7_le_rb1;
  _quadLineEdits << q0_le_rb0 << q1_le_rb0 << q2_le_rb0
                 << q3_le_rb0 << q4_le_rb0 << q5_le_rb0;

  if  ( editMode ){
    foreach(QLineEdit* le,  _vertexLineEdits)
      le->installEventFilter(this);
    foreach(QLineEdit* le,  _quadLineEdits)
      le->installEventFilter(this);

    rb0->setFocusProxy( q0_le_rb0 );
    rb1->setFocusProxy( v0_le_rb1 );
    setFocusProxy( rb0 );
  } else {
    q0_le_rb0->setReadOnly(true);
    q1_le_rb0->setReadOnly(true);
    q2_le_rb0->setReadOnly(true);
    q3_le_rb0->setReadOnly(true);
    q4_le_rb0->setReadOnly(true);
    q5_le_rb0->setReadOnly(true);
    buttonBox->clear();
  }

  // Default 
  rb0->click();
}


HexaDialog::~HexaDialog()
{

}

// void HexaDialog::showEvent ( QShowEvent * event )
// {
//   // Default 
//   rb0->click();
//   // select first quad
//   _currentObj = q0_le_rb0;
//   q0_le_rb0->setFocus();
//   QDialog::showEvent ( event );
// }


void HexaDialog::setValue(HEXA_NS::Hexa* h)
{
  char pName[12];
  HEXA_NS::Quad* q0 = h->getQuad(0);
  HEXA_NS::Quad* q1 = h->getQuad(1);
  HEXA_NS::Quad* q2 = h->getQuad(2);
  HEXA_NS::Quad* q3 = h->getQuad(3);
  HEXA_NS::Quad* q4 = h->getQuad(4);
  HEXA_NS::Quad* q5 = h->getQuad(5);

  q0_le_rb0->setText( q0->getName(pName) );
  q1_le_rb0->setText( q1->getName(pName) );
  q2_le_rb0->setText( q2->getName(pName) );
  q3_le_rb0->setText( q3->getName(pName) );
  q4_le_rb0->setText( q4->getName(pName) );
  q5_le_rb0->setText( q5->getName(pName) );


  HEXA_NS::Vertex* v0 = h->getVertex(0);
  HEXA_NS::Vertex* v1 = h->getVertex(1);
  HEXA_NS::Vertex* v2 = h->getVertex(2);
  HEXA_NS::Vertex* v3 = h->getVertex(3);
  HEXA_NS::Vertex* v4 = h->getVertex(4);
  HEXA_NS::Vertex* v5 = h->getVertex(5);
  HEXA_NS::Vertex* v6 = h->getVertex(6);
  HEXA_NS::Vertex* v7 = h->getVertex(7);


  v0_le_rb1->setText( v0->getName(pName) ); 
  v1_le_rb1->setText( v1->getName(pName) ); 
  v2_le_rb1->setText( v2->getName(pName) ); 
  v3_le_rb1->setText( v3->getName(pName) ); 
  v4_le_rb1->setText( v4->getName(pName) ); 
  v5_le_rb1->setText( v5->getName(pName) ); 
  v6_le_rb1->setText( v6->getName(pName) ); 
  v7_le_rb1->setText( v7->getName(pName) ); 

  _value = h;

//   buttonBox->clear();
//   q0_le_rb0->setReadOnly(true);
//   q1_le_rb0->setReadOnly(true);
//   q2_le_rb0->setReadOnly(true);
//   q3_le_rb0->setReadOnly(true);
//   q4_le_rb0->setReadOnly(true);
//   q5_le_rb0->setReadOnly(true);

}

HEXA_NS::Hexa* HexaDialog::getValue()
{
  return _value;
}

void HexaDialog::accept()
{
  SUIT_OverrideCursor wc;
  //if ( !_documentModel ) return;
  if ( !_patternDataSelectionModel ) return;
  const PatternDataModel* patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  if ( !patternDataModel ) return;


  QModelIndex iHexa;
  if ( rb0->isChecked() ){
    QModelIndex iq0 = patternDataModel->mapToSource( _index[q0_le_rb0] );
    QModelIndex iq1 = patternDataModel->mapToSource( _index[q1_le_rb0] );
    QModelIndex iq2 = patternDataModel->mapToSource( _index[q2_le_rb0] );
    QModelIndex iq3 = patternDataModel->mapToSource( _index[q3_le_rb0] );
    QModelIndex iq4 = patternDataModel->mapToSource( _index[q4_le_rb0] );
    QModelIndex iq5 = patternDataModel->mapToSource( _index[q5_le_rb0] );

    if ( iq0.isValid()
      && iq1.isValid()
      && iq2.isValid()
      && iq3.isValid()
      && iq4.isValid()
      && iq5.isValid() )
      iHexa = _documentModel->addHexaQuad( iq0, iq1, iq2, iq3, iq4, iq5 );

  } else if ( rb1->isChecked() ){
    QModelIndex iv0 = patternDataModel->mapToSource( _index[v0_le_rb1] );
    QModelIndex iv1 = patternDataModel->mapToSource( _index[v1_le_rb1] );
    QModelIndex iv2 = patternDataModel->mapToSource( _index[v2_le_rb1] );
    QModelIndex iv3 = patternDataModel->mapToSource( _index[v3_le_rb1] );
    QModelIndex iv4 = patternDataModel->mapToSource( _index[v4_le_rb1] );
    QModelIndex iv5 = patternDataModel->mapToSource( _index[v5_le_rb1] );
    QModelIndex iv6 = patternDataModel->mapToSource( _index[v6_le_rb1] );
    QModelIndex iv7 = patternDataModel->mapToSource( _index[v7_le_rb1] );

    if ( iv0.isValid()
      && iv1.isValid()
      && iv2.isValid()
      && iv3.isValid()
      && iv4.isValid()
      && iv5.isValid()
      && iv4.isValid()
      && iv5.isValid() )
      iHexa = _documentModel->addHexaVertices( iv0, iv1, iv2, iv3, iv4, iv5, iv6, iv7 );
  }

  if ( iHexa.isValid() ){
    QDialog::accept();
    _value = iHexa.model()->data(iHexa, HEXA_DATA_ROLE).value<HEXA_NS::Hexa*>();
    _disallowSelection();
    //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "HEXA BUILDED : %1" ).arg(iHexa.data().toString()) );
    iHexa = patternDataModel->mapFromSource( iHexa );
    _patternDataSelectionModel->setCurrentIndex ( iHexa, QItemSelectionModel::Clear );
    _patternDataSelectionModel->setCurrentIndex ( iHexa, QItemSelectionModel::Select );
    //emit editingFinished();
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT BUILD HEXA" ) );
  }
}


void HexaDialog::reject()
{
  QDialog::reject();
  _disallowSelection();
}



// ------------------------- VECTOR ----------------------------------
VectorDialog::VectorDialog( QWidget* parent, bool editMode, Qt::WindowFlags f ):
  HexaBaseDialog(parent, f),
  _value(0)
{
  setupUi( this );

  _vertexLineEdits << v0_le_rb1 << v1_le_rb1;

  if ( editMode ){
    v0_le_rb1->installEventFilter(this);
    v1_le_rb1->installEventFilter(this);

    rb0->setFocusProxy( dx_spb_rb0 );
    rb1->setFocusProxy( v0_le_rb1 );
    setFocusProxy( rb0 );
  } else {
    rb1->hide();
    dx_spb_rb0->setReadOnly(true);
    dy_spb_rb0->setReadOnly(true);
    dz_spb_rb0->setReadOnly(true);
    buttonBox->clear();
  }
  // Default 
  rb0->click();
//   setFocusProxy( rb1 );
//   setFocusProxy( v0_le );
}


VectorDialog::~VectorDialog()
{
}

// void VectorDialog::showEvent ( QShowEvent * event )
// {
//   // Default 
//   rb0->click();
// //   v0_le_rb1->setFocus();
//   QDialog::showEvent ( event );
// }


void VectorDialog::setValue(HEXA_NS::Vector* v)
{
  dx_spb_rb0->setValue( v->getDx() );
  dy_spb_rb0->setValue( v->getDy() );
  dz_spb_rb0->setValue( v->getDz() );

//   buttonBox->clear();
//   rb1->hide();
//   dx_spb_rb0->setReadOnly(true);
//   dy_spb_rb0->setReadOnly(true);
//   dz_spb_rb0->setReadOnly(true);

  _value = v;
}

HEXA_NS::Vector* VectorDialog::getValue()
{
  return _value;
}




void VectorDialog::accept()
{
  SUIT_OverrideCursor wc;
  if ( !_documentModel ) return;
  if ( !_patternDataSelectionModel )    return;
  if ( !_patternBuilderSelectionModel ) return;
  const PatternDataModel*    patternDataModel    = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  if ( !patternDataModel ) return;
  if ( !patternBuilderModel ) return;

  QModelIndex iVector;

  if ( rb0->isChecked() ){ //scalar
    double dx = dx_spb_rb0->value();
    double dy = dy_spb_rb0->value();
    double dz = dz_spb_rb0->value();

    iVector = _documentModel->addVector( dx, dy, dz );
  } else if ( rb1->isChecked() ){ //vertices
    QModelIndex iv0 = patternDataModel->mapToSource( _index[v0_le_rb1] );
    QModelIndex iv1 = patternDataModel->mapToSource( _index[v1_le_rb1] );

    if ( iv0.isValid()
      && iv1.isValid() ){
      iVector = _documentModel->addVectorVertices( iv0, iv1 );
    }
  }


  if ( iVector.isValid() ){
    QDialog::accept();
    _value = iVector.model()->data(iVector, HEXA_DATA_ROLE).value<HEXA_NS::Vector *>();
    _disallowSelection();
    //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "VECTOR BUILDED : %1" ).arg(iVector.data().toString()) );
    iVector = patternBuilderModel->mapFromSource( iVector );
    _patternBuilderSelectionModel->setCurrentIndex ( iVector, QItemSelectionModel::Clear );
    _patternBuilderSelectionModel->setCurrentIndex ( iVector, QItemSelectionModel::Select );
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT BUILD VECTOR" ) );
  }
}





// void VectorDialog::accept()
// {
//   SUIT_OverrideCursor wc;
// //if ( !_documentModel ) return;
// 
//   if ( _documentModel ){ //NEW MODE
//     QModelIndex newIndex = _documentModel->addVector( dx_spb_rb0->value(), dy_spb_rb0->value(),  dz_spb_rb0->value() );
//     if ( newIndex.isValid() ){
//       _value = newIndex.model()->data(newIndex, HEXA_DATA_ROLE).value<HEXA_NS::Vector *>();
//       QDialog::accept();
//       if ( _patternBuilderSelectionModel ){
//         const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
//         if ( patternBuilderModel ){
//           newIndex = patternBuilderModel->mapFromSource(newIndex);
//           _patternBuilderSelectionModel->select( newIndex, 
//                                             QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current );
//           _patternBuilderSelectionModel->setCurrentIndex ( newIndex, QItemSelectionModel::Current  );
//         }
//       }
//       //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "VECTOR BUILDED" ) );
//     } else {
//       SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT BUILD VECTOR" ) );
//     }
//   }
// 
// }



CylinderDialog::CylinderDialog( QWidget* parent, bool editMode, Qt::WindowFlags f )
: HexaBaseDialog(parent, f),
  _value(0)
{
  setupUi( this );

  _vertexLineEdits << vex_le;
  _vectorLineEdits << vec_le;

  if ( editMode ){
    vex_le->installEventFilter(this);
    vec_le->installEventFilter(this);

    setFocusProxy( vex_le );
  } else {
    vex_le->setReadOnly(true);
    vec_le->setReadOnly(true);
    r_spb->setReadOnly(true);
    h_spb->setReadOnly(true);
    buttonBox->clear();
  }

}

CylinderDialog::~CylinderDialog()
{
}

// void CylinderDialog::showEvent ( QShowEvent * event )
// {
//   // Default 
//   _currentObj = vex_le;
//   vex_le->setFocus();
//   QDialog::showEvent ( event );
// }


void CylinderDialog::setValue(HEXA_NS::Cylinder* c)
{
  char pName[12];

  HEXA_NS::Vertex* base      = c->getBase();
  HEXA_NS::Vector* direction = c->getDirection();
  double  r = c->getRadius();
  double  h = c->getHeight();

  vex_le->setText( base->getName(pName) );
  vec_le->setText( direction->getName(pName) );
  r_spb->setValue(r);
  h_spb->setValue(h);

//   vex_le->setReadOnly(true);
//   vec_le->setReadOnly(true);
//   r_spb->setReadOnly(true);
//   h_spb->setReadOnly(true);
//   buttonBox->clear();

  _value = c;
}

HEXA_NS::Cylinder* CylinderDialog::getValue()
{
  return _value;
}




void CylinderDialog::accept()
{
  SUIT_OverrideCursor wc;
  if ( !_documentModel ) return;
  if ( !_patternDataSelectionModel )    return;
  if ( !_patternBuilderSelectionModel ) return;
  const PatternDataModel*    patternDataModel    = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  if ( !patternDataModel ) return;
  if ( !patternBuilderModel ) return;

  QModelIndex iCyl;
  QModelIndex ivex = patternDataModel->mapToSource( _index[vex_le] );
  QModelIndex ivec = patternBuilderModel->mapToSource( _index[vec_le] );
  double r = r_spb->value();
  double h = h_spb->value();

  if ( ivex.isValid()
    && ivec.isValid() ){
    iCyl = _documentModel->addCylinder( ivex, ivec, r,  h );
  }

  if ( iCyl.isValid() ){
    QDialog::accept();
    _disallowSelection();
    //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "ADD CYLINDER DONE : %1" ).arg(iCyl.data().toString()) );
    iCyl = patternBuilderModel->mapFromSource(iCyl);
    _patternBuilderSelectionModel->setCurrentIndex ( iCyl, QItemSelectionModel::Clear );
    _patternBuilderSelectionModel->setCurrentIndex ( iCyl, QItemSelectionModel::Select );
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT ADD CYLINDER" ) );
  }
}




void CylinderDialog::reject()
{
  QDialog::reject();
  _disallowSelection();
}









PipeDialog::PipeDialog( QWidget* parent, bool editMode, Qt::WindowFlags f )
: HexaBaseDialog(parent, f),
  _value(0)
{
  setupUi( this );

  _vertexLineEdits << vex_le;
  _vectorLineEdits << vec_le;

  if ( editMode ){
    vex_le->installEventFilter(this);
    vec_le->installEventFilter(this);
    setFocusProxy( vex_le );
  } else {
    vex_le->setReadOnly(true);
    vec_le->setReadOnly(true);
    ir_spb->setReadOnly(true);
    er_spb->setReadOnly(true);
    h_spb->setReadOnly(true);
    buttonBox->clear();
  }

}

PipeDialog::~PipeDialog()
{
}


// void PipeDialog::showEvent ( QShowEvent * event )
// {
//   // Default 
//   _currentObj = vex_le;
//   vex_le->setFocus();
//   QDialog::showEvent ( event );
// }


void PipeDialog::setValue(HEXA_NS::Pipe* p)
{
  char pName[12];

  HEXA_NS::Vertex* base      = p->getBase();
  HEXA_NS::Vector* direction = p->getDirection();
  double  ir = p->getInternalRadius();
  double  er = p->getRadius();
  double  h  = p->getHeight();

  vex_le->setText( base->getName(pName) );
  vec_le->setText( direction->getName(pName) );
  ir_spb->setValue(ir);
  er_spb->setValue(er);
  h_spb->setValue(h);

//   vex_le->setReadOnly(true);
//   vec_le->setReadOnly(true);
//   ir_spb->setReadOnly(true);
//   er_spb->setReadOnly(true);
//   h_spb->setReadOnly(true);
//   buttonBox->clear();

  _value = p;
}

HEXA_NS::Pipe* PipeDialog::getValue()
{
  return _value;
}




void PipeDialog::accept()
{
  SUIT_OverrideCursor wc;
  //if ( !_documentModel ) return;
  if ( !_patternDataSelectionModel )    return;
  if ( !_patternBuilderSelectionModel ) return;
  const PatternDataModel*    patternDataModel    = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  if ( !patternDataModel ) return;
  if ( !patternBuilderModel ) return;

  QModelIndex iPipe;
  QModelIndex ivex = patternDataModel->mapToSource( _index[vex_le] );
  QModelIndex ivec = patternBuilderModel->mapToSource( _index[vec_le] );
  double ir = ir_spb->value();
  double er = er_spb->value();
  double h  = h_spb->value();

  if ( ivex.isValid()
    && ivec.isValid() ){
    iPipe = _documentModel->addPipe( ivex, ivec, ir, er, h );
  }

  if ( iPipe.isValid() ){
    QDialog::accept();
    _disallowSelection();
    //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "ADD PIPE DONE : %1" ).arg(iPipe.data().toString()) );
    iPipe = patternBuilderModel->mapFromSource( iPipe );
    _patternBuilderSelectionModel->setCurrentIndex ( iPipe, QItemSelectionModel::Clear );
    _patternBuilderSelectionModel->setCurrentIndex ( iPipe, QItemSelectionModel::Select );
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT ADD PIPE" ) );
  }
}




void PipeDialog::reject()
{
  QDialog::reject();
  _disallowSelection();
}





// ------------------------- MakeGridDialog ----------------------------------
//                  ( Cartesian, Cylindrical, Spherical )
MakeGridDialog::MakeGridDialog( QWidget* parent, bool editMode, Qt::WindowFlags f )
: HexaBaseDialog(parent, f)
{
  setupUi( this );

  _vertexLineEdits << vex_le_rb0 << vex_le_rb1 << vex_le_rb2;
  _vectorLineEdits << vec_le_rb0 << vec_x_le_rb1 << vec_z_le_rb1 << vec_le_rb2;

  if ( editMode ){
    foreach(QLineEdit* le,  _vertexLineEdits)
      le->installEventFilter(this);
    foreach(QLineEdit* le,  _vectorLineEdits)
      le->installEventFilter(this);

    rb0->setFocusProxy( vex_le_rb0 );
    rb1->setFocusProxy( vex_le_rb1 );
    setFocusProxy( rb0 );
  }
  // Default 
  rb0->click();
}


MakeGridDialog::~MakeGridDialog()
{
}


// void MakeGridDialog::showEvent ( QShowEvent * event )
// {
//   // Default 
//   rb0->click();
//   _currentObj = vex_le_rb0;
//   vex_le_rb0->setFocus();
//   QDialog::showEvent ( event );
// }


void MakeGridDialog::accept()
{

  SUIT_OverrideCursor wc;
  //if ( !_documentModel ) return;
  if ( !_patternDataSelectionModel )    return;
  if ( !_patternBuilderSelectionModel ) return;
  const PatternDataModel*    patternDataModel    = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  if ( !patternDataModel ) return;
  if ( !patternBuilderModel ) return;
  
  QModelIndex iNewElts;

  if ( rb0->isChecked() ){ //cartesian
    QModelIndex ivex_rb0 = patternDataModel->mapToSource( _index[vex_le_rb0] );
    QModelIndex ivec_rb0 = patternBuilderModel->mapToSource( _index[vec_le_rb0] );
    long nx = nx_spb_rb0->value();
    long ny = ny_spb_rb0->value();
    long nz = nz_spb_rb0->value();

    if ( ivex_rb0.isValid()
      && ivec_rb0.isValid() ){
      iNewElts = _documentModel->makeCartesian( ivex_rb0, ivec_rb0, nx, ny, nz );
    }

  } else if ( rb1->isChecked() ){ //cylindrical
    QModelIndex ivex_rb1  = patternDataModel->mapToSource( _index[vex_le_rb1] );
    QModelIndex ivecx_rb1 = patternBuilderModel->mapToSource( _index[vec_x_le_rb1] );
    QModelIndex ivecz_rb1 = patternBuilderModel->mapToSource( _index[vec_z_le_rb1] );

    double dr = dr_spb_rb1->value();
    double da = da_spb_rb1->value();
    double dl = dl_spb_rb1->value();
    double nr = nr_spb_rb1->value();
    double na = na_spb_rb1->value();
    double nl = nl_spb_rb1->value();
    bool fill = fill_cb_rb1->isChecked();

    if ( ivex_rb1.isValid()
      && ivecx_rb1.isValid()
      && ivecz_rb1.isValid() ){
      iNewElts = _documentModel->makeCylindrical( ivex_rb1,
                                                  ivecx_rb1, ivecz_rb1,
                                                  dr, da, dl, nr, na, nl, fill);
    }

  } else if ( rb2->isChecked() ){ //spherical
    QModelIndex ivex_rb2  = patternDataModel->mapToSource( _index[vex_le_rb2] );
    QModelIndex ivecx_rb2 = patternBuilderModel->mapToSource( _index[vec_le_rb2] );
    int nb = nb_spb_rb2->value();
    int k  = k_spb_rb2->value();

    if ( ivex_rb2.isValid()
      && ivecx_rb2.isValid() ){
      iNewElts = _documentModel->makeSpherical( ivex_rb2, ivecx_rb2, nb, k );
    }
  }


  if ( iNewElts.isValid() ){
    QDialog::accept();
    _disallowSelection();
    //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "MAKE GRID DONE : %1" ).arg(iNewElts.data().toString()) );
    iNewElts = patternBuilderModel->mapFromSource( iNewElts );
    _patternBuilderSelectionModel->setCurrentIndex ( iNewElts, QItemSelectionModel::Clear );
    _patternBuilderSelectionModel->setCurrentIndex ( iNewElts, QItemSelectionModel::Select );
    //emit editingFinished();
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE GRID" ) );
  }
}




void MakeGridDialog::reject()
{
  QDialog::reject();
  _disallowSelection();
}









MakeCylinderDialog::MakeCylinderDialog( QWidget* parent, bool editMode, Qt::WindowFlags f )
: HexaBaseDialog(parent, f)
{
  setupUi( this );

  _cylinderLineEdits << cyl_le;
  _vectorLineEdits << vec_le;

  if ( editMode ){
    cyl_le->installEventFilter(this);
    vec_le->installEventFilter(this);
    setFocusProxy( cyl_le );
  }
 
}

MakeCylinderDialog::~MakeCylinderDialog()
{
}

// void MakeCylinderDialog::showEvent ( QShowEvent * event )
// {
//   // Default 
//   _currentObj = cyl_le;
//   cyl_le->setFocus();
//   QDialog::showEvent ( event );
// }




void MakeCylinderDialog::accept()
{
  SUIT_OverrideCursor wc;
  //if ( !_documentModel ) return;
  if ( !_patternBuilderSelectionModel ) return;
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  if ( !patternBuilderModel ) return;

  QModelIndex iElts;
  QModelIndex icyl = patternBuilderModel->mapToSource( _index[cyl_le] );
  QModelIndex ivec = patternBuilderModel->mapToSource( _index[vec_le] );
  double nr = nr_spb->value();
  double na = na_spb->value();
  double nl = nl_spb->value();

  if ( icyl.isValid()
    && ivec.isValid() ){
    iElts = _documentModel->makeCylinder( icyl, ivec, nr, na, nl );
  }
  if ( iElts.isValid() ){
    QDialog::accept();
    _disallowSelection();
    //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "MAKE CYLINDER DONE : %1" ).arg(iElts.data().toString()) );
    iElts = patternBuilderModel->mapFromSource( iElts );
    _patternBuilderSelectionModel->setCurrentIndex ( iElts, QItemSelectionModel::Clear );
    _patternBuilderSelectionModel->setCurrentIndex ( iElts, QItemSelectionModel::Select );
    //emit editingFinished();
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE CYLINDER" ) );
  }
}




void MakeCylinderDialog::reject()
{
  QDialog::reject();
  _disallowSelection();
}






MakePipeDialog::MakePipeDialog( QWidget* parent, bool editMode, Qt::WindowFlags f )
: HexaBaseDialog(parent, f)
{
  setupUi( this );
  _pipeLineEdits << pipe_le;
  _vectorLineEdits << vec_le;

  if ( editMode ){
    pipe_le->installEventFilter(this);
    vec_le->installEventFilter(this);
    setFocusProxy( pipe_le );
  }
}

MakePipeDialog::~MakePipeDialog()
{
}

// void MakePipeDialog::showEvent ( QShowEvent * event )
// {
//   // Default 
//   _currentObj = pipe_le;
//   pipe_le->setFocus();
//   QDialog::showEvent ( event );
// }



void MakePipeDialog::accept()
{
  SUIT_OverrideCursor wc;
  //if ( !_documentModel ) return;
  if ( !_patternBuilderSelectionModel ) return;
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  if ( !patternBuilderModel ) return;

  QModelIndex iElts;
  QModelIndex ipipe = patternBuilderModel->mapToSource( _index[pipe_le] );
  QModelIndex ivec  = patternBuilderModel->mapToSource( _index[vec_le] );
  double nr = nr_spb->value();
  double na = na_spb->value();
  double nl = nl_spb->value();

  if ( ipipe.isValid()
    && ivec.isValid() ){
    iElts = _documentModel->makePipe( ipipe, ivec, nr, na, nl );
  }
  if ( iElts.isValid() ){
    QDialog::accept();
    _disallowSelection();
    //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "MAKE PIPE DONE : %1" ).arg(iElts.data().toString()) );
    iElts = patternBuilderModel->mapFromSource( iElts );
    _patternBuilderSelectionModel->setCurrentIndex ( iElts, QItemSelectionModel::Clear );
    _patternBuilderSelectionModel->setCurrentIndex ( iElts, QItemSelectionModel::Select );
    //emit editingFinished();
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE PIPE" ) );
  }
}




void MakePipeDialog::reject()
{
  QDialog::reject();
  _disallowSelection();
}








MakeCylindersDialog::MakeCylindersDialog( QWidget* parent, bool editMode, Qt::WindowFlags f )
: HexaBaseDialog(parent, f)
{
  setupUi( this );

  _cylinderLineEdits << cyl1_le << cyl2_le;

  if ( editMode ){
    cyl1_le->installEventFilter(this);
    cyl2_le->installEventFilter(this);
    setFocusProxy( cyl1_le );
  }

}

MakeCylindersDialog::~MakeCylindersDialog()
{
}

// void MakeCylindersDialog::showEvent ( QShowEvent * event )
// {
//   // Default 
//   _currentObj = cyl1_le;
//   cyl1_le->setFocus();
//   QDialog::showEvent ( event );
// }



void MakeCylindersDialog::accept()
{
  SUIT_OverrideCursor wc;
  //if ( !_documentModel ) return;
  if ( !_patternBuilderSelectionModel ) return;
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  if ( !patternBuilderModel ) return;

  QModelIndex iCrossElts;
  QModelIndex icyl1 = patternBuilderModel->mapToSource( _index[cyl1_le] );
  QModelIndex icyl2 = patternBuilderModel->mapToSource( _index[cyl2_le] );

  if ( icyl1.isValid()
    && icyl2.isValid() ){
    iCrossElts = _documentModel->makeCylinders( icyl1, icyl2 );
  }
  if ( iCrossElts.isValid() ){
    QDialog::accept();
    _disallowSelection();
    //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "MAKE CYLINDERS DONE : %1" ).arg(iCrossElts.data().toString()) );
    iCrossElts = patternBuilderModel->mapFromSource( iCrossElts );
    _patternBuilderSelectionModel->setCurrentIndex ( iCrossElts, QItemSelectionModel::Clear );
    _patternBuilderSelectionModel->setCurrentIndex ( iCrossElts, QItemSelectionModel::Select );
    //emit editingFinished();
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE CYLINDERS" ) );
  }
}


void MakeCylindersDialog::reject()
{
  QDialog::reject();
  _disallowSelection();
}




MakePipesDialog::MakePipesDialog( QWidget* parent, bool editMode, Qt::WindowFlags f )
: HexaBaseDialog(parent, f)
{
  setupUi( this );

  _pipeLineEdits << pipe1_le << pipe2_le;

  if ( editMode ){
    pipe1_le->installEventFilter(this);
    pipe2_le->installEventFilter(this);
    setFocusProxy( pipe1_le );
  }

}

MakePipesDialog::~MakePipesDialog()
{
}

// void MakePipesDialog::showEvent ( QShowEvent * event )
// {
//   // Default 
//   _currentObj = pipe1_le;
//   pipe1_le->setFocus();
//   QDialog::showEvent ( event );
// }


void MakePipesDialog::accept()
{
  SUIT_OverrideCursor wc;
  //if ( !_documentModel ) return;
  if ( !_patternBuilderSelectionModel ) return;
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  if ( !patternBuilderModel ) return;

  QModelIndex iCrossElts;
  QModelIndex ipipe1 = patternBuilderModel->mapToSource( _index[pipe1_le] );
  QModelIndex ipipe2 = patternBuilderModel->mapToSource( _index[pipe2_le] );

  if ( ipipe1.isValid()
    && ipipe2.isValid() ){
    iCrossElts = _documentModel->makeCylinders( ipipe1, ipipe2 );
  }
  if ( iCrossElts.isValid() ){
    QDialog::accept();
    _disallowSelection();
    //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "MAKE PIPES DONE : %1" ).arg(iCrossElts.data().toString()) );
    iCrossElts = patternBuilderModel->mapFromSource( iCrossElts );
    _patternBuilderSelectionModel->setCurrentIndex ( iCrossElts, QItemSelectionModel::Clear );
    _patternBuilderSelectionModel->setCurrentIndex ( iCrossElts, QItemSelectionModel::Select );
    //emit editingFinished();
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE PIPES" ) );
  }
}


void MakePipesDialog::reject()
{
  QDialog::reject();
  _disallowSelection();
}





RemoveHexaDialog::RemoveHexaDialog( QWidget* parent, bool editMode, Qt::WindowFlags f )
: HexaBaseDialog(parent, f)
{
  setupUi( this );

  _hexaLineEdits << hexa_le;
  if ( editMode ){
    hexa_le->installEventFilter(this); 
    setFocusProxy( hexa_le );
  }
}


RemoveHexaDialog::~RemoveHexaDialog()
{
}

// void RemoveHexaDialog::showEvent ( QShowEvent * event )
// {
//   // Default 
//   _currentObj = hexa_le;
//   hexa_le->setFocus();
//   QDialog::showEvent ( event );
// }


void RemoveHexaDialog::accept()
{
  SUIT_OverrideCursor wc;
  //if ( !_documentModel ) return;
  if ( !_patternDataSelectionModel ) return;
  const PatternDataModel*    patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  if ( !patternDataModel ) return;

  QModelIndex ihexa = patternDataModel->mapToSource( _index[hexa_le] );

  bool removed = false;
  if ( ihexa.isValid() ){
    if ( connected_cb->isChecked() ){
      removed = _documentModel->removeConnectedHexa( ihexa );
    } else {
      removed = _documentModel->removeHexa( ihexa );
    }
  }

  if ( removed == true ){
    QDialog::accept();
    _disallowSelection();
    //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "HEXA(S) REMOVED" ) );
    //emit editingFinished();
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT REMOVE HEXA" ) );
  }
}


void RemoveHexaDialog::reject()
{
  QDialog::reject();
  _disallowSelection();
}





PrismQuadDialog::PrismQuadDialog( QWidget* parent, bool editMode, Qt::WindowFlags f )
: HexaBaseDialog(parent, f)
{
  setupUi( this );

  _vectorLineEdits << vec_le;

  if ( editMode ){
    vec_le->installEventFilter(this);
    quads_lw->installEventFilter(this);
    setFocusProxy( quads_lw );
  }

  connect(add_pb, SIGNAL(clicked()), this, SLOT(addQuad()));
  connect(remove_pb, SIGNAL(clicked()), this, SLOT(removeQuad()));
  connect(clear_pb, SIGNAL(clicked()), this, SLOT(clearQuads()));
}


PrismQuadDialog::~PrismQuadDialog()
{
}

// void PrismQuadDialog::showEvent ( QShowEvent * event )
// {
//   // Default 
//   _currentObj = quads_lw;
//   quads_lw->setFocus();
//   QDialog::showEvent ( event );
// }


void PrismQuadDialog::addQuad()
{
  QModelIndexList iselections = _patternDataSelectionModel->selectedIndexes ();

  QListWidgetItem* item = NULL;
  foreach( const QModelIndex& isel, iselections ){
    item = new QListWidgetItem( isel.data().toString() );
    item->setData(  Qt::UserRole + 20, QVariant::fromValue<QModelIndex>(isel) );
    quads_lw->addItem(item);
    int r = quads_lw->count() - 1;
    quads_lw->setCurrentRow(r);
  }

}

// {
//   QString item = QInputDialog::getText(this, "Item",
//          "Enter new item");
//   item = item.simplified();
//   if (!item.isEmpty()) {
//     quads_lw->addItem(item);
//     int r = quads_lw->count() - 1;
//     quads_lw->setCurrentRow(r);
//   }
// 
// }

// void PrismQuadDialog::renameItem()
// {
//   QListWidgetItem *curitem = lw->currentItem();
//   int r = lw->row(curitem);
//   QString text = curitem->text();
//   QString item = QInputDialog::getText(this, "Item", 
//              "Enter new item", QLineEdit::Normal, text);
//   item = item.simplified();
//   if (!item.isEmpty()) {
//     lw->takeItem(r);
//     delete curitem;
//     lw->insertItem(r, item);
//     lw->setCurrentRow(r);
//   }
// }

void PrismQuadDialog::removeQuad()
{
  QListWidgetItem *item = quads_lw->currentItem();

  if (item) {
    int r = quads_lw->row(item);
    quads_lw->takeItem(r);
    delete item;
  }

}

void PrismQuadDialog::clearQuads()
{
  if (quads_lw->count() != 0)
    quads_lw->clear();
}


bool PrismQuadDialog::eventFilter(QObject *obj, QEvent *event)
{
  if ( ( obj == quads_lw ) and  ( event->type() == QEvent::FocusIn ) ){
    _setQuadSelectionOnly();
    _currentObj = obj;
    return false;
  } else {
    return HexaBaseDialog::eventFilter(obj, event);
  }
}


void PrismQuadDialog::accept()
{
  SUIT_OverrideCursor wc;
  //if ( !_documentModel ) return;
  if ( !_patternDataSelectionModel ) return;
  if ( !_patternBuilderSelectionModel ) return;
  const PatternDataModel*    patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  if ( !patternDataModel ) return;
  if ( !patternBuilderModel ) return;

  /////
  QModelIndexList iquads;
  QModelIndex     iquad; 
  QListWidgetItem* item = NULL;
  for ( int r = 0; r < quads_lw->count(); ++r){
    item = quads_lw->item(r);
    iquad = patternDataModel->mapToSource( item->data(Qt::UserRole + 20).value<QModelIndex>() );
    iquads << iquad;
  }
  QModelIndex ivec  = patternBuilderModel->mapToSource( _index[vec_le] );

  QModelIndex iElts;
  if ( ivec.isValid() ){
    int nb = nb_spb->value();
    if ( iquads.count() == 1 ){
      iElts = _documentModel->prismQuad( iquads[0], ivec, nb );
    } else if ( iquads.count() > 1 ){
      iElts = _documentModel->prismQuads( iquads, ivec, nb );
    }
  }

  if ( iElts.isValid() ){
    QDialog::accept();
    _disallowSelection();
    //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "PRISM QUAD(S) OK : %1" ).arg( iElts.data().toString()) );
    if ( _patternBuilderSelectionModel ){
      const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
      if ( patternBuilderModel ){
        iElts = patternBuilderModel->mapFromSource( iElts );
        _patternBuilderSelectionModel->setCurrentIndex ( iElts, QItemSelectionModel::Clear );
        _patternBuilderSelectionModel->setCurrentIndex ( iElts, QItemSelectionModel::Select );
      }
      //emit editingFinished();
    }
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT PRISM QUAD(S)" ) );
  }
}


void PrismQuadDialog::reject()
{
  QDialog::reject();
  _disallowSelection();
}





//JoinQuadDialog
JoinQuadDialog::JoinQuadDialog( QWidget* parent, bool editMode, Qt::WindowFlags f )
: HexaBaseDialog(parent, f)
{
  setupUi( this );

  _vertexLineEdits << vex0_le << vex1_le << vex2_le << vex3_le;
  _quadLineEdits   << quad_dest_le;

  if ( editMode ){
    vex0_le->installEventFilter(this);
    vex1_le->installEventFilter(this); 
    vex2_le->installEventFilter(this);
    vex3_le->installEventFilter(this);
    quad_dest_le->installEventFilter(this);
    quads_lw->installEventFilter(this);
    setFocusProxy( quads_lw );
  }

//   _currentObj = quads_lw;

  connect(add_pb, SIGNAL(clicked()), this, SLOT(addQuad()));
  connect(remove_pb, SIGNAL(clicked()), this, SLOT(removeQuad()));
  connect(clear_pb, SIGNAL(clicked()), this, SLOT(clearQuads()));
}


JoinQuadDialog::~JoinQuadDialog()
{
}

// void JoinQuadDialog::showEvent ( QShowEvent * event )
// {
//   // Default 
//   quads_lw->setFocus();
//   QDialog::showEvent ( event );
// }


void JoinQuadDialog::addQuad()
{
  QModelIndexList iselections = _patternDataSelectionModel->selectedIndexes ();

  QListWidgetItem* item = NULL;
  foreach( const QModelIndex& isel, iselections ){
    item = new QListWidgetItem( isel.data().toString() );
    item->setData(  Qt::UserRole + 20, QVariant::fromValue<QModelIndex>(isel) );
    quads_lw->addItem(item);
    int r = quads_lw->count() - 1;
    quads_lw->setCurrentRow(r);
  }

}


void JoinQuadDialog::removeQuad()
{
  QListWidgetItem *item = quads_lw->currentItem();

  if (item) {
    int r = quads_lw->row(item);
    quads_lw->takeItem(r);
    delete item;
  }

}

void JoinQuadDialog::clearQuads()
{
  if (quads_lw->count() != 0)
    quads_lw->clear();
}


bool JoinQuadDialog::eventFilter(QObject *obj, QEvent *event)
{
  if ( ( obj == quads_lw ) and  ( event->type() == QEvent::FocusIn ) ){
    _setQuadSelectionOnly();
    _currentObj = obj;
    return false;
  } else {
    return HexaBaseDialog::eventFilter(obj, event);
  }
}


void JoinQuadDialog::accept()
{
  SUIT_OverrideCursor wc;
  //if ( !_documentModel ) return;
  if ( !_patternDataSelectionModel ) return;
  if ( !_patternBuilderSelectionModel ) return;
  const PatternDataModel*    patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  if ( !patternDataModel ) return;
  if ( !patternBuilderModel ) return;

  /////
  QModelIndexList iquads;
  QModelIndex     iquad; 
  QListWidgetItem* item = NULL;
  for ( int r = 0; r < quads_lw->count(); ++r){
    item = quads_lw->item(r);
    iquad = patternDataModel->mapToSource( item->data(Qt::UserRole + 20).value<QModelIndex>() );
    if ( iquad.isValid() ) iquads << iquad;
  }
  QModelIndex iquaddest = patternDataModel->mapToSource( _index[quad_dest_le] );
  QModelIndex ivex0 = patternDataModel->mapToSource( _index[vex0_le] );
  QModelIndex ivex1 = patternDataModel->mapToSource( _index[vex1_le] );
  QModelIndex ivex2 = patternDataModel->mapToSource( _index[vex2_le] );
  QModelIndex ivex3 = patternDataModel->mapToSource( _index[vex3_le] );

  QModelIndex iElts;
  if ( iquaddest.isValid()
    && ivex0.isValid()
    && ivex1.isValid()
    && ivex2.isValid()
    && ivex3.isValid() ){
    int nb = nb_spb->value();
    if ( iquads.count() == 1 ){
      iElts = _documentModel->joinQuad( iquads[0], iquaddest,
                                        ivex0, ivex1, ivex2, ivex3,
                                        nb );
    } else if ( iquads.count() > 1 ){
      iElts = _documentModel->joinQuads( iquads, iquaddest,
                                         ivex0, ivex1, ivex2, ivex3,
                                         nb );
    }
  }

  if ( iElts.isValid() ){
    QDialog::accept();
    _disallowSelection();
    //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "JOIN QUAD(S) OK : %1" ).arg( iElts.data().toString()) );
    if ( _patternBuilderSelectionModel ){
      const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
      if ( patternBuilderModel ){
        iElts = patternBuilderModel->mapFromSource( iElts );
        _patternBuilderSelectionModel->setCurrentIndex ( iElts, QItemSelectionModel::Clear );
        _patternBuilderSelectionModel->setCurrentIndex ( iElts, QItemSelectionModel::Select );
      }
      //emit editingFinished();
    }
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT JOIN QUAD(S)" ) );
  }
}


void JoinQuadDialog::reject()
{
  QDialog::reject();
  _disallowSelection();
}








// ------------------------- Merge ----------------------------------
MergeDialog::MergeDialog( QWidget* parent, bool editMode, Qt::WindowFlags f )
: HexaBaseDialog(parent, f)
{
  setupUi( this );

  _vertexLineEdits << v0_le_rb0 << v1_le_rb0 
                   << v0_le_rb1 << v1_le_rb1 
                   << v0_le_rb2 << v1_le_rb2 << v2_le_rb2 << v3_le_rb2;
  _edgeLineEdits << e0_le_rb1 << e1_le_rb1;
  _quadLineEdits << q0_le_rb2 << q1_le_rb2;

  if ( editMode ){
    foreach(QLineEdit* le,  _vertexLineEdits)
      le->installEventFilter(this);
    foreach(QLineEdit* le,  _edgeLineEdits)
      le->installEventFilter(this);
    foreach(QLineEdit* le,  _quadLineEdits)
      le->installEventFilter(this);

    rb0->setFocusProxy( v0_le_rb0 );
    rb1->setFocusProxy( e0_le_rb1 );
    rb2->setFocusProxy( q0_le_rb2 );
    setFocusProxy( rb0 );
  }
  // Default 
  rb0->click();

}


MergeDialog::~MergeDialog()
{
}


// void MergeDialog::showEvent ( QShowEvent * event )
// {
//   // Default : vertex, 1st one
//   rb0->click();
//   v0_le_rb0->setFocus();
//   QDialog::showEvent ( event );
// }



void MergeDialog::accept()
{
  SUIT_OverrideCursor wc;
  if ( !_patternDataSelectionModel )    return;
  const PatternDataModel*    patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  if ( !patternDataModel ) return;

  bool merged = false;

  if ( rb0->isChecked() ){
    QModelIndex iv0 = patternDataModel->mapToSource( _index[v0_le_rb0] );
    QModelIndex iv1 = patternDataModel->mapToSource( _index[v1_le_rb0] );

    if ( iv0.isValid()
      && iv1.isValid() ){
      merged = _documentModel->mergeVertices( iv0, iv1 );
    }
  } else if ( rb1->isChecked() ){ 
    QModelIndex ie0 = patternDataModel->mapToSource( _index[e0_le_rb1] );
    QModelIndex ie1 = patternDataModel->mapToSource( _index[e1_le_rb1] );
    QModelIndex iv0 = patternDataModel->mapToSource( _index[v0_le_rb1] );
    QModelIndex iv1 = patternDataModel->mapToSource( _index[v1_le_rb1] );

    if ( ie0.isValid()
      && ie1.isValid()
      && iv0.isValid()
      && iv1.isValid() ){
      merged =  _documentModel->mergeEdges( ie0, ie1, iv0, iv1);
    }
  } else if ( rb2->isChecked() ){ 

    QModelIndex iq0 = patternDataModel->mapToSource( _index[q0_le_rb2] );
    QModelIndex iq1 = patternDataModel->mapToSource( _index[q1_le_rb2] );
    QModelIndex iv0 = patternDataModel->mapToSource( _index[v0_le_rb2] );
    QModelIndex iv1 = patternDataModel->mapToSource( _index[v1_le_rb2] );
    QModelIndex iv2 = patternDataModel->mapToSource( _index[v2_le_rb2] );
    QModelIndex iv3 = patternDataModel->mapToSource( _index[v3_le_rb2] );

    if ( iq0.isValid()
      && iq1.isValid()
      && iv0.isValid()
      && iv1.isValid()
      && iv2.isValid()
      && iv3.isValid() ){
      merged =  _documentModel->mergeQuads( iq0, iq1,
                                            iv0, iv1, iv2, iv3 );
    }
  }

  if ( merged == true ){
    QDialog::accept();
    _disallowSelection();
    //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "MERGED" ) );
    //emit editingFinished();
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MERGE" ) );
  }
}


void MergeDialog::reject()
{
  QDialog::reject();
  _disallowSelection();
}







// ------------------------- Disconnect ----------------------------------
DisconnectDialog::DisconnectDialog( QWidget* parent, bool editMode, Qt::WindowFlags f )
: HexaBaseDialog(parent, f)
{
  setupUi( this );

  _vertexLineEdits << v_le_rb0;
  _edgeLineEdits   << e_le_rb1;
  _quadLineEdits   << q_le_rb2;
  _hexaLineEdits   << h_le_rb0 << h_le_rb1 << h_le_rb2;

  if ( editMode ){
    //vertex
    v_le_rb0->installEventFilter(this);
    h_le_rb0->installEventFilter(this);

    //edge
    e_le_rb1->installEventFilter(this);
    h_le_rb1->installEventFilter(this);

    //quad
    q_le_rb2->installEventFilter(this);
    h_le_rb2->installEventFilter(this);

    rb0->setFocusProxy( v_le_rb0 );
    rb1->setFocusProxy( e_le_rb1 );
    rb2->setFocusProxy( q_le_rb2 );
    setFocusProxy( rb0 );
  }

  // Default
  rb0->click();

}


DisconnectDialog::~DisconnectDialog()
{
}


// void DisconnectDialog::showEvent ( QShowEvent * event )
// {
//   // Default : vertex, 1st one
//   rb0->click();
//   v_le_rb0->setFocus();
//   QDialog::showEvent ( event );
// }



void DisconnectDialog::accept()
{
  SUIT_OverrideCursor wc;
  if ( !_patternDataSelectionModel ) return;
  const PatternDataModel* patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );

  QModelIndex iElts;

  if ( rb0->isChecked() ){ 
    QModelIndex ihexa = patternDataModel->mapToSource( _index[h_le_rb0] );
    QModelIndex ivex  = patternDataModel->mapToSource( _index[v_le_rb0] );

    if ( ihexa.isValid()
      && ivex.isValid() ){
      iElts = _documentModel->disconnectVertex( ihexa, ivex );
    }
  } else if ( rb1->isChecked() ){ 
    QModelIndex ihexa = patternDataModel->mapToSource( _index[h_le_rb1] );
    QModelIndex iedge = patternDataModel->mapToSource( _index[e_le_rb1] );

    if ( ihexa.isValid()
      && iedge.isValid() ){
      iElts = _documentModel->disconnectEdge( ihexa, iedge );
    }
  } else if ( rb2->isChecked() ){ 
    QModelIndex ihexa = patternDataModel->mapToSource( _index[h_le_rb2] );
    QModelIndex iquad = patternDataModel->mapToSource( _index[q_le_rb2] );

    if ( ihexa.isValid()
      && iquad.isValid() ){
      iElts = _documentModel->disconnectQuad( ihexa, iquad );
    }
  }

  if ( iElts.isValid() ){
    QDialog::accept();
    _disallowSelection();
    //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "DISCONNECT OK : %1" ).arg( iElts.data().toString()) );

    if ( _patternBuilderSelectionModel ){
      const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
      if ( patternBuilderModel ){
        iElts = patternBuilderModel->mapFromSource( iElts );
        _patternBuilderSelectionModel->setCurrentIndex ( iElts, QItemSelectionModel::Clear );
        _patternBuilderSelectionModel->setCurrentIndex ( iElts, QItemSelectionModel::Select );
      }
      //emit editingFinished();
    }
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT DISCONNECT" ) );
  }
}


void DisconnectDialog::reject()
{
  QDialog::reject();
  _disallowSelection();
}










// ------------------------- CutEdgeDialog ----------------------------------
CutEdgeDialog::CutEdgeDialog( QWidget* parent, bool editMode, Qt::WindowFlags f ):
  HexaBaseDialog(parent, f)
{
  setupUi( this );
  _edgeLineEdits << e_le;

  if ( editMode ){
    e_le->installEventFilter(this);
    setFocusProxy( e_le );
  }

}


CutEdgeDialog::~CutEdgeDialog()
{
}

// void CutEdgeDialog::showEvent ( QShowEvent * event )
// {
//   e_le->setFocus();
//   QDialog::showEvent ( event );
// }


void CutEdgeDialog::accept()
{
  SUIT_OverrideCursor wc;
  //if ( !_documentModel ) return;
  if ( !_patternDataSelectionModel ) return;
  const PatternDataModel* patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  if ( !patternDataModel ) return;

  int nbCut = nb_cut_spb->value();
  QModelIndex iedge = patternDataModel->mapToSource( _index[e_le] );

  QModelIndex iElts;
  if ( iedge.isValid() ){
    iElts = _documentModel->cutEdge( iedge, nbCut );
  }
  if ( iElts.isValid() ){
    QDialog::accept();
    _disallowSelection();
    //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "EDGE CUTTED : %1" ).arg( iElts.data().toString()) );
    std::cout<<"_patternBuilderSelectionModel =>"<< _patternBuilderSelectionModel << std::endl;
    if ( _patternBuilderSelectionModel ){
      const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
      std::cout<<"patternBuilderModel =>"<< patternBuilderModel << std::endl;
      if  ( patternBuilderModel ){
        iElts = patternBuilderModel->mapFromSource( iElts );
        _patternBuilderSelectionModel->setCurrentIndex ( iElts, QItemSelectionModel::Clear );
        _patternBuilderSelectionModel->setCurrentIndex ( iElts, QItemSelectionModel::Select );
      }
    }
    //emit editingFinished();
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT CUT EDGE" ) );
  }
}


void CutEdgeDialog::reject()
{
  QDialog::reject();
  _disallowSelection();
}




// // ------------------------- MakeTransformationDialog ----------------------------------
MakeTransformationDialog::MakeTransformationDialog( QWidget* parent, bool editMode, Qt::WindowFlags f )
: HexaBaseDialog(parent, f)
{
  setupUi( this );
  setWindowTitle( tr("MAKE TRANSFORMATION") );

  _vertexLineEdits << vex_le_rb1 << vex_le_rb2;
  _vectorLineEdits << vec_le_rb0 << vec_le_rb2;
  _elementsLineEdits << elts_le_rb0 << elts_le_rb1 << elts_le_rb2;

  if ( editMode ){
    foreach(QLineEdit* le,  _vertexLineEdits)
      le->installEventFilter(this);
    foreach(QLineEdit* le,  _vectorLineEdits)
      le->installEventFilter(this);
    foreach(QLineEdit* le,  _elementsLineEdits)
      le->installEventFilter(this);

    rb0->setFocusProxy( elts_le_rb0 );
    rb1->setFocusProxy( elts_le_rb1 );
    rb2->setFocusProxy( elts_le_rb2 );
    setFocusProxy( rb0 );
  }

  rb0->click();
}


MakeTransformationDialog::~MakeTransformationDialog()
{
}

// void MakeTransformationDialog::showEvent ( QShowEvent * event )
// {
//   // Default : Translation, select elements
//   rb0->click();
//   elts_le_rb0->setFocus();
//   QDialog::showEvent ( event );
// }



void MakeTransformationDialog::accept()
{
  SUIT_OverrideCursor wc;
  if ( !_patternDataSelectionModel )    return;
  if ( !_patternBuilderSelectionModel ) return;
  const PatternDataModel*    patternDataModel    = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  if ( !patternDataModel ) return;
  if ( !patternBuilderModel ) return;

  QModelIndex iNewElts;

  if ( rb0->isChecked() ){ 
    QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb0] );
    QModelIndex  ivec = patternBuilderModel->mapToSource( _index[vec_le_rb0] );

    if ( ielts.isValid()
      && ivec.isValid() )
      iNewElts = _documentModel->makeTranslation( ielts, ivec );

  } else if ( rb1->isChecked() ){ 
    QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb1] );
    QModelIndex  ivex = patternDataModel->mapToSource( _index[vex_le_rb1] );
    double          k = k_spb->value();

    if ( ielts.isValid()
      && ivex.isValid() )
      iNewElts = _documentModel->makeScale( ielts, ivex, k );

  } else if ( rb2->isChecked() ){
    QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb2] );
    QModelIndex  ivex = patternDataModel->mapToSource( _index[vex_le_rb2] );
    QModelIndex  ivec = patternBuilderModel->mapToSource( _index[vec_le_rb2] );
    double      angle = angle_spb->value();

    if ( ielts.isValid()
      && ivex.isValid()
      && ivec.isValid() )
      iNewElts = _documentModel->makeRotation( ielts, ivex, ivec, angle );
  }

  if ( iNewElts.isValid() ){
    QDialog::accept();
    _disallowSelection();
    //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "TRANSFORMATION DONE : %1" ).arg( iNewElts.data().toString()) );
    iNewElts = patternBuilderModel->mapFromSource( iNewElts );
    _patternBuilderSelectionModel->setCurrentIndex ( iNewElts, QItemSelectionModel::Clear );
    _patternBuilderSelectionModel->setCurrentIndex ( iNewElts, QItemSelectionModel::Select );
    //emit editingFinished();
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE TRANSFORMATION" ) );
  }
}


void MakeTransformationDialog::reject()
{
  QDialog::reject();
  _disallowSelection();
}





// // ------------------------- MakeSymmetryDialog ----------------------------------
MakeSymmetryDialog::MakeSymmetryDialog( QWidget* parent, bool editMode, Qt::WindowFlags f )
: HexaBaseDialog(parent, f)
{
  setupUi( this );
  setWindowTitle( tr("MAKE SYMMETRY") );

  _vertexLineEdits << vex_le_rb0 << vex_le_rb1 << vex_le_rb2;
  _vectorLineEdits << vec_le_rb1 << vec_le_rb2;
  _elementsLineEdits << elts_le_rb0 << elts_le_rb1 << elts_le_rb2;

  if ( editMode ){
    foreach(QLineEdit* le,  _vertexLineEdits)
      le->installEventFilter(this);
    foreach(QLineEdit* le,  _vectorLineEdits)
      le->installEventFilter(this);
    foreach(QLineEdit* le,  _elementsLineEdits)
      le->installEventFilter(this);

    rb0->setFocusProxy( elts_le_rb0 );
    rb1->setFocusProxy( elts_le_rb1 );
    rb2->setFocusProxy( elts_le_rb2 );
    setFocusProxy( rb0 );
  }
  rb0->click();
}


MakeSymmetryDialog::~MakeSymmetryDialog()
{
}


// void MakeSymmetryDialog::showEvent ( QShowEvent * event )
// {
//   // Default : Point Symmetry, select elements
//   rb0->click();
//   elts_le_rb0->setFocus();
//   QDialog::showEvent ( event );
// }


void MakeSymmetryDialog::accept()
{
  SUIT_OverrideCursor wc;
  if ( !_patternDataSelectionModel )    return;
  if ( !_patternBuilderSelectionModel ) return;
  const PatternDataModel*    patternDataModel    = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  if ( !patternDataModel ) return;
  if ( !patternBuilderModel ) return;

  QModelIndex iNewElts;


  if ( rb0->isChecked() ){ 
    QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb0] );
    QModelIndex  ivex = patternBuilderModel->mapToSource( _index[vex_le_rb0] );

    if ( ielts.isValid()
      && ivex.isValid() )
      iNewElts = _documentModel->makeSymmetryPoint( ielts, ivex );

  } else if ( rb1->isChecked() ){ 
    QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb1] );
    QModelIndex  ivex = patternDataModel->mapToSource( _index[vex_le_rb1] );
    QModelIndex  ivec = patternBuilderModel->mapToSource( _index[vec_le_rb1] );

    if ( ielts.isValid()
      && ivex.isValid()
      && ivec.isValid() )
    iNewElts = _documentModel->makeSymmetryLine( ielts, ivex, ivec );

  } else if ( rb2->isChecked() ){
    QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb2] );
    QModelIndex  ivex = patternDataModel->mapToSource( _index[vex_le_rb2] );
    QModelIndex  ivec = patternBuilderModel->mapToSource( _index[vec_le_rb2] );

    if ( ielts.isValid()
      && ivex.isValid()
      && ivec.isValid() )
      iNewElts = _documentModel->makeSymmetryPlane( ielts, ivex, ivec );
  }

  if ( iNewElts.isValid() ){
    QDialog::accept();
    _disallowSelection();
    //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "TRANSFORMATION DONE %1" ).arg( iNewElts.data().toString()) );
    iNewElts = patternBuilderModel->mapFromSource( iNewElts );
    _patternBuilderSelectionModel->setCurrentIndex ( iNewElts, QItemSelectionModel::Clear );
    _patternBuilderSelectionModel->setCurrentIndex ( iNewElts, QItemSelectionModel::Select );
    //emit editingFinished();
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE TRANSFORMATION" ) );
  }
}


void MakeSymmetryDialog::reject()
{
  QDialog::reject();
  _disallowSelection();
}




// // ------------------------- PerformTransformationDialog ----------------------------------
PerformTransformationDialog::PerformTransformationDialog( QWidget* parent, bool editMode, Qt::WindowFlags f )
: HexaBaseDialog(parent, f)
{
  setupUi( this );
  setWindowTitle( tr("PERFORM TRANSFORMATION") );

  _vertexLineEdits << vex_le_rb1 << vex_le_rb2;
  _vectorLineEdits << vec_le_rb0 << vec_le_rb2;
  _elementsLineEdits << elts_le_rb0 << elts_le_rb1 << elts_le_rb2;

  if ( editMode ){
    foreach(QLineEdit* le,  _vertexLineEdits)
      le->installEventFilter(this);
    foreach(QLineEdit* le,  _vectorLineEdits)
      le->installEventFilter(this);
    foreach(QLineEdit* le,  _elementsLineEdits)
      le->installEventFilter(this);

    rb0->setFocusProxy( elts_le_rb0 );
    rb1->setFocusProxy( elts_le_rb1 );
    rb2->setFocusProxy( elts_le_rb2 );
    setFocusProxy( rb0 );
  }

  rb0->click();
}


PerformTransformationDialog::~PerformTransformationDialog()
{
}


// void PerformTransformationDialog::showEvent ( QShowEvent * event )
// {
//   // Default : Translation, select elements
//   rb0->click();
//   elts_le_rb0->setFocus();
//   QDialog::showEvent ( event );
// }


void PerformTransformationDialog::accept()
{
  SUIT_OverrideCursor wc;
  if ( !_patternDataSelectionModel )    return;
  if ( !_patternBuilderSelectionModel ) return;
  const PatternDataModel*    patternDataModel    = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  if ( !patternDataModel ) return;
  if ( !patternBuilderModel ) return;

  bool performed = false;

  if ( rb0->isChecked() ){ 
    QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb0] );
    QModelIndex  ivec = patternBuilderModel->mapToSource( _index[vec_le_rb0] );

    if ( ielts.isValid()
      && ivec.isValid() )
      performed = _documentModel->performTranslation( ielts, ivec );

  } else if ( rb1->isChecked() ){ 
    QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb1] );
    QModelIndex  ivex = patternDataModel->mapToSource( _index[vex_le_rb1] );
    double          k = k_spb->value();

    if ( ielts.isValid()
      && ivex.isValid() )
      performed = _documentModel->performScale( ielts, ivex, k );

  } else if ( rb2->isChecked() ){
    QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb2] );
    QModelIndex  ivex = patternDataModel->mapToSource( _index[vex_le_rb2] );
    QModelIndex  ivec = patternBuilderModel->mapToSource( _index[vec_le_rb2] );
    double      angle = angle_spb->value();

    if ( ielts.isValid()
      && ivex.isValid()
      && ivec.isValid() )
      performed = _documentModel->performRotation( ielts, ivex, ivec, angle );
  }

  if ( performed == true ){
    QDialog::accept();
    _disallowSelection();
    //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "TRANSFORMATION DONE" ) );
    //emit editingFinished();
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT PERFORM TRANSFORMATION" ) );
  }
}


void PerformTransformationDialog::reject()
{
  QDialog::reject();
  _disallowSelection();
}


// // ------------------------- PerformSymmetryDialog ----------------------------------
PerformSymmetryDialog::PerformSymmetryDialog( QWidget* parent, bool editMode, Qt::WindowFlags f )
: HexaBaseDialog(parent, f)
{
  setupUi( this );
  setWindowTitle( tr("PERFORM SYMMETRY") );

  _vertexLineEdits << vex_le_rb0 << vex_le_rb1 << vex_le_rb2;
  _vectorLineEdits << vec_le_rb1 << vec_le_rb2;
  _elementsLineEdits << elts_le_rb0 << elts_le_rb1 << elts_le_rb2;

  if ( editMode ){
    foreach(QLineEdit* le,  _vertexLineEdits)
      le->installEventFilter(this);
    foreach(QLineEdit* le,  _vectorLineEdits)
      le->installEventFilter(this);
    foreach(QLineEdit* le,  _elementsLineEdits)
      le->installEventFilter(this);

    rb0->setFocusProxy( elts_le_rb0 );
    rb1->setFocusProxy( elts_le_rb1 );
    rb2->setFocusProxy( elts_le_rb2 );
    setFocusProxy( rb0 );
  }

  // Default
  rb0->click();
}


PerformSymmetryDialog::~PerformSymmetryDialog()
{
}

// void PerformSymmetryDialog::showEvent ( QShowEvent * event )
// {
//   // Default : Point Symmetry, select elements
//   rb0->click();
//   elts_le_rb0->setFocus();
//   QDialog::showEvent ( event );
// }


void PerformSymmetryDialog::accept()
{
  SUIT_OverrideCursor wc;
  if ( !_patternDataSelectionModel )    return;
  if ( !_patternBuilderSelectionModel ) return;
  const PatternDataModel*    patternDataModel    = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  if ( !patternDataModel ) return;
  if ( !patternBuilderModel ) return;

  
  bool performed = false;

  if ( rb0->isChecked() ){ 
    QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb0] );
    QModelIndex  ivex = patternDataModel->mapToSource( _index[vex_le_rb0] );

    if ( ielts.isValid()
      && ivex.isValid() )
      performed = _documentModel->performSymmetryPoint( ielts, ivex );

  } else if ( rb1->isChecked() ){ 
    QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb1] );
    QModelIndex  ivex = patternDataModel->mapToSource( _index[vex_le_rb1] );
    QModelIndex  ivec = patternBuilderModel->mapToSource( _index[vec_le_rb1] );

    if ( ielts.isValid()
      && ivex.isValid()
      && ivec.isValid() )
      performed = _documentModel->performSymmetryLine( ielts, ivex, ivec );

  } else if ( rb2->isChecked() ){
    QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb2] );
    QModelIndex  ivex = patternDataModel->mapToSource( _index[vex_le_rb2] );
    QModelIndex  ivec = patternBuilderModel->mapToSource( _index[vec_le_rb2] );

    if ( ielts.isValid()
      && ivex.isValid()
      && ivec.isValid() )
      performed = _documentModel->performSymmetryPlane( ielts, ivex, ivec );
  }

  if ( performed == true ){
    QDialog::accept();
    _disallowSelection();
    //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "SYMMETRY DONE" ) );
    //emit editingFinished();
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT PERFORM SYMMETRY" ) );
  }
}


void PerformSymmetryDialog::reject()
{
  QDialog::reject();
  _disallowSelection();
}













MyBasicGUI_PointDlg::MyBasicGUI_PointDlg( GeometryGUI* g, QWidget* w, bool b, Qt::WindowFlags f ):
  BasicGUI_PointDlg( g, w, b, f)
{
}

MyBasicGUI_PointDlg::~MyBasicGUI_PointDlg()
{
}

QPushButton* MyBasicGUI_PointDlg::buttonCancel() const
{ 
  return BasicGUI_PointDlg::buttonCancel();
}

QPushButton* MyBasicGUI_PointDlg::buttonOk() const
{ 
  return BasicGUI_PointDlg::buttonOk();
}

QPushButton* MyBasicGUI_PointDlg::buttonApply() const
{ 
  return BasicGUI_PointDlg::buttonApply();
}



VertexAssocDialog::VertexAssocDialog( QWidget* parent, bool editMode, Qt::WindowFlags f ):
GEOMBase_Helper( dynamic_cast<SUIT_Desktop*>(parent->parent()) ),
_documentModel( NULL ),
_patternDataSelectionModel( NULL ),
_ivertex  ( new QModelIndex() )
{

  QVBoxLayout* layout = new QVBoxLayout;
  setLayout( layout );

  QHBoxLayout* up   = new QHBoxLayout;
  QHBoxLayout* down = new QHBoxLayout;

  layout->addLayout(up);
  layout->addLayout(down);

  QWidget* d = dynamic_cast<SUIT_Desktop*>(parent->parent());//SUIT_Session::session()->activeApplication()->desktop() ;
  _nested = new MyBasicGUI_PointDlg( NULL, d);


        QGroupBox *GroupBoxName = new QGroupBox();//DlgRef_Skeleton_QTD);
        GroupBoxName->setObjectName(QString::fromUtf8("GroupBoxName"));
//         sizePolicy.setHeightForWidth(GroupBoxName->sizePolicy().hasHeightForWidth());
//         GroupBoxName->setSizePolicy(sizePolicy);
        GroupBoxName->setTitle("Vertex");
        QHBoxLayout *hboxLayout1 = new QHBoxLayout(GroupBoxName);
        hboxLayout1->setSpacing(6);
        hboxLayout1->setContentsMargins(9, 9, 9, 9);
        hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
        QLabel *NameLabel = new QLabel(GroupBoxName);
        NameLabel->setObjectName(QString::fromUtf8("NameLabel"));
        NameLabel->setWordWrap(false);
        NameLabel->setText( "Name : " );
        hboxLayout1->addWidget( NameLabel );
        _vertex_le = new QLineEdit( GroupBoxName );
        _vertex_le->setObjectName(QString::fromUtf8("_vertex_le"));
        hboxLayout1->addWidget(_vertex_le);

  up->addWidget( GroupBoxName );
  down->addWidget( _nested );

  _vertex_le->installEventFilter(this);
  setFocusProxy( _vertex_le );

  connect(_nested->buttonOk(),     SIGNAL(clicked()), this, SLOT(accept()));
  connect(_nested->buttonApply(),  SIGNAL(clicked()), this, SLOT(accept()));
  connect(_nested->buttonCancel(),  SIGNAL(clicked()), this, SLOT(reject()));
}



void VertexAssocDialog::setDocumentModel(DocumentModel* m)
{
  _documentModel = m;
}

void VertexAssocDialog::setPatternDataSelectionModel(PatternDataSelectionModel* s)
{
  _patternDataSelectionModel = s;
  _patternDataSelectionModel->clearSelection();
  connect( _patternDataSelectionModel,
           SIGNAL( selectionChanged ( const QItemSelection &, const QItemSelection &) ),
           this,
           SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
}


void VertexAssocDialog::onSelectionChanged( const QItemSelection& sel, const QItemSelection& unsel )
{
  QModelIndexList l = sel.indexes();

  if ( l.count() > 0 ){
    QModelIndex selected = l[0];
    int hexaType = selected.data(HEXA_TREE_ROLE).toInt();
    if ( hexaType != VERTEX_TREE){
      std::cout << "BAD SELECTION!"<< std::endl;
      return;
    }
    _vertex_le ->setText( selected.data().toString() );
    *_ivertex = selected;
  }
}



void VertexAssocDialog::accept()
{
  std::cout << "VertexAssocDialog::accept"<< std::endl;
  SUIT_OverrideCursor wc;

  if ( !_documentModel ) return;
  if ( !_patternDataSelectionModel ) return;
  const PatternDataModel* patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  if ( !patternDataModel ) return;

  QModelIndex iVertex = patternDataModel->mapToSource( *_ivertex );
  if ( !iVertex.isValid() ) return;

  GEOM::GeomObjPtr aSelectedObject = getSelected(TopAbs_VERTEX);
  TopoDS_Shape aShape;
  if ( aSelectedObject && GEOMBase::GetShape(aSelectedObject.get(), aShape) && !aShape.IsNull() ){
    DocumentModel::GeomObj aPoint;
    aPoint.name  = GEOMBase::GetName( aSelectedObject.get() );
    aPoint.entry = aSelectedObject->GetStudyEntry();
    aPoint.brep  = shape2string( aShape ).c_str();

    _documentModel->addAssociation( iVertex, aPoint );
    _documentModel->allowEdition();
    _patternDataSelectionModel->setAllSelection();

    //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "VERTEX ASSOCIATION OK : %1" ).arg(iVertex.data().toString()) );
    _patternDataSelectionModel->setCurrentIndex ( *_ivertex , QItemSelectionModel::Clear );
    _patternDataSelectionModel->setCurrentIndex ( *_ivertex , QItemSelectionModel::Select );
    QDialog::accept();
  }

}


void VertexAssocDialog::reject()
{
  std::cout << "VertexAssocDialog::reject"<< std::endl;
  if ( _documentModel ){
    _documentModel->allowEdition();
  }
  if ( _patternDataSelectionModel ){
    _patternDataSelectionModel->setAllSelection();
  }
  QDialog::reject();
}


bool VertexAssocDialog::eventFilter(QObject *obj, QEvent *event)
{
    if ( obj == _vertex_le and event->type() == QEvent::FocusIn ){ //QEvent::KeyPress) { 
      _documentModel->disallowEdition();
      _patternDataSelectionModel->setVertexSelection();
      return false;
    } else {
         // standard event processing
      return QObject::eventFilter(obj, event);
    }
}


VertexAssocDialog::~VertexAssocDialog() 
{
}

// // ------------------------- EdgeAssocDialog ----------------------------------
EdgeAssocDialog::EdgeAssocDialog( QWidget* parent, bool editMode, Qt::WindowFlags f )
: HexaBaseDialog(parent, f),
  GEOMBase_Helper( dynamic_cast<SUIT_Desktop*>(parent->parent())  ) //
{
  setupUi( this );
  setWindowTitle( tr("MAKE EDGE ASSOCIATION") );

  pend_spb->setValue(1.);
  _firstLine.nullify();
  _lastLine.nullify();
  _currentLine.nullify();
  _currentParameter = 0.;

  _vertexLineEdits << first_vex_le;
  first_vex_le->installEventFilter(this);
  edges_lw->installEventFilter(this);
  lines_lw->installEventFilter(this);
  setFocusProxy( edges_lw );

  QShortcut* _delEdgeShortcut = new QShortcut(QKeySequence(Qt::Key_Delete), edges_lw);
  connect(_delEdgeShortcut, SIGNAL(activated()), this, SLOT(deleteEdgeItem()));

  QShortcut* _delLineShortcut = new QShortcut(QKeySequence(Qt::Key_Delete), lines_lw);
  connect(_delLineShortcut, SIGNAL(activated()), this, SLOT(deleteLineItem()));

  // for geom selection :
  SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
  _mgr = dynamic_cast<LightApp_SelectionMgr*>( anApp->selectionMgr() );
  connect( _mgr, SIGNAL(currentSelectionChanged()), this, SLOT( addLine() ) );

  connect( pstart_spb, SIGNAL(valueChanged(double)), this, SLOT( pstartChanged(double)) );
  connect( pend_spb,   SIGNAL(valueChanged(double)), this, SLOT( pendChanged(double)) );

}


EdgeAssocDialog::~EdgeAssocDialog()
{
  disconnect( _delEdgeShortcut, SIGNAL(activated()), this, SLOT(deleteEdgeItem()) );
  disconnect( _delLineShortcut, SIGNAL(activated()), this, SLOT(deleteLineItem()) );

  disconnect( _mgr, SIGNAL(currentSelectionChanged()), this, SLOT(addLine()) );
  disconnect( pstart_spb, SIGNAL(valueChanged(double)), this, SLOT( pstartChanged(double)) );
  disconnect( pend_spb,   SIGNAL(valueChanged(double)), this, SLOT( pendChanged(double)) );

  delete _delEdgeShortcut;
  delete _delLineShortcut;
}


// void EdgeAssocDialog::hideEvent( QHideEvent *event )
// {
//   disconnect( _mgr, SIGNAL(currentSelectionChanged()), this, SLOT( addLine() ) );
//   HexaBaseDialog::hideEvent( event );
// }
// 
// void EdgeAssocDialog::showEvent( QShowEvent *event )
// {
//   connect( _mgr, SIGNAL(currentSelectionChanged()), this, SLOT( addLine() ) );
//   HexaBaseDialog::showEvent( event );
// }

void EdgeAssocDialog::deleteEdgeItem()
{
//   std::cout << "EdgeAssocDialog::deleteEdgeItem() "<< std::endl;
  delete edges_lw->currentItem();
}

void EdgeAssocDialog::deleteLineItem()
{
//   std::cout << "EdgeAssocDialog::deleteLineItem() "<< std::endl;
  delete lines_lw->currentItem();
}


void EdgeAssocDialog::setGeomEngine( GEOM::GEOM_Gen_var geomEngine )
{
  _geomEngine = geomEngine;
}

bool EdgeAssocDialog::eventFilter(QObject *obj, QEvent *event)
{
//   std::cout << "EdgeAssocDialog::eventFilter"<< std::endl;
  if ( ( obj == lines_lw ) and  ( event->type() == QEvent::FocusIn ) ){
//     std::cout << "obj == lines_lw XXXXXXXXXXXXXXX "<< std::endl;
    globalSelection(); // close local contexts, if any
    localSelection(GEOM::GEOM_Object::_nil(), TopAbs_EDGE);
    _currentObj = obj;
    return false;
  } else if ( ( obj == edges_lw ) and  ( event->type() == QEvent::FocusIn ) ){
//     std::cout << "obj == edges_lw XXXXXXXXXXXXXXX "<< std::endl;
    _setEdgeSelectionOnly();
    _currentObj = obj;
    return false;
  } else {
    return HexaBaseDialog::eventFilter(obj, event);
  }
}


void EdgeAssocDialog::onSelectionChanged( const QItemSelection& sel, const QItemSelection& unsel )
{
//   std::cout << "EdgeAssocDialog::onSelectionChanged "<< std::endl;

  QModelIndexList l = sel.indexes();
  if ( l.count() == 0 ) return;

  if ( _currentObj == edges_lw ){
    QModelIndexList iselections = _patternDataSelectionModel->selectedIndexes ();
    QListWidgetItem* item = NULL;
    foreach( const QModelIndex& isel, l ){
      item = new QListWidgetItem( isel.data().toString() );
      item->setData(  LW_QMODELINDEX_ROLE, QVariant::fromValue<QModelIndex>(isel) );
      edges_lw->addItem(item);
    }
    int r = edges_lw->count() - 1;
    edges_lw->setCurrentRow(r);
  } else {
    return HexaBaseDialog::onSelectionChanged( sel, unsel );
  }

}



GEOM::GEOM_IOperations_ptr EdgeAssocDialog::createOperation()
{
  return _geomEngine->GetIBasicOperations(getStudyId());
}


bool EdgeAssocDialog::execute(ObjectList& objects)
{
  bool res = false;

  GEOM::GEOM_Object_var anObj;
  QStringList aParameters;

  GEOM::GEOM_IBasicOperations_var anOper = GEOM::GEOM_IBasicOperations::_narrow( getOperation() );
  anObj = anOper->MakePointOnCurve( _currentLine.get(), _currentParameter );
  if ( !anObj->_is_nil() ) {
    objects.push_back(anObj._retn());
    res = true;
  }

  return res;
}


void EdgeAssocDialog::pstartChanged( double val )
{
//   std::cout << "EdgeAssocDialog::pstartChanged "<< std::endl;
  if ( _firstLine->_is_nil() ) return;
  _currentLine = _firstLine;
  _currentParameter = pstart_spb->value();
  displayPreview(true);
}


void EdgeAssocDialog::pendChanged( double val )
{
//   std::cout << "EdgeAssocDialog::pendChanged "<< std::endl;
  if ( _lastLine->_is_nil() ) return;
  _currentLine      = _lastLine;
  _currentParameter = pend_spb->value();
  displayPreview(true);
}


void EdgeAssocDialog::addLine()
{
//   std::cout << "EdgeAssocDialog::addLine()"<< std::endl;
  if ( !isVisible() ) return;

  GEOM::GeomObjPtr aSelectedObject = getSelected(TopAbs_EDGE);
  TopoDS_Shape aShape;

//   std::cout << "aSelectedObject =>" << aSelectedObject << std::endl;
//   std::cout << "GEOMBase::GetShape(aSelectedObject.get(), aShape) =>" << GEOMBase::GetShape(aSelectedObject.get(), aShape) << std::endl;
//   std::cout << "!aShape.IsNull() => " << !aShape.IsNull() << std::endl;

  if ( aSelectedObject && GEOMBase::GetShape(aSelectedObject.get(), aShape) && !aShape.IsNull() ){
    DocumentModel::GeomObj aLine;
    QListWidgetItem* item = NULL;
    QString mainShapeEntry;
    int     subId = -1;
    QString brep;

    if ( aSelectedObject->IsMainShape() ){
      mainShapeEntry = aSelectedObject->GetStudyEntry();
      brep =  shape2string( aShape ).c_str();
    } else {
      TopoDS_Shape shape;
      TopoDS_Shape subshape;
      GEOM::GEOM_Object_var mainShape = aSelectedObject->GetMainShape();
      mainShapeEntry = mainShape->GetStudyEntry();
      // CS_TODO : à optimiser
      bool okShape = GEOMBase::GetShape( mainShape , shape);//,const TopAbs_ShapeEnum type = TopAbs_SHAPE );
      bool oksubShape = GEOMBase::GetShape( aSelectedObject.get(), subshape );//,const TopAbs_ShapeEnum type = TopAbs_SHAPE );
      if ( okShape && oksubShape ){
        brep =  shape2string( subshape ).c_str();
        subId = GEOMBase::GetIndex( subshape, shape );
      }
    }
    aLine.name  = GEOMBase::GetName( aSelectedObject.get() );
    aLine.entry = mainShapeEntry;
    aLine.subid = QString::number(subId);
    aLine.brep  = brep;
    aLine.start = 0.;
    aLine.end   = 1.;
// //     std::cout << "aLine.name"  << aLine.name.toStdString()  << std::endl;
// //     std::cout << "aLine.entry" << aLine.entry.toStdString() << std::endl;
// //     std::cout << "aLine.subid" << aLine.subid.toStdString() << std::endl;

    item  = new QListWidgetItem( aLine.name );
    item->setData(  LW_ASSOC_ROLE, QVariant::fromValue<DocumentModel::GeomObj>(aLine) );
    lines_lw->addItem(item);
    lines_lw->setCurrentRow( lines_lw->count() - 1 );
//     _assocs << aFace;
  }
}



void EdgeAssocDialog::accept()
{
  SUIT_OverrideCursor wc;
  bool assocOk = false;

  if ( !_patternDataSelectionModel ) return;
  const PatternDataModel* patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  if ( !patternDataModel ) return;

  QListWidgetItem* item = NULL;
  // edges
  QModelIndex     iEdge;
  QModelIndexList iEdges;
  for ( int r = 0; r < edges_lw->count(); ++r){
    item = edges_lw->item(r);
    iEdge = patternDataModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() );
    if ( iEdge.isValid() )
      iEdges << iEdge;
  }

  // lines
  QList<DocumentModel::GeomObj> assocs;
  DocumentModel::GeomObj aLine;
  for ( int r = 0; r < lines_lw->count(); ++r){
    item = lines_lw->item(r);
    aLine = item->data(LW_ASSOC_ROLE).value<DocumentModel::GeomObj>();
//     std::cout << " line added : " << aLine.name.toStdString() << std::endl;
    assocs << aLine;
  }

  if ( close_cb->isChecked() ){
    QModelIndex iFirstVertex = patternDataModel->mapToSource( _index[first_vex_le] );
    assocOk = _documentModel->associateClosedLine( iFirstVertex, iEdges, assocs, pstart_spb->value() );
  } else {
    assocOk = _documentModel->associateOpenedLine( iEdges, assocs, pstart_spb->value(), pend_spb->value() );
  }

  if ( assocOk ){
    QDialog::accept();
    _disallowSelection();
    //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "EDGE(S) ASSOCIATION OK : " ) );
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE EDGE ASSOCIATION" ) );
  }
}



void EdgeAssocDialog::reject()
{
  QDialog::reject();
  _disallowSelection();
}




// // ------------------------- QuadAssocDialog ----------------------------------
QuadAssocDialog::QuadAssocDialog( QWidget* parent, bool editMode, Qt::WindowFlags f )
  : HexaBaseDialog(parent, f),
  GEOMBase_Helper( dynamic_cast<SUIT_Desktop*>(parent->parent()) )
{
  setupUi( this );
  setWindowTitle( tr("MAKE QUAD ASSOCIATION") );

  _quadLineEdits << quad_le;
  quad_le->installEventFilter(this);
  faces_lw->installEventFilter(this);

  setFocusProxy( quad_le );

  SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
  _mgr = dynamic_cast<LightApp_SelectionMgr*>( anApp->selectionMgr() );

  _delFaceShortcut = new QShortcut( QKeySequence(Qt::Key_Delete), faces_lw );
  connect( _delFaceShortcut, SIGNAL(activated()), this, SLOT(deleteFaceItem()) );

//   connect(_mgr, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  connect( _mgr, SIGNAL(currentSelectionChanged()), this, SLOT(addFace()) );
}


QuadAssocDialog::~QuadAssocDialog()
{
  disconnect( _delFaceShortcut, SIGNAL(activated()), this, SLOT(deleteFaceItem()) );
  disconnect( _mgr, SIGNAL(currentSelectionChanged()), this, SLOT(addFace()) );
  delete _delFaceShortcut;
}


bool QuadAssocDialog::eventFilter(QObject *obj, QEvent *event)
{
  std::cout << "QuadAssocDialog::eventFilter "<< std::endl;
  if ( (obj == faces_lw) and  (event->type() == QEvent::FocusIn) ){
    std::cout << "obj == faces_lw XXXXXXXXXXXXXXX "<< std::endl;
    globalSelection(); // close local contexts, if any
    localSelection(GEOM::GEOM_Object::_nil(), TopAbs_FACE);
//     activate( TopAbs_FACE );
    _currentObj = obj;
    return true;
  } else {
    return HexaBaseDialog::eventFilter(obj, event);
  }
}


// void QuadAssocDialog::hideEvent ( QHideEvent * event )
// {
//   disconnect( _mgr, SIGNAL(currentSelectionChanged()), this, SLOT(addFace()) );
//   HexaBaseDialog::hideEvent ( event );
// }

// void QuadAssocDialog::showEvent ( QShowEvent * event )
// {
//   connect( _mgr, SIGNAL(currentSelectionChanged()), this, SLOT(addFace()) );
//   HexaBaseDialog::showEvent ( event );
// }



// bool QuadAssocDialog::isAllSubShapes() const
// {
//   return false;
// }
void QuadAssocDialog::deleteFaceItem()
{
  std::cout << "deleteFaceItem" << std::endl;
  delete faces_lw->currentItem();
}


void QuadAssocDialog::addFace()
{
  if ( !isVisible() ) return;

  GEOM::GeomObjPtr aSelectedObject = getSelected(TopAbs_FACE);
  TopoDS_Shape aShape;

  if ( aSelectedObject && GEOMBase::GetShape(aSelectedObject.get(), aShape) && !aShape.IsNull() ){
    DocumentModel::GeomObj aFace;
    QListWidgetItem* item = NULL;
    QString mainShapeEntry;
    int     subId = -1;
    QString brep;
//     std::cout << "aSelectedObject->GetStudyEntry() =>" << aSelectedObject->GetStudyEntry()<< std::endl;
//     std::cout << "aSelectedObject->GetEntry =>" << aSelectedObject->GetEntry() << std::endl;
//     std::cout << "aSelectedObject->IsMainShape()=>" << aSelectedObject->IsMainShape() << std::endl;

    if ( aSelectedObject->IsMainShape() ){
      mainShapeEntry = aSelectedObject->GetStudyEntry();
      brep =  shape2string( aShape ).c_str();
    } else {
      TopoDS_Shape shape;
      TopoDS_Shape subshape;
      GEOM::GEOM_Object_var mainShape = aSelectedObject->GetMainShape();
      mainShapeEntry = mainShape->GetStudyEntry();
      // CS_TODO : à optimiser
      bool okShape = GEOMBase::GetShape( mainShape , shape);//,const TopAbs_ShapeEnum type = TopAbs_SHAPE );
      bool oksubShape = GEOMBase::GetShape( aSelectedObject.get(), subshape );//,const TopAbs_ShapeEnum type = TopAbs_SHAPE );
      if ( okShape && oksubShape ){
        brep =  shape2string( subshape ).c_str();
        subId = GEOMBase::GetIndex( subshape, shape );
      }
    }
    aFace.name  = GEOMBase::GetName( aSelectedObject.get() );
    aFace.entry = mainShapeEntry;
    aFace.subid = QString::number(subId);
    aFace.brep  = brep;
    aFace.start = 0.;
    aFace.end   = 1.;
    std::cout << "aFace.name"  << aFace.name.toStdString()  << std::endl;
    std::cout << "aFace.entry" << aFace.entry.toStdString() << std::endl;
    std::cout << "aFace.subid" << aFace.subid.toStdString() << std::endl;

    item  = new QListWidgetItem( aFace.name );
    item->setData(  LW_ASSOC_ROLE, QVariant::fromValue<DocumentModel::GeomObj>(aFace) );
    faces_lw->addItem(item);
    faces_lw->setCurrentRow( faces_lw->count() - 1 );
//     _assocs << aFace;
  }
}


void QuadAssocDialog::accept()
{
  SUIT_OverrideCursor wc;
  //if ( !_documentModel ) return;
  if ( !_patternDataSelectionModel ) return;
  const PatternDataModel* patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  if ( !patternDataModel ) return;

  // quad
  QModelIndex iQuad = patternDataModel->mapToSource( _index[quad_le] );

  // faces
  QListWidgetItem* item = NULL;
  DocumentModel::GeomObj aFace;
  for ( int r = 0; r < faces_lw->count(); ++r ){
    item = faces_lw->item(r);
    aFace = item->data(LW_ASSOC_ROLE).value<DocumentModel::GeomObj>();
    std::cout << " aFace added : " << aFace.name.toStdString() << std::endl;
    std::cout << " entry : " << aFace.entry.toStdString() << std::endl;
    std::cout << " subid : " << aFace.subid.toStdString() << std::endl;
    _documentModel->addAssociation( iQuad, aFace );
  }

  QDialog::accept();
  _disallowSelection();
  //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "QUAD ASSOCIATION OK : %1" ).arg(iQuad.data().toString()) );
  iQuad = patternDataModel->mapFromSource( iQuad );
  _patternDataSelectionModel->setCurrentIndex ( iQuad, QItemSelectionModel::Clear );
  _patternDataSelectionModel->setCurrentIndex ( iQuad, QItemSelectionModel::Select );
}

void QuadAssocDialog::reject()
{
  QDialog::reject();
  _disallowSelection();
}


// ------------------------- GROUP ----------------------------------
GroupDialog::GroupDialog( QWidget* parent, bool editMode, Qt::WindowFlags f )
: HexaBaseDialog(parent, f),
  _value(NULL)
{
  setupUi( this );
  strKind[ HEXA_NS::HexaCell ] = "HexaCell";
  strKind[ HEXA_NS::QuadCell ] = "QuadCell";
  strKind[ HEXA_NS::EdgeCell ] = "EdgeCell";
  strKind[ HEXA_NS::HexaNode ] = "HexaNode";
  strKind[ HEXA_NS::QuadNode ] = "QuadNode";
  strKind[ HEXA_NS::EdgeNode ] = "EdgeNode";
  strKind[ HEXA_NS::Vertex_Node ] = "VertexNode";

  kind_cb->clear();
  QMap<HEXA_NS::EnumGroup, QString>::ConstIterator iKind;
  for( iKind = strKind.constBegin(); iKind != strKind.constEnd(); ++iKind )
      kind_cb->addItem( iKind.value(), QVariant(iKind.key()) );

  if ( editMode ){
    eltBase_lw->installEventFilter(this);
    setFocusProxy( eltBase_lw );
  }

  connect(kind_cb,  SIGNAL(activated(int)), this, SLOT(onKindChanged(int)) );
  connect(add_pb, SIGNAL(clicked()), this, SLOT(addEltBase()));
  connect(remove_pb, SIGNAL(clicked()), this, SLOT(removeEltBase()));
  connect(clear_pb, SIGNAL(clicked()), this, SLOT(clearEltBase()));
}


GroupDialog::~GroupDialog()
{
}


// void GroupDialog::showEvent ( QShowEvent * event )
// {
//   eltBase_lw->setFocus();
//   QDialog::showEvent ( event );
// }



void GroupDialog::onKindChanged(int index)
{
  //   onKind
  switch ( kind_cb->itemData(index).toInt() ){
  case HEXA_NS::HexaCell: case HEXA_NS::HexaNode: _setHexaSelectionOnly(); break;
  case HEXA_NS::QuadCell: case HEXA_NS::QuadNode: _setQuadSelectionOnly(); break;
  case HEXA_NS::EdgeCell: case HEXA_NS::EdgeNode: _setEdgeSelectionOnly(); break;
  case HEXA_NS::Vertex_Node: _setVertexSelectionOnly(); break;
  default:Q_ASSERT( false );
  }

}


void GroupDialog::setValue(HEXA_NS::Group* g)
{
  char pName[12];
  name_le->setText( g->getName() );

  kind_cb->clear();
  kind_cb->addItem ( strKind[g->getKind()], QVariant( g->getKind() ) );

  HEXA_NS::EltBase* eltBase = NULL;
  QListWidgetItem* item = NULL;
  QModelIndex iEltBase;
  QList<QStandardItem *> eltBaseItems;
  QVariant q;

  if ( !_documentModel ) return;

  for ( int nr = 0; nr < g->countElement(); ++nr ){
    eltBase = g->getElement( nr );
//     eltBase = dynamic_cast<HEXA_NS::Quad*>( g->getElement( nr ) );
    std::cout<< "eltBase"<< eltBase << std::endl;
    switch ( g->getKind() ){
      case HEXA_NS::HexaCell: case HEXA_NS::HexaNode: q = QVariant::fromValue( (HEXA_NS::Hexa *)eltBase ); break;
      case HEXA_NS::QuadCell: case HEXA_NS::QuadNode: q = QVariant::fromValue( (HEXA_NS::Quad *)eltBase ); break;
      case HEXA_NS::EdgeCell: case HEXA_NS::EdgeNode: q = QVariant::fromValue( (HEXA_NS::Edge *)eltBase ); break;
      case HEXA_NS::Vertex_Node: q = QVariant::fromValue( (HEXA_NS::Vertex *)eltBase ); break;
    }

    QModelIndexList iElts = _documentModel->match(
          _documentModel->index(0, 0),
          HEXA_DATA_ROLE,
          q,
          1,
          Qt::MatchRecursive);
    if ( !iElts.isEmpty() ){
      eltBase->getName(pName);
      item = new QListWidgetItem( QString(pName) );
      iEltBase = iElts[0];
      item->setData(  LW_QMODELINDEX_ROLE, QVariant::fromValue<QModelIndex>(iEltBase) );
      eltBase_lw->addItem( item );
    }
  }
  int r = eltBase_lw->count()-1;
  eltBase_lw->setCurrentRow(r);

  _value = g;
}

HEXA_NS::Group* GroupDialog::getValue()
{
  return _value;
}



void GroupDialog::addEltBase()
{
  if ( !_patternDataSelectionModel ) return;
  QModelIndexList iselections = _patternDataSelectionModel->selectedIndexes ();

  QListWidgetItem* item = NULL;
  foreach( const QModelIndex& isel, iselections ){
    item = new QListWidgetItem( isel.data().toString() );
    item->setData(  LW_QMODELINDEX_ROLE, QVariant::fromValue<QModelIndex>(isel) );
    eltBase_lw->addItem(item);
  }
  int r = eltBase_lw->count() - 1;
  eltBase_lw->setCurrentRow(r);
}


void GroupDialog::removeEltBase()
{
  QListWidgetItem *item = eltBase_lw->currentItem();

  if (item) {
    int r = eltBase_lw->row(item);
    eltBase_lw->takeItem(r);
    delete item;
  }

}

void GroupDialog::clearEltBase()
{
  if (eltBase_lw->count() != 0)
    eltBase_lw->clear();
}


// bool GroupDialog::eventFilter( QObject *obj, QEvent *event )
// {
//   if ( ( obj == eltBase_lw ) and  ( event->type() == QEvent::FocusIn ) ){
//     _setQuadSelectionOnly();
//     _currentObj = obj;
//     return false;
//   } else {
//     return HexaBaseDialog::eventFilter(obj, event);
//   }
// }


void GroupDialog::accept()
{
  SUIT_OverrideCursor wc;
  if ( !_documentModel ) return;
//                           HexaCell, QuadCell, EdgeCell,
//                           HexaNode, QuadNode, EdgeNode, Vertex_Node
  if ( !_patternDataSelectionModel ) return;
//   if ( !_groupsSelectionModel )       return;
  const PatternDataModel* patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
//   const GroupsModel*           groupsModel = dynamic_cast<const GroupsModel*>( _groupsSelectionModel->model() );

  QString               grpName = name_le->text();
  DocumentModel::Group  grpKind = static_cast<DocumentModel::Group>( kind_cb->itemData( kind_cb->currentIndex() ).toInt());
  QModelIndex iGrp;
  if ( _value == NULL ){ // create group
    iGrp = _documentModel->addGroup( grpName, grpKind );
  } else {
    QList<QStandardItem *> grpItems = _documentModel->findItems( _value->getName(), Qt::MatchExactly | Qt::MatchRecursive, 1);
    iGrp = grpItems[0]->index();
  }

//   kind_cb->
  if ( iGrp.isValid() ){ //fill it and select it
    QModelIndex iEltBase;
    QListWidgetItem* item = NULL;
    bool eltAdded = false;
    _documentModel->clearGroupElement(iGrp);
    for ( int r = 0; r < eltBase_lw->count(); ++r){
      item     = eltBase_lw->item(r);
      std::cout << " item " << item->text().toStdString() << std::endl;
      iEltBase = patternDataModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() );
      if ( iEltBase.isValid() )
        eltAdded = _documentModel->addGroupElement( iGrp, iEltBase );
      if ( eltAdded == false ){
        //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "ELEMENT NOT ADDED : %1" ).arg( iEltBase.data().toString() ));
      }
    }
    QDialog::accept();
    _disallowSelection();
    //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "GROUP ADDED : %1" ).arg( iGrp.data().toString()) );
//     iGrp = groupsModel->mapFromSource( iGrp );
//     _groupsSelectionModel->setCurrentIndex ( iGrp, QItemSelectionModel::Clear );
//     _groupsSelectionModel->setCurrentIndex ( iGrp, QItemSelectionModel::Select );

  } else {
      SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT ADD GROUP" ) );
  }

}

void GroupDialog::reject()
{
  QDialog::reject();
  _disallowSelection();
}








// ------------------------- LAW ----------------------------------
LawDialog::LawDialog( QWidget* parent, bool editMode, Qt::WindowFlags f )
: HexaBaseDialog(parent, f),
  _value(NULL)
{
  setupUi( this );
  strKind[ HEXA_NS::Uniform ]    = "Uniform";
  strKind[ HEXA_NS::Arithmetic ] = "Arithmetic";
  strKind[ HEXA_NS::Geometric ]  = "Geometric";

  kind_cb->clear();
  QMap<HEXA_NS::KindLaw, QString>::ConstIterator iKind;
  for( iKind = strKind.constBegin(); iKind != strKind.constEnd(); ++iKind )
      kind_cb->addItem( iKind.value(), QVariant(iKind.key()) );
}


LawDialog::~LawDialog()
{
}


void LawDialog::setValue(HEXA_NS::Law* l)
{
//   char pName[12];
  name_le->setText( l->getName() );
  nb_nodes_spb->setValue( l->getNodes() );
  coeff_spb->setValue( l->getCoefficient() );
  HEXA_NS::KindLaw k = l->getKind();
  kind_cb->setCurrentIndex( kind_cb->findData(k) );

  _value = l;
}

HEXA_NS::Law* LawDialog::getValue()
{
  return _value;
}




void LawDialog::accept()
{
  SUIT_OverrideCursor wc;
  if ( !_documentModel ) return;

  if ( !_meshSelectionModel ) return;
  const MeshModel* meshModel = dynamic_cast<const MeshModel*>( _meshSelectionModel->model() );

  QString lawName = name_le->text();
  int     nbnodes = nb_nodes_spb->value();
  double  coeff   = coeff_spb->value();
  DocumentModel::KindLaw  lawKind = static_cast<DocumentModel::KindLaw>( kind_cb->itemData( kind_cb->currentIndex() ).toInt());


  QModelIndex iLaw;
  if ( _value == NULL ){ // create Law
    iLaw = _documentModel->addLaw( lawName, nbnodes );
  } else {
    QList<QStandardItem *> lawItems = _documentModel->findItems( _value->getName(), Qt::MatchExactly | Qt::MatchRecursive, 1);
    iLaw = lawItems[0]->index();
  }


  if ( iLaw.isValid() ){ //fill it and select it
    QDialog::accept();
    bool setOk = _documentModel->setLaw( iLaw, nbnodes, coeff, lawKind );
    _disallowSelection();
    //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "LAW ADDED : %1" ).arg( iLaw.data().toString()) );
//     _meshSelectionModel->select( iLaw, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current );
//     _meshSelectionModel->setCurrentIndex( iLaw, QItemSelectionModel::Current );
    _meshSelectionModel->setCurrentIndex ( iLaw, QItemSelectionModel::Clear );
    _meshSelectionModel->setCurrentIndex ( iLaw, QItemSelectionModel::Select );


  } else {
      SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT ADD LAW" ) );
  }

}

void LawDialog::reject()
{
  QDialog::reject();
  _disallowSelection();
}





// ------------------------- PROPAGATION ----------------------------------
PropagationDialog::PropagationDialog( QWidget* parent, bool editMode, Qt::WindowFlags f )
: HexaBaseDialog(parent, f),
  _value(NULL)
{
  setupUi( this );
  _lawLineEdits << law_le;

  if ( editMode ){
    law_le->installEventFilter(this);
    setFocusProxy( law_le );
  } else {
  }
}


PropagationDialog::~PropagationDialog()
{
}

// void PropagationDialog::showEvent ( QShowEvent * event )
// {
// //   law_le->setFocus();
//   QDialog::showEvent ( event );
// }


void PropagationDialog::setValue(HEXA_NS::Propagation* p)
{
  char pName[12];

  HEXA_NS::Law* l = p->getLaw();
  bool way = p->getWay();

  if ( l != NULL )
    law_le->setText( l->getName() );
  way_cb->setChecked(way);

  _value = p;
}

HEXA_NS::Propagation* PropagationDialog::getValue()
{
  return _value;
}




void PropagationDialog::accept()
{
  SUIT_OverrideCursor wc;
  if ( !_documentModel ) return;

  if ( !_meshSelectionModel ) return;
  const MeshModel* meshModel = dynamic_cast<const MeshModel*>( _meshSelectionModel->model() );

  QModelIndex iPropagation;
  QModelIndex iLaw;
  bool way = way_cb->isChecked();
  if ( _value == NULL ){
    return;
  } else {
//     QList<QStandardItem *> propagationItems = _documentModel->findItems( _value->getName(), Qt::MatchExactly | Qt::MatchRecursive, 1);
//     QList<QStandardItem *> lawItems = _documentModel->findItems( lawName, Qt::MatchExactly | Qt::MatchRecursive, 1);
//     iPropagation = propagationItems[0]->index();
//     iLaw         = lawItems[0]->index();

    QModelIndexList iPropagations = _documentModel->match(
            _documentModel->index(0, 0),
            HEXA_DATA_ROLE,
            QVariant::fromValue( _value ),
            1,
            Qt::MatchRecursive);
    iPropagation = iPropagations[0];
    iLaw = meshModel->mapToSource( _index[law_le] );
  }

  bool setOk = false;
  if ( iPropagation.isValid() && iLaw.isValid() ){ //fill it and select it
    setOk = _documentModel->setPropagation( iPropagation, iLaw, way );
  }
//     _meshSelectionModel->select( iPropagation, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current );
//     _meshSelectionModel->setCurrentIndex( iPropagation, QItemSelectionModel::Current );
  if ( setOk ){
    QDialog::accept();
    _disallowSelection();
    //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "PROPAGATION SETTED" ) );
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT SET PROPAGATION" ) );
  }
}

void PropagationDialog::reject()
{
  _disallowSelection();
  QDialog::reject();
}

// ------------------------- COMPUTE MESH ----------------------------------

ComputeMeshDialog::ComputeMeshDialog( QWidget* parent, bool editMode, Qt::WindowFlags f )
        : HexaBaseDialog(parent, f) {

    QVBoxLayout* layout = new QVBoxLayout;
    setLayout(layout);

    QHBoxLayout* up   = new QHBoxLayout;
    QHBoxLayout* down = new QHBoxLayout;

    layout->addLayout(up);
    layout->addLayout(down);

    QVBoxLayout* vlg = new QVBoxLayout;
    QVBoxLayout* vld = new QVBoxLayout;

    up->addLayout(vlg);
    up->addLayout(vld);

    vlg->addWidget(new QLabel("Name"));
    vlg->addWidget(new QLabel("Dimension"));
    vlg->addWidget(new QLabel("Container"));

    _name = new QLineEdit("aMesh");
    _dim  = new QSpinBox();
    _fact = new QLineEdit("FactoryServer");

    vld->addWidget(_name);
    vld->addWidget(_dim);
    vld->addWidget(_fact);

    _dim->setRange(1, 3);
    _dim->setValue(3);

    QPushButton* ok     = new QPushButton("OK");
    QPushButton* cancel = new QPushButton("Cancel");
    QPushButton* help   = new QPushButton("Help");

    down->addWidget(ok);
    down->addWidget(cancel);
    down->addWidget(help);

    connect(ok    , SIGNAL( clicked() ), this, SLOT( accept() ));
    connect(cancel, SIGNAL( clicked() ), this, SLOT( reject() ));
}

ComputeMeshDialog::~ComputeMeshDialog() {
}

void ComputeMeshDialog::accept() {
    _disallowSelection();
    QDialog::accept();

    QString command = QString("import hexablock ; %1 = hexablock.mesh(\"%1\", \"%2\", %3, \"%4\")")
      .arg( _name->text() )
      .arg( _documentModel->documentEntry() )
      .arg( _dim->value() )
      .arg( _fact->text() );
    std::cout << "command: " << command.toStdString() << std::endl;

    SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
    PyConsole_Console* pyConsole = app->pythonConsole();

    if ( pyConsole ) pyConsole->exec( command );
}

void ComputeMeshDialog::reject() {
    _disallowSelection();
    QDialog::reject();
}
