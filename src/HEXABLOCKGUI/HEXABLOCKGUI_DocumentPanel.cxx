#include "HEXABLOCKGUI_DocumentPanel.hxx"
#include "HEXABLOCKGUI_DocumentGraphicView.hxx"

#include "HEXABLOCKGUI.hxx"

// #include "SALOME_Selection.h"
#include <SUIT_OverrideCursor.h>
#include <SUIT_MessageBox.h>
#include "SVTK_Selection.h"

#include <iostream>
#include <QtGui>
#include <QFlags>




using namespace std;
using namespace HEXABLOCK::GUI;




HexaBaseDialog::HexaBaseDialog( QWidget * parent, Qt::WindowFlags f ):
//   _model(0),
//   _selectionModel(0),
  QDialog( parent, f),
  _documentModel(0),
//   _patternDataModel(0),
//   _patternBuilderModel(0),
  _patternDataSelectionModel(0),
  _patternBuilderSelectionModel(0),
  _groupsSelectionModel(0),
  _meshSelectionModel(0),
  _currentObj(0)
{
}


HexaBaseDialog::~HexaBaseDialog()
{
}



void HexaBaseDialog::_setAllSelection()
{
  //allow all selection
  if ( _documentModel ){
    _documentModel->allowAllSelection();
  }

  if ( _patternDataSelectionModel ){
    //CS_TODO?
  }

}


void HexaBaseDialog::_setHexaSelectionOnly()
{
  //allow hexa selection only 
  if ( _documentModel ){
    _documentModel->allowHexaSelectionOnly();
  }

//   if ( _patternDataSelectionModel ){ CS_TODO
//     _patternDataSelectionModel->SetSelectionMode(HexaSelection);
// const Selection_Mode VolumeSelection = 5; 
// const Selection_Mode ActorSelection = 6;
//   }

}




void HexaBaseDialog::_setQuadSelectionOnly()
{
  std::cout << "_setQuadSelectionOnly() _setQuadSelectionOnly() _setQuadSelectionOnly() "<< std::endl;
  //allow quad selection only 
  if ( _documentModel ){
    _documentModel->allowQuadSelectionOnly();
  }

  if ( _patternDataSelectionModel ){
    _patternDataSelectionModel->SetSelectionMode(FaceSelection);
  }
}

void HexaBaseDialog::_setEdgeSelectionOnly()
{
  std::cout << "_setEdgeSelectionOnly() _setEdgeSelectionOnly() _setEdgeSelectionOnly() "<< std::endl;
  //allow edge selection only
  if ( _documentModel ){
    _documentModel->allowEdgeSelectionOnly();
  }

  if ( _patternDataSelectionModel ){
    _patternDataSelectionModel->SetSelectionMode(EdgeSelection);
//  NodeSelection,
//  CellSelection,
//  EdgeOfCellSelection,
//  EdgeSelection,
//  FaceSelection,
//  VolumeSelection,
//  ActorSelection };
  }
}


void HexaBaseDialog::_setVertexSelectionOnly()
{
  std::cout << "_setVertexSelectionOnly() _setVertexSelectionOnly() _setVertexSelectionOnly() "<< std::endl;
  //allow vertex selection only
  if ( _documentModel ){
    _documentModel->allowVertexSelectionOnly();
  }

  if ( _patternDataSelectionModel ){
    _patternDataSelectionModel->SetSelectionMode(NodeSelection);
  }
}

void HexaBaseDialog::_setElementsSelectionOnly()
{
std::cout << "_setElementsSelectionOnly() _setElementsSelectionOnly() _setElementsSelectionOnly() "<< std::endl;
  //allow vertex selection only
  if ( _documentModel ){
    _documentModel->allowElementsSelectionOnly();
  }

//   if ( _selectionModel ){
//   }
}

void HexaBaseDialog::_setVectorSelectionOnly()
{
std::cout << "_setVectorSelectionOnly() _setVectorSelectionOnly() _setVectorSelectionOnly() "<< std::endl;
  //allow vertex selection only
  if ( _documentModel ){
    _documentModel->allowVectorSelectionOnly();
  }

//   if ( _selectionModel ){
//   }
}

void HexaBaseDialog::_setCylinderSelectionOnly()
{
  //allow cylinder selection only
  if ( _documentModel ){
    _documentModel->allowCylinderSelectionOnly();
  }
}


