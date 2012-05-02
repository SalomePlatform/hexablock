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

#include <OCCViewer_ViewWindow.h>
#include "HEXABLOCKGUI_DocumentPanel.hxx"
#include "HEXABLOCKGUI_DocumentGraphicView.hxx"
#include "HEXABLOCKGUI_SalomeTools.hxx"
#include "HEXABLOCKGUI.hxx"


#include <iostream>
#include <QtGui>
#include <QFlags>

#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <PyConsole_Console.h>
#include <SalomeApp_Tools.h>

#include <Standard_GUID.hxx>
#include <TDF_Label.hxx>

#include <OCCViewer_ViewWindow.h>
#include <OCCViewer_ViewManager.h>
// #include <OCCViewer_ViewModel.h>


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
#include <VTKViewer_ViewModel.h>

#include <SUIT_ResourceMgr.h>


// #include <GeometryGUI.h>

#include <GEOMBase.h>
#include <GEOMImpl_Types.hxx>



#include "MyGEOMBase_Helper.hxx"
#include "GEOMBase.h"
#include "GEOM_Operation.h"


#define VERTEX_COORD_MIN -1000000
#define VERTEX_COORD_MAX  1000000


using namespace std;
using namespace HEXABLOCK::GUI;


Q_DECLARE_METATYPE(HEXABLOCK::GUI::HexaTreeRole);
Q_DECLARE_METATYPE(TopAbs_ShapeEnum);

class HexaDoubleSpinBoxDelegate : public QStyledItemDelegate  {
    public:
	HexaDoubleSpinBoxDelegate(QObject *parent=0) : QStyledItemDelegate (parent){}

        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                const QModelIndex &index) const{
          return new QDoubleSpinBox( parent );
        }
};




HexaBaseDialog::HexaBaseDialog( QWidget * parent, Mode editmode, Qt::WindowFlags f ):
//   _model(0),
//   _selectionModel(0),
  QDialog(parent, f),
  MyGEOMBase_Helper( dynamic_cast<SUIT_Desktop*>(parent->parent()) ),
  _editMode( editmode ),
  _documentModel(0),
//   _patternDataModel(0),
//   _patternBuilderModel(0),
  _patternDataSelectionModel(0),
  _patternBuilderSelectionModel(0),
  _groupsSelectionModel(0),
  _meshSelectionModel(0),
  _mgr(0),
  _vtkVm(0),
  _occVm(0),
  _currentObj(0),
  _expectedSelection(-1),
  _selectionMutex( false ),
  _applyCloseButton(0),
  _applyButton(0)
{
  MESSAGE("HexaBaseDialog::HexaBaseDialog() dynamic_cast<SUIT_Desktop*>(parent->parent()) =>" << dynamic_cast<SUIT_Desktop*>(parent->parent()) );
  _strHexaWidgetType[VERTEX_TREE] = tr( "VERTEX" );
  _strHexaWidgetType[EDGE_TREE]   = tr( "EDGE" );
  _strHexaWidgetType[QUAD_TREE]   = tr( "QUAD" );
  _strHexaWidgetType[HEXA_TREE]   = tr( "HEXA" );

  _strHexaWidgetType[VECTOR_TREE]   = tr( "VECTOR" );
  _strHexaWidgetType[CYLINDER_TREE] = tr( "CYLINDER" );
  _strHexaWidgetType[PIPE_TREE]     = tr( "PIPE" );
  _strHexaWidgetType[ELEMENTS_TREE] = tr( "ELEMENTS" );
  _strHexaWidgetType[CROSSELEMENTS_TREE]= tr( "CROSSELEMENTS" );

  _strHexaWidgetType[GROUP_TREE]      = tr( "GROUP" );
  _strHexaWidgetType[LAW_TREE]        = tr( "LAW" );
  _strHexaWidgetType[PROPAGATION_TREE]= tr( "PROPAGATION" );

}


HexaBaseDialog::~HexaBaseDialog()
{
//   _mgr->clearSelectionCache();
//   _mgr->clearSelected();
//   globalSelection();
//   erasePreview();
}

QDialogButtonBox* HexaBaseDialog::_initButtonBox( Mode editmode )
{
  if ( editmode == INFO_MODE ) return NULL;
  QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
  buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
  buttonBox->setOrientation(Qt::Horizontal);
  layout()->addWidget(buttonBox);

  QPushButton* closeButton = new QPushButton(tr("Close"));
  QPushButton* helpButton  = new QPushButton(tr("Help"));
  _applyCloseButton = new QPushButton(tr("Apply And Close"));
  _applyButton      = new QPushButton(tr("Apply"));

  buttonBox->addButton( _applyCloseButton, QDialogButtonBox::AcceptRole );
  buttonBox->addButton( _applyButton,      QDialogButtonBox::ApplyRole );
  buttonBox->addButton( closeButton, QDialogButtonBox::RejectRole );
  buttonBox->addButton( helpButton, QDialogButtonBox::HelpRole );

  connect( buttonBox, SIGNAL(accepted()), this, SLOT(accept()) );
  connect( buttonBox, SIGNAL(rejected()), this, SLOT(reject()) );
  connect( _applyButton, SIGNAL(clicked()), this, SLOT(apply()) );
  connect( buttonBox, SIGNAL(helpRequested()), this, SLOT(onHelpRequested()) );

  return buttonBox;
}

void HexaBaseDialog::_initWidget( Mode editmode )
{
  _initInputWidget( editmode );
  _initButtonBox( editmode );
}


void HexaBaseDialog::_initViewManager()
{
  SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
  _mgr   = dynamic_cast<LightApp_SelectionMgr*>( anApp->selectionMgr() );
  _vtkVm = anApp->getViewManager( SVTK_Viewer::Type(),      true );
  _occVm = anApp->getViewManager( OCCViewer_Viewer::Type(), true );
  SUIT_ViewManager* activeVm = anApp->activeViewManager();

//   connect( _mgr,  SIGNAL( currentSelectionChanged() ), this, SLOT( addFace() ) );
//   connect( _vtkVm, SIGNAL( activated(SUIT_ViewManager*) ), this, SLOT( onWindowActivated(SUIT_ViewManager*) ) );
//   connect( _occVm, SIGNAL( activated(SUIT_ViewManager*) ), this, SLOT( onWindowActivated(SUIT_ViewManager*) ) );
  onWindowActivated ( activeVm );
}


bool HexaBaseDialog::apply()
{
  MESSAGE("HexaBaseDialog::apply()");
  QModelIndex iNew;
  bool applied = apply(iNew);
  if ( applied ){
    // clear all selection
    if (_patternDataSelectionModel)    _patternDataSelectionModel->clearSelection();
    if (_patternBuilderSelectionModel) _patternBuilderSelectionModel->clearSelection();
    if (_groupsSelectionModel)         _groupsSelectionModel->clearSelection();
    if (_meshSelectionModel)           _meshSelectionModel->clearSelection();
    // select and highlight in vtk view the result
    _selectAndHighlight( iNew );
    // reinitialization
    _currentObj = NULL;
  }
  return applied;
}

void HexaBaseDialog::accept()
{
  bool applied = apply();
  if ( applied ){
    QDialog::accept();
    _disallowSelection();
  }
}

void HexaBaseDialog::reject()
{
  QDialog::reject();
  _currentObj = NULL;
  _disallowSelection();
}

void HexaBaseDialog::onHelpRequested()
{
  LightApp_Application* app = (LightApp_Application*)( SUIT_Session::session()->activeApplication() );
  if ( app )
//     app->onHelpContextModule( myGeometryGUI ? app->moduleName( myGeometryGUI->moduleName() ) : QString( "" ), _helpFileName );
    app->onHelpContextModule( "HEXABLOCK", _helpFileName );

  else {
    QString platform;
#ifdef WIN32
    platform = "winapplication";
#else
    platform = "application";
#endif

    SUIT_MessageBox::warning( 0, QObject::tr( "WRN_WARNING" ),
                              QObject::tr( "EXTERNAL_BROWSER_CANNOT_SHOW_PAGE" ).
                              arg( app->resourceMgr()->stringValue( "ExternalBrowser", platform ) ).arg( _helpFileName ),
                              QObject::tr( "BUT_OK" ) );
  }

}


void HexaBaseDialog::_selectAndHighlight( const QModelIndex& i )
{
  if ( !i.isValid() ) return;
  setProperty("QModelIndex",  QVariant::fromValue(i));
  setFocus();
}



void HexaBaseDialog::_allowSelection()
{
  MESSAGE("HexaBaseDialog::_allowSelection(){");
  if ( _documentModel ){
    _documentModel->disallowEdition();
  }
  MESSAGE("}");
}

void HexaBaseDialog::_disallowSelection()
{
  MESSAGE("HexaBaseDialog::_disallowSelection(){");
  if ( _documentModel ){
    _documentModel->allowEdition();
  }
  if ( _patternDataSelectionModel ){
    _patternDataSelectionModel->setAllSelection();
  }
  MESSAGE("}");
//   if ( _patternBuilderSelectionModel ){
//     _patternBuilderSelectionModel->setAllSelection();
//   }
//   if ( _meshSelectionModel ){
//     _meshSelectionModel->setAllSelection();
//   }
}






bool HexaBaseDialog::_allowVTKSelection( QObject* obj )
{
  MESSAGE("HexaBaseDialog::_allowModelSelection(){");
  if ( !_patternDataSelectionModel ) return false;
  bool isOk = false;

  HexaWidgetType wType;
  QVariant v  = obj->property("HexaWidgetType");
  if ( !v.isValid() ) {
    MESSAGE("*  no HexaWidgetType property");
    return false;
  }
  MESSAGE("*  HEXABLOCKGUI::currentVtkView->raise()");
  HEXABLOCKGUI::currentVtkView->raise();
//   HEXABLOCKGUI::currentVtkView->setFocus();
  wType = v.value<HexaWidgetType>();
  MESSAGE("*  HexaWidgetType property is " << wType );

//   _documentModel->disallowEdition(); //CS_HERE
  switch (wType){
    case VERTEX_TREE:
      _patternDataSelectionModel->setVertexSelection(); isOk = true;
      break;
    case EDGE_TREE:  
      _patternDataSelectionModel->setEdgeSelection(); isOk = true;
      break;
    case QUAD_TREE:  
      _patternDataSelectionModel->setQuadSelection(); isOk = true;
      break;
    case HEXA_TREE:  
      _patternDataSelectionModel->setHexaSelection(); isOk = true;
      break;
    case VECTOR_TREE:
    case CYLINDER_TREE:
    case PIPE_TREE:
    case ELEMENTS_TREE:
    case CROSSELEMENTS_TREE: //selector = _patternBuilderSelectionModel;
    case GROUP_TREE:      //selector = _groupsSelectionModel;
    case LAW_TREE:
    case PROPAGATION_TREE: // selector = _meshSelectionModel;
    default : MESSAGE("NOT YET");
  }
  MESSAGE("}");
  return isOk;
}


bool HexaBaseDialog::_allowOCCSelection( QObject* obj )
{
  MESSAGE("HexaBaseDialog::_allowOCCCSelection(){");

  GeomWidgetType wType;
  QVariant v = obj->property("GeomWidgetType");
  if ( !v.isValid() ) {
    MESSAGE("*  no GeomWidgetType property");
    return false;
  }
  MESSAGE("*  HEXABLOCKGUI::currentOccView->raise()");
  HEXABLOCKGUI::currentOccView->raise();
  wType = v.value<GeomWidgetType>();
  MESSAGE("*  GeomWidgetType property is " << wType );
  MESSAGE("*  TopAbs_VERTEX           is " << TopAbs_VERTEX );
  MESSAGE("*  TopAbs_EDGE             is " << TopAbs_EDGE );
  MESSAGE("*  TopAbs_FACE             is " << TopAbs_FACE );

  globalSelection(); // close local contexts, if any                 
  localSelection(GEOM::GEOM_Object::_nil(), wType/*TopAbs_EDGE*/);
  return true;
}


QItemSelectionModel* HexaBaseDialog::_getSelector( QObject* obj )
{
  QItemSelectionModel* selector = NULL;
  MESSAGE("HexaBaseDialog::getSelector(){");

  QString objectName = obj->objectName();
  QString className = obj->metaObject()->className();
  MESSAGE("*  obj->objectName() is "<< objectName.toStdString() ); //toStdString()
  MESSAGE("*  obj->metaObject()->className() is "<< className.toStdString() );

  HexaWidgetType wtype;
  QVariant v  = obj->property("HexaWidgetType");
  if ( !v.isValid() ) {
    MESSAGE("*  no HexaWidgetType property");
    return NULL;
  }

  wtype = v.value<HexaWidgetType>();
  MESSAGE("*  HexaWidgetType property is " << wtype);

  switch (wtype){
    case VERTEX_TREE:
    case EDGE_TREE:
    case QUAD_TREE:
    case HEXA_TREE:  
      selector = _patternDataSelectionModel; break;
    case VECTOR_TREE:
    case CYLINDER_TREE:
    case PIPE_TREE:
    case ELEMENTS_TREE:
    case CROSSELEMENTS_TREE:
      selector = _patternBuilderSelectionModel; break;
    case GROUP_TREE:
      selector = _groupsSelectionModel; break;
    case LAW_TREE:
    case PROPAGATION_TREE: 
      selector = _meshSelectionModel; break;
    default : MESSAGE("NOT YET");
  }
  MESSAGE("}");
  return selector;
}


bool HexaBaseDialog::_onSelectionChanged( const QItemSelection& sel, QLineEdit*  le )
{
  MESSAGE("HexaBaseDialog::_onSelectionChanged(const QItemSelection& sel, QLineEdit*  le)");
  MESSAGE("*  le is "<< le->objectName().toStdString() );
  QModelIndexList l = sel.indexes();
  if ( l.count() == 0 ) return false;

  //type from selection (first)
  QModelIndex selected = l[0];
  int selType = selected.data(HEXA_TREE_ROLE).toInt();

  //type of widget
  QVariant v = le->property("HexaWidgetType");
  if ( !v.isValid() ) return false;
  HexaWidgetType wType = v.value<HexaWidgetType>();

  // check selection compatibility between selection and widget
  if ( selType != wType ){
    MESSAGE("*  bad selection : " << selType << " is not  " << wType );
    SUIT_MessageBox::information( 0,
      tr("HEXA_INFO"), 
      tr("%1: Bad type selected\nPlease select a %2.").arg(windowTitle()).arg( _strHexaWidgetType[wType]) );
    return false;
  }

  //fill the lineEdit if selection is OK
  le->setText( selected.data().toString() );// name
  le->setProperty("QModelIndex",  QVariant::fromValue(selected) );
  _index[le] = selected;
  MESSAGE("}");
  return true;
}



bool HexaBaseDialog::_onSelectionChanged( const QItemSelection& sel, QListWidget* lw )
{
  MESSAGE("HexaBaseDialog::_onSelectionChanged( const QItemSelection& sel, QListWidget* lw )");
  MESSAGE("*  lw is "<< lw->objectName().toStdString() );
  QModelIndexList l = sel.indexes();

  if ( l.count() == 0 ) return false;

  //type of widget
  QVariant v = lw->property("HexaWidgetType");
  if ( !v.isValid() ) return false;
  HexaWidgetType wType = v.value<HexaWidgetType>();

  //fill the listWidget
  QListWidgetItem* item = NULL;
  int selType;
  QString selName;
//   int maxSize = 8;
  foreach( const QModelIndex& isel, l ){
//     if ( lw->count() == maxSize) break;
    selType = isel.data(HEXA_TREE_ROLE).toInt();
    if ( selType != wType ){ // check selection compatibility between selection and widget
      MESSAGE("*  bad selection : " << selType<< " is not  " << wType );
      SUIT_MessageBox::information( 0,
        tr("HEXA_INFO"), 
        tr("%1: Bad type selected\nPlease select a %2.").arg(windowTitle()).arg( _strHexaWidgetType[wType]) );
      return false;
    }
    // add selection to listWidget if selection is OK
    selName = isel.data().toString();
    QList<QListWidgetItem *> twice = lw->findItems( selName, Qt::MatchExactly);
    if ( twice.count() == 0 ){
      item = new QListWidgetItem( selName );
      item->setData(  LW_QMODELINDEX_ROLE, QVariant::fromValue<QModelIndex>(isel) );
      lw->addItem(item);
      updateButtonBox();
    }
  }
  MESSAGE("}");
  return true;
}


void HexaBaseDialog::onCurrentSelectionChanged()
{
}


void HexaBaseDialog::setDocumentModel(DocumentModel* m)
{
  _documentModel = m;
}

void HexaBaseDialog::clear()
{
}


void HexaBaseDialog::setPatternDataSelectionModel(PatternDataSelectionModel* s)
{
  _patternDataSelectionModel = s;
}


void HexaBaseDialog::setPatternBuilderSelectionModel(PatternBuilderSelectionModel* s)
{
  _patternBuilderSelectionModel = s;
//   _patternBuilderSelectionModel->clearSelection();
//   connect( _patternBuilderSelectionModel,
//            SIGNAL( selectionChanged( const QItemSelection &, const QItemSelection &) ),
//            this,
//            SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
}

void HexaBaseDialog::setGroupsSelectionModel( GroupsSelectionModel* s )
{
  _groupsSelectionModel = s;
}


void HexaBaseDialog::setMeshSelectionModel( MeshSelectionModel* s )
{
  _meshSelectionModel = s;
//   _meshSelectionModel->clearSelection();

//   connect( _meshSelectionModel,
//            SIGNAL( selectionChanged( const QItemSelection &, const QItemSelection &) ),
//            this,
//            SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
}



void HexaBaseDialog::onSelectionChanged( const QItemSelection& sel, const QItemSelection& unsel )
{
  MESSAGE( "HexaBaseDialog::onSelectionChanged(){" );
  QString className = metaObject()->className();
  MESSAGE( "*  I am                          : " << className.toStdString() );
  MESSAGE( "*  sender is                     : " << sender() );
  MESSAGE( "*  _patternDataSelectionModel    : " << _patternDataSelectionModel );
  MESSAGE( "*  _patternBuilderSelectionModel : " << _patternBuilderSelectionModel );
  MESSAGE( "*  _groupsSelectionModel         : " << _groupsSelectionModel );
  MESSAGE( "*  _meshSelectionModel           : " << _meshSelectionModel );


  QItemSelectionModel* selector = dynamic_cast<QItemSelectionModel*>(sender());
  MESSAGE( "*  selector           : " << selector);

  foreach( const QModelIndex& isel, sel.indexes() ){
    MESSAGE("*  selected : " << isel.data().toString().toStdString());
  }
  foreach( const QModelIndex& iunsel, unsel.indexes() ){
    MESSAGE("*  unselected : " << iunsel.data().toString().toStdString());
  }
  if ( _selectionMutex ) return;
  if ( _editMode == INFO_MODE ) return;


  bool selOk = false;
  QLineEdit*   aLineEdit   = 0;
  QListWidget* aListWidget = 0;
  aLineEdit = dynamic_cast<QLineEdit*>(_currentObj);
  if ( aLineEdit){ //fill the lineedit with selection
    selOk = _onSelectionChanged( sel, aLineEdit );
  }
  aListWidget = dynamic_cast<QListWidget*>(_currentObj);
  if ( aListWidget){ //fill the listWidget with selection
    selOk = _onSelectionChanged( sel, aListWidget );
  }

  if ( !selOk && selector && ( aLineEdit || aListWidget ) ){
    selector->clearSelection();
  }

  MESSAGE("}");
}

void HexaBaseDialog::onWindowActivated( SUIT_ViewManager* vm )
{
}


