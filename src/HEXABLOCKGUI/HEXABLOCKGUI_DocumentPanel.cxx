#include "HEXABLOCKGUI_DocumentPanel.hxx"
#include "HEXABLOCKGUI_DocumentGraphicView.hxx"

#include "HEXABLOCKGUI.hxx"

// #include "SALOME_Selection.h"
#include <SUIT_MessageBox.h>
#include "SVTK_Selection.h"

#include <iostream>
#include <QtGui>
#include <QFlags>


using namespace std;
using namespace HEXABLOCK::GUI;




HexaBaseDialog::HexaBaseDialog():
  _model(0),
  _selectionModel(0)
{
}


HexaBaseDialog::~HexaBaseDialog()
{
}


void HexaBaseDialog::_setAllSelection()
{
  //allow all selection
  if ( _model ){
    _model->vertexItemFlags =  Qt::NoItemFlags ;
    _model->edgeItemFlags   =  Qt::NoItemFlags ;
    _model->quadItemFlags   =  Qt::NoItemFlags ;
    _model->hexaItemFlags   =  Qt::NoItemFlags ;
  }
}





void HexaBaseDialog::_setQuadSelectionOnly()
{
  //allow quad selection only 
  if ( _model ){
    _model->vertexItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _model->edgeItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _model->quadItemFlags = Qt::ItemFlags( ~Qt::ItemIsEditable );
    _model->hexaItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  }

  if ( _selectionModel ){
    _selectionModel->SetSelectionMode(FaceSelection);
  }
}

void HexaBaseDialog::_setEdgeSelectionOnly()
{
  //allow edge selection only
  if ( _model ){
    _model->vertexItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _model->edgeItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEditable );
    _model->quadItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _model->hexaItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  }

  if ( _selectionModel ){
    _selectionModel->SetSelectionMode(EdgeSelection);
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
  //allow vertex selection only
  if ( _model ){
    _model->vertexItemFlags = Qt::ItemFlags( ~Qt::ItemIsEditable);
    _model->edgeItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _model->quadItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _model->hexaItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  }

  if ( _selectionModel ){
    _selectionModel->SetSelectionMode(NodeSelection);
  }
}


void HexaBaseDialog::setModel(PatternDataModel*  m)
{
  _model = m;
}


void HexaBaseDialog::setSelectionModel(DocumentSelectionModel* s)
{
  _selectionModel = s;
  _selectionModel->clearSelection();
}









// ------------------------- VERTEX ----------------------------------
VertexDialog::VertexDialog( QWidget* parent, Qt::WindowFlags f )
: QDialog( parent, f ),
  HexaBaseDialog(),
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