void HexaBaseDialog::_setPipeSelectionOnly()
{
  //allow cylinder selection only
  if ( _documentModel ){
    _documentModel->allowPipeSelectionOnly();
  }
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
           SLOT( onPatternDataSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
}


void HexaBaseDialog::setPatternBuilderSelectionModel(PatternBuilderSelectionModel* s)
{
  _patternBuilderSelectionModel = s;
  _patternBuilderSelectionModel->clearSelection();

  connect( _patternBuilderSelectionModel,
           SIGNAL( selectionChanged( const QItemSelection &, const QItemSelection &) ),
           this,
           SLOT( onPatternBuilderSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
}


void HexaBaseDialog::onPatternDataSelectionChanged( const QItemSelection& sel, const QItemSelection& unsel )
{
    QModelIndexList l = _patternDataSelectionModel->selectedIndexes ();

    if ( l.count() > 0 ){
      QModelIndex selected = l[0];
      QLineEdit* currentLineEdit = dynamic_cast<QLineEdit*>(_currentObj);
      if ( currentLineEdit ){
        currentLineEdit->setText( selected.data().toString() );
      }
      _index[_currentObj] = selected;
    }
}


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
    if ( event->type() == QEvent::FocusIn ){ //QEvent::KeyPress) { 
        QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(obj);
        if ( lineEdit ){
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
        }
        _currentObj = obj;
        return false;
    } else {
         // standard event processing
         return QObject::eventFilter(obj, event);
    }
}



// ------------------------- VERTEX ----------------------------------
VertexDialog::VertexDialog( QWidget* parent, Qt::WindowFlags f )
: HexaBaseDialog(parent, f),
  _value(0)
{
  setupUi( this );

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


// void VertexDialog::accept()
// {
//     if (_value)
//     { //EDITION MODE     //CS_BP vérifier que setModelData, getValue appelé
//       std::cout<<"edit vertex!! edit vertex!! edit vertex!!"<<std::endl;
//         _value->setX( x_spb->value() );
//         _value->setY( y_spb->value() );
//         _value->setZ( z_spb->value() );
//         // mise a jour de la vue :
//         // pourquoi on a un pointeur ver le modele (HexVertex)
//         // et pas vers l'index ??? 
//         // plantage ici ???
//         if (_patternDataSelectionModel != NULL)
//         {
//             std::cout << "accept() : OK" << std::endl;
// 
//             if (_patternDataModel != NULL)
//             {
//                 std::cout << "accept() : _model != NULL" << std::endl;
//                 _patternDataModel->updateVertex( _patternDataSelectionModel->currentIndex() );
//                 std::cout << "accept() 2 : _model != NULL" << std::endl;
//             }
//             else
//             {
//                 std::cout << "accept() : _model NULL" << std::endl;
//             }
//         }
//         else
//         {
//             std::cout << "accept() : NOT OK" << std::endl;
// 
//         }
//         // ????
// 
//     }
//     else if ( _patternDataModel )
//     { 
//       //NEW MODE
// //     std::cout<<"add vertex!! add vertex!! add vertex!! add vertex!!"<<std::endl;
//         QModelIndex newIndex = _patternDataModel->addVertex( x_spb->value(), y_spb->value(),  z_spb->value() );
// 
//         _value = newIndex.model()->data(newIndex, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
//         _patternDataSelectionModel->select( newIndex, 
//                                             QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current );
//         _patternDataSelectionModel->setCurrentIndex ( newIndex, 
//                                                       QItemSelectionModel::Current  );
//     }
// 
//     QDialog::accept();
// }


void VertexDialog::accept()
{
    if (_value) //EDITION MODE     //CS_BP vérifier que setModelData, getValue appelé
    { 
        _value->setX( x_spb->value() );
        _value->setY( y_spb->value() );
        _value->setZ( z_spb->value() );
        QDialog::accept();
    }
    else if ( _documentModel ) //NEW MODE
    {
      QModelIndex newIndex = _documentModel->addVertex( x_spb->value(), y_spb->value(),  z_spb->value() );
      if ( newIndex.isValid() ){
        _value = newIndex.model()->data(newIndex, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
        QDialog::accept();
        if ( _patternDataSelectionModel ){
          const PatternDataModel* patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
          if ( patternDataModel ){
            newIndex = patternDataModel->mapFromSource(newIndex);
            _patternDataSelectionModel->select( newIndex,
                QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current );
            _patternDataSelectionModel->setCurrentIndex ( newIndex,
                QItemSelectionModel::Current  );
          }
        }
        SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "VERTEX BUILDED" ) );
      } else {
        SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT BUILD VERTEX" ) );
      }
    }
    
}






// ------------------------- EDGE ----------------------------------
EdgeDialog::EdgeDialog( QWidget* parent, Qt::WindowFlags f ):
  HexaBaseDialog(parent, f),
  _value(0)
{
  setupUi( this );

  _vertexLineEdits << vex_le_rb1 << v0_le_rb0 << v1_le_rb0;
  _vectorLineEdits << vec_le_rb1;

  foreach(QLineEdit* le,  _vertexLineEdits)
    le->installEventFilter(this);
  foreach(QLineEdit* le,  _vectorLineEdits)
    le->installEventFilter(this);

  // Default 
  rb0->click();
  // select first Vertex
  _currentObj = v0_le_rb0;
//   _currentVertexLineEdit->setFocus();
}


EdgeDialog::~EdgeDialog()
{

}


void EdgeDialog::setValue(HEXA_NS::Edge* e)
{
  char pName[12];

  HEXA_NS::Vertex* v0 = e->getVertex(0);
  HEXA_NS::Vertex* v1 = e->getVertex(1);

  v0_le_rb0->setText( v0->getName(pName) );
  v1_le_rb0->setText( v1->getName(pName) );

  _value = e;

  buttonBox->clear();
  rb1->hide();
  v0_le_rb0->setReadOnly(true);
  v1_le_rb0->setReadOnly(true);
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
    Q_ASSERT(iv0.isValid());
    Q_ASSERT(iv1.isValid());
    iEdge = _documentModel->addEdgeVertices( iv0, iv1 );
  } else if ( rb1->isChecked() ){
    QModelIndex ivex = patternDataModel->mapToSource( _index[vex_le_rb1] );
    QModelIndex ivec = patternBuilderModel->mapToSource( _index[vec_le_rb1] );
    Q_ASSERT(ivex.isValid());
    Q_ASSERT(ivec.isValid());
    iEdge = _documentModel->addEdgeVector( ivex, ivec );
  }

  if ( iEdge.isValid() ){
    _setAllSelection();
    QDialog::accept();
    iEdge = patternDataModel->mapFromSource(iEdge);
    _patternDataSelectionModel->select( iEdge, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current );
    _patternDataSelectionModel->setCurrentIndex( iEdge, QItemSelectionModel::Current );
    SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "EDGE ADDED" ) );
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "EDGE ADDED" ) );
  }
}


void EdgeDialog::reject()
{
  _setAllSelection();
  QDialog::reject();
}





// ------------------------- QUAD ----------------------------------
QuadDialog::QuadDialog( QWidget* parent, Qt::WindowFlags f ):
  HexaBaseDialog(parent, f),
  _value(0)
{
  setupUi( this );

  _vertexLineEdits << v0_le_rb0 << v1_le_rb0 << v2_le_rb0 << v3_le_rb0;
  _edgeLineEdits   << e0_le_rb1 << e1_le_rb1 << e2_le_rb1 << e3_le_rb1;

  foreach(QLineEdit* le,  _vertexLineEdits)
    le->installEventFilter(this);
  foreach(QLineEdit* le,  _edgeLineEdits)
    le->installEventFilter(this);

  // Default 
  rb0->click();
  // select first Vertex
  _currentObj = v0_le_rb0;
}


QuadDialog::~QuadDialog()
{

}


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
  v0_le_rb0->setReadOnly(true);
  v1_le_rb0->setReadOnly(true);
  v2_le_rb0->setReadOnly(true);
  v3_le_rb0->setReadOnly(true);


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
  e0_le_rb1->setReadOnly(true);
  e1_le_rb1->setReadOnly(true);
  e2_le_rb1->setReadOnly(true);
  e3_le_rb1->setReadOnly(true);

  rb0->click();
  buttonBox->clear();

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

    Q_ASSERT(iv0.isValid());
    Q_ASSERT(iv1.isValid());
    Q_ASSERT(iv2.isValid());
    Q_ASSERT(iv3.isValid());

    iQuad = _documentModel->addQuadVertices( iv0, iv1, iv2, iv3 );
  } else if ( rb1->isChecked() ){ //edges
    QModelIndex ie0 = patternDataModel->mapToSource( _index[e0_le_rb1] );
    QModelIndex ie1 = patternDataModel->mapToSource( _index[e1_le_rb1] );
    QModelIndex ie2 = patternDataModel->mapToSource( _index[e2_le_rb1] );
    QModelIndex ie3 = patternDataModel->mapToSource( _index[e3_le_rb1] );

    Q_ASSERT(ie0.isValid());
    Q_ASSERT(ie1.isValid());
    Q_ASSERT(ie2.isValid());
    Q_ASSERT(ie3.isValid());

    iQuad = _documentModel->addQuadEdges( ie0, ie1, ie2, ie3 );
  }


  if ( iQuad.isValid() ){
    _value = iQuad.model()->data(iQuad, HEXA_DATA_ROLE).value<HEXA_NS::Quad *>();
    _setAllSelection();
    QDialog::accept();
    iQuad = patternDataModel->mapFromSource( iQuad );
    _patternDataSelectionModel->select( iQuad,
                                        QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current );
    _patternDataSelectionModel->setCurrentIndex( iQuad,
                                        QItemSelectionModel::Current );
    SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "QUAD BUILDED" ) );
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT BUILD QUAD" ) );
  }
}


void QuadDialog::reject()
{
  _setAllSelection();
  QDialog::reject();
}