void HexaBaseDialog::hideEvent ( QHideEvent * event )
{
  MESSAGE("HexaBaseDialog::hideEvent(){");
  QString className = metaObject()->className();
  MESSAGE( "*  I am                          : " << className.toStdString() );

  if ( _patternDataSelectionModel )
    disconnect( _patternDataSelectionModel, SIGNAL( selectionChanged ( const QItemSelection &, const QItemSelection &) ),
                this,                         SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
  if ( _patternBuilderSelectionModel )
    disconnect( _patternBuilderSelectionModel, SIGNAL( selectionChanged( const QItemSelection &, const QItemSelection &) ),
             this,                            SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
  if ( _groupsSelectionModel )
    disconnect( _groupsSelectionModel, SIGNAL( selectionChanged( const QItemSelection &, const QItemSelection &) ),
             this,                    SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
  if ( _meshSelectionModel )
    disconnect( _meshSelectionModel, SIGNAL( selectionChanged( const QItemSelection &, const QItemSelection &) ),
             this,                SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
//   if ( _mgr )
//     disconnect( _mgr, 0/*SIGNAL(currentSelectionChanged())*/, this, 0/*SLOT(onCurrentSelectionChanged())*/ );
  disconnect( HEXABLOCKGUI::selectionMgr(), SIGNAL(currentSelectionChanged()), this, SLOT(onCurrentSelectionChanged()) );
  if ( _vtkVm )
    disconnect( _vtkVm, SIGNAL( activated(SUIT_ViewManager*) ), this, SLOT( onWindowActivated(SUIT_ViewManager*) ) );
  if ( _occVm )
    disconnect( _occVm, SIGNAL( activated(SUIT_ViewManager*) ), this, SLOT( onWindowActivated(SUIT_ViewManager*) ) );

  _documentModel->allowEdition();

  QDialog::hideEvent( event );
  MESSAGE("}");
}

void HexaBaseDialog::showEvent( QShowEvent * event )
{
  MESSAGE("HexaBaseDialog::showEvent(){");
  QString className = metaObject()->className();
  MESSAGE( "*  I am                          : " << className.toStdString() );

  if ( _editMode == INFO_MODE ){
    _documentModel->allowEdition();
  } else {
    _documentModel->disallowEdition();
  }

  if ( _patternDataSelectionModel ){
//     _patternDataSelectionModel->clearSelection();
    connect( _patternDataSelectionModel, SIGNAL( selectionChanged ( const QItemSelection &, const QItemSelection &) ),
                this,                    SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
  }
  if ( _patternBuilderSelectionModel ){
//     _patternBuilderSelectionModel->clearSelection();
    connect( _patternBuilderSelectionModel, SIGNAL( selectionChanged( const QItemSelection &, const QItemSelection &) ),
             this,                            SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
  }
  if ( _groupsSelectionModel ){
//     _groupsSelectionModel->clearSelection();
    connect( _groupsSelectionModel, SIGNAL( selectionChanged( const QItemSelection &, const QItemSelection &) ),
             this,                    SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
  }
  if ( _meshSelectionModel ){
//     _meshSelectionModel->clearSelection();
    connect( _meshSelectionModel, SIGNAL( selectionChanged( const QItemSelection &, const QItemSelection &) ),
             this,                SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
  }
  if ( _mgr )
    connect( _mgr, SIGNAL(currentSelectionChanged()), this, SLOT(onCurrentSelectionChanged()) );

  if ( _vtkVm )
    connect( _vtkVm, SIGNAL( activated(SUIT_ViewManager*) ), this, SLOT( onWindowActivated(SUIT_ViewManager*) ) );
  if ( _occVm )
    connect( _occVm, SIGNAL( activated(SUIT_ViewManager*) ), this, SLOT( onWindowActivated(SUIT_ViewManager*) ) );

  QDialog::showEvent ( event );
  MESSAGE("}");
}




void HexaBaseDialog::updateButtonBox()
{
}

void HexaBaseDialog::updateName()
{
  if (!_documentModel) return;
  if ( !_patternDataSelectionModel )  return;
  const PatternDataModel*   patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );

  QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(sender());
  if (!lineEdit) return;
  QString newName = lineEdit->text();
  if ( newName.isEmpty() ) return;

  QVariant v = lineEdit->property("QModelIndex");
  if ( !v.isValid() ) return;

  QModelIndex index = v.value<QModelIndex>();
  if ( index.isValid() )
    _documentModel->setName( patternDataModel->mapToSource(index), newName );
}



void HexaBaseDialog::selectElementOfModel()
{
  MESSAGE("HexaBaseDialog::selectElementOfModel()");
  if (!_patternDataSelectionModel) return;

  QListWidget* currentListWidget = dynamic_cast<QListWidget*>( sender() );
  if ( !currentListWidget ) return;

  _selectionMutex = true;
  QList<QListWidgetItem *> sel = currentListWidget->selectedItems();
  QModelIndex index;
  _patternDataSelectionModel->clearSelection();
  foreach ( QListWidgetItem *item, sel ){
    MESSAGE( "*  selecting the element : " << index.data().toString().toStdString() );
    index = item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>();
//     _patternDataSelectionModel->select( index, QItemSelectionModel::Clear );
    if ( index.isValid() )
//       _patternDataSelectionModel->select( index, QItemSelectionModel::Clear );
      _patternDataSelectionModel->select( index, QItemSelectionModel::Select );
  }
  _selectionMutex = false;

  MESSAGE("}");
}


bool HexaBaseDialog::eventFilter(QObject *obj, QEvent *event)
{
  if ( event->type() == QEvent::FocusIn ){ 
    QString className  = obj->metaObject()->className();
    QString objectName = obj->objectName();
    MESSAGE("QEvent::FocusIn : I am "<< objectName.toStdString() );
    MESSAGE("QEvent::FocusIn : I am "<< className.toStdString() );
  }

  if ( event->type() == QEvent::FocusOut ){ 
    QString className = obj->metaObject()->className();
    QString objectName = obj->objectName();
    MESSAGE("QEvent::FocusOut : I am "<< objectName.toStdString() );
    MESSAGE("QEvent::FocusOut : I am "<< className.toStdString() ); 
  }

  if ( event->type() != QEvent::FocusIn ){ //QEvent::KeyPress) { 
    return QObject::eventFilter(obj, event);
  }
  MESSAGE("HexaBaseDialog::eventFilter{");
  MESSAGE("*  QEvent::FocusIn");
  _currentObj = obj;

  /* ON FOCUS ON A WIDGET*/
  QVariant v;
  QModelIndex    index;

  QWidget* w = 0;
  QLineEdit*   lineEdit   = dynamic_cast<QLineEdit*>(obj);
  QListWidget* listWidget = dynamic_cast<QListWidget*>(obj);
  HexaBaseDialog* dialog  = dynamic_cast<HexaBaseDialog*>(obj);

  QItemSelectionModel *selector = 0;

//   When editing(new/update) mode is on : allow selection of expected type 
//     ( selection from model(vtk,treeview) or cao (geom:occ) )
//     if ( _editMode != INFO_MODE ){
//   if ( _editMode == NEW_MODE ){  //CS_TODO
//   if ( _editMode != INFO_MODE ){  //CS_TODO
//     model selection
    _allowVTKSelection( obj );
    //geom selection
    _allowOCCSelection( obj );
//   }

//   if ( _editMode == INFO_MODE ){
//     _documentModel->allowEdition();
//   } else {
//     _documentModel->disallowEdition();
//   }

//   Depending of focused widget type, get the right selector for it
  selector = _getSelector( obj );
  if ( !selector ) return false;

  //If there is a current selection fill the widget with it
  if ( selector->hasSelection() ){
    QItemSelection currentSelection  = selector->selection();
    bool selOk = false;
    if ( lineEdit ){ //element can be from lineEdit
      selOk = _onSelectionChanged( currentSelection, lineEdit );
    } else if ( listWidget ){
      selOk = _onSelectionChanged( currentSelection, listWidget );
    }
    if ( !selOk &&  ( lineEdit || listWidget ) ){
      selector->clearSelection();
    }
//     foreach( const QModelIndex& i, currentSelection.indexes() ){
//       MESSAGE( "current element selected" << i.data().toString().toStdString() );
//       QString name = i.data().toString();
//       if ( lineEdit ){ //element can be from lineEdit
//         lineEdit->setProperty( "QModelIndex",  QVariant::fromValue<QModelIndex>(i) );
//         lineEdit->setText( name );
//       } else if ( listWidget ){
//         QListWidgetItem* item  = new QListWidgetItem( name );
//         item->setData(  LW_QMODELINDEX_ROLE, QVariant::fromValue<QModelIndex>(i) );
//         listWidget->addItem( item );
//       }
//     }
  } else { //If the widget contains an hexa element, select it in model/view
    if ( lineEdit ){ //element can be from lineEdit
      MESSAGE("*  on QLineEdit");
      v = lineEdit->property("QModelIndex");
      if ( !v.isValid() ) return QObject::eventFilter(obj, event);
      index = v.value<QModelIndex>();
      _selectionMutex = true;
      MESSAGE("*  selecting the element : " << index.data().toString().toStdString());
      MESSAGE("*  with selector : " << selector);
      MESSAGE( "*  _patternDataSelectionModel    : " << _patternDataSelectionModel );
      MESSAGE( "*  _patternBuilderSelectionModel : " << _patternBuilderSelectionModel );
      MESSAGE( "*  _groupsSelectionModel         : " << _groupsSelectionModel );
      MESSAGE( "*  _meshSelectionModel           : " << _meshSelectionModel );
      selector->select( index, QItemSelectionModel::Clear );
      selector->select( index, QItemSelectionModel::Select );
      _selectionMutex = false;
    }
    
  
    if ( dialog ){ //element can be from a dialog box
      MESSAGE("*  on Dialog");
      v = dialog->property("QModelIndex");
      if ( !v.isValid() ) return QObject::eventFilter(obj, event);
      index = v.value<QModelIndex>();
      _selectionMutex = true;
      MESSAGE("*  selecting the element : " << index.data().toString().toStdString());
      selector->select( index, QItemSelectionModel::Clear );
      selector->select( index, QItemSelectionModel::Select );
  //QItemSelectionModel::SelectCurrent );
      _selectionMutex = false;
    }
  
  
  //   if ( listWidget ){//element can be from listWidget
  //     MESSAGE("*  on QListWidget");
  //     QList<QListWidgetItem *> sel = listWidget->selectedItems();
  // //     _allowSelection(); //CS_??
  //     _selectionMutex = true;
  //     _patternDataSelectionModel->clearSelection();
  //     foreach ( QListWidgetItem *item, sel ){
  //       index = item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>();
  //       if ( index.isValid() ){
  //         MESSAGE("*  selecting the element : " << index.data().toString().toStdString());
  //         _patternDataSelectionModel->select( index, QItemSelectionModel::Select );
  //       }
  //     }
  //     _selectionMutex = false;
  //   }
  
  //    if ( _editMode == INFO_MODE ){
  //     _documentModel->allowEdition();
  //   } else {
  //   if ( _editMode != INFO_MODE ){
  //     _documentModel->disallowEdition();
  //   }
  }
  MESSAGE("}");
  return false;
//   return QObject::eventFilter(obj, event);
//   return true;
}


// ------------------------- VERTEX ----------------------------------
VertexDialog::VertexDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f ):
  HexaBaseDialog(parent, editmode, f),
  _value(0)
{
  _helpFileName = "gui_vertex.html";
  setupUi( this );
  _initWidget(editmode);
//   setFocusProxy( name_le );

  if ( editmode  == NEW_MODE ){
    setWindowTitle( tr("Vertex Construction") );
  } else if ( editmode == UPDATE_MODE ){
    setWindowTitle( tr("Vertex Modification") );
  }
}


VertexDialog::~VertexDialog()
{
}


void VertexDialog::_initInputWidget( Mode editmode )
{
  x_spb->setRange(VERTEX_COORD_MIN, VERTEX_COORD_MAX);
  y_spb->setRange(VERTEX_COORD_MIN, VERTEX_COORD_MAX);
  z_spb->setRange(VERTEX_COORD_MIN, VERTEX_COORD_MAX);

  setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  installEventFilter(this);
  name_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  name_le->installEventFilter(this);

  connect( name_le, SIGNAL(returnPressed()), this, SLOT(updateName()));
}

void VertexDialog::clear()
{
//   name_le->clear();
}

void VertexDialog::setValue(HEXA_NS::Vertex* v)
{
  MESSAGE("*  setValue : " << v);
  //0) Name
  name_le->setText( v->getName() );

  //1) Value (x,y,z)
  x_spb->setValue( v->getX() );
  y_spb->setValue( v->getY() );
  z_spb->setValue( v->getZ() );

  MESSAGE("*  _patternDataSelectionModel  : " << _patternDataSelectionModel );
  if ( _patternDataSelectionModel ){
    QModelIndex iv = _patternDataSelectionModel->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(v) );
    MESSAGE("*  selecting the element : " << iv.data().toString().toStdString());

    setProperty( "QModelIndex",  QVariant::fromValue<QModelIndex>(iv) );
    name_le->setProperty( "QModelIndex",  QVariant::fromValue<QModelIndex>(iv) );
  }
  _value = v;
}


HEXA_NS::Vertex* VertexDialog::getValue()
{
  return _value;
}




bool VertexDialog::apply(QModelIndex& result)
{
  SUIT_OverrideCursor wc;
  if ( !_documentModel ) return false;
  if ( !_patternDataSelectionModel ) return false;
  const PatternDataModel* patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  if ( !patternDataModel ) return false;
  _currentObj = NULL;

  bool isOk = false;

  QModelIndex iVertex; 
  double newX = x_spb->value();
  double newY = y_spb->value();
  double newZ = z_spb->value();

  if ( _editMode == UPDATE_MODE){ 
    QVariant v = property("QModelIndex");
    if ( v.isValid() ){
      iVertex = patternDataModel->mapToSource( v.value<QModelIndex>() );
      if ( iVertex.isValid() )
        isOk = _documentModel->updateVertex( iVertex, newX, newY, newZ );
    }
  } else if ( _editMode == NEW_MODE){
    iVertex = _documentModel->addVertex( newX, newY, newZ );
    if ( iVertex.isValid() ) 
      isOk = true;
  }
  if (!isOk) { 
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "VERTEX UPDATE/CONSTRUCTION" ) );
    return false;
  }

  QString newName = name_le->text();
  if ( !newName.isEmpty() )/*{*/
    _documentModel->setName( iVertex, newName );
//     name_le->setProperty("QModelIndex", QVariant::fromValue( patternDataModel->mapFromSource(iVertex) ));
//   } else {
//     name_le->setText( _value->getName() );
//   }

  // to select/highlight result
  result = patternDataModel->mapFromSource(iVertex);

  return isOk;
}






// bool VertexDialog::apply(QModelIndex& result)
// {
//     QString newName = name_le->text();
//     double newX = x_spb->value();
//     double newY = y_spb->value();
//     double newZ = z_spb->value();
// 
//     if ( _documentModel ){
//       if ( _ivalue.isValid() ){ //EDITION MODE
//         bool ok = _documentModel->updateVertex( _ivalue, newX, newY, newZ );
//         _documentModel->setName(_ivalue, newName);
//         if ( ok ){
//           //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "VERTEX UPDATED : %1" ).arg(_ivalue.data().toString()) );
// //           clear();
//           return true;
//         } else {
//           SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT UPDATE VERTEX" ) );
//           return false;
//         }
//         //emit editingFinished();
//       } else { //NEW MODE
//         QModelIndex newIndex = _documentModel->addVertex( newX, newY, newZ );
//         if ( newIndex.isValid() ){
//           _value  = newIndex.model()->data(newIndex, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
//           _ivalue = newIndex;
//           if (!newName.isEmpty()) _documentModel->setName( _ivalue, newName );
//           return true;
//         } else {
//           SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT BUILD VERTEX" ) );
//           return false;
//         }
//       }
//     } else {
//       return false;
//     }
// }




// bool VertexDialog::apply(QModelIndex& result)
// {
//     QString newName = name_le->text();
//     double newX = x_spb->value();
//     double newY = y_spb->value();
//     double newZ = z_spb->value();
// 
//     if ( _documentModel ){
//       if ( _ivalue.isValid() ){ //EDITION MODE
//         bool ok = _documentModel->updateVertex( _ivalue, newX, newY, newZ );
//         _documentModel->setName(_ivalue, newName);
//         if ( ok ){
//           //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "VERTEX UPDATED : %1" ).arg(_ivalue.data().toString()) );
// //           clear();
//           return true;
//         } else {
//           SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT UPDATE VERTEX" ) );
//           return false;
//         }
//         //emit editingFinished();
//       } else { //NEW MODE
//         QModelIndex newIndex = _documentModel->addVertex( newX, newY, newZ );
//         if ( newIndex.isValid() ){
//           _value  = newIndex.model()->data(newIndex, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
//           _ivalue = newIndex;
//           if (!newName.isEmpty()) _documentModel->setName( _ivalue, newName );
//           //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "VERTEX BUILDED : %1" ).arg(newIndex.data().toString()) );
// //           QDialog::accept();
// //           if ( _patternDataSelectionModel ){
// //             const PatternDataModel* patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
// //             if ( patternDataModel ){
// //               newIndex = patternDataModel->mapFromSource(newIndex);
// // //               _patternDataSelectionModel->select( newIndex, QItemSelectionModel::Clear );
// // //               _patternDataSelectionModel->select( newIndex, QItemSelectionModel::Select );
// //               _patternDataSelectionModel->setCurrentIndex ( newIndex, QItemSelectionModel::Clear );
// //               _patternDataSelectionModel->setCurrentIndex ( newIndex, QItemSelectionModel::Select );
// //             }
// //           }
//           /*clear()*/;
//           return true;
//         } else {
//           SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT BUILD VERTEX" ) );
//           return false;
//         }
//       }
//     } else {
//       return false;
//     }
// }






// ------------------------- EDGE ----------------------------------
EdgeDialog::EdgeDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f ):
  HexaBaseDialog(parent, editmode, f),
  _value(0)
{
  _helpFileName = "gui_edge.html";
  setupUi( this );
  _initWidget(editmode);

  rb0->setFocusProxy( v0_le_rb0 );
  rb1->setFocusProxy( vex_le_rb1 );
  rb0->click();

  if  ( editmode == INFO_MODE ){
    setWindowTitle( tr("Edge Information") );
    rb1->hide();
  }
}



EdgeDialog::~EdgeDialog()
{
}


void EdgeDialog::_initInputWidget( Mode editmode )
{
  QRegExp rx("");
  QValidator *validator = new QRegExpValidator(rx, this);

  setProperty( "HexaWidgetType",  QVariant::fromValue(EDGE_TREE) );
  installEventFilter(this);

  name_le->setProperty( "HexaWidgetType",  QVariant::fromValue(EDGE_TREE) );
  name_le->installEventFilter(this);

  v0_le_rb0->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  v1_le_rb0->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  v0_le_rb0->setValidator( validator );
  v1_le_rb0->setValidator( validator );
  v0_le_rb0->installEventFilter(this);
  v1_le_rb0->installEventFilter(this);

  vex_le_rb1->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  vec_le_rb1->setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );
  vex_le_rb1->setValidator( validator );
  vec_le_rb1->setValidator( validator );
  vex_le_rb1->installEventFilter(this);
  vec_le_rb1->installEventFilter(this);

  connect( name_le, SIGNAL(returnPressed()), this, SLOT(updateName()) );
}


void EdgeDialog::clear()
{
  name_le->clear();
  v0_le_rb0->clear();
  v1_le_rb0->clear();
  vex_le_rb1->clear();
  vec_le_rb1->clear();
}




void EdgeDialog::setValue(HEXA_NS::Edge* e)
{
  HEXA_NS::Vertex* v0 = e->getVertex(0);
  HEXA_NS::Vertex* v1 = e->getVertex(1);

  name_le->setText( e->getName() );
  v0_le_rb0->setText( v0->getName() );
  v1_le_rb0->setText( v1->getName() );

  if ( _patternDataSelectionModel ){
    QModelIndex ie  = _patternDataSelectionModel->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(e) );
    QModelIndex iv0 = _patternDataSelectionModel->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(v0) );
    QModelIndex iv1 = _patternDataSelectionModel->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(v1) );

    name_le->setProperty( "QModelIndex",  QVariant::fromValue(ie) );
    v0_le_rb0->setProperty( "QModelIndex",  QVariant::fromValue(iv0) );
    v1_le_rb0->setProperty( "QModelIndex",  QVariant::fromValue(iv1) );
  }
  _value = e;
}


HEXA_NS::Edge* EdgeDialog::getValue()
{
  return _value;
}