void VertexDialog::accept()
{
    if (_value)
    { //EDITION MODE     //CS_BP vérifier que setModelData, getValue appelé
      std::cout<<"edit vertex!! edit vertex!! edit vertex!!"<<std::endl;
        _value->setX( x_spb->value() );
        _value->setY( y_spb->value() );
        _value->setZ( z_spb->value() );
        // mise a jour de la vue :
        // pourquoi on a un pointeur ver le modele (HexVertex)
        // et pas vers l'index ??? 
        // plantage ici ???
        if (_selectionModel != NULL)
        {
            std::cout << "accept() : OK" << std::endl;
            
            if (_model != NULL)
            {
                std::cout << "accept() : _model != NULL" << std::endl;                
                _model->updateVertex(_selectionModel->currentIndex());
                std::cout << "accept() 2 : _model != NULL" << std::endl;                                
            }
            else
            {
                std::cout << "accept() : _model NULL" << std::endl;
            }
        }
        else
        {
            std::cout << "accept() : NOT OK" << std::endl;

        }
        // ????

    }
    else if ( _model )
    { //NEW MODE
//     std::cout<<"add vertex!! add vertex!! add vertex!! add vertex!!"<<std::endl;
        QModelIndex newIndex = _model->addVertex( x_spb->value(), y_spb->value(),  z_spb->value() );
        _value = newIndex.model()->data(newIndex, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();

        _selectionModel->select( newIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current );
        _selectionModel->setCurrentIndex ( newIndex, QItemSelectionModel::Current  );
    }

    QDialog::accept();
}








// ------------------------- QUAD ----------------------------------
QuadDialog::QuadDialog( QWidget* parent, Qt::WindowFlags f ):
  QDialog( parent, f ),
  HexaBaseDialog(),
  _value(0)
{
  setupUi( this );

  // Default : select first Vertex
  _currentVertexIndex    = &_v0Index;
  _currentVertexLineEdit = v0_le;

  v0_le->installEventFilter(this);
  v1_le->installEventFilter(this);
  v2_le->installEventFilter(this);
  v3_le->installEventFilter(this);
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



void QuadDialog::setSelectionModel(DocumentSelectionModel* s)
{
  HexaBaseDialog::setSelectionModel(s);
  connect( _selectionModel, SIGNAL( selectionChanged ( const QItemSelection &, const QItemSelection &) ),
          this,             SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
//   connect( _selectionModel, SIGNAL( selectionChanged( const QItemSelection & )),
//            this, SLOT( selectionChanged( const QItemSelection & )));
}


void QuadDialog::onSelectionChanged(  const QItemSelection& sel,
                                      const QItemSelection& unsel )
{
  if ( _currentVertexIndex == NULL ) return;

  QModelIndexList l = _selectionModel->selectedIndexes ();

  if ( l.count() > 0 ){ 
    *_currentVertexIndex = l[0];
    _currentVertexLineEdit->setText( _currentVertexIndex->data().toString() );
  }
}



void QuadDialog::accept()
{
  if ( !_model ) return;

  QModelIndex newIndex = _model->addQuad( _v0Index, _v1Index, _v2Index, _v3Index );
  _value = newIndex.model()->data(newIndex, HEXA_DATA_ROLE).value<HEXA_NS::Quad *>();
  _setAllSelection();
  QDialog::accept();
  _selectionModel->select( newIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current );
  _selectionModel->setCurrentIndex ( newIndex, QItemSelectionModel::Current  );
}


void QuadDialog::reject()
{
  _setAllSelection();
  QDialog::reject();
}





// ------------------------- HEXA ----------------------------------
HexaDialog::HexaDialog( QWidget* parent, Qt::WindowFlags f ): 
  QDialog( parent, f ),
  HexaBaseDialog(),
  _value(0)
{
  setupUi( this );

  // Default : select first Quad
  _currentQuadIndex    = &_q0Index;
  _currentQuadLineEdit = q0_le;

  q0_le->installEventFilter(this);
  q1_le->installEventFilter(this);
  q2_le->installEventFilter(this);
  q3_le->installEventFilter(this);
  q4_le->installEventFilter(this);
  q5_le->installEventFilter(this);
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


void HexaDialog::setSelectionModel(DocumentSelectionModel* s)
{
  HexaBaseDialog::setSelectionModel(s);
  connect( _selectionModel, SIGNAL( selectionChanged ( const QItemSelection &, const QItemSelection &) ),
          this,             SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
//   connect( _selectionModel, SIGNAL( selectionChanged( const QItemSelection & )),
//            this, SLOT( selectionChanged( const QItemSelection & )));
}

void HexaDialog::onSelectionChanged(  const QItemSelection& sel,
                                      const QItemSelection& unsel )
{
  QModelIndexList l = _selectionModel->selectedIndexes ();

  if ( l.count() > 0 ){ 
    *_currentQuadIndex = l[0];
    _currentQuadLineEdit->setText( _currentQuadIndex->data().toString() );
  }
}


void HexaDialog::accept()
{
  if ( !_model ) return;

  QModelIndex newIndex = _model->addHexaFromQuad( _q0Index, _q1Index, _q2Index, _q3Index, _q4Index, _q5Index );

  if ( newIndex.isValid() ){
    _value = newIndex.model()->data(newIndex, HEXA_DATA_ROLE).value<HEXA_NS::Hexa*>();
    _setAllSelection();
    QDialog::accept();
    _selectionModel->select( newIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current );
    _selectionModel->setCurrentIndex ( newIndex, QItemSelectionModel::Current  );
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
: QDialog( parent, f ),
HexaBaseDialog()
{
  setupUi( this );

  // Default : select first Vertex
  _currentVertexIndex    = &_v0Index;
  _currentVertexLineEdit = v0_le;

  v0_le->installEventFilter(this);
  v1_le->installEventFilter(this);
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


void MergeVerticesDialog::setSelectionModel(DocumentSelectionModel* s)
{
  HexaBaseDialog::setSelectionModel(s);
  connect( _selectionModel, SIGNAL( selectionChanged ( const QItemSelection &, const QItemSelection &) ),
          this,             SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
//   connect( _selectionModel, SIGNAL( selectionChanged( const QItemSelection & )),
//            this, SLOT( selectionChanged( const QItemSelection & )));
}



void MergeVerticesDialog::onSelectionChanged(  const QItemSelection& sel,
                                               const QItemSelection& unsel )
{
  QModelIndexList l = _selectionModel->selectedIndexes ();

  if ( l.count() > 0 ){ 
    *_currentVertexIndex = l[0];
    _currentVertexLineEdit->setText( _currentVertexIndex->data().toString() );
  }
}



void MergeVerticesDialog::accept()
{
  if ( !_model ) return;

  bool merged = _model->mergeVertices( _v0Index, _v1Index );
//   _value = newIndex.model()->data(newIndex, HEXA_DATA_ROLE).value<HEXA_NS::Quad *>();

  if ( merged == true ){
    _setAllSelection();
    QDialog::accept();
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MERGE VERTEX" ) );
  }

//   _selectionModel->select( newIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current );
//   _selectionModel->setCurrentIndex ( newIndex, QItemSelectionModel::Current  );
}




void MergeVerticesDialog::reject()
{
  _setAllSelection();
  QDialog::reject();
}




// ------------------------- MergeEdgesDialog ----------------------------------
MergeEdgesDialog::MergeEdgesDialog( QWidget* parent, Qt::WindowFlags f )
: QDialog( parent, f ),
HexaBaseDialog()
{
  setupUi( this );

  // Default : select first Vertex
  _currentIndex    = &_v0Index;
  _currentLineEdit = v0_le;

  e0_le->installEventFilter(this);
  e1_le->installEventFilter(this);
  v0_le->installEventFilter(this);
  v1_le->installEventFilter(this);

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
          _currentIndex     = &_v1Index;
          _currentLineEdit  = v1_le;
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


void MergeEdgesDialog::setSelectionModel(DocumentSelectionModel* s)
{
  HexaBaseDialog::setSelectionModel(s);
  connect( _selectionModel, SIGNAL( selectionChanged ( const QItemSelection &, const QItemSelection &) ),
           this,             SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
//   connect( _selectionModel, SIGNAL( selectionChanged( const QItemSelection & )),
//            this, SLOT( selectionChanged( const QItemSelection & )));
}



void MergeEdgesDialog::onSelectionChanged(  const QItemSelection& sel,
                                            const QItemSelection& unsel )
{
  QModelIndexList l = _selectionModel->selectedIndexes ();

  if ( l.count() > 0 ){ 
    *_currentIndex = l[0];
    _currentLineEdit->setText( _currentIndex->data().toString() );
  }
}



void MergeEdgesDialog::accept()
{
  if ( !_model ) return;

  bool merged = _model->mergeEdges( _e0Index, _e1Index, _v0Index, _v1Index );

  if ( merged ){
    _setAllSelection();
    QDialog::accept();
//     _selectionModel->select( newIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current );
//     _selectionModel->setCurrentIndex ( newIndex, QItemSelectionModel::Current  );
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
  QDialog( parent, f ),
  HexaBaseDialog()
{
  setupUi( this );
}


CutEdgeDialog::~CutEdgeDialog()
{
}



void CutEdgeDialog::setSelectionModel( DocumentSelectionModel* s )
{
  HexaBaseDialog::setSelectionModel(s);
  _setEdgeSelectionOnly();
  connect( _selectionModel, SIGNAL( selectionChanged ( const QItemSelection &, const QItemSelection &) ),
           this,             SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
//   connect( _selectionModel, SIGNAL( selectionChanged( const QItemSelection & )),
//            this, SLOT( selectionChanged( const QItemSelection & )));
}



void CutEdgeDialog::onSelectionChanged(  const QItemSelection& sel,
                                         const QItemSelection& unsel )
{
  QModelIndexList l = _selectionModel->selectedIndexes ();

  if ( l.count() > 0 ){ 
    _eIndex = l[0];
    e_le->setText( _eIndex.data().toString() );
  }
}


void CutEdgeDialog::accept()
{
  if ( !_model ) return;

  int nbCut = nb_cut_spb->value();
  QModelIndex newEltsIndex = _model->cutEdge( _eIndex, nbCut );

  if ( newEltsIndex.isValid() ){
    _setAllSelection();
    QDialog::accept();
    _selectionModel->select( newEltsIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current );
    _selectionModel->setCurrentIndex ( newEltsIndex, QItemSelectionModel::Current  );
  } else {
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT CUT EDGE" ) );
  }
}


void CutEdgeDialog::reject()
{
  _setAllSelection();
  QDialog::reject();
}

