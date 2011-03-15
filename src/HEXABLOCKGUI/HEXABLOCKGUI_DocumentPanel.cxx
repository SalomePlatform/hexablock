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
  _patternDataModel(0),
  _patternBuilderModel(0),
  _patternDataSelectionModel(0),
  _patternBuilderSelectionModel(0)
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


void HexaBaseDialog::onPatternDataSelectionChanged(  const QItemSelection& sel, const QItemSelection& unsel )
{
}

void HexaBaseDialog::onPatternBuilderSelectionChanged(  const QItemSelection& sel, const QItemSelection& unsel )
{
}





// ------------------------- VERTEX ----------------------------------
VertexDialog::VertexDialog( QWidget* parent, Qt::WindowFlags f )
: HexaBaseDialog(parent, f),
  _value(0)
{
  setupUi( this );
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




// ------------------------- QUAD ----------------------------------
QuadDialog::QuadDialog( QWidget* parent, Qt::WindowFlags f ):
  HexaBaseDialog(parent, f),
  _value(0)
{
  setupUi( this );

  v0_le->installEventFilter(this);
  v1_le->installEventFilter(this);
  v2_le->installEventFilter(this);
  v3_le->installEventFilter(this);

  // Default : select first Vertex
  _currentVertexIndex    = &_v0Index;
  _currentVertexLineEdit = v0_le;
//   _currentVertexLineEdit->setFocus();
}


QuadDialog::~QuadDialog()
{

}

bool QuadDialog::eventFilter(QObject *obj, QEvent *event)
{
    if ( event->type() == QEvent::FocusIn ){ //QEvent::KeyPress) { 

        if ( obj == v0_le or obj == v1_le or obj == v2_le or obj == v3_le ) {
          _setVertexSelectionOnly();
        }

        if ( obj == v0_le ) {
          _currentVertexIndex     = &_v0Index;
          _currentVertexLineEdit  = v0_le;
        } else if ( obj == v1_le ) {
          _currentVertexIndex     = &_v1Index;
          _currentVertexLineEdit  = v1_le;
        } else if ( obj == v2_le ) {
          _currentVertexIndex     = &_v2Index;
          _currentVertexLineEdit  = v2_le;
        } else if ( obj == v3_le ) {
          _currentVertexIndex     = &_v3Index;
          _currentVertexLineEdit  = v3_le;
        }
        
        return false;
    } else {
         // standard event processing
         return QObject::eventFilter(obj, event);   
    }

}


void QuadDialog::setValue(HEXA_NS::Quad* h)
{
  char pName[12];
  HEXA_NS::Vertex* v0 = h->getVertex(0);
  HEXA_NS::Vertex* v1 = h->getVertex(1);
  HEXA_NS::Vertex* v2 = h->getVertex(2);
  HEXA_NS::Vertex* v3 = h->getVertex(3);

  v0_le->setText( v0->getName(pName) );
  v1_le->setText( v1->getName(pName) );
  v2_le->setText( v2->getName(pName) );
  v3_le->setText( v3->getName(pName) );

  _value = h;

  buttonBox->clear();
}

HEXA_NS::Quad* QuadDialog::getValue()
{
  return _value;
}


void QuadDialog::onPatternDataSelectionChanged(  const QItemSelection& sel,
                                                 const QItemSelection& unsel )
{
//   if ( _currentVertexIndex == NULL ) return;

  QModelIndexList l = _patternDataSelectionModel->selectedIndexes ();

  if ( l.count() > 0 ){ 
    *_currentVertexIndex = l[0];
    _currentVertexLineEdit->setText( _currentVertexIndex->data().toString() );
  }
}


void QuadDialog::accept()
{
  SUIT_OverrideCursor wc;
  //if ( !_documentModel ) return;
  if ( !_patternDataSelectionModel ) return;
  const PatternDataModel* patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  if ( !patternDataModel ) return;

  QModelIndex v0Index = patternDataModel->mapToSource( _v0Index );
  QModelIndex v1Index = patternDataModel->mapToSource( _v1Index );
  QModelIndex v2Index = patternDataModel->mapToSource( _v2Index );
  QModelIndex v3Index = patternDataModel->mapToSource( _v3Index );

  Q_ASSERT(v0Index.isValid());
  Q_ASSERT(v1Index.isValid());
  Q_ASSERT(v2Index.isValid());
  Q_ASSERT(v3Index.isValid());

  QModelIndex newIndex = _documentModel->addQuad( v0Index, v1Index, v2Index, v3Index );

  if ( newIndex.isValid() ){
    _value = newIndex.model()->data(newIndex, HEXA_DATA_ROLE).value<HEXA_NS::Quad *>();
    _setAllSelection();
    QDialog::accept();
    newIndex = patternDataModel->mapFromSource( newIndex );
    _patternDataSelectionModel->select( newIndex,
                                        QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current );
    _patternDataSelectionModel->setCurrentIndex( newIndex,
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

  q0_le->installEventFilter(this);
  q1_le->installEventFilter(this);
  q2_le->installEventFilter(this);
  q3_le->installEventFilter(this);
  q4_le->installEventFilter(this);
  q5_le->installEventFilter(this);

  // Default : select first Quad
  _currentQuadIndex    = &_q0Index;
  _currentQuadLineEdit = q0_le;
//   _currentQuadLineEdit->setFocus();


}


HexaDialog::~HexaDialog()
{

}

bool HexaDialog::eventFilter(QObject *obj, QEvent *event)
{
    if ( event->type() == QEvent::FocusIn ){ //QEvent::KeyPress) { 

        if (  obj == q0_le or obj == q1_le or obj == q2_le 
           or obj == q3_le or obj == q4_le or obj == q5_le ) {
          _setQuadSelectionOnly();
        }

        if ( obj == q0_le ) {
          _currentQuadIndex    = &_q0Index;
          _currentQuadLineEdit = q0_le;
        } else if ( obj == q1_le ) {
          _currentQuadIndex    = &_q1Index;
          _currentQuadLineEdit = q1_le;
        } else if ( obj == q2_le ) {
          _currentQuadIndex    = &_q2Index;
          _currentQuadLineEdit = q2_le;
        } else if ( obj == q3_le ) {
          _currentQuadIndex    = &_q3Index;
          _currentQuadLineEdit = q3_le;
        } else if ( obj == q4_le ) {
          _currentQuadIndex    = &_q4Index;
          _currentQuadLineEdit = q4_le;
        } else if ( obj == q5_le ) {
          _currentQuadIndex    = &_q5Index;
          _currentQuadLineEdit = q5_le;
        }
        
        return false;
    } else {
         // standard event processing
         return QObject::eventFilter(obj, event);
    }
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

  q0_le->setText( q0->getName(pName) );
  q1_le->setText( q1->getName(pName) );
  q2_le->setText( q2->getName(pName) );
  q3_le->setText( q3->getName(pName) );
  q4_le->setText( q4->getName(pName) );
  q5_le->setText( q5->getName(pName) );

  _value = h;

  buttonBox->clear();
}

HEXA_NS::Hexa* HexaDialog::getValue()
{
  return _value;
}


void HexaDialog::onPatternDataSelectionChanged(  const QItemSelection& sel,
                                                 const QItemSelection& unsel )
{
//   if ( _currentVertexIndex == NULL ) return;

  QModelIndexList l = _patternDataSelectionModel->selectedIndexes ();

  if ( l.count() > 0 ){ 
    *_currentQuadIndex = l[0];
    _currentQuadLineEdit->setText( _currentQuadIndex->data().toString() );
  }
}


void HexaDialog::accept()
{
  SUIT_OverrideCursor wc;
  //if ( !_documentModel ) return;
  if ( !_patternDataSelectionModel ) return;
  const PatternDataModel* patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  if ( !patternDataModel ) return;

  QModelIndex q0Index = patternDataModel->mapToSource( _q0Index );
  QModelIndex q1Index = patternDataModel->mapToSource( _q1Index );
  QModelIndex q2Index = patternDataModel->mapToSource( _q2Index );
  QModelIndex q3Index = patternDataModel->mapToSource( _q3Index );
  QModelIndex q4Index = patternDataModel->mapToSource( _q4Index );
  QModelIndex q5Index = patternDataModel->mapToSource( _q5Index );

  Q_ASSERT(q0Index.isValid());
  Q_ASSERT(q1Index.isValid());
  Q_ASSERT(q2Index.isValid());
  Q_ASSERT(q3Index.isValid());
  Q_ASSERT(q4Index.isValid());
  Q_ASSERT(q5Index.isValid());

  QModelIndex newIndex = _documentModel->addHexaFromQuad( q0Index, q1Index, q2Index, q3Index, q4Index, q5Index );

  if ( newIndex.isValid() ){
    _value = newIndex.model()->data(newIndex, HEXA_DATA_ROLE).value<HEXA_NS::Hexa*>();
    _setAllSelection();
    QDialog::accept();
    newIndex = patternDataModel->mapFromSource( newIndex );
    _patternDataSelectionModel->select( newIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current );
    _patternDataSelectionModel->setCurrentIndex ( newIndex, QItemSelectionModel::Current  );
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



// ------------------------- MergeVertices ----------------------------------
MergeVerticesDialog::MergeVerticesDialog( QWidget* parent, Qt::WindowFlags f )
: HexaBaseDialog(parent, f)
{
  setupUi( this );

  v0_le->installEventFilter(this);
  v1_le->installEventFilter(this);

  // Default : select first Vertex
  _currentVertexIndex    = &_v0Index;
  _currentVertexLineEdit = v0_le;
//   _currentVertexLineEdit->setFocus();"
}


MergeVerticesDialog::~MergeVerticesDialog()
{
}



bool MergeVerticesDialog::eventFilter(QObject *obj, QEvent *event)
{
    if ( event->type() == QEvent::FocusIn ){ //QEvent::KeyPress) { 
        if ( obj == v0_le or obj == v1_le ){
          _setVertexSelectionOnly();  //allow vertex selection only 
        }

        if ( obj == v0_le ) {
          _currentVertexIndex = &_v0Index;
          _currentVertexLineEdit    = v0_le;
        } else if ( obj == v1_le ) {
          _currentVertexIndex = &_v1Index;
          _currentVertexLineEdit    = v1_le;
        }

        return false;
    } else {
         // standard event processing
         return QObject::eventFilter(obj, event);
    }
}


void MergeVerticesDialog::onPatternDataSelectionChanged(  const QItemSelection& sel,
                                                 const QItemSelection& unsel )
{
//   if ( _currentVertexIndex == NULL ) return;

  QModelIndexList l = _patternDataSelectionModel->selectedIndexes ();

  if ( l.count() > 0 ){ 
    *_currentVertexIndex = l[0];
    _currentVertexLineEdit->setText( _currentVertexIndex->data().toString() );
  }
}

void MergeVerticesDialog::accept()
{
  SUIT_OverrideCursor wc;
  //if ( !_documentModel ) return;
  if ( !_patternDataSelectionModel ) return;
  const PatternDataModel* patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  if ( !patternDataModel ) return;

  QModelIndex v0Index = patternDataModel->mapToSource( _v0Index );
  QModelIndex v1Index = patternDataModel->mapToSource( _v1Index );

  Q_ASSERT(v0Index.isValid());
  Q_ASSERT(v1Index.isValid());

  bool merged = _documentModel->mergeVertices( v0Index, v1Index );

  if ( merged == true ){
    _setAllSelection();
    QDialog::accept();
    SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "HEXA VERTEX MERGED" ) );
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MERGE VERTEX" ) );
  }
}


void MergeVerticesDialog::reject()
{
  _setAllSelection();
  QDialog::reject();
}


// ------------------------- MergeEdgesDialog ----------------------------------
MergeEdgesDialog::MergeEdgesDialog( QWidget* parent, Qt::WindowFlags f )
: HexaBaseDialog(parent, f)
{
  setupUi( this );

  e0_le->installEventFilter(this);
  e1_le->installEventFilter(this);
  v0_le->installEventFilter(this);
  v1_le->installEventFilter(this);

  // Default : select first edge
  _currentIndex    = &_e0Index;
  _currentLineEdit = e0_le;
//   _currentLineEdit->setFocus();

}


MergeEdgesDialog::~MergeEdgesDialog()
{
}


bool MergeEdgesDialog::eventFilter(QObject *obj, QEvent *event)
{
    if ( event->type() == QEvent::FocusIn ){ //QEvent::KeyPress) { 
        if ( obj == e0_le ) {
          _currentIndex     = &_e0Index;
          _currentLineEdit  = e0_le;
          _setEdgeSelectionOnly();
        } else if ( obj == e1_le ) {
          _currentIndex     = &_e1Index;
          _currentLineEdit  = e1_le;
          _setEdgeSelectionOnly();
        } else if ( obj == v0_le ) {
          _currentIndex     = &_v0Index;
          _currentLineEdit  = v0_le;
          _setVertexSelectionOnly();
        } else if ( obj == v1_le ) {
          _currentIndex     = &_v1Index;
          _currentLineEdit  = v1_le;
          _setVertexSelectionOnly();
        }
        return false;
    } else {
         // standard event processing
         return QObject::eventFilter(obj, event);
    }
}



void MergeEdgesDialog::onPatternDataSelectionChanged( const QItemSelection& sel,
                                                      const QItemSelection& unsel )
{
  QModelIndexList l = _patternDataSelectionModel->selectedIndexes ();

  if ( l.count() > 0 ){ 
    *_currentIndex = l[0];
    _currentLineEdit->setText( _currentIndex->data().toString() );
  }
}

void MergeEdgesDialog::accept()
{
  SUIT_OverrideCursor wc;
  //if ( !_documentModel ) return;
  if ( !_patternDataSelectionModel ) return;
  const PatternDataModel* patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  if ( !patternDataModel ) return;

  QModelIndex e0Index = patternDataModel->mapToSource( _e0Index );
  QModelIndex e1Index = patternDataModel->mapToSource( _e1Index );

  QModelIndex v0Index = patternDataModel->mapToSource( _v0Index );
  QModelIndex v1Index = patternDataModel->mapToSource( _v1Index );

  Q_ASSERT(e0Index.isValid());
  Q_ASSERT(e1Index.isValid());
  Q_ASSERT(v0Index.isValid());
  Q_ASSERT(v1Index.isValid());

  bool merged = _documentModel->mergeEdges( e0Index, e1Index, v0Index, v1Index );

  if ( merged == true ){
    _setAllSelection();
    QDialog::accept();
    SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "EDGE MERGED" ) );
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MERGE EDGE" ) );
  }
}


void MergeEdgesDialog::reject()
{
  _setAllSelection();
  QDialog::reject();
}



// ------------------------- CutEdgeDialog ----------------------------------
CutEdgeDialog::CutEdgeDialog( QWidget* parent, Qt::WindowFlags f ):
  HexaBaseDialog(parent, f)
{
  setupUi( this );
//   e_le->setFocus();
}


CutEdgeDialog::~CutEdgeDialog()
{
}


void CutEdgeDialog::onPatternDataSelectionChanged( const QItemSelection& sel,
                                                 const QItemSelection& unsel )
{
//   if ( _currentVertexIndex == NULL ) return;

  QModelIndexList l = _patternDataSelectionModel->selectedIndexes ();

  if ( l.count() > 0 ){ 
    _eIndex = l[0];
    e_le->setText( _eIndex.data().toString() );
  }
}



// void CutEdgeDialog::accept()
// {
//   if ( !_patternDataModel ) return;
// 
//   int nbCut = nb_cut_spb->value();
//   QModelIndex newEltsIndex = _patternDataModel->cutEdge( _eIndex, nbCut );
// 
//   if ( newEltsIndex.isValid() ){
//     _setAllSelection();
//     QDialog::accept();
// //     _selectionModel->select( newEltsIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current ); CS_TODO
// //     _selectionModel->setCurrentIndex ( newEltsIndex, QItemSelectionModel::Current  );
//   } else {
//     SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT CUT EDGE" ) );
//   }
// }



void CutEdgeDialog::accept()
{
  SUIT_OverrideCursor wc;
  //if ( !_documentModel ) return;
  if ( !_patternDataSelectionModel ) return;
  const PatternDataModel* patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  if ( !patternDataModel ) return;

  int nbCut = nb_cut_spb->value();
  QModelIndex eIndex = patternDataModel->mapToSource( _eIndex );

  QModelIndex newEltsIndex = _documentModel->cutEdge( eIndex, nbCut );
  if ( newEltsIndex.isValid() ){
    _setAllSelection();
    QDialog::accept();
    if ( _patternBuilderSelectionModel ){
      const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
      if  ( patternBuilderModel ){
        newEltsIndex = patternBuilderModel->mapFromSource( newEltsIndex );
        _patternBuilderSelectionModel->select( newEltsIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current ); 
        _patternBuilderSelectionModel->setCurrentIndex ( newEltsIndex, QItemSelectionModel::Current  );
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
}

HEXA_NS::Vector* VectorDialog::getValue()
{
  return _value;
}



// void VectorDialog::accept()
// {
//     if ( _patternBuilderModel ){ //NEW MODE
// //     std::cout<<"add vertex!! add vertex!! add vertex!! add vertex!!"<<std::endl;
//         QModelIndex newIndex = _patternBuilderModel->addVector( dx_spb->value(), dy_spb->value(),  dz_spb->value() );
//         if ( newIndex.isValid() ){
//           _value = newIndex.model()->data(newIndex, HEXA_DATA_ROLE).value<HEXA_NS::Vector *>();
//           QDialog::accept();
//           if ( _patternBuilderSelectionModel ){
//             _patternBuilderSelectionModel->select( newIndex,QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current );
//             _patternBuilderSelectionModel->setCurrentIndex ( newIndex, QItemSelectionModel::Current  );
//           }
//         } else {
//           SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT BUILD VECTOR" ) );
//         }
//     }
// }



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









// ------------------------- MakeCartesianDialog ----------------------------------
MakeCartesianDialog::MakeCartesianDialog( QWidget* parent, Qt::WindowFlags f )
: HexaBaseDialog(parent, f)
{
  setupUi( this );

  pt_le->installEventFilter(this);
  vec_x_le->installEventFilter(this);
  vec_y_le->installEventFilter(this);
  vec_z_le->installEventFilter(this);


  // Default : select pt
  _currentIndex    = &_ptIndex;
  _currentLineEdit = pt_le;
//   _currentLineEdit->setFocus();


}


MakeCartesianDialog::~MakeCartesianDialog()
{
}



bool MakeCartesianDialog::eventFilter(QObject *obj, QEvent *event)
{
    if ( event->type() == QEvent::FocusIn ){ //QEvent::KeyPress) { 
        if ( obj == pt_le ) {
          _currentIndex     = &_ptIndex;
          _currentLineEdit  = pt_le;
           _setVertexSelectionOnly();
        } else if ( obj == vec_x_le ) {
          _currentIndex     = &_vecXIndex;
          _currentLineEdit  = vec_x_le;
          _setVectorSelectionOnly();
        } else if ( obj == vec_y_le ) {
          _currentIndex     = &_vecYIndex;
          _currentLineEdit  = vec_y_le;
          _setVectorSelectionOnly();
        } else if ( obj == vec_z_le ) {
          _currentIndex     = &_vecZIndex;
          _currentLineEdit  = vec_z_le;
          _setVectorSelectionOnly();
        }
        return false;
    } else {
         // standard event processing
         return QObject::eventFilter(obj, event);
    }
}



void MakeCartesianDialog::onPatternDataSelectionChanged( const QItemSelection& sel,
                                                  const QItemSelection& unsel )
{
  if ( _currentLineEdit == pt_le ){
    QModelIndexList l = _patternDataSelectionModel->selectedIndexes ();

    if ( l.count() > 0 ){ 
      *_currentIndex = l[0];
      _currentLineEdit->setText( _currentIndex->data().toString() );
    }
  }
}


void MakeCartesianDialog::onPatternBuilderSelectionChanged( const QItemSelection& sel,
                                                     const QItemSelection& unsel )
{
  if (    _currentLineEdit == vec_x_le
      or  _currentLineEdit == vec_y_le 
      or  _currentLineEdit == vec_z_le ){
    QModelIndexList l = _patternBuilderSelectionModel->selectedIndexes ();

    if ( l.count() > 0 ){
      *_currentIndex = l[0];
      _currentLineEdit->setText( _currentIndex->data().toString() );
    }
  }
}


void MakeCartesianDialog::accept()
{
  SUIT_OverrideCursor wc;
  //if ( !_documentModel ) return;
  if ( !_patternDataSelectionModel )    return;
  if ( !_patternBuilderSelectionModel ) return;
  const PatternDataModel*    patternDataModel    = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  if ( !patternDataModel ) return;
  if ( !patternBuilderModel ) return;


  QModelIndex ptIndex   = patternDataModel->mapToSource( _ptIndex );

  QModelIndex vecXIndex = patternBuilderModel->mapToSource( _vecXIndex );
  QModelIndex vecYIndex = patternBuilderModel->mapToSource( _vecYIndex );
  QModelIndex vecZIndex = patternBuilderModel->mapToSource( _vecZIndex );

  Q_ASSERT(ptIndex.isValid());
  Q_ASSERT(vecXIndex.isValid());
  Q_ASSERT(vecYIndex.isValid());
  Q_ASSERT(vecZIndex.isValid());

  long nx = nx_spb->value();
  long ny = ny_spb->value();
  long nz = nz_spb->value();


  QModelIndex newEltsIndex = _documentModel->makeCartesian( ptIndex,
                                                    vecXIndex, vecYIndex, vecZIndex,
                                                    nx, ny, nz );
  if ( newEltsIndex.isValid() ){
    _setAllSelection();
    QDialog::accept();
    newEltsIndex = patternBuilderModel->mapFromSource(newEltsIndex);
    _patternBuilderSelectionModel->select( newEltsIndex, 
                                            QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current );
    _patternBuilderSelectionModel->setCurrentIndex( newEltsIndex,
                                            QItemSelectionModel::Current  );
    SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "MAKE CARTESIAN GRID DONE" ) );
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE CARTESIAN GRID" ) );
  }
}




void MakeCartesianDialog::reject()
{
  _setAllSelection();
  QDialog::reject();
}



// ------------------------- MakeCylindricalDialog ----------------------------------
MakeCylindricalDialog::MakeCylindricalDialog( QWidget* parent, Qt::WindowFlags f )
: HexaBaseDialog( parent, f )
{
  setupUi( this );

  pt_le->installEventFilter(this);
  vec_x_le->installEventFilter(this);
  vec_z_le->installEventFilter(this);


  // Default : select pt
  _currentIndex    = &_ptIndex;
  _currentLineEdit = pt_le;
//   _currentLineEdit->setFocus();


}


MakeCylindricalDialog::~MakeCylindricalDialog()
{
}



bool MakeCylindricalDialog::eventFilter(QObject *obj, QEvent *event)
{
    if ( event->type() == QEvent::FocusIn ){ //QEvent::KeyPress) { 
        if ( obj == pt_le ) {
          _currentIndex     = &_ptIndex;
          _currentLineEdit  = pt_le;
           _setVertexSelectionOnly();
        } else if ( obj == vec_x_le ) {
          _currentIndex     = &_vecXIndex;
          _currentLineEdit  = vec_x_le;
          _setVectorSelectionOnly();
        } else if ( obj == vec_z_le ) {
          _currentIndex     = &_vecZIndex;
          _currentLineEdit  = vec_z_le;
          _setVectorSelectionOnly();
        }
        return false;
    } else {
         // standard event processing
         return QObject::eventFilter(obj, event);
    }
}


void MakeCylindricalDialog::onPatternDataSelectionChanged( const QItemSelection& sel,
                                                  const QItemSelection& unsel )
{
  if ( _currentLineEdit == pt_le ){
    QModelIndexList l = _patternDataSelectionModel->selectedIndexes ();

    if ( l.count() > 0 ){ 
      *_currentIndex = l[0];
      _currentLineEdit->setText( _currentIndex->data().toString() );
    }
  }
}


void MakeCylindricalDialog::onPatternBuilderSelectionChanged( const QItemSelection& sel,
                                                     const QItemSelection& unsel )
{
  if (    _currentLineEdit == vec_x_le
      or  _currentLineEdit == vec_z_le ){
    QModelIndexList l = _patternBuilderSelectionModel->selectedIndexes ();

    if ( l.count() > 0 ){
      *_currentIndex = l[0];
      _currentLineEdit->setText( _currentIndex->data().toString() );
    }
  }
}



void MakeCylindricalDialog::accept()
{
  SUIT_OverrideCursor wc;
  //if ( !_documentModel ) return;
  if ( !_patternDataSelectionModel ) return;
  if ( !_patternBuilderSelectionModel ) return;

  const PatternDataModel*    patternDataModel    = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );

  if ( !patternDataModel ) return;
  if ( !patternBuilderModel ) return;

  QModelIndex ptIndex   = patternDataModel->mapToSource( _ptIndex );
  QModelIndex vecXIndex = patternBuilderModel->mapToSource( _vecXIndex );
  QModelIndex vecZIndex = patternBuilderModel->mapToSource( _vecZIndex );

  Q_ASSERT(ptIndex.isValid());
  Q_ASSERT(vecXIndex.isValid());
  Q_ASSERT(vecZIndex.isValid());

  double dr = dr_spb->value();
  double da = da_spb->value();
  double dl = dl_spb->value();

  long nr = nr_spb->value();
  long na = na_spb->value();
  long nl = nl_spb->value();
  
  bool fill = fill_cb->isChecked();

  QModelIndex newEltsIndex = _documentModel->makeCylindrical( ptIndex,
                                                      vecXIndex, vecZIndex,
                                                      dr, da, dl, 
                                                      nr, na, nl,
                                                      fill );
  if ( newEltsIndex.isValid() ){
    _setAllSelection();
    QDialog::accept();
    newEltsIndex = patternBuilderModel->mapFromSource( newEltsIndex );
    _patternBuilderSelectionModel->select( newEltsIndex,
                                           QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current );
    _patternBuilderSelectionModel->setCurrentIndex( newEltsIndex,
                                                    QItemSelectionModel::Current );
    SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "MAKE CYLINDRICAL GRID DONE" ) );
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE CYLINDRICAL GRID" ) );
  }
}