bool EdgeDialog::apply(QModelIndex& result)
{
  SUIT_OverrideCursor wc;
  //if ( !_documentModel ) return;
  if ( !_patternDataSelectionModel )    return false;
  if ( !_patternBuilderSelectionModel ) return false;
  const PatternDataModel*    patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  if ( !patternDataModel ) return false;
  if ( !patternBuilderModel ) return false;
  _currentObj = NULL;

  QModelIndex iEdge;

  if ( rb0->isChecked() ){
    QModelIndex iv0 = patternDataModel->mapToSource( _index[v0_le_rb0] );
    QModelIndex iv1 = patternDataModel->mapToSource( _index[v1_le_rb0] );
    if ( iv0.isValid()&& iv1.isValid() ){
      iEdge = _documentModel->addEdgeVertices( iv0, iv1 );
    }
  } else if ( rb1->isChecked() ){
    QModelIndex ivex = patternDataModel->mapToSource( _index[vex_le_rb1] );
    QModelIndex ivec = patternBuilderModel->mapToSource( _index[vec_le_rb1] );
    if ( ivex.isValid() && ivec.isValid() ){
      iEdge = _documentModel->addEdgeVector( ivex, ivec );
    }
  }

  if ( !iEdge.isValid() ){
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT BUILD EDGE" ) );
    return false;
  }
  _value  = iEdge.model()->data(iEdge, HEXA_DATA_ROLE).value<HEXA_NS::Edge*>();

  QString newName = name_le->text();
  if ( !newName.isEmpty() )/*{*/
    _documentModel->setName( iEdge, newName );
//     name_le->setProperty("QModelIndex", QVariant::fromValue( patternDataModel->mapFromSource(iEdge) ));
//   } else {
//     name_le->setText( _value->getName() );
//   }

  result = patternDataModel->mapFromSource(iEdge);

  return true;
}




// ------------------------- QUAD ----------------------------------
QuadDialog::QuadDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f ):
  HexaBaseDialog(parent, editmode, f),
  _value(0)
{
  _helpFileName = "gui_quadrangle.html";
  setupUi( this );
  _initWidget(editmode);
  rb0->setFocusProxy( v0_le_rb0 );
  rb1->setFocusProxy( e0_le_rb1 );
  rb0->click();

  if  ( editmode == INFO_MODE ){
    setWindowTitle( tr("Quad Information") );
  }
}


QuadDialog::~QuadDialog()
{
}

void QuadDialog::_initInputWidget( Mode editmode )
{
  QRegExp rx("");
  QValidator *validator = new QRegExpValidator(rx, this);

  setProperty( "HexaWidgetType",  QVariant::fromValue(QUAD_TREE) );
  installEventFilter(this);

  name_le->setProperty( "HexaWidgetType",  QVariant::fromValue(QUAD_TREE) );
  name_le->installEventFilter(this);

  v0_le_rb0->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  v1_le_rb0->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  v2_le_rb0->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  v3_le_rb0->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );

  v0_le_rb0->setValidator( validator );
  v1_le_rb0->setValidator( validator );
  v2_le_rb0->setValidator( validator );
  v3_le_rb0->setValidator( validator );
  v0_le_rb0->installEventFilter(this);
  v1_le_rb0->installEventFilter(this);
  v2_le_rb0->installEventFilter(this);
  v3_le_rb0->installEventFilter(this);

  e0_le_rb1->setProperty( "HexaWidgetType",  QVariant::fromValue(EDGE_TREE) );
  e1_le_rb1->setProperty( "HexaWidgetType",  QVariant::fromValue(EDGE_TREE) );
  e2_le_rb1->setProperty( "HexaWidgetType",  QVariant::fromValue(EDGE_TREE) );
  e3_le_rb1->setProperty( "HexaWidgetType",  QVariant::fromValue(EDGE_TREE) );

  e0_le_rb1->setValidator( validator );
  e1_le_rb1->setValidator( validator );
  e2_le_rb1->setValidator( validator );
  e3_le_rb1->setValidator( validator );

  e0_le_rb1->installEventFilter(this);
  e1_le_rb1->installEventFilter(this);
  e2_le_rb1->installEventFilter(this);
  e3_le_rb1->installEventFilter(this);

  connect( name_le, SIGNAL(returnPressed()), this, SLOT(updateName()) );
}


void QuadDialog::clear()
{
  v0_le_rb0->clear();
  v1_le_rb0->clear();
  v2_le_rb0->clear();
  v3_le_rb0->clear();

  e0_le_rb1->clear();
  e1_le_rb1->clear();
  e2_le_rb1->clear();
  e3_le_rb1->clear();

  name_le->clear();
}


void QuadDialog::setValue(HEXA_NS::Quad* q)
{
  Q_ASSERT( q->countEdge() == 4 );
  Q_ASSERT( q->countVertex() == 4 );

  //0) Name
  name_le->setText( q->getName() );

  //1) Vertices
  HEXA_NS::Vertex* v0 = q->getVertex(0);
  HEXA_NS::Vertex* v1 = q->getVertex(1);
  HEXA_NS::Vertex* v2 = q->getVertex(2);
  HEXA_NS::Vertex* v3 = q->getVertex(3);

  v0_le_rb0->setText( v0->getName() );
  v1_le_rb0->setText( v1->getName() );
  v2_le_rb0->setText( v2->getName() );
  v3_le_rb0->setText( v3->getName() );


  //2) Edges
  HEXA_NS::Edge* e0 = q->getEdge(0);
  HEXA_NS::Edge* e1 = q->getEdge(1);
  HEXA_NS::Edge* e2 = q->getEdge(2);
  HEXA_NS::Edge* e3 = q->getEdge(3);

  e0_le_rb1->setText( e0->getName() );
  e1_le_rb1->setText( e1->getName() );
  e2_le_rb1->setText( e2->getName() );
  e3_le_rb1->setText( e3->getName() );

  if ( _patternDataSelectionModel ){
    QModelIndex iq = _patternDataSelectionModel->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(q) );

    QModelIndex iv0 = _patternDataSelectionModel->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(v0) );
    QModelIndex iv1 = _patternDataSelectionModel->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(v1) );
    QModelIndex iv2 = _patternDataSelectionModel->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(v2) );
    QModelIndex iv3 = _patternDataSelectionModel->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(v3) );

    QModelIndex ie0 = _patternDataSelectionModel->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(e0) );
    QModelIndex ie1 = _patternDataSelectionModel->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(e1) );
    QModelIndex ie2 = _patternDataSelectionModel->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(e2) );
    QModelIndex ie3 = _patternDataSelectionModel->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(e3) );

    name_le->setProperty( "QModelIndex",  QVariant::fromValue(iq) );

    v0_le_rb0->setProperty( "QModelIndex",  QVariant::fromValue(iv0) );
    v1_le_rb0->setProperty( "QModelIndex",  QVariant::fromValue(iv1) );
    v2_le_rb0->setProperty( "QModelIndex",  QVariant::fromValue(iv2) );
    v3_le_rb0->setProperty( "QModelIndex",  QVariant::fromValue(iv3) );

    e0_le_rb1->setProperty( "QModelIndex",  QVariant::fromValue(ie0) );
    e1_le_rb1->setProperty( "QModelIndex",  QVariant::fromValue(ie1) );
    e2_le_rb1->setProperty( "QModelIndex",  QVariant::fromValue(ie2) );
    e3_le_rb1->setProperty( "QModelIndex",  QVariant::fromValue(ie3) );
  }
  _value = q;

}

HEXA_NS::Quad* QuadDialog::getValue()
{
  return _value;
}


bool QuadDialog::apply(QModelIndex& result)
{
  SUIT_OverrideCursor wc;
  //if ( !_documentModel ) return;
  if ( !_patternDataSelectionModel ) return false;
  const PatternDataModel* patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  if ( !patternDataModel ) return false;
  _currentObj = NULL;

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

  if ( !iQuad.isValid() ){
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT BUILD QUAD" ) );
    return false;
  }
  _value  = iQuad.model()->data(iQuad, HEXA_DATA_ROLE).value<HEXA_NS::Quad *>();

  QString newName = name_le->text();
  if ( !newName.isEmpty() )/*{*/
    _documentModel->setName( iQuad, newName );
//     name_le->setProperty("QModelIndex", QVariant::fromValue( patternDataModel->mapFromSource(iQuad) ));
//   } else {
//     name_le->setText( _value->getName() );
//   }

  // to select/highlight result
  result = patternDataModel->mapFromSource(iQuad);

  return true;
}



// ------------------------- HEXA ----------------------------------
HexaDialog::HexaDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f ):
  HexaBaseDialog(parent, editmode, f),
  _value(0)
{
  _helpFileName = "gui_hexahedron.html";
  setupUi( this );
  _initWidget(editmode);
  quads_rb->setFocusProxy( quads_lw );
  vertices_rb->setFocusProxy( vertices_lw );
  quads_rb->click();
//   quads_rb->setFocus();

  if  ( editmode == INFO_MODE ){
    setWindowTitle( tr("Hexahedron Information") );
  }
}

HexaDialog::~HexaDialog()
{
}


void HexaDialog::_initInputWidget( Mode editmode )
{
  QRegExp rx("");
  QValidator *validator = new QRegExpValidator(rx, this);

  setProperty( "HexaWidgetType",  QVariant::fromValue(HEXA_TREE) );
  installEventFilter(this);

  name_le->setProperty( "HexaWidgetType",  QVariant::fromValue(HEXA_TREE) );
  name_le->installEventFilter(this);

  quads_lw->setProperty( "HexaWidgetType",  QVariant::fromValue(QUAD_TREE) );
  quads_lw->installEventFilter(this);

  vertices_lw->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  vertices_lw->installEventFilter(this);


  if ( editmode != INFO_MODE ) {
    // delete item from listwidget
    QShortcut* delQuadShortcut   = new QShortcut( QKeySequence(Qt::Key_X), quads_lw );
    QShortcut* delVertexShortcut = new QShortcut( QKeySequence(Qt::Key_X), vertices_lw );
    delQuadShortcut->setContext( Qt::WidgetShortcut );
    delVertexShortcut->setContext( Qt::WidgetShortcut );
    connect(delQuadShortcut,   SIGNAL(activated()), this, SLOT(deleteQuadItem()));
    connect(delVertexShortcut, SIGNAL(activated()), this, SLOT(deleteVertexItem()));
//     connect( quads_lw, SIGNAL(currentRowChanged(int)), this, SLOT(updateButtonBox(int)) );
  }
  // edit name
  connect( name_le, SIGNAL(returnPressed()), this, SLOT(updateName()) );
  // highlight item on model view (VTK) from listwidget
  connect( quads_lw,    SIGNAL(itemSelectionChanged()), this, SLOT(selectElementOfModel()) );
  connect( vertices_lw, SIGNAL(itemSelectionChanged()), this, SLOT(selectElementOfModel()) );
}


void HexaDialog::clear()
{
  name_le->clear();
  quads_lw->clear();
  vertices_lw->clear();
}



void HexaDialog::updateButtonBox()
{
  std::cout << "updateButtonBox row=> " /*<< i */<< std::endl;
  if ( quads_rb->isChecked() ){  // build from quads count() must be between [2,6]
    int nbQuads = quads_lw->count();
    std::cout << "nbQuads => " <<  nbQuads << std::endl;
    if ( nbQuads >= 2 && nbQuads <= 6 ){
      _applyCloseButton->setEnabled(true);
      _applyButton->setEnabled(true);
//       _applyCloseButton->setFlat( false );
//       _applyButton->setFlat( false );
    } else {
      _applyCloseButton->setEnabled(false);
      _applyButton->setEnabled(false);
//       _applyCloseButton->setFlat( true );
//       _applyButton->setFlat( true );
    }
  } else if ( vertices_rb->isChecked() ){ // build from vertices count() must be equals to 8
    int nbVertices = vertices_lw->count();
    std::cout << "nbVertices => " <<  nbVertices << std::endl;
    if ( nbVertices == 8 ){
      _applyCloseButton->setEnabled(true);
      _applyButton->setEnabled(true);
//       _applyCloseButton->setFlat( false );
//       _applyButton->setFlat( false );
    } else {
      _applyCloseButton->setEnabled(false);
      _applyButton->setEnabled(false);
//       _applyCloseButton->setFlat( true );
//       _applyButton->setFlat( true );
    }
  }
}



void HexaDialog::deleteQuadItem()
{
  delete quads_lw->currentItem();
  updateButtonBox();
}

void HexaDialog::deleteVertexItem()
{
  delete vertices_lw->currentItem();
  updateButtonBox();
}



void HexaDialog::_setValueQuads( HEXA_NS::Hexa* h )
{
  QListWidgetItem *qItem = NULL;
  HEXA_NS::Quad   *q     = NULL;
  QModelIndex      qIndex;

  quads_lw->clear();
  for( int i = 0; i <= 5; ++i ){
    q      = h->getQuad(i);
    qIndex = _patternDataSelectionModel->indexBy( HEXA_ENTRY_ROLE, QString::number(reinterpret_cast<intptr_t>(q)) );
    qItem  = new QListWidgetItem( q->getName() );
    qItem->setData(  LW_QMODELINDEX_ROLE, QVariant::fromValue<QModelIndex>(qIndex) );
    quads_lw->addItem( qItem );
  }
}



void HexaDialog::_setValueVertices( HEXA_NS::Hexa* h )
{
  QListWidgetItem *vItem = NULL;
  HEXA_NS::Vertex* v     = NULL;
  QModelIndex      vIndex;

  vertices_lw->clear();
  for( int i = 0; i <= 7; ++i ){
    v = h->getVertex(i);
    vIndex = _patternDataSelectionModel->indexBy( HEXA_ENTRY_ROLE, QString::number(reinterpret_cast<intptr_t>(v)) );
    vItem  = new QListWidgetItem( v->getName() );
    vItem->setData(  LW_QMODELINDEX_ROLE, QVariant::fromValue<QModelIndex>(vIndex) );
    vertices_lw->addItem( vItem );
  }
}



void HexaDialog::setValue(HEXA_NS::Hexa* h)
{
  // 0) name
  name_le->setText( h->getName() );

  if ( _patternDataSelectionModel){
    QModelIndex hIndex = _patternDataSelectionModel->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(h) );
    _setValueVertices(h);
    _setValueQuads(h);
    name_le->setProperty( "QModelIndex",  QVariant::fromValue(hIndex) );
  }
  _value = h;
}


HEXA_NS::Hexa* HexaDialog::getValue()
{
  return _value;
}


bool HexaDialog::apply(QModelIndex& result)
{
  SUIT_OverrideCursor wc;
  if ( !_documentModel ) return false;
  if ( !_patternDataSelectionModel ) return false;
  const PatternDataModel* patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  if ( !patternDataModel ) return false;
  _currentObj = NULL;

  QModelIndex  iHexa;

  QListWidget* currentLw = NULL;
  QListWidgetItem*  item = NULL;

  if ( quads_rb->isChecked() )
    currentLw = dynamic_cast<QListWidget*>( quads_lw );
  else if ( vertices_rb->isChecked() )
    currentLw = dynamic_cast<QListWidget*>( vertices_lw );

  QModelIndex     iElt;
  QModelIndexList iElts;
  for ( int r = 0; r < currentLw->count(); ++r){
    item = currentLw->item(r);
    iElt = patternDataModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() );
    if ( iElt.isValid() )
      iElts << iElt;
  }

  if ( quads_rb->isChecked() ){ // build from quads iElts.count() should be between [2,6]
    iHexa = _documentModel->addHexaQuads( iElts );
  } else if ( vertices_rb->isChecked() and iElts.count()== 8 ){ // build from vertices
    iHexa = _documentModel->addHexaVertices( iElts[0], iElts[1], iElts[2], iElts[3],
                                             iElts[4], iElts[5], iElts[6], iElts[7] );
  }

  if ( !iHexa.isValid() ){
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT BUILD HEXA" ) );
    return false;
  }
  _value  = iHexa.model()->data(iHexa, HEXA_DATA_ROLE).value<HEXA_NS::Hexa*>();

  QString newName = name_le->text();
  if ( !newName.isEmpty() )/*{*/
    _documentModel->setName( iHexa, newName );
//     name_le->setProperty("QModelIndex", QVariant::fromValue( patternDataModel->mapFromSource(iHexa) ));
//   } else {
//     name_le->setText( _value->getName() );
//   }

  // to select/highlight result
  result = patternDataModel->mapFromSource(iHexa);

  return true;
}











// ------------------------- VECTOR ----------------------------------
VectorDialog::VectorDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f ):
  HexaBaseDialog(parent, editmode, f),
  _value(0)
{
  _helpFileName = "gui_vector.html";
  setupUi( this );
  _initWidget(editmode);

  rb0->setFocusProxy( dx_spb_rb0 );
  rb1->setFocusProxy( v0_le_rb1 );
  rb0->click();
//   setFocusProxy( rb0 );

  if ( editmode == INFO_MODE ){
    setWindowTitle( tr("Vector Information") );
    rb1->hide();
  }
}

VectorDialog::~VectorDialog()
{
}


void VectorDialog::_initInputWidget( Mode editmode )
{
  QRegExp rx("");
  QValidator *validator = new QRegExpValidator(rx, this);

  setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );
  installEventFilter(this);

  name_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );
  name_le->installEventFilter(this);

  v0_le_rb1->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  v0_le_rb1->setValidator( validator );
  v0_le_rb1->installEventFilter(this);

  v1_le_rb1->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  v1_le_rb1->setValidator( validator );
  v1_le_rb1->installEventFilter(this);

  if ( editmode == INFO_MODE ){
    dx_spb_rb0->setReadOnly(true);
    dy_spb_rb0->setReadOnly(true);
    dz_spb_rb0->setReadOnly(true);
  }

  connect( name_le, SIGNAL(returnPressed()), this, SLOT(updateName()) );
}

void VectorDialog::clear()
{
  name_le->clear();
  dx_spb_rb0->clear();
  dy_spb_rb0->clear();
  dz_spb_rb0->clear();
  v0_le_rb1->clear();
  v1_le_rb1->clear();
}


void VectorDialog::setValue(HEXA_NS::Vector* v)
{
  name_le->setText( v->getName() );
  dx_spb_rb0->setValue( v->getDx() );
  dy_spb_rb0->setValue( v->getDy() );
  dz_spb_rb0->setValue( v->getDz() );

  if ( _patternDataSelectionModel ){
    QModelIndex ivec = _patternDataSelectionModel->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(v) );
    name_le->setProperty( "QModelIndex",  QVariant::fromValue(ivec) );
  }
  _value = v;
}

HEXA_NS::Vector* VectorDialog::getValue()
{
  return _value;
}




bool VectorDialog::apply(QModelIndex& result)
{
  SUIT_OverrideCursor wc;
  if ( !_documentModel ) return false;
  if ( !_patternDataSelectionModel )  return false;
  if ( !_patternBuilderSelectionModel ) return false;
  const PatternDataModel*    patternDataModel    = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  if ( !patternDataModel ) return false;
  if ( !patternBuilderModel ) return false;
  _currentObj = NULL;

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

  if ( !iVector.isValid() ){
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT BUILD VECTOR" ) );
    return false;
  }

  _value  = iVector.model()->data(iVector, HEXA_DATA_ROLE).value<HEXA_NS::Vector *>();

  QString newName = name_le->text();
  if ( !newName.isEmpty() )/*{*/
    _documentModel->setName( iVector, newName );
//     name_le->setProperty("QModelIndex", QVariant::fromValue( patternBuilderModel->mapFromSource(iVector) ));
//   } else {
//     name_le->setText( _value->getName() );
//   }

  // to select/highlight result
  result = patternBuilderModel->mapFromSource(iVector);

  return true;
}



CylinderDialog::CylinderDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f )
: HexaBaseDialog(parent, editmode, f),
  _value(0)
{
  _helpFileName = "gui_cyl.html";
  setupUi( this );
  _initWidget(editmode);
//   setFocusProxy( vex_le );
  if ( editmode == INFO_MODE ){
    setWindowTitle( tr("Cylinder Information") );
  }
}


CylinderDialog::~CylinderDialog()
{
}

void CylinderDialog::_initInputWidget( Mode editmode )
{
  QRegExp rx("");
  QValidator *validator = new QRegExpValidator(rx, this);

  setProperty( "HexaWidgetType",  QVariant::fromValue(CYLINDER_TREE) );
  installEventFilter(this);

  name_le->setProperty( "HexaWidgetType",  QVariant::fromValue(CYLINDER_TREE) );
  name_le->installEventFilter(this);

  vex_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  vec_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );

  vex_le->setValidator( validator );
  vec_le->setValidator( validator );

  vex_le->installEventFilter(this);
  vec_le->installEventFilter(this);

  connect( name_le, SIGNAL(returnPressed()), this, SLOT(updateName()));
}