// ------------------------- HEXA ----------------------------------
HexaDialog::HexaDialog( QWidget* parent, Qt::WindowFlags f ):
  HexaBaseDialog(parent, f),
  _value(0)
{
  setupUi( this );

  _vertexLineEdits << v0_le_rb1 << v1_le_rb1 << v2_le_rb1 << v3_le_rb1 
                   << v4_le_rb1 << v5_le_rb1 << v6_le_rb1 << v7_le_rb1;
  _quadLineEdits << q0_le_rb0 << q1_le_rb0 << q2_le_rb0
                 << q3_le_rb0 << q4_le_rb0 << q5_le_rb0;

  foreach(QLineEdit* le,  _vertexLineEdits)
    le->installEventFilter(this);
  foreach(QLineEdit* le,  _quadLineEdits)
    le->installEventFilter(this);

  // Default 
  rb0->click();
  // select first quad
  _currentObj = q0_le_rb0;
//   _currentQuadLineEdit->setFocus();

}


HexaDialog::~HexaDialog()
{

}


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

  _value = h;

  buttonBox->clear();
  q0_le_rb0->setReadOnly(true);
  q1_le_rb0->setReadOnly(true);
  q2_le_rb0->setReadOnly(true);
  q3_le_rb0->setReadOnly(true);
  q4_le_rb0->setReadOnly(true);
  q5_le_rb0->setReadOnly(true);

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

  QModelIndex iq0 = patternDataModel->mapToSource( _index[q0_le_rb0] );
  QModelIndex iq1 = patternDataModel->mapToSource( _index[q1_le_rb0] );
  QModelIndex iq2 = patternDataModel->mapToSource( _index[q2_le_rb0] );
  QModelIndex iq3 = patternDataModel->mapToSource( _index[q3_le_rb0] );
  QModelIndex iq4 = patternDataModel->mapToSource( _index[q4_le_rb0] );
  QModelIndex iq5 = patternDataModel->mapToSource( _index[q5_le_rb0] );

  Q_ASSERT(iq0.isValid());
  Q_ASSERT(iq1.isValid());
  Q_ASSERT(iq2.isValid());
  Q_ASSERT(iq3.isValid());
  Q_ASSERT(iq4.isValid());
  Q_ASSERT(iq5.isValid());

  QModelIndex iHexa = _documentModel->addHexaQuad( iq0, iq1, iq2, iq3, iq4, iq5 );

  if ( iHexa.isValid() ){
    _value = iHexa.model()->data(iHexa, HEXA_DATA_ROLE).value<HEXA_NS::Hexa*>();
    _setAllSelection();
    QDialog::accept();
    iHexa = patternDataModel->mapFromSource( iHexa );
    _patternDataSelectionModel->select( iHexa, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current );
    _patternDataSelectionModel->setCurrentIndex ( iHexa, QItemSelectionModel::Current  );
    SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "HEXA BUILDED" ) );
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT BUILD HEXA" ) );
  }
}


void HexaDialog::reject()
{
  _setAllSelection();
  QDialog::reject();
}



// ------------------------- VECTOR ----------------------------------
VectorDialog::VectorDialog( QWidget* parent, Qt::WindowFlags f )
: HexaBaseDialog(parent, f),
  _value(0)
{
  setupUi( this );
}


VectorDialog::~VectorDialog()
{
}


void VectorDialog::setValue(HEXA_NS::Vector* v)
{
  _value = v;

  dx_spb->setValue( v->getDx() );
  dy_spb->setValue( v->getDy() );
  dz_spb->setValue( v->getDz() );


  buttonBox->clear();
  dx_spb->setReadOnly(true);
  dy_spb->setReadOnly(true);
  dz_spb->setReadOnly(true);

}

HEXA_NS::Vector* VectorDialog::getValue()
{
  return _value;
}




void VectorDialog::accept()
{
  SUIT_OverrideCursor wc;
//if ( !_documentModel ) return;

  if ( _documentModel ){ //NEW MODE
    QModelIndex newIndex = _documentModel->addVector( dx_spb->value(), dy_spb->value(),  dz_spb->value() );
    if ( newIndex.isValid() ){
      _value = newIndex.model()->data(newIndex, HEXA_DATA_ROLE).value<HEXA_NS::Vector *>();
      QDialog::accept();
      if ( _patternBuilderSelectionModel ){
        const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
        if ( patternBuilderModel ){
          newIndex = patternBuilderModel->mapFromSource(newIndex);
          _patternBuilderSelectionModel->select( newIndex, 
                                            QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current );
          _patternBuilderSelectionModel->setCurrentIndex ( newIndex, QItemSelectionModel::Current  );
        }
      }
      SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "VECTOR BUILDED" ) );
    } else {
      SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT BUILD VECTOR" ) );
    }
  }

}



CylinderDialog::CylinderDialog( QWidget* parent, Qt::WindowFlags f )
: HexaBaseDialog(parent, f),
  _value(0)
{
  setupUi( this );

  _vertexLineEdits << vex_le;
  _vectorLineEdits << vec_le;

  vex_le->installEventFilter(this);
  vec_le->installEventFilter(this);

  _currentObj = vex_le;
}

CylinderDialog::~CylinderDialog()
{
}


void CylinderDialog::setValue(HEXA_NS::Cylinder* c)
{
  char pName[12];

  HEXA_NS::Vertex* base      = c->getBase();
  HEXA_NS::Vector* direction = c->getDirection();
  double  r = c->getRadius();
  double  h = c->getHeight();

  vex_le->setText( base->getName(pName) );
  vex_le->setReadOnly(true);
  vec_le->setText( direction->getName(pName) );
  vec_le->setReadOnly(true);
  r_spb->setValue(r);
  r_spb->setReadOnly(true);
  h_spb->setValue(h);
  h_spb->setReadOnly(true);

  buttonBox->clear();
  _value = c;
}

HEXA_NS::Cylinder* CylinderDialog::getValue()
{
  return _value;
}




void CylinderDialog::accept()
{
  SUIT_OverrideCursor wc;
  //if ( !_documentModel ) return;
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

  Q_ASSERT( ivex.isValid() );
  Q_ASSERT( ivec.isValid() );
  iCyl = _documentModel->addCylinder( ivex, ivec, r,  h );

  if ( iCyl.isValid() ){
    _setAllSelection();
    QDialog::accept();
    iCyl = patternBuilderModel->mapFromSource(iCyl);
    _patternBuilderSelectionModel->select( iCyl,
                                           QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current );
    _patternBuilderSelectionModel->setCurrentIndex( iCyl, QItemSelectionModel::Current  );
    SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "ADD CYLINDER DONE" ) );
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT ADD CYLINDER" ) );
  }
}




void CylinderDialog::reject()
{
  _setAllSelection();
  QDialog::reject();
}









PipeDialog::PipeDialog( QWidget* parent, Qt::WindowFlags f )
: HexaBaseDialog(parent, f),
  _value(0)
{
  setupUi( this );

  _vertexLineEdits << vex_le;
  _vectorLineEdits << vec_le;

  vex_le->installEventFilter(this);
  vec_le->installEventFilter(this);

  _currentObj = vex_le;
}

PipeDialog::~PipeDialog()
{
}