void MakeCylindricalDialog::reject()
{
  _setAllSelection();
  QDialog::reject();
}








// ------------------------- MakeTranslationDialog ----------------------------------
MakeTranslationDialog::MakeTranslationDialog( QWidget* parent, Qt::WindowFlags f )
: HexaBaseDialog( parent, f )
{
  setupUi( this );

  elt_le->installEventFilter(this);
  vec_le->installEventFilter(this);

  // Default : select pt
  _currentIndex    = &_eltIndex;
  _currentLineEdit = elt_le;
//   _currentLineEdit->setFocus();

}


MakeTranslationDialog::~MakeTranslationDialog()
{
}



bool MakeTranslationDialog::eventFilter(QObject *obj, QEvent *event)
{
    if ( event->type() == QEvent::FocusIn ){ //QEvent::KeyPress) { 
        if ( obj == elt_le ) {
          _currentIndex     = &_eltIndex;
          _currentLineEdit  = elt_le;
           _setElementsSelectionOnly();
        } else if ( obj == vec_le ) {
          _currentIndex     = &_vecIndex;
          _currentLineEdit  = vec_le;
          _setVectorSelectionOnly();
        }
        return false;
    } else {
         // standard event processing
         return QObject::eventFilter(obj, event);
    }
}


void MakeTranslationDialog::onPatternBuilderSelectionChanged( const QItemSelection& sel,
                                                            const QItemSelection& unsel )
{
  if ( _currentLineEdit == elt_le
       or _currentLineEdit == vec_le ){
    QModelIndexList l = _patternBuilderSelectionModel->selectedIndexes ();

    if ( l.count() > 0 ){
      *_currentIndex = l[0];
      _currentLineEdit->setText( _currentIndex->data().toString() );
    }
  }
}


void MakeTranslationDialog::accept()
{
  SUIT_OverrideCursor wc;
  if ( !_patternBuilderSelectionModel ) return;
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  if ( !patternBuilderModel ) return;

  QModelIndex eltIndex = patternBuilderModel->mapToSource( _eltIndex );
  QModelIndex vecIndex = patternBuilderModel->mapToSource( _vecIndex );

  Q_ASSERT(eltIndex.isValid());
  Q_ASSERT(vecIndex.isValid());

  QModelIndex newEltsIndex = _documentModel->makeTranslation( eltIndex, vecIndex );
  if ( newEltsIndex.isValid() ){
    _setAllSelection();
    QDialog::accept();
    newEltsIndex = patternBuilderModel->mapFromSource(newEltsIndex);
    _patternBuilderSelectionModel->select( newEltsIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current );
    _patternBuilderSelectionModel->setCurrentIndex ( newEltsIndex, QItemSelectionModel::Current  );
    SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "MAKE TRANSLATION DONE" ) );
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE TRANSLATION" ) );
  }

}



void MakeTranslationDialog::reject()
{
  _setAllSelection();
  QDialog::reject();
}