void CylinderDialog::clear()
{
  name_le->clear();
  vex_le->clear();
  vec_le->clear();
  r_spb->clear();
  h_spb->clear();
}


void CylinderDialog::setValue(HEXA_NS::Cylinder* c)
{
  HEXA_NS::Vertex* base      = c->getBase();
  HEXA_NS::Vector* direction = c->getDirection();
  double  r = c->getRadius();
  double  h = c->getHeight();

  name_le->setText( c->getName() );
  vex_le->setText( base->getName() );
  vec_le->setText( direction->getName() );
  r_spb->setValue(r);
  h_spb->setValue(h);

  if ( _patternDataSelectionModel ){
    QModelIndex iCyl       = _patternDataSelectionModel->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(c) );
    QModelIndex iBase      = _patternDataSelectionModel->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(base) );
    QModelIndex iDirection = _patternDataSelectionModel->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(direction) );

    name_le->setProperty( "QModelIndex",  QVariant::fromValue(iCyl) );
    vex_le->setProperty( "QModelIndex",  QVariant::fromValue(iBase) );
    vec_le->setProperty( "QModelIndex",  QVariant::fromValue(iDirection) );
  }
  _value = c;
}

HEXA_NS::Cylinder* CylinderDialog::getValue()
{
  return _value;
}




bool CylinderDialog::apply(QModelIndex& result)
{
  SUIT_OverrideCursor wc;
  if ( !_documentModel ) return false;
  if ( !_patternDataSelectionModel ) return false;
  if ( !_patternBuilderSelectionModel ) return false;
  const PatternDataModel*    patternDataModel    = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  if ( !patternDataModel ) return false;
  if ( !patternBuilderModel ) return false;
  _currentObj = NULL;

  QModelIndex iCyl;
  QModelIndex ivex = patternDataModel->mapToSource( _index[vex_le] );
  QModelIndex ivec = patternBuilderModel->mapToSource( _index[vec_le] );
  double r = r_spb->value();
  double h = h_spb->value();

  if ( ivex.isValid()
    && ivec.isValid() ){
    iCyl = _documentModel->addCylinder( ivex, ivec, r,  h );
  }

  if ( !iCyl.isValid() ){
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT ADD CYLINDER" ) );
    return false;
  }

  _value  = iCyl.model()->data(iCyl, HEXA_DATA_ROLE).value<HEXA_NS::Cylinder *>();

  QString newName = name_le->text();
  if (!newName.isEmpty()) /*{*/
    _documentModel->setName( _ivalue, newName );
//     name_le->setProperty("QModelIndex", QVariant::fromValue( patternBuilderModel->mapFromSource(iCyl) ));
//   } else {
//     name_le->setText( _value->getName() );
//   }

  // to select/highlight result
  result = patternBuilderModel->mapFromSource(iCyl);

  return true;
}




PipeDialog::PipeDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f )
: HexaBaseDialog(parent, editmode, f),
  _value(0)
{
  _helpFileName = "gui_pipe.html";
  setupUi( this );
  _initWidget(editmode);
//   setFocusProxy( vex_le );

  if ( editmode == INFO_MODE ){
    setWindowTitle( tr("Pipe Information") );
  }

}

PipeDialog::~PipeDialog()
{
}

void PipeDialog::_initInputWidget( Mode editmode )
{
  QRegExp rx("");
  QValidator *validator = new QRegExpValidator(rx, this);


  setProperty( "HexaWidgetType",  QVariant::fromValue(PIPE_TREE) );
  installEventFilter(this);

  name_le->setProperty( "HexaWidgetType",  QVariant::fromValue(PIPE_TREE) );
  name_le->installEventFilter(this);

  vex_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  vec_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );

  vex_le->setValidator( validator );
  vec_le->setValidator( validator );

  vex_le->installEventFilter(this);
  vec_le->installEventFilter(this);

  if ( editmode == INFO_MODE ){
    ir_spb->setReadOnly(true);
    er_spb->setReadOnly(true);
    h_spb->setReadOnly(true);
  }

  connect( name_le, SIGNAL(returnPressed()), this, SLOT(updateName()));
}


void PipeDialog::clear()
{
  name_le->clear();
  vex_le->clear();
  vec_le->clear();
}

void PipeDialog::setValue(HEXA_NS::Pipe* p)
{
  HEXA_NS::Vertex* base      = p->getBase();
  HEXA_NS::Vector* direction = p->getDirection();
  double  ir = p->getInternalRadius();
  double  er = p->getRadius();
  double  h  = p->getHeight();

  name_le->setText( p->getName() );
  vex_le->setText( base->getName() );
  vec_le->setText( direction->getName() );
  ir_spb->setValue(ir);
  er_spb->setValue(er);
  h_spb->setValue(h);

  if ( _patternDataSelectionModel ){
    QModelIndex iPipe      = _patternDataSelectionModel->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(p) );
    QModelIndex iBase      = _patternDataSelectionModel->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(base) );
    QModelIndex iDirection = _patternDataSelectionModel->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(direction) );

    name_le->setProperty( "QModelIndex",  QVariant::fromValue(iPipe) );
    vex_le->setProperty( "QModelIndex",  QVariant::fromValue(iBase) );
    vec_le->setProperty( "QModelIndex",  QVariant::fromValue(iDirection) );
  }
  _value = p;
}

HEXA_NS::Pipe* PipeDialog::getValue()
{
  return _value;
}




bool PipeDialog::apply(QModelIndex& result)
{
  SUIT_OverrideCursor wc;
  //if ( !_documentModel ) return;
  if ( !_patternDataSelectionModel )    return false;
  if ( !_patternBuilderSelectionModel ) return false;
  const PatternDataModel*    patternDataModel    = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  if ( !patternDataModel )    return false;
  if ( !patternBuilderModel ) return false;
  _currentObj = NULL;

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

  if ( !iPipe.isValid() ){
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT ADD PIPE" ) );
    return false;
  }
  _value  = iPipe.model()->data(iPipe, HEXA_DATA_ROLE).value<HEXA_NS::Pipe *>();

  QString newName = name_le->text();
  if ( !newName.isEmpty() )/*{*/
    _documentModel->setName( iPipe, newName );
//     name_le->setProperty("QModelIndex", QVariant::fromValue( patternBuilderModel->mapFromSource(iPipe) ));
//   } else {
//     name_le->setText( _value->getName() );
//   }

  // to select/highlight result
  result = patternBuilderModel->mapFromSource(iPipe);

  return true;
}





// ------------------------- MakeGridDialog ----------------------------------
//                  ( Cartesian, Cylindrical, Spherical )
MakeGridDialog::MakeGridDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f )
: HexaBaseDialog(parent, editmode, f)
{
  setupUi( this );
  _initWidget(editmode);
  rb0->click();// Default : cartesian grid
  uniform_rb->click();
  rb0->setFocusProxy( vex_le_rb0 );
  rb1->setFocusProxy( center_le_rb1 );
  rb2->setFocusProxy( vex_le_rb2 );
//   setFocusProxy( rb0 );

  _helpFileName = "creategrids.html#guicartgrid";
  connect( rb0, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
  connect( rb1, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
  connect( rb2, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
}


MakeGridDialog::~MakeGridDialog()
{
}

void MakeGridDialog::_initInputWidget( Mode editmode )
{

  QRegExp rx("");
  QValidator *validator = new QRegExpValidator(rx, this);

//   name_le->setProperty( "HexaWidgetType",  QVariant::fromValue(QUAD_TREE) );
//   name_le->installEventFilter(this);
  setProperty( "HexaWidgetType",  QVariant::fromValue(ELEMENTS_TREE) );
  installEventFilter(this);

  vex_le_rb0->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  vec_le_rb0->setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );
  vex_le_rb0->setValidator( validator );
  vec_le_rb0->setValidator( validator );
  vex_le_rb0->installEventFilter(this);
  vec_le_rb0->installEventFilter(this);

  center_le_rb1->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  base_le_rb1->setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );
  height_le_rb1->setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );
  center_le_rb1->setValidator( validator );
  base_le_rb1->setValidator( validator );
  height_le_rb1->setValidator( validator );
  center_le_rb1->installEventFilter(this);
  base_le_rb1->installEventFilter(this);
  height_le_rb1->installEventFilter(this);

  vex_le_rb2->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  vex_le_rb2->setValidator( validator );
  vex_le_rb2->installEventFilter(this);


  if ( editmode != INFO_MODE ){
    //Cylindrical
    radius_lw->setItemDelegate(new HexaDoubleSpinBoxDelegate(radius_lw));
    radius_lw->setEditTriggers(QAbstractItemView::DoubleClicked);

    angle_lw->setItemDelegate(new HexaDoubleSpinBoxDelegate(angle_lw));
    angle_lw->setEditTriggers(QAbstractItemView::DoubleClicked);

    height_lw->setItemDelegate(new HexaDoubleSpinBoxDelegate(height_lw));
    height_lw->setEditTriggers(QAbstractItemView::DoubleClicked);

    connect( add_radius_pb, SIGNAL(clicked()), this, SLOT(addRadiusItem()) );
    connect( del_radius_pb, SIGNAL(clicked()), this, SLOT(delRadiusItem()) );
    connect( add_angle_pb, SIGNAL(clicked()), this, SLOT(addAngleItem()) );
    connect( del_angle_pb, SIGNAL(clicked()), this, SLOT(delAngleItem()) );
    connect( add_height_pb, SIGNAL(clicked()), this, SLOT(addHeightItem()) );
    connect( del_height_pb, SIGNAL(clicked()), this, SLOT(delHeightItem()) );
  }

}

void MakeGridDialog::clear()
{
  vex_le_rb0->clear();
  vec_le_rb0->clear();
//   nx_spb_rb0->clear();
//   ny_spb_rb0->clear();
//   nz_spb_rb0->clear();

  base_le_rb1->clear();
  center_le_rb1->clear();
  height_le_rb1->clear();
  
  random_param_w->clear();
  radius_lw->clear();
  angle_lw->clear();
  height_lw->clear();
  
//   dr_spb_rb1->clear();
//   nr_spb_rb1->clear();
//   da_spb_rb1->clear();
//   na_spb_rb1->clear();
//   dl_spb_rb1->clear();
//   nl_spb_rb1->clear();
  
  vex_le_rb2->clear();
//   radius_spb_rb2->clear();
//   nb_spb_rb2->clear();
//   k_spb_rb2->clear();
}


void MakeGridDialog::updateHelpFileName()
{
  if ( sender() == rb0 ){
    _helpFileName = "creategrids.html#guicartgrid";
  } else if ( sender() == rb1 ){
    _helpFileName = "creategrids.html#guicylgrid";
  } else if ( sender() == rb2 ){
    _helpFileName = "creategrids.html#guisphergrid";
  }
}


void MakeGridDialog::updateButtonBox() //CS_TODO?
{
//   int nbQuads  = quads_lw->count();
//   int nbAngles = angles_lw->count();
// 
//   if ( nbQuads>0 && nbAngles> 0 ){
//     _applyCloseButton->setEnabled(true);
//     _applyButton->setEnabled(true);
//   } else {
//     _applyCloseButton->setEnabled(false);
//     _applyButton->setEnabled(false);
//   }

//   if ( rb0->isChecked() ){ //cartesian
//   } else if ( rb1->isChecked() ){ //cylindrical
//     nb_radius = radius_lw->item(r);
//     nb_angle  = angle_lw->item(r);
//     nb_height = height_lw->item(r);
//   } else if ( rb2->isChecked() ){ //spherical
//   }
}


void MakeGridDialog::addRadiusItem()
{
  QListWidgetItem* previousItem = radius_lw->currentItem();
  QListWidgetItem* newItem      = new QListWidgetItem();

  double defaultValue = 0.;
  if ( previousItem )
    defaultValue = previousItem->data(Qt::EditRole).toDouble();

  newItem->setData(  Qt::EditRole, QVariant(defaultValue) );
  newItem->setFlags( newItem->flags () | Qt::ItemIsEditable);
  radius_lw->addItem(newItem);

  updateButtonBox();
}

void MakeGridDialog::delRadiusItem()
{
//   std::cout << "delRadiusItem()" << std::endl;
  delete radius_lw->currentItem();
  updateButtonBox();
}


void MakeGridDialog::addAngleItem()
{
  QListWidgetItem* previousItem = angle_lw->currentItem();
  QListWidgetItem* newItem      = new QListWidgetItem();

  double defaultValue = 0.;
  if ( previousItem )
    defaultValue = previousItem->data(Qt::EditRole).toDouble();

  newItem->setData(  Qt::EditRole, QVariant(defaultValue) );
  newItem->setFlags( newItem->flags () | Qt::ItemIsEditable);
  angle_lw->addItem(newItem);

  updateButtonBox();
}

void MakeGridDialog::delAngleItem()
{
  std::cout << "delAngleItem()" << std::endl;
  delete angle_lw->currentItem();
  updateButtonBox();
}


void MakeGridDialog::addHeightItem()
{
  QListWidgetItem* previousItem = height_lw->currentItem();
  QListWidgetItem* newItem      = new QListWidgetItem();

  double defaultValue = 0.;
  if ( previousItem )
    defaultValue = previousItem->data(Qt::EditRole).toDouble();

  newItem->setData(  Qt::EditRole, QVariant(defaultValue) );
  newItem->setFlags( newItem->flags () | Qt::ItemIsEditable);
  height_lw->addItem(newItem);

  updateButtonBox();
}

void MakeGridDialog::delHeightItem()
{
  std::cout << "delHeightItem()" << std::endl;
  delete height_lw->currentItem();
  updateButtonBox();
}


bool MakeGridDialog::apply(QModelIndex& result)
{
  SUIT_OverrideCursor wc;
  //if ( !_documentModel ) return;
  if ( !_patternDataSelectionModel )    return false;
  if ( !_patternBuilderSelectionModel ) return false;
  const PatternDataModel*    patternDataModel    = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  if ( !patternDataModel )    return false;
  if ( !patternBuilderModel ) return false;
  _currentObj = NULL;

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
    QModelIndex icenter_rb1 = patternDataModel->mapToSource( _index[center_le_rb1] );
    QModelIndex ibase_rb1   = patternBuilderModel->mapToSource( _index[base_le_rb1] );
    QModelIndex iheight_rb1 = patternBuilderModel->mapToSource( _index[height_le_rb1] );

    if ( icenter_rb1.isValid()
      && ibase_rb1.isValid()
      && iheight_rb1.isValid() ){

      bool fill = fill_cb_rb1->isChecked();
      if ( uniform_rb->isChecked() ){
        double dr = dr_spb_rb1->value();
        double da = da_spb_rb1->value();
        double dl = dl_spb_rb1->value();
        double nr = nr_spb_rb1->value();
        double na = na_spb_rb1->value();
        double nl = nl_spb_rb1->value();

        iNewElts = _documentModel->makeCylindrical( icenter_rb1,
                                                    ibase_rb1, iheight_rb1,
                                                    dr, da, dl, nr, na, nl, fill );
      }
      if ( random_rb->isChecked() ){
        QListWidgetItem* item = NULL;
        QDoubleSpinBox*   spb = NULL;

        QList<double> radius;
        QList<double> angles;
        QList<double> heights;

        for ( int r = 0; r < radius_lw->count(); ++r){
          item = radius_lw->item(r);
//           std::cout << "radius : " << item->data(Qt::EditRole).toDouble()<< std::endl;
          radius << item->data(Qt::EditRole).toDouble();
        }

        for ( int r = 0; r < angle_lw->count(); ++r){
          item = angle_lw->item(r);
          angles << item->data(Qt::EditRole).toDouble();
        }

        for ( int r = 0; r < height_lw->count(); ++r){
          item = height_lw->item(r);
          heights << item->data(Qt::EditRole).toDouble();
        }

        iNewElts =  _documentModel->makeCylindricals(
            icenter_rb1, ibase_rb1, iheight_rb1,
            radius, angles, heights,
            fill ); //NEW HEXA3

      }
    }

  } else if ( rb2->isChecked() ){ //spherical
    QModelIndex ivex_rb2  = patternDataModel->mapToSource( _index[vex_le_rb2] );
//     QModelIndex ivecx_rb2 = patternBuilderModel->mapToSource( _index[vec_le_rb2] );
    double radius = radius_spb_rb2->value();
    int    nb     = nb_spb_rb2->value();
    int    k      = k_spb_rb2->value();

    if ( ivex_rb2.isValid() ){
      iNewElts = _documentModel->makeSpherical( ivex_rb2, radius, nb, k );
    }
  }

  if ( !iNewElts.isValid() ){
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE GRID" ) );
    return false;
  }


  // to select/highlight result
  result = patternBuilderModel->mapFromSource( iNewElts );

  return true;
}




MakeCylinderDialog::MakeCylinderDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f )
: HexaBaseDialog(parent, editmode, f)
{
  _helpFileName = "gui_blocks_for_cyl_pipe.html#make-cylinder";
  setupUi( this );
  _initWidget(editmode);
//   setFocusProxy( cyl_le );
}

MakeCylinderDialog::~MakeCylinderDialog()
{
}

void MakeCylinderDialog::_initInputWidget( Mode editmode )
{
  QRegExp rx("");
  QValidator *validator = new QRegExpValidator(rx, this);

  setProperty( "HexaWidgetType",  QVariant::fromValue(ELEMENTS_TREE) );
  installEventFilter(this);

  cyl_le->setProperty( "HexaWidgetType",  QVariant::fromValue(CYLINDER_TREE) );
  vec_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );
  cyl_le->setValidator( validator );
  vec_le->setValidator( validator );
  cyl_le->installEventFilter(this);
  vec_le->installEventFilter(this);
}


void MakeCylinderDialog::clear()
{
  cyl_le->clear();
  vec_le->clear();
}


bool MakeCylinderDialog::apply(QModelIndex& result)
{
  SUIT_OverrideCursor wc;
  //if ( !_documentModel ) return;
  if ( !_patternBuilderSelectionModel ) return false;
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  if ( !patternBuilderModel ) return false;
  _currentObj = NULL;

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

  if ( !iElts.isValid() ){
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE CYLINDER" ) );
    return false;
  }

  // to select/highlight result
  result = patternBuilderModel->mapFromSource(iElts);

  return true;
}




MakePipeDialog::MakePipeDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f )
: HexaBaseDialog(parent, editmode, f)
{
  _helpFileName = "gui_blocks_for_cyl_pipe.html#make-pipe";
  setupUi( this );
  _initWidget(editmode);
//   setFocusProxy( pipe_le );
}

MakePipeDialog::~MakePipeDialog()
{
}

void MakePipeDialog::_initInputWidget( Mode editmode )
{
  QRegExp rx("");
  QValidator *validator = new QRegExpValidator(rx, this);

  setProperty( "HexaWidgetType",  QVariant::fromValue(ELEMENTS_TREE) );
  installEventFilter(this);

  pipe_le->setProperty( "HexaWidgetType", QVariant::fromValue(PIPE_TREE) );
  vec_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );

  pipe_le->setValidator( validator );
  vec_le->setValidator( validator );

  pipe_le->installEventFilter(this);
  vec_le->installEventFilter(this);

}

void MakePipeDialog::clear()
{
  pipe_le->clear();
  vec_le->clear();
}


bool MakePipeDialog::apply(QModelIndex& result)
{
  SUIT_OverrideCursor wc;
  //if ( !_documentModel ) return;
  if ( !_patternBuilderSelectionModel ) return false;
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  if ( !patternBuilderModel ) return false;
  _currentObj = NULL;

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
  if ( !iElts.isValid() ){
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE PIPE" ) );
    return false;
  }

  // to select/highlight result
  result = patternBuilderModel->mapFromSource(iElts);

  return true;
}



MakeCylindersDialog::MakeCylindersDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f )
: HexaBaseDialog(parent, editmode, f)
{
  _helpFileName = "gui_blocks_for_cyl_pipe.html#make-cylinders";
  setupUi( this );
  _initWidget(editmode);
//   setFocusProxy( cyl1_le );
}