void PipeDialog::setValue(HEXA_NS::Pipe* p)
{
  char pName[12];

  HEXA_NS::Vertex* base      = p->getBase();
  HEXA_NS::Vector* direction = p->getDirection();
  double  ir = p->getInternalRadius();
  double  er = p->getRadius();
  double  h  = p->getHeight();

  vex_le->setText( base->getName(pName) );
  vex_le->setReadOnly(true);
  vec_le->setText( direction->getName(pName) );
  vec_le->setReadOnly(true);
  ir_spb->setValue(ir);
  ir_spb->setReadOnly(true);
  er_spb->setValue(er);
  er_spb->setReadOnly(true);
  h_spb->setValue(h);
  h_spb->setReadOnly(true);

  buttonBox->clear();
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

  Q_ASSERT( ivex.isValid() );
  Q_ASSERT( ivec.isValid() );
  iPipe = _documentModel->addPipe( ivex, ivec, ir, er, h );

  if ( iPipe.isValid() ){
    _setAllSelection();
    QDialog::accept();
    iPipe = patternBuilderModel->mapFromSource( iPipe );
    _patternBuilderSelectionModel->select( iPipe,
                                           QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current );
    _patternBuilderSelectionModel->setCurrentIndex( iPipe, QItemSelectionModel::Current  );
    SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "ADD PIPE DONE" ) );
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT ADD PIPE" ) );
  }
}




void PipeDialog::reject()
{
  _setAllSelection();
  QDialog::reject();
}


















// ------------------------- MakeGridDialog ----------------------------------
//                  ( Cartesian, Cylindrical, Spherical )
MakeGridDialog::MakeGridDialog( QWidget* parent, Qt::WindowFlags f )
: HexaBaseDialog(parent, f)
{
  setupUi( this );

  _vertexLineEdits << vex_le_rb0 << vex_le_rb1 << vex_le_rb2;
  _vectorLineEdits << vec_le_rb0 << vec_x_le_rb1 << vec_z_le_rb1 << vec_le_rb2;

  foreach(QLineEdit* le,  _vertexLineEdits)
    le->installEventFilter(this);
  foreach(QLineEdit* le,  _vectorLineEdits)
    le->installEventFilter(this);

  rb0->click();
  _currentObj = vex_le_rb0;
//   _currentLineEdit->setFocus();
}


MakeGridDialog::~MakeGridDialog()
{
}

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

    Q_ASSERT(ivex_rb0.isValid());
    Q_ASSERT(ivec_rb0.isValid());
    iNewElts = _documentModel->makeCartesian( ivex_rb0, ivec_rb0, nx, ny, nz );

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

    Q_ASSERT(ivex_rb1.isValid());
    Q_ASSERT(ivecx_rb1.isValid());
    Q_ASSERT(ivecz_rb1.isValid());
    iNewElts = _documentModel->makeCylindrical( ivex_rb1,
                                                ivecx_rb1, ivecz_rb1,
                                                dr, da, dl, nr, na, nl, fill);

  } else if ( rb2->isChecked() ){ //spherical
    QModelIndex ivex_rb2  = patternDataModel->mapToSource( _index[vex_le_rb2] );
    QModelIndex ivecx_rb2 = patternBuilderModel->mapToSource( _index[vec_le_rb2] );
    int nb = nb_spb_rb2->value();
    int k  = k_spb_rb2->value();

    Q_ASSERT(ivex_rb2.isValid());
    Q_ASSERT(ivecx_rb2.isValid());
    iNewElts = _documentModel->makeSpherical( ivex_rb2, ivecx_rb2, nb, k );
  }


  if ( iNewElts.isValid() ){
    _setAllSelection();
    QDialog::accept();
    iNewElts = patternBuilderModel->mapFromSource(iNewElts);
    _patternBuilderSelectionModel->select( iNewElts,
                                           QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current );
    _patternBuilderSelectionModel->setCurrentIndex( iNewElts, QItemSelectionModel::Current  );
    SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "MAKE GRID DONE" ) );
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE GRID" ) );
  }
}




void MakeGridDialog::reject()
{
  _setAllSelection();
  QDialog::reject();
}









MakeCylinderDialog::MakeCylinderDialog( QWidget* parent, Qt::WindowFlags f )
: HexaBaseDialog(parent, f)
{
  setupUi( this );

  _cylinderLineEdits << cyl_le;
  _vectorLineEdits << vec_le;

  cyl_le->installEventFilter(this);
  vec_le->installEventFilter(this);

  _currentObj = cyl_le;
}

MakeCylinderDialog::~MakeCylinderDialog()
{
}


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

  Q_ASSERT( icyl.isValid() );
  Q_ASSERT( ivec.isValid() );
  iElts = _documentModel->makeCylinder( icyl, ivec, nr, na, nl );
  if ( iElts.isValid() ){
    _setAllSelection();
    QDialog::accept();
    iElts = patternBuilderModel->mapFromSource( iElts );
    _patternBuilderSelectionModel->select( iElts,
                                           QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current );
    _patternBuilderSelectionModel->setCurrentIndex( iElts, QItemSelectionModel::Current  );
    SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "MAKE CYLINDER DONE" ) );
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE CYLINDER" ) );
  }
}




void MakeCylinderDialog::reject()
{
  _setAllSelection();
  QDialog::reject();
}






MakePipeDialog::MakePipeDialog( QWidget* parent, Qt::WindowFlags f )
: HexaBaseDialog(parent, f)
{
  setupUi( this );
  _pipeLineEdits << pipe_le;
  _vectorLineEdits << vec_le;
  pipe_le->installEventFilter(this);
  vec_le->installEventFilter(this);

  _currentObj = pipe_le;
}

MakePipeDialog::~MakePipeDialog()
{
}

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

  Q_ASSERT( ipipe.isValid() );
  Q_ASSERT( ivec.isValid() );
  iElts = _documentModel->makeCylinder( ipipe, ivec, nr, na, nl );
  if ( iElts.isValid() ){
    _setAllSelection();
    QDialog::accept();
    iElts = patternBuilderModel->mapFromSource( iElts );
    _patternBuilderSelectionModel->select( iElts,
                                           QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current );
    _patternBuilderSelectionModel->setCurrentIndex( iElts, QItemSelectionModel::Current  );
    SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "MAKE PIPE DONE" ) );
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE PIPE" ) );
  }
}




void MakePipeDialog::reject()
{
  _setAllSelection();
  QDialog::reject();
}








MakeCylindersDialog::MakeCylindersDialog( QWidget* parent, Qt::WindowFlags f )
: HexaBaseDialog(parent, f)
{
  setupUi( this );

  _cylinderLineEdits << cyl1_le << cyl2_le;

  cyl1_le->installEventFilter(this);
  cyl2_le->installEventFilter(this);

  _currentObj = cyl1_le;
}

MakeCylindersDialog::~MakeCylindersDialog()
{
}


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

  Q_ASSERT( icyl1.isValid() );
  Q_ASSERT( icyl2.isValid() );
  iCrossElts = _documentModel->makeCylinders( icyl1, icyl2 );
  if ( iCrossElts.isValid() ){
    _setAllSelection();
    QDialog::accept();
    iCrossElts = patternBuilderModel->mapFromSource( iCrossElts );
    _patternBuilderSelectionModel->select( iCrossElts,
                                           QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current );
    _patternBuilderSelectionModel->setCurrentIndex( iCrossElts, QItemSelectionModel::Current  );
    SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "MAKE CYLINDERS DONE" ) );
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE CYLINDERS" ) );
  }
}


