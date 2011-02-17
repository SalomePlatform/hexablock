#include "HEXABLOCKGUI_DocumentPanel.hxx"
#include "HEXABLOCKGUI_DocumentGraphicView.hxx"

#include "HEXABLOCKGUI.h"

// #include "SALOME_Selection.h"
#include "SVTK_Selection.h"

#include <iostream>
#include <QtGui>
#include <QFlags>


using namespace std;
using namespace HEXABLOCK::GUI;


// ------------------------- VERTEX ----------------------------------
VertexDialog::VertexDialog( QWidget* parent, Qt::WindowFlags f )
: QDialog( parent, f ),
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

void VertexDialog::setModel(PatternDataModel*  m)
{
  _model = m;
}


void VertexDialog::setSelectionModel(DocumentSelectionModel* s)
{
  _selectionModel = s;
}


void VertexDialog::accept()
{
  if (_value){ //EDITION MODE     //CS_BP vérifier que setModelData, getValue appelé
//      std::cout<<"edit vertex!! edit vertex!! edit vertex!!"<<std::endl;
    _value->setX( x_spb->value() );
    _value->setY( y_spb->value() );
    _value->setZ( z_spb->value() );

  } else if ( _model ){ //NEW MODE
//     std::cout<<"add vertex!! add vertex!! add vertex!! add vertex!!"<<std::endl;
    QModelIndex newIndex = _model->addVertex( x_spb->value(), y_spb->value(),  z_spb->value() );
    _value = newIndex.model()->data(newIndex, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();

    _selectionModel->select( newIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current );
  }

  QDialog::accept();
}



// ------------------------- QUAD ----------------------------------
QuadDialog::QuadDialog( QWidget* parent, Qt::WindowFlags f )
: QDialog( parent, f ),
_value(0),
_model(0),
_selectionModel(0)
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
        if ( obj == v0_le ) {
          _currentVertexIndex = &_v0Index;
          _currentVertexLineEdit    = v0_le;
        } else if ( obj == v1_le ) {
          _currentVertexIndex = &_v1Index;
          _currentVertexLineEdit    = v1_le;
        } else if ( obj == v2_le ) {
          _currentVertexIndex = &_v2Index;
          _currentVertexLineEdit    = v2_le;
        } else if ( obj == v3_le ) {
          _currentVertexIndex = &_v3Index;
          _currentVertexLineEdit    = v3_le;
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


void QuadDialog::setModel(PatternDataModel*  m)
{
  _model = m;

  //allow vertex selection only when building a quad
  _model->vertexItemFlags = Qt::ItemFlags(~Qt::ItemIsEditable);
  _model->edgeItemFlags   = Qt::ItemFlags(~Qt::ItemIsEnabled );
  _model->quadItemFlags = Qt::ItemFlags(~Qt::ItemIsEnabled );
  _model->hexaItemFlags = Qt::ItemFlags(~Qt::ItemIsEnabled );

}


void QuadDialog::setSelectionModel(DocumentSelectionModel* s)
{
  _selectionModel = s;

  _selectionModel->clearSelection();
  _selectionModel->SetSelectionMode(NodeSelection);
//  NodeSelection,
//  CellSelection,
//  EdgeOfCellSelection,
//  EdgeSelection,
//  FaceSelection,
//  VolumeSelection,
//  ActorSelection };
  connect( _selectionModel, SIGNAL( selectionChanged ( const QItemSelection &, const QItemSelection &)  ),
            this,             SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
//   connect( _selectionModel, SIGNAL( selectionChanged( const QItemSelection & )),
//            this, SLOT( selectionChanged( const QItemSelection & )));

}



void QuadDialog::onSelectionChanged(  const QItemSelection& sel,
                                      const QItemSelection& unsel )
{
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

  _model->vertexItemFlags =  Qt::NoItemFlags ;
  _model->edgeItemFlags   =  Qt::NoItemFlags ;
  _model->quadItemFlags   =  Qt::NoItemFlags ;
  _model->hexaItemFlags   =  Qt::NoItemFlags ;
  QDialog::accept();
  _selectionModel->select( newIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current );
}



void QuadDialog::reject()
{
  _model->vertexItemFlags =  Qt::NoItemFlags ;
  _model->edgeItemFlags   =  Qt::NoItemFlags ;
  _model->quadItemFlags   =  Qt::NoItemFlags ;
  _model->hexaItemFlags   =  Qt::NoItemFlags ;

  QDialog::reject();

}