MakeCylindersDialog::~MakeCylindersDialog()
{
}

void MakeCylindersDialog::_initInputWidget( Mode editmode )
{
  QRegExp rx("");
  QValidator *validator = new QRegExpValidator(rx, this);

  setProperty( "HexaWidgetType",  QVariant::fromValue(CROSSELEMENTS_TREE) );
  installEventFilter(this);

  cyl1_le->setProperty( "HexaWidgetType",  QVariant::fromValue(CYLINDER_TREE) );
  cyl2_le->setProperty( "HexaWidgetType",  QVariant::fromValue(CYLINDER_TREE) );
 
  cyl1_le->setValidator( validator );
  cyl2_le->setValidator( validator );
 
  cyl1_le->installEventFilter(this);
  cyl2_le->installEventFilter(this);
}

void MakeCylindersDialog::clear()
{
  cyl1_le->clear();
  cyl2_le->clear();
}


bool MakeCylindersDialog::apply(QModelIndex& result)
{
  SUIT_OverrideCursor wc;
  //if ( !_documentModel ) return;
  if ( !_patternBuilderSelectionModel ) return false;
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  if ( !patternBuilderModel ) return false;
  _currentObj = NULL;

  QModelIndex iCrossElts;
  QModelIndex icyl1 = patternBuilderModel->mapToSource( _index[cyl1_le] );
  QModelIndex icyl2 = patternBuilderModel->mapToSource( _index[cyl2_le] );

  if ( icyl1.isValid()
    && icyl2.isValid() ){
    iCrossElts = _documentModel->makeCylinders( icyl1, icyl2 );
  }

  if ( !iCrossElts.isValid() ){
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE CYLINDERS" ) );
    return false;
  }

  // to select/highlight result
  result = patternBuilderModel->mapFromSource(iCrossElts);

  return true;
}



MakePipesDialog::MakePipesDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f )
: HexaBaseDialog(parent, editmode, f)
{
  _helpFileName = "gui_blocks_for_cyl_pipe.html#make-pipes";
  setupUi( this );
  _initWidget(editmode);
//   setFocusProxy( pipe1_le );
}

MakePipesDialog::~MakePipesDialog()
{
}

void MakePipesDialog::_initInputWidget( Mode editmode )
{
  QRegExp rx("");
  QValidator *validator = new QRegExpValidator(rx, this);

  pipe1_le->setProperty( "HexaWidgetType",  QVariant::fromValue(PIPE_TREE) );
  pipe2_le->setProperty( "HexaWidgetType",  QVariant::fromValue(PIPE_TREE) );

  pipe1_le->setValidator( validator );
  pipe2_le->setValidator( validator );

  pipe1_le->installEventFilter(this);
  pipe2_le->installEventFilter(this);
}


void MakePipesDialog::clear()
{
  pipe1_le->clear();
  pipe2_le->clear();
}


bool MakePipesDialog::apply(QModelIndex& result)
{
  SUIT_OverrideCursor wc;
  //if ( !_documentModel ) return;
  if ( !_patternBuilderSelectionModel ) return false;
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  if ( !patternBuilderModel ) return false;
  _currentObj = NULL;

  QModelIndex iCrossElts;
  QModelIndex ipipe1 = patternBuilderModel->mapToSource( _index[pipe1_le] );
  QModelIndex ipipe2 = patternBuilderModel->mapToSource( _index[pipe2_le] );

  if ( ipipe1.isValid()
    && ipipe2.isValid() ){
    iCrossElts = _documentModel->makePipes( ipipe1, ipipe2 );
  }
  if ( !iCrossElts.isValid() ){
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE PIPES" ) );
    return false;
  }
//   SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "MAKE PIPES DONE : %1" ).arg(iCrossElts.data().toString()) );
  // to select/highlight result
  result = patternBuilderModel->mapFromSource(iCrossElts);

  return true;
}


RemoveHexaDialog::RemoveHexaDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f )
: HexaBaseDialog(parent, editmode, f)
{
  _helpFileName = "gui_remove.html";
  setupUi( this );
  _initWidget(editmode);
//   setFocusProxy( hexa_le );
}


RemoveHexaDialog::~RemoveHexaDialog()
{
}

void RemoveHexaDialog::_initInputWidget( Mode editmode )
{
  QRegExp rx("");
  QValidator *validator = new QRegExpValidator(rx, this);

  hexa_le->setProperty( "HexaWidgetType",  QVariant::fromValue(HEXA_TREE) );
  hexa_le->setValidator( validator );
  hexa_le->installEventFilter(this);
}

void RemoveHexaDialog::clear()
{
  hexa_le->clear();
}


bool RemoveHexaDialog::apply(QModelIndex& result)
{
  SUIT_OverrideCursor wc;
  //if ( !_documentModel ) return;
  if ( !_patternDataSelectionModel ) return false;
  const PatternDataModel*    patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  if ( !patternDataModel ) return false;
  _currentObj = NULL;

  QModelIndex ihexa = patternDataModel->mapToSource( _index[hexa_le] );

  bool removed = false;
  if ( ihexa.isValid() ){
    if ( connected_cb->isChecked() ){
      removed = _documentModel->removeConnectedHexa( ihexa );
    } else {
      removed = _documentModel->removeHexa( ihexa );
    }
  }

  if ( removed == false ){
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT REMOVE HEXA" ) );
    return false;
  }
  //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "HEXA(S) REMOVED" ) );
//   clear();
  return true;
}



PrismQuadDialog::PrismQuadDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f )
: HexaBaseDialog(parent, editmode, f)
{
  _helpFileName = "gui_prism_join_quad.html#prism-quadrangles";
  setupUi( this );
  _initWidget(editmode);
  setFocusProxy( quads_lw );
}


PrismQuadDialog::~PrismQuadDialog()
{
}


void PrismQuadDialog::_initInputWidget( Mode editmode )
{
  QRegExp rx("");
  QValidator *validator = new QRegExpValidator(rx, this);

  setProperty( "HexaWidgetType",  QVariant::fromValue(ELEMENTS_TREE) );
  installEventFilter(this);

  vec_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );
  quads_lw->setProperty( "HexaWidgetType",  QVariant::fromValue(QUAD_TREE) );

  vec_le->setValidator( validator );
//   quads_lw->setValidator( validator );

  vec_le->installEventFilter(this);
  quads_lw->installEventFilter(this);

  if ( editmode != INFO_MODE ){
    QShortcut* delQuadShortcut   = new QShortcut( QKeySequence(Qt::Key_X), quads_lw );
    delQuadShortcut->setContext( Qt::WidgetShortcut );
    connect( delQuadShortcut, SIGNAL(activated()), this, SLOT(removeQuad()) );
  }

  connect( quads_lw,    SIGNAL(itemSelectionChanged()), this, SLOT(selectElementOfModel()) );
}

void PrismQuadDialog::clear()
{
  vec_le->clear();
  quads_lw->clear();
}


void PrismQuadDialog::removeQuad()
{
  QListWidgetItem *item = quads_lw->currentItem();

  if (item) {
    int r = quads_lw->row(item);
    quads_lw->takeItem(r);
    delete item;
  }

}

// void PrismQuadDialog::clearQuads()
// {
//   if (quads_lw->count() != 0)
//     quads_lw->clear();
// }


bool PrismQuadDialog::apply(QModelIndex& result)
{
  SUIT_OverrideCursor wc;
  //if ( !_documentModel ) return;
  if ( !_patternDataSelectionModel )    return false;
  if ( !_patternBuilderSelectionModel ) return false;
  const PatternDataModel*    patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  if ( !patternDataModel )    return false;
  if ( !patternBuilderModel ) return false;
  _currentObj = NULL;

  /////
  QModelIndexList iquads;
  QModelIndex     iquad; 
  QListWidgetItem* item = NULL;
  for ( int r = 0; r < quads_lw->count(); ++r){
    item = quads_lw->item(r);
    iquad = patternDataModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() );
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

  if ( !iElts.isValid() ){
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT PRISM QUAD(S)" ) );
    return false;
  }

  result = patternBuilderModel->mapFromSource(iElts);

  return true;
}




//JoinQuadDialog
JoinQuadDialog::JoinQuadDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f )
: HexaBaseDialog(parent, editmode, f)
{
  _helpFileName = "gui_prism_join_quad.html#join-quadrangles";
  setupUi( this );
  _initWidget(editmode);
//   setFocusProxy( quads_lw );
}


JoinQuadDialog::~JoinQuadDialog()
{
}


void JoinQuadDialog::_initInputWidget( Mode editmode )
{
  QRegExp rx("");
  QValidator *validator = new QRegExpValidator(rx, this);

  setProperty( "HexaWidgetType",  QVariant::fromValue(ELEMENTS_TREE) );
  installEventFilter(this);

  vex0_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  vex1_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  vex2_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  vex3_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  quad_dest_le->setProperty( "HexaWidgetType",  QVariant::fromValue(QUAD_TREE) );
  quads_lw->setProperty( "HexaWidgetType",  QVariant::fromValue(QUAD_TREE) );

  vex0_le->setValidator( validator );
  vex1_le->setValidator( validator );
  vex2_le->setValidator( validator );
  vex3_le->setValidator( validator );
  quad_dest_le->setValidator( validator );

  vex0_le->installEventFilter(this);
  vex1_le->installEventFilter(this);
  vex2_le->installEventFilter(this);
  vex3_le->installEventFilter(this);
  quad_dest_le->installEventFilter(this);
  quads_lw->installEventFilter(this);

  if ( editmode != INFO_MODE ){
    QShortcut* delQuadShortcut = new QShortcut( QKeySequence(Qt::Key_X), quads_lw );
    delQuadShortcut->setContext( Qt::WidgetShortcut );
    connect( delQuadShortcut,   SIGNAL(activated()), this, SLOT(removeQuad()) );
  }
  connect( quads_lw,    SIGNAL(itemSelectionChanged()), this, SLOT(selectElementOfModel()) );
}