void MakeCylindersDialog::reject()
{
  _setAllSelection();
  QDialog::reject();
}




MakePipesDialog::MakePipesDialog( QWidget* parent, Qt::WindowFlags f )
: HexaBaseDialog(parent, f)
{
  setupUi( this );

  _pipeLineEdits << pipe1_le << pipe2_le;
  _currentObj = pipe1_le;
}

MakePipesDialog::~MakePipesDialog()
{
}


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

  Q_ASSERT( ipipe1.isValid() );
  Q_ASSERT( ipipe2.isValid() );
  iCrossElts = _documentModel->makeCylinders( ipipe1, ipipe2 );
  if ( iCrossElts.isValid() ){
    _setAllSelection();
    QDialog::accept();
    iCrossElts = patternBuilderModel->mapFromSource( iCrossElts );
    _patternBuilderSelectionModel->select( iCrossElts,
                                           QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current );
    _patternBuilderSelectionModel->setCurrentIndex( iCrossElts, QItemSelectionModel::Current  );
    SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "MAKE PIPES DONE" ) );
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE PIPES" ) );
  }
}


void MakePipesDialog::reject()
{
  _setAllSelection();
  QDialog::reject();
}





RemoveHexaDialog::RemoveHexaDialog( QWidget* parent, Qt::WindowFlags f )
: HexaBaseDialog(parent, f)
{
  setupUi( this );

  _hexaLineEdits << hexa_le;
  hexa_le->installEventFilter(this);
  _currentObj = hexa_le;
}


RemoveHexaDialog::~RemoveHexaDialog()
{
}


void RemoveHexaDialog::accept()
{
  SUIT_OverrideCursor wc;
  //if ( !_documentModel ) return;
  if ( !_patternDataSelectionModel ) return;
  const PatternDataModel*    patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  if ( !patternDataModel ) return;

  QModelIndex ihexa = patternDataModel->mapToSource( _index[hexa_le] );
  Q_ASSERT( ihexa.isValid() );

  bool removed = false;
  if ( connected_cb->isChecked() ){
    removed = _documentModel->removeConnectedHexa( ihexa );
  } else {
    removed = _documentModel->removeHexa( ihexa );
  }

  if ( removed == true ){
    _setAllSelection();
    QDialog::accept();
    SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "HEXA(S) REMOVED" ) );
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT REMOVE HEXA" ) );
  }
}


void RemoveHexaDialog::reject()
{
  _setAllSelection();
  QDialog::reject();
}





PrismQuadDialog::PrismQuadDialog( QWidget* parent, Qt::WindowFlags f )
: HexaBaseDialog(parent, f)
{
  setupUi( this );

  _vectorLineEdits << vec_le;
  vec_le->installEventFilter(this);
  quads_lw->installEventFilter(this);

  _currentObj = quads_lw;

  connect(add_pb, SIGNAL(clicked()), this, SLOT(addQuad()));
  connect(remove_pb, SIGNAL(clicked()), this, SLOT(removeQuad()));
  connect(clear_pb, SIGNAL(clicked()), this, SLOT(clearQuads()));
}


PrismQuadDialog::~PrismQuadDialog()
{
}


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
  Q_ASSERT( ivec.isValid() );
  int nb = nb_spb->value();
  ////

  QModelIndex iElts;
  if ( iquads.count() == 1 ){
    iElts = _documentModel->prismQuad( iquads[0], ivec, nb );
  } else if ( iquads.count() > 1 ){
    iElts = _documentModel->prismQuads( iquads, ivec, nb );
  }

  if ( iElts.isValid() ){
    _setAllSelection();
    QDialog::accept();
    if ( _patternBuilderSelectionModel ){
      const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
      if ( patternBuilderModel ){
        iElts = patternBuilderModel->mapFromSource( iElts );
        _patternBuilderSelectionModel->select( iElts,
                  QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current );
        _patternBuilderSelectionModel->setCurrentIndex( iElts, QItemSelectionModel::Current  );
      }
      SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "PRISM QUAD(S) OK" ) );
    } else {
      SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT PRISM QUAD(S)" ) );
    }
  }
}


void PrismQuadDialog::reject()
{
  _setAllSelection();
  QDialog::reject();
}





//JoinQuadDialog
JoinQuadDialog::JoinQuadDialog( QWidget* parent, Qt::WindowFlags f )
: HexaBaseDialog(parent, f)
{
  setupUi( this );

  _vertexLineEdits << vex0_le << vex1_le << vex2_le << vex3_le;
  _quadLineEdits   << quad_dest_le;

  vex0_le->installEventFilter(this);
  vex1_le->installEventFilter(this); 
  vex2_le->installEventFilter(this);
  vex3_le->installEventFilter(this);
  quad_dest_le->installEventFilter(this);
  quads_lw->installEventFilter(this);

  _currentObj = quads_lw;

  connect(add_pb, SIGNAL(clicked()), this, SLOT(addQuad()));
  connect(remove_pb, SIGNAL(clicked()), this, SLOT(removeQuad()));
  connect(clear_pb, SIGNAL(clicked()), this, SLOT(clearQuads()));
}


JoinQuadDialog::~JoinQuadDialog()
{
}


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
    Q_ASSERT( iquad.isValid() );
    iquads << iquad;
  }
  QModelIndex iquaddest = patternDataModel->mapToSource( _index[quad_dest_le] );
  Q_ASSERT( iquaddest.isValid() );
  QModelIndex ivex0 = patternDataModel->mapToSource( _index[vex0_le] );
  QModelIndex ivex1 = patternDataModel->mapToSource( _index[vex1_le] );
  QModelIndex ivex2 = patternDataModel->mapToSource( _index[vex2_le] );
  QModelIndex ivex3 = patternDataModel->mapToSource( _index[vex3_le] );
  int nb = nb_spb->value();
  Q_ASSERT( ivex0.isValid() );
  Q_ASSERT( ivex1.isValid() );
  Q_ASSERT( ivex2.isValid() );
  Q_ASSERT( ivex3.isValid() );

  //
  QModelIndex iElts;
  if ( iquads.count() == 1 ){
    iElts = _documentModel->joinQuad( iquads[0], iquaddest,
                                      ivex0, ivex1, ivex2, ivex3,
                                      nb );
  } else if ( iquads.count() > 1 ){
    iElts = _documentModel->joinQuads( iquads, iquaddest,
                                       ivex0, ivex1, ivex2, ivex3,
                                       nb );
  }

  if ( iElts.isValid() ){
    _setAllSelection();
    QDialog::accept();
    if ( _patternBuilderSelectionModel ){
      const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
      if ( patternBuilderModel ){
        iElts = patternBuilderModel->mapFromSource( iElts );
        _patternBuilderSelectionModel->select( iElts,
                  QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current );
        _patternBuilderSelectionModel->setCurrentIndex( iElts, QItemSelectionModel::Current  );
      }
      SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "JOIN QUAD(S) OK" ) );
    } else {
      SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT JOIN QUAD(S)" ) );
    }
  }
}


void JoinQuadDialog::reject()
{
  _setAllSelection();
  QDialog::reject();
}