void JoinQuadDialog::clear()
{
  quads_lw->clear();
  vex0_le->clear();
  vex2_le->clear();


  quad_dest_le->clear();
  vex1_le->clear();
  vex3_le->clear();
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

// void JoinQuadDialog::clearQuads()
// {
//   if (quads_lw->count() != 0)
//     quads_lw->clear();
// }



bool JoinQuadDialog::apply(QModelIndex& result)
{
  SUIT_OverrideCursor wc;
  //if ( !_documentModel ) return;
  if ( !_patternDataSelectionModel )    return false;
  if ( !_patternBuilderSelectionModel ) return false;
  const PatternDataModel*    patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  if ( !patternDataModel )    return false;
  if ( !patternBuilderModel ) return false;
  _currentObj = NULL;

  /////
  QModelIndexList iquads;
  QModelIndex     iquad; 
  QListWidgetItem* item = NULL;
  for ( int r = 0; r < quads_lw->count(); ++r){
    item = quads_lw->item(r);
    iquad = patternDataModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() );
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

  if ( !iElts.isValid() ){
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT JOIN QUAD(S)" ) );
    return false;
  }

  result = patternBuilderModel->mapFromSource(iElts);

  return true;
}


// void JoinQuadDialog::reject()
// {
//   QDialog::reject();
//   _disallowSelection();
// }



// ------------------------- Merge ----------------------------------
MergeDialog::MergeDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f )
: HexaBaseDialog(parent, editmode, f)
{
  setupUi( this );
  _initWidget(editmode);
  rb0->setFocusProxy( v0_le_rb0 );
  rb1->setFocusProxy( e0_le_rb1 );
  rb2->setFocusProxy( q0_le_rb2 );
//   setFocusProxy( rb0 );
  rb0->click();

  _helpFileName = "gui_merge_elmts.html#merge-2-vertices";
  connect( rb0, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
  connect( rb1, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
  connect( rb2, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
}


MergeDialog::~MergeDialog()
{
}


void MergeDialog::_initInputWidget( Mode editmode )
{
  QRegExp rx("");
  QValidator *validator = new QRegExpValidator(rx, this);


  v0_le_rb0->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  v1_le_rb0->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  v0_le_rb0->setValidator( validator );
  v1_le_rb0->setValidator( validator );
  v0_le_rb0->installEventFilter(this);
  v1_le_rb0->installEventFilter(this);

  v0_le_rb1->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  v1_le_rb1->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  e0_le_rb1->setProperty( "HexaWidgetType",  QVariant::fromValue(EDGE_TREE) );
  e1_le_rb1->setProperty( "HexaWidgetType",  QVariant::fromValue(EDGE_TREE) );

  v0_le_rb1->setValidator( validator );
  v1_le_rb1->setValidator( validator );
  e0_le_rb1->setValidator( validator );
  e1_le_rb1->setValidator( validator );

  v0_le_rb1->installEventFilter(this);
  v1_le_rb1->installEventFilter(this);
  e0_le_rb1->installEventFilter(this);
  e1_le_rb1->installEventFilter(this);
 

  v0_le_rb2->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  v1_le_rb2->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  v2_le_rb2->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  v3_le_rb2->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  q0_le_rb2->setProperty( "HexaWidgetType",  QVariant::fromValue(QUAD_TREE) );
  q1_le_rb2->setProperty( "HexaWidgetType",  QVariant::fromValue(QUAD_TREE) );


  v0_le_rb2->setValidator( validator );
  v1_le_rb2->setValidator( validator );
  v2_le_rb2->setValidator( validator );
  v3_le_rb2->setValidator( validator );
  q0_le_rb2->setValidator( validator );
  q1_le_rb2->setValidator( validator );

  v0_le_rb2->installEventFilter(this);
  v1_le_rb2->installEventFilter(this);
  v2_le_rb2->installEventFilter(this);
  v3_le_rb2->installEventFilter(this);
  q0_le_rb2->installEventFilter(this);
  q1_le_rb2->installEventFilter(this);
}


void MergeDialog::clear()
{
  v0_le_rb0->clear();
  v1_le_rb0->clear();
  v0_le_rb1->clear();
  v1_le_rb1->clear();
  v0_le_rb2->clear();
  v1_le_rb2->clear();
  v2_le_rb2->clear();
  v3_le_rb2->clear();
  e0_le_rb1->clear();
  e1_le_rb1->clear();
  q0_le_rb2->clear();
  q1_le_rb2->clear();
}

void MergeDialog::updateHelpFileName()
{
  if ( sender() == rb0 ){
    _helpFileName = "gui_merge_elmts.html#merge-2-vertices";
  } else if ( sender() == rb1 ){
    _helpFileName = "gui_merge_elmts.html#merge-2-edges";
  } else if ( sender() == rb2 ){
    _helpFileName = "gui_merge_elmts.html#merge-2-quadrangles";
  }
}



bool MergeDialog::apply(QModelIndex& result)
{
  SUIT_OverrideCursor wc;
  if ( !_patternDataSelectionModel ) return false;
  const PatternDataModel*    patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  if ( !patternDataModel ) return false;
  _currentObj = NULL;

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

  if ( merged == false ){
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MERGE" ) );
    return false;
  }
  //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "MERGED" ) );
//   clear();
  return true;
}




// ------------------------- Disconnect ----------------------------------
DisconnectDialog::DisconnectDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f )
: HexaBaseDialog(parent, editmode, f)
{
  setupUi( this );
  _initWidget(editmode);
  rb0->setFocusProxy( v_le_rb0 );
  rb1->setFocusProxy( e_le_rb1 );
  rb2->setFocusProxy( q_le_rb2 );
  rb0->click();
//   setFocusProxy( rb0 );

  _helpFileName = "gui_disc_elmts.html#disconnect-a-vertex";

  connect( rb0, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
  connect( rb1, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
  connect( rb2, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
}


DisconnectDialog::~DisconnectDialog()
{
}

void DisconnectDialog::_initInputWidget( Mode editmode )
{
  QRegExp rx("");
  QValidator *validator = new QRegExpValidator(rx, this);

  setProperty( "HexaWidgetType",  QVariant::fromValue(ELEMENTS_TREE) );
  installEventFilter(this);

  //vertex
  v_le_rb0->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  h_le_rb0->setProperty( "HexaWidgetType",  QVariant::fromValue(HEXA_TREE) );
  v_le_rb0->setValidator( validator );
  h_le_rb0->setValidator( validator );
  v_le_rb0->installEventFilter(this);
  h_le_rb0->installEventFilter(this);

  //edge
  e_le_rb1->setProperty( "HexaWidgetType",  QVariant::fromValue(EDGE_TREE) );
  h_le_rb1->setProperty( "HexaWidgetType",  QVariant::fromValue(HEXA_TREE) );
  e_le_rb1->setValidator( validator );
  h_le_rb1->setValidator( validator );
  e_le_rb1->installEventFilter(this);
  h_le_rb1->installEventFilter(this);

  //quad
  q_le_rb2->setProperty( "HexaWidgetType",  QVariant::fromValue(QUAD_TREE) );
  h_le_rb2->setProperty( "HexaWidgetType",  QVariant::fromValue(HEXA_TREE) );
  q_le_rb2->setValidator( validator );
  h_le_rb2->setValidator( validator );
  q_le_rb2->installEventFilter(this);
  h_le_rb2->installEventFilter(this);
}


void DisconnectDialog::clear()
{
  v_le_rb0->clear();
  e_le_rb1->clear();
  q_le_rb2->clear();
  h_le_rb0->clear();
  h_le_rb1->clear();
  h_le_rb2->clear();
}


void DisconnectDialog::updateHelpFileName()
{
  if ( sender() == rb0 ){
    _helpFileName = "gui_disc_elmts.html#disconnect-a-vertex";
  } else if ( sender() == rb1 ){
    _helpFileName = "gui_disc_elmts.html#disconnect-an-edge";
  } else if ( sender() == rb2 ){
    _helpFileName = "gui_disc_elmts.html#disconnect-a-quadrangle";
  }
}



bool DisconnectDialog::apply(QModelIndex& result)
{
  SUIT_OverrideCursor wc;

  if ( !_patternDataSelectionModel )    return false;
  if ( !_patternBuilderSelectionModel ) return false;
  const PatternDataModel*    patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  if ( !patternDataModel )    return false;
  if ( !patternBuilderModel ) return false;
  _currentObj = NULL;

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

  if ( !iElts.isValid() ){
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT DISCONNECT" ) );
    return false;
  }

  result = patternBuilderModel->mapFromSource(iElts);

  return true;
}




// ------------------------- CutEdgeDialog ----------------------------------
CutEdgeDialog::CutEdgeDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f ):
HexaBaseDialog(parent, editmode, f)
{
  _helpFileName = "gui_cut_hexa.html";
  setupUi( this );
  _initWidget(editmode);
//   setFocusProxy( e_le );
}

CutEdgeDialog::~CutEdgeDialog()
{
}

void CutEdgeDialog::_initInputWidget( Mode editmode )
{
  QRegExp rx("");
  QValidator *validator = new QRegExpValidator(rx, this);

  setProperty( "HexaWidgetType",  QVariant::fromValue(ELEMENTS_TREE) );
  installEventFilter(this);

  e_le->setProperty( "HexaWidgetType",  QVariant::fromValue(EDGE_TREE) );
  e_le->setValidator( validator );
  e_le->installEventFilter(this);
}

void CutEdgeDialog::clear()
{
  e_le->clear();
}


bool CutEdgeDialog::apply(QModelIndex& result)
{
  SUIT_OverrideCursor wc;
  //if ( !_documentModel ) return;
  if ( !_patternDataSelectionModel )    return false;
  if ( !_patternBuilderSelectionModel ) return false;
  const PatternDataModel*    patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  if ( !patternDataModel )    return false;
  if ( !patternBuilderModel ) return false;
  _currentObj = NULL;

  int nbCut = nb_cut_spb->value();
  QModelIndex iedge = patternDataModel->mapToSource( _index[e_le] );

  QModelIndex iElts;
  if ( iedge.isValid() ){
    iElts = _documentModel->cutEdge( iedge, nbCut );
  }

  if ( !iElts.isValid() ){
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT CUT EDGE" ) );
    return false;
  }

  result = patternBuilderModel->mapFromSource(iElts);

   return true;
}




// // ------------------------- MakeTransformationDialog ----------------------------------
MakeTransformationDialog::MakeTransformationDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f )
: HexaBaseDialog(parent, editmode, f)
{
  setupUi( this );
  _initWidget(editmode);
  rb0->setFocusProxy( elts_le_rb0 );
  rb1->setFocusProxy( elts_le_rb1 );
  rb2->setFocusProxy( elts_le_rb2 );
  setFocusProxy( rb0 );
  rb0->click();

  _helpFileName = "gui_make_elmts.html#make-elements-by-translation";
  connect( rb0, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
  connect( rb1, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
  connect( rb2, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
}


MakeTransformationDialog::~MakeTransformationDialog()
{
}

void MakeTransformationDialog::_initInputWidget( Mode editmode )
{
  QRegExp rx("");
  QValidator *validator = new QRegExpValidator(rx, this);

  setProperty( "HexaWidgetType",  QVariant::fromValue(ELEMENTS_TREE) );
  installEventFilter(this);

  vec_le_rb0->setProperty( "HexaWidgetType", QVariant::fromValue(VECTOR_TREE) );
  elts_le_rb0->setProperty( "HexaWidgetType", QVariant::fromValue(ELEMENTS_TREE) );

  vec_le_rb0->setValidator( validator );
  elts_le_rb0->setValidator( validator );
  vec_le_rb0->installEventFilter(this);
  elts_le_rb0->installEventFilter(this);

  vex_le_rb1->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  elts_le_rb1->setProperty( "HexaWidgetType",  QVariant::fromValue(ELEMENTS_TREE) );
  vex_le_rb1->setValidator( validator );
  elts_le_rb1->setValidator( validator );
  vex_le_rb1->installEventFilter(this);
  elts_le_rb1->installEventFilter(this);

  vex_le_rb2->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  vec_le_rb2->setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );
  elts_le_rb2->setProperty( "HexaWidgetType",  QVariant::fromValue(ELEMENTS_TREE) );
  vex_le_rb2->setValidator( validator );
  vec_le_rb2->setValidator( validator );
  elts_le_rb2->setValidator( validator );
  vex_le_rb2->installEventFilter(this);
  vec_le_rb2->installEventFilter(this);
  elts_le_rb2->installEventFilter(this);

}

void MakeTransformationDialog::clear()
{
  vex_le_rb1->clear(); 
  vex_le_rb2->clear();
  vec_le_rb0->clear();
  vec_le_rb2->clear();
  elts_le_rb0->clear();
  elts_le_rb1->clear();
  elts_le_rb2->clear();
}


void MakeTransformationDialog::updateHelpFileName()
{
  if ( sender() == rb0 ){
    _helpFileName = "gui_make_elmts.html#make-elements-by-translation";
  } else if ( sender() == rb1 ){
    _helpFileName = "gui_make_elmts.html#make-elements-by-scaling";
  } else if ( sender() == rb2 ){
    _helpFileName = "gui_make_elmts.html#make-elements-by-rotation";
  }
}


bool MakeTransformationDialog::apply(QModelIndex& result)
{
  SUIT_OverrideCursor wc;
  if ( !_patternDataSelectionModel )    return false;
  if ( !_patternBuilderSelectionModel ) return false;
  const PatternDataModel*    patternDataModel    = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  if ( !patternDataModel )    return false;
  if ( !patternBuilderModel ) return false;
  _currentObj = NULL;

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

  if ( !iNewElts.isValid() ){
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE TRANSFORMATION" ) );
    return false;
  }

  result = patternBuilderModel->mapFromSource(iNewElts);

  return true;
}



// // ------------------------- MakeSymmetryDialog ----------------------------------
MakeSymmetryDialog::MakeSymmetryDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f ): 
HexaBaseDialog(parent, editmode, f)
{
  setupUi( this );
  _initWidget(editmode);
  rb0->setFocusProxy( elts_le_rb0 );
  rb1->setFocusProxy( elts_le_rb1 );
  rb2->setFocusProxy( elts_le_rb2 );
  rb0->click();
//     setFocusProxy( rb0 );

  _helpFileName = "gui_make_symmetry.html#make-elements-by-point-symmetry";
  connect( rb0, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
  connect( rb1, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
  connect( rb2, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
}


MakeSymmetryDialog::~MakeSymmetryDialog()
{
}

void MakeSymmetryDialog::_initInputWidget( Mode editmode )
{
  QRegExp rx("");
  QValidator *validator = new QRegExpValidator(rx, this);

  setProperty( "HexaWidgetType",  QVariant::fromValue(ELEMENTS_TREE) );
  installEventFilter(this);

  vex_le_rb0->setProperty( "HexaWidgetType", QVariant::fromValue(VERTEX_TREE) );
  elts_le_rb0->setProperty( "HexaWidgetType", QVariant::fromValue(ELEMENTS_TREE) );
  vex_le_rb0->setValidator( validator );
  elts_le_rb0->setValidator( validator );
  vex_le_rb0->installEventFilter(this);
  elts_le_rb0->installEventFilter(this);

  vex_le_rb1->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  vec_le_rb1->setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );
  elts_le_rb1->setProperty( "HexaWidgetType",  QVariant::fromValue(ELEMENTS_TREE) );
  vex_le_rb1->setValidator( validator );
  vec_le_rb1->setValidator( validator );
  elts_le_rb1->setValidator( validator );
  vex_le_rb1->installEventFilter(this);
  vec_le_rb1->installEventFilter(this);
  elts_le_rb1->installEventFilter(this);


  vex_le_rb2->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  vec_le_rb2->setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );
  elts_le_rb2->setProperty( "HexaWidgetType",  QVariant::fromValue(ELEMENTS_TREE) );
  vex_le_rb2->setValidator( validator );
  vec_le_rb2->setValidator( validator );
  elts_le_rb2->setValidator( validator );
  vex_le_rb2->installEventFilter(this);
  vec_le_rb2->installEventFilter(this);
  elts_le_rb2->installEventFilter(this);

}

void MakeSymmetryDialog::clear()
{
  vex_le_rb0->clear();
  vex_le_rb1->clear();
  vex_le_rb2->clear();

  vec_le_rb1->clear();
  vec_le_rb2->clear();

  elts_le_rb0->clear();
  elts_le_rb1->clear();
  elts_le_rb2->clear();
}



void MakeSymmetryDialog::updateHelpFileName()
{
  if ( sender() == rb0 ){
    _helpFileName = "gui_make_symmetry.html#make-elements-by-point-symmetry";
  } else if ( sender() == rb1 ){
    _helpFileName = "gui_make_symmetry.html#make-elements-by-line-symmetry";
  } else if ( sender() == rb2 ){
    _helpFileName = "gui_make_symmetry.html#make-elements-by-plan-symmetry";
  }
}



bool MakeSymmetryDialog::apply(QModelIndex& result)
{
  SUIT_OverrideCursor wc;
  if ( !_patternDataSelectionModel )    return false;
  if ( !_patternBuilderSelectionModel ) return false;
  const PatternDataModel*    patternDataModel    = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  if ( !patternDataModel )    return false;
  if ( !patternBuilderModel ) return false;
  _currentObj = NULL;

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

  if ( !iNewElts.isValid() ){
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE TRANSFORMATION" ) );
    return false;
  }

  result = patternBuilderModel->mapFromSource(iNewElts);

  return true;
}





// // ------------------------- PerformTransformationDialog ----------------------------------
PerformTransformationDialog::PerformTransformationDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f ): HexaBaseDialog(parent, editmode, f)
{
  setupUi( this );
  _initWidget(editmode);
  rb0->setFocusProxy( elts_le_rb0 );
  rb1->setFocusProxy( elts_le_rb1 );
  rb2->setFocusProxy( elts_le_rb2 );
//   setFocusProxy( rb0 );
  rb0->click();
 
  _helpFileName = "gui_modify_elmts.html#modify-elements-by-translation";
  connect( rb0, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
  connect( rb1, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
  connect( rb2, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
}


PerformTransformationDialog::~PerformTransformationDialog()
{
}

void PerformTransformationDialog::_initInputWidget( Mode editmode )
{
  QRegExp rx("");
  QValidator *validator = new QRegExpValidator(rx, this);

  vec_le_rb0->setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );
  elts_le_rb0->setProperty( "HexaWidgetType", QVariant::fromValue(ELEMENTS_TREE) );
  vec_le_rb0->setValidator( validator );
  elts_le_rb0->setValidator( validator );
  vec_le_rb0->installEventFilter(this);
  elts_le_rb0->installEventFilter(this);

  vex_le_rb1->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  elts_le_rb1->setProperty( "HexaWidgetType",  QVariant::fromValue(ELEMENTS_TREE) );
  vex_le_rb1->setValidator( validator );
  elts_le_rb1->setValidator( validator );
  vex_le_rb1->installEventFilter(this);
  elts_le_rb1->installEventFilter(this);


  vex_le_rb2->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  vec_le_rb2->setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );
  elts_le_rb2->setProperty( "HexaWidgetType",  QVariant::fromValue(ELEMENTS_TREE) );
  vex_le_rb2->setValidator( validator );
  vec_le_rb2->setValidator( validator );
  elts_le_rb2->setValidator( validator );
  vex_le_rb2->installEventFilter(this);
  vec_le_rb2->installEventFilter(this);
  elts_le_rb2->installEventFilter(this);

}

void PerformTransformationDialog::clear()
{
  vex_le_rb1->clear(); vex_le_rb2->clear();
  vec_le_rb0->clear(); vec_le_rb2->clear();
  elts_le_rb0->clear(); elts_le_rb1->clear(); elts_le_rb2->clear();
}

void PerformTransformationDialog::updateHelpFileName()
{
  if ( sender() == rb0 ){
    _helpFileName = "gui_modify_elmts.html#modify-elements-by-translation";
  } else if ( sender() == rb1 ){
    _helpFileName = "gui_modify_elmts.html#modify-elements-by-scaling";
  } else if ( sender() == rb2 ){
    _helpFileName = "gui_modify_elmts.html#modify-elements-by-rotation";
  }
}





bool PerformTransformationDialog::apply(QModelIndex& result)
{
  SUIT_OverrideCursor wc;
  if ( !_patternDataSelectionModel )    return false;
  if ( !_patternBuilderSelectionModel ) return false;
  const PatternDataModel*    patternDataModel    = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  if ( !patternDataModel )    return false;
  if ( !patternBuilderModel ) return false;
  _currentObj = NULL;

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

  if ( performed == false){
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT PERFORM TRANSFORMATION" ) );
    return false;
  }


  return true;
}





// // ------------------------- PerformSymmetryDialog ----------------------------------
PerformSymmetryDialog::PerformSymmetryDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f )
: HexaBaseDialog(parent, editmode, f)
{
  setupUi( this );
  _initWidget( editmode );
  rb0->setFocusProxy( elts_le_rb0 );
  rb1->setFocusProxy( elts_le_rb1 );
  rb2->setFocusProxy( elts_le_rb2 );
//   setFocusProxy( rb0 );
  rb0->click();

  _helpFileName = "gui_modify_symmetry.html#modify-elements-by-point-symmetry";
  connect( rb0, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
  connect( rb1, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
  connect( rb2, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
}


PerformSymmetryDialog::~PerformSymmetryDialog()
{
}

void PerformSymmetryDialog::_initInputWidget( Mode editmode )
{
  QRegExp rx("");
  QValidator *validator = new QRegExpValidator(rx, this);

  vex_le_rb0->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  elts_le_rb0->setProperty( "HexaWidgetType", QVariant::fromValue(ELEMENTS_TREE) );
  vex_le_rb0->setValidator( validator );
  elts_le_rb0->setValidator( validator );
  vex_le_rb0->installEventFilter(this);
  elts_le_rb0->installEventFilter(this);

  vex_le_rb1->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  elts_le_rb1->setProperty( "HexaWidgetType",  QVariant::fromValue(ELEMENTS_TREE) );
  vex_le_rb1->setValidator( validator );
  elts_le_rb1->setValidator( validator );
  vex_le_rb1->installEventFilter(this);
  elts_le_rb1->installEventFilter(this);


  vex_le_rb2->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  vec_le_rb2->setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );
  elts_le_rb2->setProperty( "HexaWidgetType",  QVariant::fromValue(ELEMENTS_TREE) );
  vex_le_rb2->setValidator( validator );
  vec_le_rb2->setValidator( validator );
  elts_le_rb2->setValidator( validator );
  vex_le_rb2->installEventFilter(this);
  vec_le_rb2->installEventFilter(this);
  elts_le_rb2->installEventFilter(this);

}

void PerformSymmetryDialog::clear()
{
  vex_le_rb0->clear(); vex_le_rb1->clear(); vex_le_rb2->clear();
  vec_le_rb1->clear(); vec_le_rb2->clear();
  elts_le_rb0->clear(); elts_le_rb1->clear(); elts_le_rb2->clear();
}


void PerformSymmetryDialog::updateHelpFileName()
{
  if ( sender() == rb0 ){
    _helpFileName = "gui_modify_symmetry.html#modify-elements-by-point-symmetry";
  } else if ( sender() == rb1 ){
    _helpFileName = "gui_modify_symmetry.html#modify-elements-by-line-symmetry";
  } else if ( sender() == rb2 ){
    _helpFileName = "gui_modify_symmetry.html#modify-elements-by-plan-symmetry";
  }
}

bool PerformSymmetryDialog::apply(QModelIndex& result)
{
  SUIT_OverrideCursor wc;
  if ( !_patternDataSelectionModel )    return false;
  if ( !_patternBuilderSelectionModel ) return false;
  const PatternDataModel*    patternDataModel    = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  if ( !patternDataModel )    return false;
  if ( !patternBuilderModel ) return false;
  _currentObj = NULL;
  
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

  if ( performed == false ){
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT PERFORM SYMMETRY" ) );
    return false;
  }

  return true;
}



// // ------------------------- EdgeAssocDialog ----------------------------------
EdgeAssocDialog::EdgeAssocDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f ): 
HexaBaseDialog( parent, editmode, f )
{
  _helpFileName ="gui_asso_quad_to_geom.html#associate-to-edges-or-wires-of-the-geometry";
  setupUi( this );
  _initWidget(editmode);
  _initViewManager();
  setFocusProxy( edges_lw );

//   _firstLine.nullify();
//   _lastLine.nullify();
//   _currentLine.nullify();
  _currentParameter = 0.;

}


EdgeAssocDialog::~EdgeAssocDialog()
{
//   disconnect( delEdgeShortcut, SIGNAL(activated()), this, SLOT(deleteEdgeItem()) );
//   disconnect( delLineShortcut, SIGNAL(activated()), this, SLOT(deleteLineItem()) );
  disconnect( HEXABLOCKGUI::selectionMgr(), SIGNAL(currentSelectionChanged()), this, SLOT(addLine()) );
  disconnect( pstart_spb, SIGNAL(valueChanged(double)), this, SLOT( pstartChanged(double)) );
  disconnect( pend_spb,   SIGNAL(valueChanged(double)), this, SLOT( pendChanged(double)) );
}

void EdgeAssocDialog::_initInputWidget( Mode editmode )
{
  QRegExp rx("");
  QValidator *validator = new QRegExpValidator(rx, this);

  //model
  first_vex_le->setProperty( "HexaWidgetType", QVariant::fromValue(VERTEX_TREE) );
  first_vex_le->installEventFilter(this);
  first_vex_le->setValidator( validator );

  edges_lw->setProperty( "HexaWidgetType", QVariant::fromValue(EDGE_TREE) );
  edges_lw->installEventFilter(this);


  //geom
  lines_lw->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_EDGE) );
  lines_lw->installEventFilter(this);


  QShortcut* delEdgeShortcut = new QShortcut(QKeySequence(/*Qt::Key_Delete*/Qt::Key_X/*Qt::Key_Alt*//*Qt::Key_Space*/), edges_lw);
  QShortcut* delLineShortcut = new QShortcut(QKeySequence(Qt::Key_X), lines_lw);
  delLineShortcut->setContext( Qt::WidgetWithChildrenShortcut );//Qt::ApplicationShortcut );//);//Qt::ApplicationShortcut ); //Qt::WidgetShortcut );
  delEdgeShortcut->setContext( Qt::WidgetWithChildrenShortcut );//Qt::WidgetWithChildrenShortcut);//Qt::ApplicationShortcut ); //Qt::WidgetShortcut );

  pend_spb->setValue(1.);

  connect( delEdgeShortcut, SIGNAL(activated()), this, SLOT(deleteEdgeItem()) );
  connect( delLineShortcut, SIGNAL(activated()), this, SLOT(deleteLineItem()) );
  connect( pstart_spb, SIGNAL(valueChanged(double)), this, SLOT( pstartChanged(double)) );
  connect( pend_spb,   SIGNAL(valueChanged(double)), this, SLOT( pendChanged(double)) );
  connect( edges_lw,   SIGNAL(itemSelectionChanged()), this, SLOT(selectElementOfModel()) );

}

void EdgeAssocDialog::clear()
{
  first_vex_le->clear();
  edges_lw->clear();
  lines_lw->clear();
}


void EdgeAssocDialog::deleteEdgeItem()
{
  delete edges_lw->currentItem();
}

void EdgeAssocDialog::deleteLineItem()
{
  delete lines_lw->currentItem();
}


void EdgeAssocDialog::setGeomEngine( GEOM::GEOM_Gen_var geomEngine )
{
  _geomEngine = geomEngine;
}

void EdgeAssocDialog::onWindowActivated(SUIT_ViewManager* vm)
{
  SUIT_ViewWindow* v = vm->getActiveView();
  QString vmType = vm->getType();
  if ( (vmType == SVTK_Viewer::Type()) || (vmType == VTKViewer_Viewer::Type()) ){
    edges_lw->setFocus();
  } else if ( vmType == OCCViewer_Viewer::Type() ){
    lines_lw->setFocus();
  }
}


GEOM::GEOM_IOperations_ptr EdgeAssocDialog::createOperation()
{
  return _geomEngine->GetIBasicOperations(getStudyId());
}


bool EdgeAssocDialog::execute(ObjectList& objects)
{
  MESSAGE("execute(){");
  MESSAGE("*    objects.size()" << objects.size() );

  bool res = false;

  GEOM::GEOM_Object_var anObj;
  QStringList aParameters;

  GEOM::GEOM_IBasicOperations_var anOper = GEOM::GEOM_IBasicOperations::_narrow( getOperation() );
  anObj = anOper->MakePointOnCurve( _currentLine.get(), _currentParameter );
  if ( !anObj->_is_nil() ) {
    objects.push_back(anObj._retn());
    res = true;
  }
  MESSAGE("}execute()");
  return res;
}


void EdgeAssocDialog::pstartChanged( double val )
{
  MESSAGE("pstartChanged("<<val<<") { ");
  QListWidgetItem* lineItem = lines_lw->item( 0 );
  if (!lineItem) return;
//   MESSAGE("data : "<<lineItem->text().toStdString());
  _currentLine = lineItem->data(LW_GEOM_OBJ_ROLE).value<GEOM::GeomObjPtr>();
  _currentParameter = pstart_spb->value();
  displayPreview(true);
  MESSAGE("}pstartChanged()");
}


void EdgeAssocDialog::pendChanged( double val )
{
  MESSAGE("pendChanged("<<val<<") { ");
//   if ( _lastLine->_is_nil() ) return;
  QListWidgetItem* lineItem = lines_lw->item( lines_lw->count()-1 );
  if (!lineItem) return;
//   MESSAGE("data : "<<lineItem->text().toStdString());
  _currentLine      = lineItem->data(LW_GEOM_OBJ_ROLE).value<GEOM::GeomObjPtr>();
  _currentParameter = pend_spb->value();
  displayPreview(true);
  MESSAGE("}pendChanged()");
}


void EdgeAssocDialog::onCurrentSelectionChanged()
{
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
    std::cout << "aLine.name"  << aLine.name.toStdString()  << std::endl;
    std::cout << "aLine.entry" << aLine.entry.toStdString() << std::endl;
    std::cout << "aLine.subid" << aLine.subid.toStdString() << std::endl;
    std::cout << "aLine.brep"  << aLine.brep.toStdString() << std::endl;


    item  = new QListWidgetItem( aLine.name );
    item->setData(  LW_ASSOC_ROLE, QVariant::fromValue<DocumentModel::GeomObj>(aLine) );
    item->setData(  LW_GEOM_OBJ_ROLE, QVariant::fromValue<GEOM::GeomObjPtr>(aSelectedObject) );
    lines_lw->addItem(item);
//     lines_lw->setCurrentRow( lines_lw->count() - 1 );

//     // preview
//     if ( _firstLine->_is_nil() ) _firstLine = aSelectedObject;
//     _lastLine = aSelectedObject;
  }
}



bool EdgeAssocDialog::apply(QModelIndex& result)
{
  SUIT_OverrideCursor wc;
  bool assocOk = false;

  if ( !_patternDataSelectionModel ) return false;
  const PatternDataModel* patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  if ( !patternDataModel ) return false;
  _currentObj = NULL;

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
    MESSAGE(" assocs => " << aLine.name.toStdString() );
    MESSAGE(" assocs => " << aLine.brep.toStdString() );
    assocs << aLine;
  }

  if ( close_cb->isChecked() ){
    QModelIndex iFirstVertex = patternDataModel->mapToSource( _index[first_vex_le] );
    assocOk = _documentModel->associateClosedLine( iFirstVertex, iEdges, assocs, pstart_spb->value() );
  } else {
    assocOk = _documentModel->associateOpenedLine( iEdges, assocs, pstart_spb->value(), pend_spb->value() );
  }

  if ( !assocOk ){
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE EDGE ASSOCIATION" ) );
    return false;
  }

  //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "EDGE(S) ASSOCIATION OK : " ) );
//   clear();
  return true;
}






// // ------------------------- QuadAssocDialog ----------------------------------
QuadAssocDialog::QuadAssocDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f ): 
HexaBaseDialog(parent, editmode, f)
{
  _helpFileName = "gui_asso_quad_to_geom.html#associate-to-a-face-or-a-shell-of-the-geometry";
  setupUi( this );
  _initWidget(editmode);
  _initViewManager();
  setFocusProxy( quad_le );
}