// ------------------------- Merge ----------------------------------
MergeDialog::MergeDialog( QWidget* parent, Qt::WindowFlags f )
: HexaBaseDialog(parent, f)
{
  setupUi( this );

  _vertexLineEdits << v0_le_rb0 << v1_le_rb0 
                   << v0_le_rb1 << v1_le_rb1 
                   << v0_le_rb2 << v1_le_rb2 << v2_le_rb2 << v3_le_rb2;
  _edgeLineEdits << e0_le_rb1 << e1_le_rb1;
  _quadLineEdits << q0_le_rb2 << q1_le_rb2;

  foreach(QLineEdit* le,  _vertexLineEdits)
    le->installEventFilter(this);
  foreach(QLineEdit* le,  _edgeLineEdits)
    le->installEventFilter(this);
  foreach(QLineEdit* le,  _quadLineEdits)
    le->installEventFilter(this);

  // Default : vertex, 1st one
  rb0->click();
  _currentObj = v0_le_rb0;
//   _currentLineEdit->setFocus();
}


MergeDialog::~MergeDialog()
{
}


void MergeDialog::accept()
{
  SUIT_OverrideCursor wc;
  if ( !_patternDataSelectionModel )    return;
  if ( !_patternBuilderSelectionModel ) return;
  const PatternDataModel*    patternDataModel    = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  if ( !patternDataModel ) return;
  if ( !patternBuilderModel ) return;
  
  bool merged = false;

  if ( rb0->isChecked() ){ 
    QModelIndex iv0 = patternDataModel->mapToSource( _index[v0_le_rb0] );
    QModelIndex iv1 = patternDataModel->mapToSource( _index[v1_le_rb0] );

    Q_ASSERT(iv0.isValid());
    Q_ASSERT(iv1.isValid());
    merged =  _documentModel->mergeVertices( iv0, iv1 );

  } else if ( rb1->isChecked() ){ 
    QModelIndex ie0 = patternDataModel->mapToSource( _index[e0_le_rb1] );
    QModelIndex ie1 = patternDataModel->mapToSource( _index[e1_le_rb1] );
    QModelIndex iv0 = patternDataModel->mapToSource( _index[v0_le_rb1] );
    QModelIndex iv1 = patternDataModel->mapToSource( _index[v1_le_rb1] );

    Q_ASSERT(ie0.isValid());
    Q_ASSERT(ie1.isValid());
    Q_ASSERT(iv0.isValid());
    Q_ASSERT(iv1.isValid());
    merged =  _documentModel->mergeEdges( ie0, ie1, iv0, iv1);

  } else if ( rb2->isChecked() ){ 

    QModelIndex iq0 = patternDataModel->mapToSource( _index[q0_le_rb2] );
    QModelIndex iq1 = patternDataModel->mapToSource( _index[q1_le_rb2] );

    QModelIndex iv0 = patternDataModel->mapToSource( _index[v0_le_rb2] );
    QModelIndex iv1 = patternDataModel->mapToSource( _index[v1_le_rb2] );
    QModelIndex iv2 = patternDataModel->mapToSource( _index[v2_le_rb2] );
    QModelIndex iv3 = patternDataModel->mapToSource( _index[v3_le_rb2] );

    Q_ASSERT(iq0.isValid());
    Q_ASSERT(iq1.isValid());
    Q_ASSERT(iv0.isValid());
    Q_ASSERT(iv1.isValid());
    Q_ASSERT(iv2.isValid());
    Q_ASSERT(iv3.isValid());
    merged =  _documentModel->mergeQuads( iq0, iq1,
                                          iv0, iv1,
                                          iv2, iv3 );
  }

  if ( merged == true ){
    _setAllSelection();
    QDialog::accept();
    SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "MERGED" ) );
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MERGE" ) );
  }
}


void MergeDialog::reject()
{
  _setAllSelection();
  QDialog::reject();
}







// ------------------------- Disconnect ----------------------------------
DisconnectDialog::DisconnectDialog( QWidget* parent, Qt::WindowFlags f )
: HexaBaseDialog(parent, f)
{
  setupUi( this );

  _vertexLineEdits << v_le_rb0;
  _edgeLineEdits   << e_le_rb1;
  _quadLineEdits   << q_le_rb2;
  _hexaLineEdits   << h_le_rb0 << h_le_rb1 << h_le_rb2;

  //vertex
  v_le_rb0->installEventFilter(this);
  h_le_rb0->installEventFilter(this);

  //edge
  e_le_rb1->installEventFilter(this);
  h_le_rb1->installEventFilter(this);

  //quad
  q_le_rb2->installEventFilter(this);
  h_le_rb2->installEventFilter(this);

  // Default : vertex, 1st one
  rb0->click();
  _currentObj = v_le_rb0;
//   _currentLineEdit->setFocus();
}


DisconnectDialog::~DisconnectDialog()
{
}



void DisconnectDialog::accept()
{
  SUIT_OverrideCursor wc;
  if ( !_patternDataSelectionModel ) return;
  const PatternDataModel* patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );

  QModelIndex iElts;

  if ( rb0->isChecked() ){ 
    QModelIndex ihexa = patternDataModel->mapToSource( _index[h_le_rb0] );
    QModelIndex ivex  = patternDataModel->mapToSource( _index[v_le_rb0] );

    Q_ASSERT(ihexa.isValid());
    Q_ASSERT(ivex.isValid());
    iElts = _documentModel->disconnectVertex( ihexa, ivex );
  } else if ( rb1->isChecked() ){ 
    QModelIndex ihexa = patternDataModel->mapToSource( _index[h_le_rb1] );
    QModelIndex iedge = patternDataModel->mapToSource( _index[e_le_rb1] );

    Q_ASSERT(ihexa.isValid());
    Q_ASSERT(iedge.isValid());
    iElts = _documentModel->disconnectEdge( ihexa, iedge );
  } else if ( rb2->isChecked() ){ 
    QModelIndex ihexa = patternDataModel->mapToSource( _index[h_le_rb2] );
    QModelIndex iquad = patternDataModel->mapToSource( _index[q_le_rb2] );

    Q_ASSERT(ihexa.isValid());
    Q_ASSERT(iquad.isValid());
    iElts = _documentModel->disconnectQuad( ihexa, iquad );
  }

  if ( iElts.isValid() ){
    _setAllSelection();
    QDialog::accept();

    if ( _patternBuilderSelectionModel ){
      const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
      if ( patternBuilderModel ){
        iElts = patternBuilderModel->mapFromSource( iElts );
        _patternBuilderSelectionModel->select( iElts,
                  QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current );
        _patternBuilderSelectionModel->setCurrentIndex( iElts, QItemSelectionModel::Current  );
      }
      SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "DISCONNECT OK" ) );
    } else {
      SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT DISCONNECT" ) );
    }
  }
}


void DisconnectDialog::reject()
{
  _setAllSelection();
  QDialog::reject();
}










// ------------------------- CutEdgeDialog ----------------------------------
CutEdgeDialog::CutEdgeDialog( QWidget* parent, Qt::WindowFlags f ):
  HexaBaseDialog(parent, f)
{
  setupUi( this );
  _edgeLineEdits   << e_le;

  e_le->installEventFilter(this);
  _currentObj = e_le;
//   e_le->setFocus();
}


CutEdgeDialog::~CutEdgeDialog()
{
}