QuadAssocDialog::~QuadAssocDialog()
{
  disconnect( _delFaceShortcut, SIGNAL(activated()), this, SLOT(deleteFaceItem()) );
  disconnect( HEXABLOCKGUI::selectionMgr(), SIGNAL(currentSelectionChanged()), this, SLOT(addFace()) );
  delete _delFaceShortcut;
}

void QuadAssocDialog::_initInputWidget( Mode editmode )
{
  QRegExp rx("");
  QValidator *validator = new QRegExpValidator(rx, this);

  quad_le->setProperty( "HexaWidgetType", QVariant::fromValue(QUAD_TREE) );
  quad_le->installEventFilter(this);
  quad_le->setValidator( validator );


  faces_lw->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_FACE) );
  faces_lw->installEventFilter(this);
  _delFaceShortcut = new QShortcut( QKeySequence(Qt::Key_X/*Qt::Key_Delete*/), faces_lw );
  _delFaceShortcut->setContext( Qt::WidgetShortcut );

  connect( _delFaceShortcut, SIGNAL(activated()), this, SLOT(deleteFaceItem()) );

}

void QuadAssocDialog::clear()
{
  quad_le->clear();
  faces_lw->clear();
}

void QuadAssocDialog::onCurrentSelectionChanged()
{
  if ( !isVisible() ) return;

  SUIT_ViewWindow* window = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );

  bool isOCC = (window && window->getViewManager()->getType() == OCCViewer_Viewer::Type());
//   bool isVTK = (window && window->getViewManager()->getType() == SVTK_Viewer::Type());

  if (!isOCC) return;

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
    std::cout << "aFace.brep"  << aFace.brep.toStdString() << std::endl;

    item  = new QListWidgetItem( aFace.name );
    item->setData(  LW_ASSOC_ROLE, QVariant::fromValue<DocumentModel::GeomObj>(aFace) );
    faces_lw->addItem(item);
//     faces_lw->setCurrentRow( faces_lw->count() - 1 );
  }
}


void QuadAssocDialog::onWindowActivated(SUIT_ViewManager* vm)
{
  SUIT_ViewWindow* v = vm->getActiveView();
  QString     vmType = vm->getType();
  if ( (vmType == SVTK_Viewer::Type()) || (vmType == VTKViewer_Viewer::Type()) ){
    quad_le->setFocus();
  } else if ( vmType == OCCViewer_Viewer::Type() ){
    faces_lw->setFocus();
  }
}


void QuadAssocDialog::deleteFaceItem()
{
  delete faces_lw->currentItem();
}


bool QuadAssocDialog::apply(QModelIndex& result)
{
  SUIT_OverrideCursor wc;
  //if ( !_documentModel ) return;
  if ( !_patternDataSelectionModel ) return false;
  const PatternDataModel* patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  if ( !patternDataModel )           return false;
  _currentObj = NULL;

  // quad
  QModelIndex iQuad = patternDataModel->mapToSource( _index[quad_le] );

  // faces
  QListWidgetItem* item = NULL;
  DocumentModel::GeomObj aFace;
  for ( int r = 0; r < faces_lw->count(); ++r ){
    item = faces_lw->item(r);
    aFace = item->data(LW_ASSOC_ROLE).value<DocumentModel::GeomObj>();
    std::cout << " face association : " << aFace.name.toStdString() << std::endl;
    std::cout << " entry : " << aFace.entry.toStdString() << std::endl;
    std::cout << " subid : " << aFace.subid.toStdString() << std::endl;
    _documentModel->addAssociation( iQuad, aFace );
  }

  result = patternDataModel->mapFromSource(iQuad);

  return true;
}



// ------------------------- GROUP ----------------------------------
GroupDialog::GroupDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f ):
HexaBaseDialog(parent, editmode, f),
_value(NULL)
{
  _helpFileName = "gui_groups.html#add-group";
  setupUi( this );
  _initWidget(editmode);
//   setFocusProxy( name_le/*eltBase_lw */);

  if ( editmode  == NEW_MODE ){
    setWindowTitle( tr("Group Construction") );
  } else if ( editmode == UPDATE_MODE ){
    setWindowTitle( tr("Group Modification") );
  } else if ( editmode == INFO_MODE ){
    setWindowTitle( tr("Group Information") );
  }
}


GroupDialog::~GroupDialog()
{
}


void GroupDialog::_initInputWidget( Mode editmode )
{
  setProperty( "HexaWidgetType",  QVariant::fromValue(GROUP_TREE) );
  installEventFilter(this);

  name_le->setProperty( "HexaWidgetType",  QVariant::fromValue(GROUP_TREE) );
  name_le->installEventFilter(this);

  // kind checkbox
  strKind[ HEXA_NS::HexaCell ] = "HexaCell";
  strKind[ HEXA_NS::QuadCell ] = "QuadCell";
  strKind[ HEXA_NS::EdgeCell ] = "EdgeCell";
  strKind[ HEXA_NS::HexaNode ] = "HexaNode";
  strKind[ HEXA_NS::QuadNode ] = "QuadNode";
  strKind[ HEXA_NS::EdgeNode ] = "EdgeNode";
  strKind[ HEXA_NS::VertexNode ] = "VertexNode";
  kind_cb->clear();
  QMap<HEXA_NS::EnumGroup, QString>::ConstIterator iKind;
  for( iKind = strKind.constBegin(); iKind != strKind.constEnd(); ++iKind )
      kind_cb->addItem( iKind.value(), QVariant(iKind.key()) );

  // list widget
//   eltBase_lw->setProperty( "HexaWidgetType",  QVariant::fromValue() );
  onKindChanged( kind_cb->currentIndex() );
  eltBase_lw->installEventFilter(this);

  if ( editmode != INFO_MODE ){
    QShortcut* delEltShortcut = new QShortcut( QKeySequence(Qt::Key_X), eltBase_lw );
    delEltShortcut->setContext( Qt::WidgetShortcut );
    connect(delEltShortcut,   SIGNAL(activated()), this, SLOT(removeEltBase()));
    connect(kind_cb,  SIGNAL(activated(int)), this, SLOT(onKindChanged(int)) );
  }

  connect(eltBase_lw,    SIGNAL(itemSelectionChanged()), this, SLOT(selectElementOfModel()));
}


void GroupDialog::clear()
{
  name_le->clear();
  eltBase_lw->clear();
}


void GroupDialog::onKindChanged(int index)
{
  //   onKind
  MESSAGE("GroupDialog::onKindChanged(" << index << ") ");
  switch ( kind_cb->itemData(index).toInt() ){
  case HEXA_NS::HexaCell: case HEXA_NS::HexaNode: eltBase_lw->setProperty("HexaWidgetType", QVariant::fromValue(HEXA_TREE)); MESSAGE("====>HEXA_TREE"); break;
  case HEXA_NS::QuadCell: case HEXA_NS::QuadNode: eltBase_lw->setProperty("HexaWidgetType", QVariant::fromValue(QUAD_TREE)); MESSAGE("====>QUAD_TREE"); break;
  case HEXA_NS::EdgeCell: case HEXA_NS::EdgeNode: eltBase_lw->setProperty("HexaWidgetType", QVariant::fromValue(EDGE_TREE)); MESSAGE("====>EDGE_TREE"); break;
  case HEXA_NS::VertexNode: eltBase_lw->setProperty("HexaWidgetType", QVariant::fromValue(VERTEX_TREE)); MESSAGE("====>VERTEX_TREE"); break;
  default:Q_ASSERT( false );
  }
  eltBase_lw->clear();
  eltBase_lw->setFocus();
}


void GroupDialog::setValue(HEXA_NS::Group* g)
{
  //0) name
  name_le->setText( g->getName() );

  //1) kind
  kind_cb->clear();
  kind_cb->addItem ( strKind[g->getKind()], QVariant( g->getKind() ) );
//   onKindChanged( kind_cb->currentIndex() );

  //2) elts
  HEXA_NS::EltBase* eltBase = NULL;
  QListWidgetItem* item = NULL;
  QModelIndex iEltBase;
  QList<QStandardItem *> eltBaseItems;
  QVariant v;

  if ( !_patternDataSelectionModel ) return;
  if ( !_groupsSelectionModel ) return;

  QModelIndex iGroup = _groupsSelectionModel->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(g) );
  name_le->setProperty( "QModelIndex",  QVariant::fromValue(iGroup) );


  for ( int nr = 0; nr < g->countElement(); ++nr ){
    eltBase = g->getElement( nr );
    switch ( g->getKind() ){
      case HEXA_NS::HexaCell: case HEXA_NS::HexaNode: v = QVariant::fromValue( (HEXA_NS::Hexa *)eltBase ); break;
      case HEXA_NS::QuadCell: case HEXA_NS::QuadNode: v = QVariant::fromValue( (HEXA_NS::Quad *)eltBase ); break;
      case HEXA_NS::EdgeCell: case HEXA_NS::EdgeNode: v = QVariant::fromValue( (HEXA_NS::Edge *)eltBase ); break;
      case HEXA_NS::VertexNode:                       v = QVariant::fromValue( (HEXA_NS::Vertex *)eltBase ); break;
    }
    iEltBase  = _patternDataSelectionModel->indexBy( HEXA_DATA_ROLE, v);
    if ( iEltBase.isValid() ){
      item = new QListWidgetItem( eltBase->getName() );
      item->setData(  LW_QMODELINDEX_ROLE, QVariant::fromValue<QModelIndex>(iEltBase) );
      eltBase_lw->addItem( item );
    }
  }
//   int r = eltBase_lw->count()-1;
//   eltBase_lw->setCurrentRow(r);

  _value = g;
}

HEXA_NS::Group* GroupDialog::getValue()
{
  return _value;
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


bool GroupDialog::apply(QModelIndex& result)
{
  SUIT_OverrideCursor wc;
  if ( !_documentModel ) return false;
//                           HexaCell, QuadCell, EdgeCell,
//                           HexaNode, QuadNode, EdgeNode, VertexNode
  if ( !_patternDataSelectionModel ) return false;
  if ( !_groupsSelectionModel )      return false;
  const PatternDataModel* patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  const GroupsModel*      groupsModel = dynamic_cast<const GroupsModel*>( _groupsSelectionModel->model() );
  _currentObj = NULL;

  QString               grpName = name_le->text();
  DocumentModel::Group  grpKind = static_cast<DocumentModel::Group>( kind_cb->itemData( kind_cb->currentIndex() ).toInt());
  QModelIndex iGrp;
  if ( _value == NULL ){ // create group
    iGrp = _documentModel->addGroup( grpName, grpKind );
  } else {
    QModelIndexList iGrps = _documentModel->match(
          _documentModel->index(0, 0),
          HEXA_DATA_ROLE,
          QVariant::fromValue( _value ),
          1,
          Qt::MatchRecursive );
    if ( !iGrps.isEmpty() )
      iGrp = iGrps[0];
  }

//   kind_cb->
  if ( !iGrp.isValid() ){ 
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT ADD GROUP" ) );
    return false;
  }

//fill it and select it
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

  QString newName = name_le->text();
  if ( !newName.isEmpty() )/*{*/
    _documentModel->setName( iGrp, newName );
//     name_le->setProperty("QModelIndex", QVariant::fromValue( groupsModel->mapFromSource(iGrp) ));
//   } else {
//     name_le->setText( _value->getName() );
//   }

  result = groupsModel->mapFromSource(iGrp);

  return true;
}



// ------------------------- LAW ----------------------------------
LawDialog::LawDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f )
: HexaBaseDialog(parent, editmode, f),
  _value(NULL)
{
  _helpFileName = "gui_discret_law.html#add-law";
  setupUi( this );
  _initWidget(editmode);
  if ( editmode  == NEW_MODE ){
    setWindowTitle( tr("Law Construction") );
  } else if ( editmode == UPDATE_MODE ){
    setWindowTitle( tr("Law Modification") );
  }
}

LawDialog::~LawDialog()
{
}

void LawDialog::_initInputWidget( Mode editmode )
{
  QRegExp rx("");
  QValidator *validator = new QRegExpValidator(rx, this);

  setProperty( "HexaWidgetType",  QVariant::fromValue(LAW_TREE) );
  installEventFilter(this);

  name_le->setProperty( "HexaWidgetType",  QVariant::fromValue(LAW_TREE) );
  name_le->installEventFilter(this);

  // kind checkbox
  strKind[ HEXA_NS::Uniform ]    = "Uniform";
  strKind[ HEXA_NS::Arithmetic ] = "Arithmetic";
  strKind[ HEXA_NS::Geometric ]  = "Geometric";

  kind_cb->clear();
  QMap<HEXA_NS::KindLaw, QString>::ConstIterator iKind;
  for( iKind = strKind.constBegin(); iKind != strKind.constEnd(); ++iKind )
      kind_cb->addItem( iKind.value(), QVariant(iKind.key()) );

}

void LawDialog::clear()
{
  name_le->clear();
}

void LawDialog::setValue(HEXA_NS::Law* l)
{
  // 0) name
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




bool LawDialog::apply(QModelIndex& result)
{
  SUIT_OverrideCursor wc;
  if ( !_documentModel ) return false;
  if ( !_meshSelectionModel ) return false;

  _currentObj = NULL;
  const MeshModel* meshModel = dynamic_cast<const MeshModel*>( _meshSelectionModel->model() );

  QString lawName = name_le->text();
  int     nbnodes = nb_nodes_spb->value();
  double  coeff   = coeff_spb->value();
  DocumentModel::KindLaw  lawKind = static_cast<DocumentModel::KindLaw>( kind_cb->itemData( kind_cb->currentIndex() ).toInt());


  QModelIndex iLaw;
  if ( _value == NULL ){ // create Law
    iLaw = _documentModel->addLaw( lawName, nbnodes );
  } else {
    QModelIndexList iLaws = _documentModel->match(
          _documentModel->index(0, 0),
          HEXA_DATA_ROLE,
          QVariant::fromValue( _value ),
          1,
          Qt::MatchRecursive );
    if ( !iLaws.isEmpty() )
      iLaw = iLaws[0];
  }

  if ( !iLaw.isValid() ){ 
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT ADD LAW" ) );
    return false;
  } 
  //fill it and select it
  bool setOk = _documentModel->setLaw( iLaw, nbnodes, coeff, lawKind );
  if ( !setOk ){ 
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT ADD LAW" ) );
    return false;
  }

  QString newName = name_le->text();
  if ( !newName.isEmpty() )/*{*/
    _documentModel->setName( iLaw, newName );
//     name_le->setProperty("QModelIndex", QVariant::fromValue( meshModel->mapFromSource(iLaw) ));
//   } else {
//     name_le->setText( _value->getName() );
//   }

  result = meshModel->mapFromSource(iLaw);

  return true;
}




// ------------------------- PROPAGATION ----------------------------------
PropagationDialog::PropagationDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f )
: HexaBaseDialog(parent, editmode, f),
  _value(NULL)
{
  _helpFileName = "gui_propag.html";
  setupUi( this );
  _initWidget(editmode);
//   setFocusProxy( law_le );

  if ( editmode == INFO_MODE ){
    setWindowTitle( tr("Propagation Information") );
  } else if ( editmode == UPDATE_MODE ){
    setWindowTitle( tr("Propagation Modification") );
  } else if ( editmode == NEW_MODE ){
    setWindowTitle( tr("Propagation Setting") );
  }

}


PropagationDialog::~PropagationDialog()
{
}


void PropagationDialog::_initInputWidget( Mode editmode )
{
  QRegExp rx("");
  QValidator *validator = new QRegExpValidator(rx, this);

  setProperty( "HexaWidgetType",  QVariant::fromValue(PROPAGATION_TREE) );
  installEventFilter(this);

  law_le->setProperty( "HexaWidgetType",  QVariant::fromValue(LAW_TREE) );
  law_le->installEventFilter(this);
  law_le->setValidator( validator );
}

void PropagationDialog::clear()
{
  law_le->clear();
}

void PropagationDialog::setValue(HEXA_NS::Propagation* p)
{
  HEXA_NS::Law* l = p->getLaw();
  bool way = p->getWay();

  // propagation
  QModelIndex ip = _meshSelectionModel->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(p) );
  setProperty( "QModelIndex",  QVariant::fromValue<QModelIndex>(ip) );


  // law on propagation
  if ( l != NULL ){
    law_le->setText( l->getName() );
    QModelIndex il = _meshSelectionModel->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(l) );
    law_le->setProperty( "QModelIndex",  QVariant::fromValue<QModelIndex>(il) );
  }

  // way of propagation
  way_cb->setChecked(way);

  _value = p;
}


HEXA_NS::Propagation* PropagationDialog::getValue()
{
  return _value;
}


bool PropagationDialog::apply(QModelIndex& result)
{
  bool isOk = false;
  SUIT_OverrideCursor wc;
  if ( !_documentModel ) return false;
  if ( !_meshSelectionModel ) return false;
  _currentObj = NULL;

  const MeshModel* meshModel = dynamic_cast<const MeshModel*>( _meshSelectionModel->model() );

  QModelIndex iPropagation;
  QModelIndex iLaw;
  bool way = way_cb->isChecked();

  QVariant vPropagation = property("QModelIndex");
  QVariant vLaw         = law_le->property("QModelIndex");
  if ( vPropagation.isValid() ){
    iPropagation = meshModel->mapToSource( vPropagation.value<QModelIndex>() );
  }
  if ( vLaw.isValid() ){
    iLaw = meshModel->mapToSource( vLaw.value<QModelIndex>() );
  }
  MESSAGE("vPropagation.isValid() => " << vPropagation.isValid() );
  MESSAGE("vLaw.isValid()         => " << vLaw.isValid() );
  MESSAGE("iPropagation.isValid() => " << iPropagation.isValid() );
  MESSAGE("iLaw.isValid()         => " << iLaw.isValid() );

  if ( !iPropagation.isValid() || !iLaw.isValid() ){ 
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT SET PROPAGATION" ) );
    return false;
  }

  //fill it and select it
  isOk = _documentModel->setPropagation( iPropagation, iLaw, way );
  if ( !isOk ){
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT SET PROPAGATION" ) );
    return false;
  }

  result = meshModel->mapFromSource(iPropagation);

  return true;
}