void CutEdgeDialog::accept()
{
  SUIT_OverrideCursor wc;
  //if ( !_documentModel ) return;
  if ( !_patternDataSelectionModel ) return;
  const PatternDataModel* patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  if ( !patternDataModel ) return;

  int nbCut = nb_cut_spb->value();
  QModelIndex iedge = patternDataModel->mapToSource( _index[e_le] );

  Q_ASSERT(iedge.isValid());
  QModelIndex iElts = _documentModel->cutEdge( iedge, nbCut );
  if ( iElts.isValid() ){
    _setAllSelection();
    QDialog::accept();
    if ( _patternBuilderSelectionModel ){
      const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
      if  ( patternBuilderModel ){
        iElts = patternBuilderModel->mapFromSource( iElts );
        _patternBuilderSelectionModel->select( iElts, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current ); 
        _patternBuilderSelectionModel->setCurrentIndex ( iElts, QItemSelectionModel::Current  );
      }
    }
    SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "EDGE CUTTED" ) );
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT CUT EDGE" ) );
  }
}


void CutEdgeDialog::reject()
{
  _setAllSelection();
  QDialog::reject();
}




// // ------------------------- MakeTransformationDialog ----------------------------------
MakeTransformationDialog::MakeTransformationDialog( QWidget* parent, Qt::WindowFlags f )
: HexaBaseDialog(parent, f)
{
  setupUi( this );
  setWindowTitle( tr("MAKE TRANSFORMATION") );

  _vertexLineEdits << vex_le_rb1 << vex_le_rb2;
  _vectorLineEdits << vec_le_rb0 << vec_le_rb2;
  _elementsLineEdits << elts_le_rb0 << elts_le_rb1 << elts_le_rb2;

  foreach(QLineEdit* le,  _vertexLineEdits)
    le->installEventFilter(this);
  foreach(QLineEdit* le,  _vectorLineEdits)
    le->installEventFilter(this);
  foreach(QLineEdit* le,  _elementsLineEdits)
    le->installEventFilter(this);

  // Default : Translation, select elements
  rb0->click();
  _currentObj = elts_le_rb0;
//   _currentLineEdit->setFocus();
}


MakeTransformationDialog::~MakeTransformationDialog()
{
}


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

    Q_ASSERT(ielts.isValid());
    Q_ASSERT(ivec.isValid());
    iNewElts = _documentModel->makeTranslation( ielts, ivec );

  } else if ( rb1->isChecked() ){ 
    QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb1] );
    QModelIndex  ivex = patternDataModel->mapToSource( _index[vex_le_rb1] );
    double          k = k_spb->value();

    Q_ASSERT(ielts.isValid());
    Q_ASSERT(ivex.isValid());
    iNewElts = _documentModel->makeScale( ielts, ivex, k );

  } else if ( rb2->isChecked() ){
    QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb2] );
    QModelIndex  ivex = patternDataModel->mapToSource( _index[vex_le_rb2] );
    QModelIndex  ivec = patternBuilderModel->mapToSource( _index[vec_le_rb2] );
    double      angle = angle_spb->value();

    Q_ASSERT(ielts.isValid());
    Q_ASSERT(ivex.isValid());
    Q_ASSERT(ivec.isValid());
    iNewElts = _documentModel->makeRotation( ielts, ivex, ivec, angle );
  }

  if ( iNewElts.isValid() ){
    _setAllSelection();
    QDialog::accept();
    iNewElts = patternBuilderModel->mapFromSource( iNewElts );
    _patternBuilderSelectionModel->select( iNewElts,
              QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current );
    _patternBuilderSelectionModel->setCurrentIndex( iNewElts, QItemSelectionModel::Current  );
    SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "TRANSFORMATION DONE" ) );
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE TRANSFORMATION" ) );
  }
}


void MakeTransformationDialog::reject()
{
  _setAllSelection();
  QDialog::reject();
}





// // ------------------------- MakeSymmetryDialog ----------------------------------
MakeSymmetryDialog::MakeSymmetryDialog( QWidget* parent, Qt::WindowFlags f )
: HexaBaseDialog(parent, f)
{
  setupUi( this );
  setWindowTitle( tr("MAKE SYMMETRY") );

  _vertexLineEdits << vex_le_rb0 << vex_le_rb1 << vex_le_rb2;
  _vectorLineEdits << vec_le_rb1 << vec_le_rb2;
  _elementsLineEdits << elts_le_rb0 << elts_le_rb1 << elts_le_rb2;

  foreach(QLineEdit* le,  _vertexLineEdits)
    le->installEventFilter(this);
  foreach(QLineEdit* le,  _vectorLineEdits)
    le->installEventFilter(this);
  foreach(QLineEdit* le,  _elementsLineEdits)
    le->installEventFilter(this);

  // Default : Point Symmetry, select elements
  rb0->click();
  _currentObj = elts_le_rb0;
//   _currentLineEdit->setFocus();
}


MakeSymmetryDialog::~MakeSymmetryDialog()
{
}


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

    Q_ASSERT(ielts.isValid());
    Q_ASSERT(ivex.isValid());
    iNewElts = _documentModel->makeSymmetryPoint( ielts, ivex );

  } else if ( rb1->isChecked() ){ 
    QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb1] );
    QModelIndex  ivex = patternDataModel->mapToSource( _index[vex_le_rb1] );
    QModelIndex  ivec = patternBuilderModel->mapToSource( _index[vec_le_rb1] );

    Q_ASSERT(ielts.isValid());
    Q_ASSERT(ivex.isValid());
    Q_ASSERT(ivec.isValid());
    iNewElts = _documentModel->makeSymmetryLine( ielts, ivex, ivec );

  } else if ( rb2->isChecked() ){
    QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb2] );
    QModelIndex  ivex = patternDataModel->mapToSource( _index[vex_le_rb2] );
    QModelIndex  ivec = patternBuilderModel->mapToSource( _index[vec_le_rb2] );

    Q_ASSERT(ielts.isValid());
    Q_ASSERT(ivex.isValid());
    Q_ASSERT(ivec.isValid());
    iNewElts = _documentModel->makeSymmetryPlane( ielts, ivex, ivec );
  }

  if ( iNewElts.isValid() ){
    _setAllSelection();
    QDialog::accept();
    iNewElts = patternBuilderModel->mapFromSource( iNewElts );
    _patternBuilderSelectionModel->select( iNewElts,
              QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current );
    _patternBuilderSelectionModel->setCurrentIndex( iNewElts, QItemSelectionModel::Current  );
    SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "TRANSFORMATION DONE" ) );
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE TRANSFORMATION" ) );
  }
}


void MakeSymmetryDialog::reject()
{
  _setAllSelection();
  QDialog::reject();
}




// // ------------------------- PerformTransformationDialog ----------------------------------
PerformTransformationDialog::PerformTransformationDialog( QWidget* parent, Qt::WindowFlags f )
: HexaBaseDialog(parent, f)
{
  setupUi( this );
  setWindowTitle( tr("PERFORM TRANSFORMATION") );

  _vertexLineEdits << vex_le_rb1 << vex_le_rb2;
  _vectorLineEdits << vec_le_rb0 << vec_le_rb2;
  _elementsLineEdits << elts_le_rb0 << elts_le_rb1 << elts_le_rb2;

  foreach(QLineEdit* le,  _vertexLineEdits)
    le->installEventFilter(this);
  foreach(QLineEdit* le,  _vectorLineEdits)
    le->installEventFilter(this);
  foreach(QLineEdit* le,  _elementsLineEdits)
    le->installEventFilter(this);

  // Default : Translation, select elements
  rb0->click();
  _currentObj = elts_le_rb0;
//   _currentLineEdit->setFocus();
}


PerformTransformationDialog::~PerformTransformationDialog()
{
}


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

    Q_ASSERT(ielts.isValid());
    Q_ASSERT(ivec.isValid());
    performed = _documentModel->performTranslation( ielts, ivec );

  } else if ( rb1->isChecked() ){ 
    QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb1] );
    QModelIndex  ivex = patternDataModel->mapToSource( _index[vex_le_rb1] );
    double          k = k_spb->value();

    Q_ASSERT(ielts.isValid());
    Q_ASSERT(ivex.isValid());
    performed = _documentModel->performScale( ielts, ivex, k );

  } else if ( rb2->isChecked() ){
    QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb2] );
    QModelIndex  ivex = patternDataModel->mapToSource( _index[vex_le_rb2] );
    QModelIndex  ivec = patternBuilderModel->mapToSource( _index[vec_le_rb2] );
    double      angle = angle_spb->value();

    Q_ASSERT(ielts.isValid());
    Q_ASSERT(ivex.isValid());
    Q_ASSERT(ivec.isValid());
    performed = _documentModel->performRotation( ielts, ivex, ivec, angle );
  }

  if ( performed == true ){
    _setAllSelection();
    QDialog::accept();
    SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "TRANSFORMATION DONE" ) );
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT PERFORM TRANSFORMATION" ) );
  }
}


void PerformTransformationDialog::reject()
{
  _setAllSelection();
  QDialog::reject();
}


// // ------------------------- PerformSymmetryDialog ----------------------------------
PerformSymmetryDialog::PerformSymmetryDialog( QWidget* parent, Qt::WindowFlags f )
: HexaBaseDialog(parent, f)
{
  setupUi( this );
  setWindowTitle( tr("PERFORM SYMMETRY") );

  _vertexLineEdits << vex_le_rb0 << vex_le_rb1 << vex_le_rb2;
  _vectorLineEdits << vec_le_rb1 << vec_le_rb2;
  _elementsLineEdits << elts_le_rb0 << elts_le_rb1 << elts_le_rb2;

  foreach(QLineEdit* le,  _vertexLineEdits)
    le->installEventFilter(this);
  foreach(QLineEdit* le,  _vectorLineEdits)
    le->installEventFilter(this);
  foreach(QLineEdit* le,  _elementsLineEdits)
    le->installEventFilter(this);

  // Default : Point Symmetry, select elements
  rb0->click();
  _currentObj = elts_le_rb0;
//   _currentLineEdit->setFocus();
}


PerformSymmetryDialog::~PerformSymmetryDialog()
{
}


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

    Q_ASSERT(ielts.isValid());
    Q_ASSERT(ivex.isValid());
    performed = _documentModel->performSymmetryPoint( ielts, ivex );

  } else if ( rb1->isChecked() ){ 
    QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb1] );
    QModelIndex  ivex = patternDataModel->mapToSource( _index[vex_le_rb1] );
    QModelIndex  ivec = patternBuilderModel->mapToSource( _index[vec_le_rb1] );

    Q_ASSERT(ielts.isValid());
    Q_ASSERT(ivex.isValid());
    Q_ASSERT(ivec.isValid());
    performed = _documentModel->performSymmetryLine( ielts, ivex, ivec );

  } else if ( rb2->isChecked() ){
    QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb2] );
    QModelIndex  ivex = patternDataModel->mapToSource( _index[vex_le_rb2] );
    QModelIndex  ivec = patternBuilderModel->mapToSource( _index[vec_le_rb2] );

    Q_ASSERT(ielts.isValid());
    Q_ASSERT(ivex.isValid());
    Q_ASSERT(ivec.isValid());
    performed = _documentModel->performSymmetryPlane( ielts, ivex, ivec );
  }


  if ( performed == true ){
    _setAllSelection();
    QDialog::accept();
    SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "SYMMETRY DONE" ) );
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT PERFORM SYMMETRY" ) );
  }
}


void PerformSymmetryDialog::reject()
{
  _setAllSelection();
  QDialog::reject();
}






// ------------------------- GROUP ----------------------------------
GroupDialog::GroupDialog( QWidget* parent, Qt::WindowFlags f )
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

  eltBase_lw->installEventFilter(this);
  _currentObj = eltBase_lw;

  connect(kind_cb,  SIGNAL(activated(int)), this, SLOT(onKindChanged(int)) );
  connect(add_pb, SIGNAL(clicked()), this, SLOT(addEltBase()));
  connect(add_pb, SIGNAL(clicked()), this, SLOT(addEltBase()));
  connect(remove_pb, SIGNAL(clicked()), this, SLOT(removeEltBase()));
  connect(clear_pb, SIGNAL(clicked()), this, SLOT(clearEltBase()));
}


GroupDialog::~GroupDialog()
{
}

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
  for ( int nr = 0; nr < g->countElement(); ++nr ){
    eltBase = g->getElement( nr );
    eltBase->getName(pName);
    item = new QListWidgetItem( QString(pName) );
    if ( _documentModel ){
      eltBaseItems = _documentModel->findItems( _value->getName(), Qt::MatchExactly | Qt::MatchRecursive, 1);
      iEltBase = eltBaseItems[0]->index();
      item->setData(  LW_QMODELINDEX_ROLE, QVariant::fromValue<QModelIndex>(iEltBase) );
    }
    eltBase_lw->addItem( item );
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
  if ( !_groupsSelectionModel )       return;
  const PatternDataModel* patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  const GroupsModel*           groupsModel = dynamic_cast<const GroupsModel*>( _groupsSelectionModel->model() );

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
      iEltBase = patternDataModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() );
      Q_ASSERT( iEltBase.isValid() );
      eltAdded = _documentModel->addGroupElement( iGrp, iEltBase );
      if ( eltAdded == false ){
        SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "ELEMENT NOT ADDED" ) );
      }
    }
    iGrp = groupsModel->mapFromSource( iGrp );
    _groupsSelectionModel->select( iGrp, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current );
    _groupsSelectionModel->setCurrentIndex( iGrp, QItemSelectionModel::Current );
    SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "GROUP ADDED" ) );
    _setAllSelection();
    QDialog::accept();
  } else {
      SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT ADD GROUP" ) );
  }

}

void GroupDialog::reject()
{
  _setAllSelection();
  QDialog::reject();
}








// ------------------------- LAW ----------------------------------
LawDialog::LawDialog( QWidget* parent, Qt::WindowFlags f )
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
  char pName[12];
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
    bool setOk = _documentModel->setLaw( iLaw, nbnodes, coeff, lawKind );
    _groupsSelectionModel->select( iLaw, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current );
    _groupsSelectionModel->setCurrentIndex( iLaw, QItemSelectionModel::Current );
    SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "LAW ADDED" ) );
    _setAllSelection();
    QDialog::accept();
  } else {
      SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT ADD LAW" ) );
  }

}

void LawDialog::reject()
{
  _setAllSelection();
  QDialog::reject();
}