// ------------------------- COMPUTE MESH ----------------------------------
ComputeMeshDialog::ComputeMeshDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f ): 
HexaBaseDialog(parent, editmode, f)
{
    _helpFileName = "gui_mesh.html";
    setWindowTitle( tr("Compute mesh") );
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

    _name = new QLineEdit("Mesh");
    _dim  = new QSpinBox();
    _fact = new QLineEdit("FactoryServer");

    vld->addWidget(_name);
    vld->addWidget(_dim);
    vld->addWidget(_fact);

    _dim->setRange(1, 3);
    _dim->setValue(3);

    _initWidget(editmode);

//     _initButtonBox( editmode );

//     QPushButton* ok     = new QPushButton("OK");
//     QPushButton* cancel = new QPushButton("Cancel");
//     QPushButton* help   = new QPushButton("Help");
// 
//     down->addWidget(ok);
//     down->addWidget(cancel);
//     down->addWidget(help);
// 
//     connect(ok    , SIGNAL( clicked() ), this, SLOT( accept() ));
//     connect(cancel, SIGNAL( clicked() ), this, SLOT( reject() ));
}

ComputeMeshDialog::~ComputeMeshDialog() {
}

void ComputeMeshDialog::_initInputWidget( Mode editmode )
{
}

void ComputeMeshDialog::setDocumentModel(DocumentModel* m)
{
  HexaBaseDialog::setDocumentModel(m);
  _name->setText(_documentModel->getName());
}

void ComputeMeshDialog::clear()
{
}

bool ComputeMeshDialog::apply(QModelIndex& result)
{
  SUIT_OverrideCursor wc;

  QString command = QString("import hexablock ; %1 = hexablock.mesh(\"%2\", \"%1\", %3, \"%4\")")
      .arg( _name->text() )
      .arg( _documentModel->documentEntry() )
      .arg( _dim->value() )
      .arg( _fact->text() );
  std::cout << "command: " << command.toStdString() << std::endl;

  SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
  PyConsole_Console* pyConsole = app->pythonConsole();

  if ( pyConsole ) 
    pyConsole->exec( command );
  else
    return false;

//   clear();
  return true;
}

// void  ComputeMeshDialog::accept() {
//     _disallowSelection();
//     QDialog::accept();
// 
//     QString command = QString("import hexablock ; %1 = hexablock.mesh(\"%1\", \"%2\", %3, \"%4\")")
//       .arg( _name->text() )
//       .arg( _documentModel->documentEntry() )
//       .arg( _dim->value() )
//       .arg( _fact->text() );
//     std::cout << "command: " << command.toStdString() << std::endl;
// 
//     SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
//     PyConsole_Console* pyConsole = app->pythonConsole();
// 
//     if ( pyConsole ) pyConsole->exec( command );
// }

// void ComputeMeshDialog::reject() {
//     _disallowSelection();
//     QDialog::reject();
// }






ReplaceHexaDialog::ReplaceHexaDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f ): 
HexaBaseDialog(parent, editmode, f)
{
  _helpFileName = "gui_replace_hexa.html";
  setupUi( this );
  _initWidget(editmode);
  setFocusProxy( quads_lw );
}

ReplaceHexaDialog::~ReplaceHexaDialog()
{
}

void ReplaceHexaDialog::_initInputWidget( Mode editmode )
{
  QRegExp rx("");
  QValidator *validator = new QRegExpValidator(rx, this);

  setProperty( "HexaWidgetType",  QVariant::fromValue(ELEMENTS_TREE) );
  installEventFilter(this);

  c1_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  c1_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  c1_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );

  p1_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  p2_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  p3_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );

  c1_le->installEventFilter(this);
  c2_le->installEventFilter(this);
  c3_le->installEventFilter(this);

  p1_le->installEventFilter(this);
  p2_le->installEventFilter(this);
  p3_le->installEventFilter(this);

  quads_lw->setProperty( "HexaWidgetType",  QVariant::fromValue(QUAD_TREE) );
  quads_lw->installEventFilter(this);

  if ( editmode == NEW_MODE ){
    QShortcut* delQuadShortcut = new QShortcut( QKeySequence(Qt::Key_X), quads_lw );
    delQuadShortcut->setContext( Qt::WidgetShortcut );

    connect( delQuadShortcut, SIGNAL(activated()), this, SLOT(deleteQuadItem()) );
    connect( quads_lw, SIGNAL(currentRowChanged(int)), this, SLOT(updateButtonBox(int)) );
  }

  connect(quads_lw,    SIGNAL(itemSelectionChanged()), this, SLOT(selectElementOfModel()));
}

void ReplaceHexaDialog::clear()
{
}


void ReplaceHexaDialog::updateButtonBox()
{
  int nbQuad = quads_lw->count();
  std::cout << "K" << std::endl;
//   std::cout << "nbHexa => " << nbHexa << std::endl;
  if ( nbQuad > 0 ){
    std::cout << "L" << std::endl;
    _applyCloseButton->setEnabled(true);
    _applyButton->setEnabled(true);
  } else {
    std::cout << "M" << std::endl;
    _applyCloseButton->setEnabled(false);
    _applyButton->setEnabled(false);
  }
}


void ReplaceHexaDialog::deleteQuadItem()
{
  delete quads_lw->currentItem();
  updateButtonBox();
}


bool ReplaceHexaDialog::apply(QModelIndex& result)
{
  SUIT_OverrideCursor wc;
  if ( !_documentModel ) return false;
  if ( !_patternDataSelectionModel ) return false;
  if ( !_patternBuilderSelectionModel ) return false;
  const PatternDataModel* patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  if ( !patternDataModel )    return false;
  if ( !patternBuilderModel ) return false;
  _currentObj = NULL;

  QModelIndex ielts; //result

  QListWidgetItem* item = NULL;
  QModelIndexList iquads;
  QModelIndex     iquad;
  for ( int r = 0; r < quads_lw->count(); ++r){
    item = quads_lw->item(r);
    iquad = patternDataModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() );
//     std::cout << "iquad => " << iquad.data().toString().toStdString() << std::endl;
    if ( iquad.isValid() )
      iquads << iquad;
  }

  QModelIndex ic1 = patternDataModel->mapToSource( _index[c1_le] );
  QModelIndex ic2 = patternDataModel->mapToSource( _index[c2_le] );
  QModelIndex ic3 = patternDataModel->mapToSource( _index[c3_le] );

  QModelIndex ip1 = patternDataModel->mapToSource( _index[p1_le] );
  QModelIndex ip2 = patternDataModel->mapToSource( _index[p2_le] );
  QModelIndex ip3 = patternDataModel->mapToSource( _index[p3_le] );

//   std::cout << "nbHexa => " << nbHexa << std::endl;

  if ( ic1.isValid() && ic2.isValid() && ic3.isValid()
       && ip1.isValid() && ip2.isValid() && ip3.isValid() ){
    std::cout << "ip1 => " << ip1.data().toString().toStdString() << std::endl;
    std::cout << "ic1 => " << ic1.data().toString().toStdString() << std::endl;
    std::cout << "ip2 => " << ip2.data().toString().toStdString() << std::endl;
    std::cout << "ic2 => " << ic2.data().toString().toStdString() << std::endl;
    std::cout << "ip3 => " << ip3.data().toString().toStdString() << std::endl;
    std::cout << "ic3 => " << ic3.data().toString().toStdString() << std::endl;

    ielts = _documentModel->replace( iquads,
                                     ip1, ic1,
                                     ip2, ic2,
                                     ip3, ic3 );
  }

  if ( !ielts.isValid() ){
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT REPLACE HEXA" ) );
    return false;
  }
  _value  = ielts.model()->data(ielts, HEXA_DATA_ROLE).value<HEXA_NS::Elements*>();

  QString newName = name_le->text();
  if ( !newName.isEmpty() )/*{*/
    _documentModel->setName( ielts, newName );
//     name_le->setProperty("QModelIndex", QVariant::fromValue( patternBuilderModel->mapFromSource(ielts) ));
//   } else {
//     name_le->setText( _value->getName() );
//   }

  result = patternBuilderModel->mapFromSource(ielts);

  return true;
}





QuadRevolutionDialog::QuadRevolutionDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f ): 
HexaBaseDialog(parent, editmode, f)
{
  _helpFileName = "gui_quad_revolution.html";
  setupUi( this );
  _initWidget(editmode);
  setFocusProxy( quads_lw );
}

QuadRevolutionDialog::~QuadRevolutionDialog()
{
}


void QuadRevolutionDialog::_initInputWidget( Mode editmode )
{
  QRegExp rx("");
  QValidator *validator = new QRegExpValidator(rx, this);

  setProperty( "HexaWidgetType",  QVariant::fromValue(ELEMENTS_TREE) );
  installEventFilter(this);

  center_pt_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  axis_vec_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );
  quads_lw->setProperty( "HexaWidgetType",  QVariant::fromValue(QUAD_TREE) );

  center_pt_le->setValidator( validator );
  axis_vec_le->setValidator( validator );
//   quads_lw->setValidator( validator );

  center_pt_le->installEventFilter(this);
  axis_vec_le->installEventFilter(this);
  quads_lw->installEventFilter(this);

  if ( editmode == NEW_MODE ){
    angles_lw->setItemDelegate( new HexaDoubleSpinBoxDelegate(angles_lw) );
    angles_lw->setEditTriggers( QAbstractItemView::DoubleClicked );

    QShortcut* delQuadShortcut = new QShortcut( QKeySequence(Qt::Key_X), quads_lw );
    delQuadShortcut->setContext( Qt::WidgetShortcut );
    connect( delQuadShortcut, SIGNAL(activated()), this, SLOT(delQuadItem()) );
    connect( add_angle_pb, SIGNAL(clicked()), this, SLOT(addAngleItem()));
    connect( del_angle_pb, SIGNAL(clicked()), this, SLOT(delAngleItem()));
//     connect(clear_pb, SIGNAL(clicked()), this, SLOT(clearQuads()));
  }

  connect(quads_lw,    SIGNAL(itemSelectionChanged()), this, SLOT(selectElementOfModel()));
}

void QuadRevolutionDialog::clear()
{
}



void QuadRevolutionDialog::updateButtonBox() //CS_TODO? : check center, axis
{
  int nbQuads  = quads_lw->count();
  int nbAngles = angles_lw->count();

  if ( nbQuads>0 && nbAngles> 0 ){
    _applyCloseButton->setEnabled(true);
    _applyButton->setEnabled(true);
  } else {
    _applyCloseButton->setEnabled(false);
    _applyButton->setEnabled(false);
  }
}



void QuadRevolutionDialog::addAngleItem() //CS_TODO
{
  QListWidgetItem* previousItem = angles_lw->currentItem();
  QListWidgetItem* newItem      = new QListWidgetItem();

  double defaultValue = 0.;
  if ( previousItem )
    defaultValue =  previousItem->data(Qt::EditRole).toDouble();

  newItem->setData(  Qt::EditRole, QVariant(defaultValue) );
  newItem->setFlags( newItem->flags () | Qt::ItemIsEditable);
  angles_lw->addItem(newItem);
 
  updateButtonBox();
}

void QuadRevolutionDialog::delAngleItem()
{
  delete angles_lw->currentItem();
  updateButtonBox();
}

void QuadRevolutionDialog::delQuadItem()
{
  delete quads_lw->currentItem();
  updateButtonBox();
}


bool QuadRevolutionDialog::apply(QModelIndex& result)
{
  SUIT_OverrideCursor wc;
  if ( !_documentModel ) return false;
  if ( !_patternDataSelectionModel )    return false;
  if ( !_patternBuilderSelectionModel ) return false;
  const PatternDataModel* patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  if ( !patternDataModel )    return false;
  if ( !patternBuilderModel ) return false;
  _currentObj = NULL;

  QModelIndex ielts; //result
  QListWidgetItem* item = NULL;

  QModelIndexList istartquads;
  QModelIndex     iquad;
  for ( int r = 0; r < quads_lw->count(); ++r){
    item = quads_lw->item(r);
    iquad = patternDataModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() );
    if ( iquad.isValid() )
      istartquads << iquad;
  }

  QModelIndex icenter_pt = patternDataModel->mapToSource( _index[center_pt_le] );
  QModelIndex iaxis_vec  = patternBuilderModel->mapToSource( _index[axis_vec_le] );

  QList<double> angles;
  for ( int r = 0; r < angles_lw->count(); ++r){
    item = angles_lw->item(r);
    std::cout << "angle : " << item->data(Qt::EditRole).toDouble()<< std::endl;
    angles << item->data(Qt::EditRole).toDouble();
  }

  if ( icenter_pt.isValid() && iaxis_vec.isValid() ){
    ielts = _documentModel->revolutionQuads( istartquads, icenter_pt, iaxis_vec, angles );
  }

  if ( !ielts.isValid() ){
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE QUAD REVOLUTION" ) );
    return false;
  }
  _value  = ielts.model()->data(ielts, HEXA_DATA_ROLE).value<HEXA_NS::Elements*>();

  QString newName = name_le->text();
  if ( !newName.isEmpty() )/*{*/
    _documentModel->setName( ielts, newName );
//     name_le->setProperty("QModelIndex", QVariant::fromValue( patternBuilderModel->mapFromSource(ielts) ));
//   } else {
//     name_le->setText( _value->getName() );
//   }

  result = patternBuilderModel->mapFromSource(ielts);

  return true;
}




MakeHemiSphereDialog::MakeHemiSphereDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f ):
HexaBaseDialog(parent, editmode, f)
{
  _helpFileName = "gui_hemisphere.html";
  setupUi( this );
  _initWidget(editmode);
}


MakeHemiSphereDialog::~MakeHemiSphereDialog()
{
}

void MakeHemiSphereDialog::_initInputWidget( Mode editmode )
{
  QRegExp rx("");
  QValidator *validator = new QRegExpValidator(rx, this);

  setProperty( "HexaWidgetType",  QVariant::fromValue(ELEMENTS_TREE) );
  installEventFilter(this);

  sphere_center_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  cross_pt_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
  hole_axis_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );
  cross_vec_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );
  radial_vec_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );

  sphere_center_le->setValidator( validator );
  cross_pt_le->setValidator( validator );
  hole_axis_le->setValidator( validator );
  cross_vec_le->setValidator( validator );
  radial_vec_le->setValidator( validator );


  sphere_center_le->installEventFilter(this);
  cross_pt_le->installEventFilter(this);
  hole_axis_le->installEventFilter(this);
  cross_vec_le->installEventFilter(this);
  radial_vec_le->installEventFilter(this);
}

void MakeHemiSphereDialog::clear()
{
}

bool MakeHemiSphereDialog::apply(QModelIndex& result)
{
  SUIT_OverrideCursor wc;
  if ( !_documentModel ) return false;
  if ( !_patternBuilderSelectionModel ) return false;
  if ( !_patternDataSelectionModel )    return false;
  const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
  const PatternDataModel* patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
  if ( !patternBuilderModel ) return false;
  if ( !patternDataModel )    return false;
  _currentObj = NULL;

  QModelIndex iElts;
  QModelIndex icenter = patternDataModel->mapToSource( _index[sphere_center_le] );
  QModelIndex iplorig = patternDataModel->mapToSource( _index[cross_pt_le] );
  QModelIndex ivecx  = patternBuilderModel->mapToSource( _index[radial_vec_le] );
  QModelIndex ivecz  = patternBuilderModel->mapToSource( _index[hole_axis_le] );

  double radext  = sphere_radext_spb->value();
  double radhole = hole_rad_spb->value();

  int nrad  = ngrid_rad_spb->value();
  int nang  = ngrid_ang_spb->value();
  int nhaut = ngrid_height_spb->value();


  if ( icenter.isValid() && ivecx.isValid() && ivecz.isValid() && iplorig.isValid() ){
    if ( sphere_rind_cb->isChecked() ){ // rind
      double radint  = sphere_radint_spb->value();
      if ( radial_partial_cb->isChecked() ){
        double angle = radial_angle_spb->value();
        iElts = _documentModel->makePartRind( icenter, ivecx, ivecz,
                                        radext, radint, radhole,
                                        iplorig,      angle,
                                        nrad, nang, nhaut );
      } else {
        iElts = _documentModel->makeRind( icenter,
                                        ivecx, ivecz,
                                        radext, radint, radhole,
                                        iplorig,
                                        nrad, nang, nhaut );
      }
    } else { // sphere
      if ( radial_partial_cb->isChecked() ){
        double angle = radial_angle_spb->value();
        iElts = _documentModel->makePartSphere( icenter, ivecx, ivecz,
                                        radext, radhole,
                                        iplorig, angle,
                                        nrad, nang, nhaut );
      } else {
        iElts = _documentModel->makeSphere( icenter,
                                        ivecx, ivecz,
                                        radext, radhole,
                                        iplorig,
                                        nrad, nang, nhaut );
      }
    }
  }


  if ( !iElts.isValid() ){
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE RIND" ) );
    return false;
  }
  _value  = iElts.model()->data(iElts, HEXA_DATA_ROLE).value<HEXA_NS::Elements*>();

  QString newName = name_le->text();
  if ( !newName.isEmpty() )/*{*/
    _documentModel->setName( iElts, newName );
//     name_le->setProperty("QModelIndex", QVariant::fromValue( patternBuilderModel->mapFromSource(iElts) ));
//   } else {
//     name_le->setText( _value->getName() );
//   }

  result = patternBuilderModel->mapFromSource(iElts);

  return true;
}



// MakeRindDialog::MakeRindDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f ): 
// HexaBaseDialog(parent, editmode, f)
// {
// //   _helpFileName()
//   setupUi( this );
//   setWindowTitle( tr("MAKE RIND") );
// 
//   if ( editmode == NEW_MODE ){
//     _initButtonBox( editmode );
// 
//     //selection management
//     _vertexLineEdits << center_le << plorig_le;
//     _vectorLineEdits << vx_le << vz_le;
// 
//     center_le->installEventFilter(this);
//     plorig_le->installEventFilter(this);
//     vx_le->installEventFilter(this);
//     vz_le->installEventFilter(this);
//   }
// }
// 
// 
// MakeRindDialog::~MakeRindDialog()
// {
// }
// 
// 
// 
// bool MakeRindDialog::apply(QModelIndex& result)
// {
//   SUIT_OverrideCursor wc;
//   if ( !_documentModel ) return false;
//   if ( !_patternBuilderSelectionModel ) return false;
//   if ( !_patternDataSelectionModel )    return false;
//   const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( _patternBuilderSelectionModel->model() );
//   const PatternDataModel* patternDataModel = dynamic_cast<const PatternDataModel*>( _patternDataSelectionModel->model() );
//   if ( !patternBuilderModel ) return false;
//   if ( !patternDataModel )    return false;
// 
//   QModelIndex iElts;
//   QModelIndex icenter = patternDataModel->mapToSource( _index[center_le] );
//   QModelIndex iplorig = patternDataModel->mapToSource( _index[plorig_le] );
//   QModelIndex ivecx  = patternBuilderModel->mapToSource( _index[vx_le] );
//   QModelIndex ivecz  = patternBuilderModel->mapToSource( _index[vz_le] );
// 
//   double radext = radext_spb->value();
//   double radint = radint_spb->value();
//   double radhole = radhole_spb->value();
// 
//   int nrad = nrad_spb->value();
//   int nang = nang_spb->value();
//   int nhaut = nhaut_spb->value();
// 
//   if ( icenter.isValid() && ivecx.isValid() && ivecz.isValid() && iplorig.isValid() ){
//     if ( partial_cb->isChecked() ){
//       double angle = angle_spb->value(); // Part Rind only
//       iElts = _documentModel->makePartRind( icenter, ivecx, ivecz,
//                                         radext, radint, radhole,
//                                         iplorig,      angle,
//                                         nrad, nang, nhaut );
//     } else {
//       iElts = _documentModel->makeRind( icenter,
//                                         ivecx, ivecz,
//                                         radext, radint, radhole,
//                                         iplorig,
//                                         nrad, nang, nhaut );
//     }
//   }
// 
//   if ( !iElts.isValid() ){
//     SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE RIND" ) );
//     return false;
//   }
// 
//   QString newName = name_le->text();
//   if (!newName.isEmpty()) _documentModel->setName( iElts, newName );
// 
//   return true;
// }

// QVariant v = lineEdit->property("index");
//           if ( v.isValid() ){
//             MESSAGE("*  get index ... " );
//             QModelIndex i = v.value<QModelIndex>();
//             _selectionMutex = true;
//             _patternDataSelectionModel->select( i, QItemSelectionModel::Clear );
//             _patternDataSelectionModel->select( i, QItemSelectionModel::Select );
//             _selectionMutex = false;
//           }
