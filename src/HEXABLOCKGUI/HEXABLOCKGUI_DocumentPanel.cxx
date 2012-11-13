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
#include "HEXABLOCKGUI_VtkDocumentGraphicView.hxx"
#include "HEXABLOCKGUI_OccGraphicView.hxx"
#include "HEXABLOCKGUI_SalomeTools.hxx"
//#include "HEXABLOCKGUI.hxx"


#include <iostream>
#include <QtGui>
#include <QFlags>

#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <PyConsole_Console.h>
#include <SalomeApp_Tools.h>

#include <Standard_GUID.hxx>
#include <TDF_Label.hxx>

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
#include <SVTK_View.h>

#include <SUIT_ResourceMgr.h>


// #include <GeometryGUI.h>

#include <GEOMBase.h>
#include <GEOMImpl_Types.hxx>



#include "MyGEOMBase_Helper.hxx"
#include "GEOMBase.h"
#include "GEOM_Operation.h"

#define BUTTON_BOX_MIN_WIDTH 5
#define VERTEX_COORD_MIN -1000000
#define VERTEX_COORD_MAX  1000000
#define SPINBOX_ANGLE_MAX 360
#define SPINBOX_DOUBLE_MAX 1000000000
#define SPINBOX_POSITIVE_DOUBLE_MIN 0
#define NB_DECIMALS 6


using namespace std;
using namespace HEXABLOCK::GUI;


Q_DECLARE_METATYPE(HEXABLOCK::GUI::HexaTreeRole);
Q_DECLARE_METATYPE(TopAbs_ShapeEnum);

//General SpinBox Delegate
class HexaDoubleSpinBoxDelegate : public QStyledItemDelegate  {
public:
   HexaDoubleSpinBoxDelegate(QObject *parent=0) : QStyledItemDelegate (parent){}

   QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
         const QModelIndex &index) const{
      QDoubleSpinBox *sb = new QDoubleSpinBox( parent );
      sb->setDecimals(NB_DECIMALS);
      return sb;
   }
};

//Angle SpinBox Delegate
class HexaAngleDoubleSpinBoxDelegate : public QStyledItemDelegate  {
public:
   HexaAngleDoubleSpinBoxDelegate(QObject *parent=0) : QStyledItemDelegate (parent){}

   QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
         const QModelIndex &index) const{
      QDoubleSpinBox *sb = new QDoubleSpinBox( parent );
      sb->setMinimum(SPINBOX_POSITIVE_DOUBLE_MIN);
      sb->setMaximum(SPINBOX_ANGLE_MAX);
      sb->setDecimals(NB_DECIMALS);
      return sb;
   }
};


//Positive DoubleSpinBox Delegate (for heigth, radius, ...)
class HexaPositiveDoubleSpinBoxDelegate : public QStyledItemDelegate  {
public:
   HexaPositiveDoubleSpinBoxDelegate(QObject *parent=0) : QStyledItemDelegate (parent){}

   QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
         const QModelIndex &index) const{
      QDoubleSpinBox *sb = new QDoubleSpinBox( parent );
      sb->setMinimum(SPINBOX_POSITIVE_DOUBLE_MIN);
      /////// sb->setMaximum(1000000000000000); //10e15   Abu : Pb en 32 bits
      sb->setMaximum(SPINBOX_DOUBLE_MAX);       //10e9
      sb->setDecimals(NB_DECIMALS);
      return sb;
   }
};

// ======================================================== Constructeur
HexaBaseDialog::HexaBaseDialog( QWidget * parent, Mode editmode, Qt::WindowFlags f ):
                  QDialog(parent, f),
                  MyGEOMBase_Helper( dynamic_cast<SUIT_Desktop*>(parent->parent()) ),
                  _editMode( editmode ),
                  _currentObj(NULL),
                  _expectedSelection(-1),
                  _selectionMutex( false ),
                  // _applyCloseButton(0),
                  _applyButton(NULL),
                  debugEdgeAssoc(false)
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

// ============================================================= getIndexList
QModelIndexList HexaBaseDialog::getIndexList(QListWidget* itemsList)
{
   QModelIndexList iItems;
   QModelIndex     iItem;
   QListWidgetItem* item = NULL;

   const PatternDataModel*    patternDataModel = getPatternDataModel();
   if (patternDataModel == NULL) return iItems;

   unsigned int nbItems = itemsList->count();
   for ( int r = 0; r < nbItems; ++r){
         item = itemsList->item(r);
         iItem = patternDataModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() );
         iItems << iItem;
   }
   return iItems;
}


// ============================================================= lockSizeToSizeHint
void HexaBaseDialog::lockSizeToSizeHint()
{
   setMaximumHeight(sizeHint().height());
//   setMaximumWidth(sizeHint().width());
   //	setMinimumHeight(sizeHint().height());
   //	setMinimumWidth(sizeHint().width());
}

// ============================================================= unlockSizeModification
void HexaBaseDialog::unlockSizeModification()
{
   setMaximumHeight(MAX_HEIGHT);
//   setMaximumWidth(MAX_WIDTH);
   //	setMinimumHeight(MIN_HEIGHT);
   //	setMinimumWidth(MIN_WIDTH);
}

// ============================================================= resetSizeAndShow
void HexaBaseDialog::resetSizeAndShow(QDockWidget* parent)
{
   //force the dialog to fit its contain
   lockSizeToSizeHint();

   //set the dialog in the dockwidget
   parent->setWidget(this);
   parent->setWindowTitle(windowTitle());
   parent->setVisible(true);
   show();

   unlockSizeModification();
}

//unlink the widget from the model
void HexaBaseDialog::modelUnregister(QWidget* widget)
{
    if (widget == NULL) return;

    //Update the line edit
    QVariant invalid;
    widget->setProperty("QModelIndex",  invalid );

    QModelIndex invalidIndex;
    _index[widget] = invalidIndex;
}

// ============================================================= connectDocumentGraphicView
void HexaBaseDialog::connectDocumentGraphicView(VtkDocumentGraphicView* docGView)
{
    if (docGView == NULL) docGView = HEXABLOCKGUI::currentDocGView;
    if (docGView == NULL) return;

    disconnectDocumentGraphicView(docGView); //to avoid double connect

    //Connect the graphic view and its model to the dialog box
    connect( docGView->getPatternDataSelectionModel(), SIGNAL( selectionChanged ( const QItemSelection &, const QItemSelection &) ),
            this, SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ), Qt::UniqueConnection );
    connect( docGView->getPatternBuilderSelectionModel(), SIGNAL( selectionChanged( const QItemSelection &, const QItemSelection &) ),
            this, SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ), Qt::UniqueConnection );
    connect( docGView->getGroupsSelectionModel(), SIGNAL( selectionChanged( const QItemSelection &, const QItemSelection &) ),
            this, SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ), Qt::UniqueConnection );
    connect( docGView->getMeshSelectionModel(), SIGNAL( selectionChanged( const QItemSelection &, const QItemSelection &) ),
            this, SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ), Qt::UniqueConnection );
}

// ============================================================= disconnectDocumentGraphicView
void HexaBaseDialog::disconnectDocumentGraphicView(VtkDocumentGraphicView* docGView)
{
   if (docGView == NULL) docGView = HEXABLOCKGUI::currentDocGView;
   if (docGView == NULL) return;


   //Disconnect graphic view signals form the dialog box
   disconnect(docGView->getPatternDataSelectionModel(), SIGNAL( selectionChanged ( const QItemSelection &, const QItemSelection &) ),
         this, SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );

   disconnect(docGView->getPatternBuilderSelectionModel(), SIGNAL( selectionChanged( const QItemSelection &, const QItemSelection &) ),
         this, SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );

   disconnect( docGView->getGroupsSelectionModel(), SIGNAL( selectionChanged( const QItemSelection &, const QItemSelection &) ),
         this, SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );

   disconnect( docGView->getMeshSelectionModel(), SIGNAL( selectionChanged( const QItemSelection &, const QItemSelection &) ),
         this, SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
}


// ============================================================= onCurrentSelectionChanged
void HexaBaseDialog::onCurrentSelectionChanged()
{
   highlightSelectedAssocs();
}

// ============================================================= setFocusToNextField
//Sets focus to the next field of the current object
void HexaBaseDialog::setFocusToNextField()
{
   activateWindow ();
   _highlightWidget(_currentObj, Qt::white);
   if (!debugEdgeAssoc) focusNextChild ();
}

// ============================================================== _initButtonBox
QGroupBox* HexaBaseDialog::_initButtonBox( Mode editmode )
{
   if ( editmode == INFO_MODE )
      return NULL;

   //QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
   QGroupBox* buttonBox = new QGroupBox();
   buttonBox->setMinimumWidth(BUTTON_BOX_MIN_WIDTH);
   buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
   //buttonBox->setOrientation(Qt::Horizontal);
   QHBoxLayout* buttonsHLayout = new QHBoxLayout();
   _applyButton      = new QPushButton(tr("Apply"));
   QPushButton* closeButton = new QPushButton(tr("Close"));
   QPushButton* helpButton  = new QPushButton(tr("Help"));
   //_applyCloseButton = new QPushButton(tr("Apply And Close"));

   //connect( buttonBox, SIGNAL(clicked()), this, SLOT(applyAndClose()) );
   connect( _applyButton, SIGNAL(clicked()), this, SLOT(apply()), Qt::UniqueConnection );
   connect( closeButton, SIGNAL(clicked()), this, SLOT(close()), Qt::UniqueConnection );
   connect( helpButton, SIGNAL(clicked()), this, SLOT(onHelpRequested()), Qt::UniqueConnection );

   buttonsHLayout->addWidget( _applyButton );
   buttonsHLayout->addWidget( closeButton );
   buttonsHLayout->addStretch(1);
   buttonsHLayout->addWidget( helpButton );
   //buttonsHLayout->addButton( _applyCloseButton );
   buttonBox->setLayout(buttonsHLayout);
   layout()->addWidget(buttonBox);
   buttonBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
   return buttonBox;
}

// ============================================================== _initWidget
void HexaBaseDialog::_initWidget( Mode editmode )
{
   _initInputWidget( editmode );
   _initButtonBox( editmode );
}

// ============================================================== _initViewManager
void HexaBaseDialog::_initViewManager()
{
//   SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
//   if (anApp == NULL) return;
//   _mgr   = dynamic_cast<LightApp_SelectionMgr*>( anApp->selectionMgr() );
//   _occVm = anApp->getViewManager( OCCViewer_Viewer::Type(), true );
//   _vtkVm = anApp->getViewManager( SVTK_Viewer::Type(),      true );
//   SUIT_ViewManager* activeVm = anApp->activeViewManager();
//   onWindowActivated ( activeVm );
}

// ============================================================== apply
bool HexaBaseDialog::apply()
{
   MESSAGE("HexaBaseDialog::apply()");
   QModelIndex iNew;
   bool applied = apply(iNew);
   if ( applied ){
         // clear all selection
         if (getPatternDataSelectionModel() != NULL)    getPatternDataSelectionModel()->clearSelection();
         if (getPatternBuilderSelectionModel() != NULL) getPatternBuilderSelectionModel()->clearSelection();
         if (getGroupsSelectionModel() != NULL)         getGroupsSelectionModel()->clearSelection();
         if (getMeshSelectionModel() != NULL)           getMeshSelectionModel()->clearSelection();
         // select and highlight in vtk view the result
         _selectAndHighlight( iNew );
         // reinitialization
         //		_currentObj = NULL;
   }
   return applied;
}

// ============================================================== accept
//void HexaBaseDialog::accept()
//{
//  bool applied = apply();
//  if ( applied ){
//    QDialog::accept();
//    _disallowSelection();
//  }
//}

void HexaBaseDialog::clearVTKSelection()
{
    HEXABLOCKGUI::currentDocGView->clearSelection();
//   _disallowSelection();
}

void HexaBaseDialog::clearOCCSelection()
{
    HEXABLOCKGUI::currentOccGView->clearSelection();
}

// ============================================================== close
void HexaBaseDialog::close()
{
    MESSAGE("HexaBaseDialog::close()");
   //	_currentObj = NULL;

   //Clear vtk selection
    clearVTKSelection();

    //reset the data selection pattern (forget all selections of the current context)
    if (getPatternDataSelectionModel() != NULL)
        getPatternDataSelectionModel()->reset();

    //Clear occ selection
    clearOCCSelection();

   //Close the dialog box
   if (parentWidget()) parentWidget()->close();
}

// ============================================================== onHelpRequested
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

// ============================================================== highlightSelectedAssocs
void HexaBaseDialog::highlightSelectedAssocs()
{
   if (getPatternDataSelectionModel() == NULL ||
         HEXABLOCKGUI::selectionMgr() == NULL ||
         HEXABLOCKGUI::currentDocGView->getViewWindow() == NULL) return;

   QMultiMap<QString, int> geomAssocs = getAssocsGEOM();
   QModelIndexList vtkAssocs = getAssocsVTK();

   //highlight geom selected elts
   if (geomAssocs.size() > 0)
      {
         HEXABLOCKGUI::selectionMgr()->clearSelected();
         HEXABLOCKGUI::currentOccGView->highlight(geomAssocs);
      }

   //highlight vtk selected elts
   if (!vtkAssocs.isEmpty())
      HEXABLOCKGUI::currentDocGView->highlight(vtkAssocs);
}

// ============================================================== refreshHighlight
void HexaBaseDialog::refreshHighlight()
{
   if (HEXABLOCKGUI::selectionMgr() != NULL)
      HEXABLOCKGUI::selectionMgr()->clearSelected();
   highlightSelectedAssocs();
}

// ============================================================== getObjectViewType
HEXABLOCKGUI::ViewType HexaBaseDialog::getObjectViewType(QObject* obj)
{
   if (obj == NULL) return HEXABLOCKGUI::UNKNOWN;
   QVariant v  = obj->property("HexaWidgetType");
   if ( v.isValid() ) return HEXABLOCKGUI::VTK;
   v = obj->property("GeomWidgetType");
   if ( v.isValid() ) return HEXABLOCKGUI::OCC;
   return HEXABLOCKGUI::UNKNOWN;
}

// ============================================================== _selectAndHighlight
void HexaBaseDialog::_selectAndHighlight( const QModelIndex& i )
{
   if ( !i.isValid() ) return;
   setProperty("QModelIndex",  QVariant::fromValue(i));
   setFocus();
}

// ============================================================== _allowSelection
void HexaBaseDialog::_allowSelection()
{
   MESSAGE("HexaBaseDialog::_allowSelection(){");
   if ( getDocumentModel() ){
         getDocumentModel()->disallowEdition();
   }
   MESSAGE("}");
}

// ============================================================== _disallowSelection
void HexaBaseDialog::_disallowSelection()
{
   MESSAGE("HexaBaseDialog::_disallowSelection(){");
   if ( getDocumentModel() ){
         getDocumentModel()->allowEdition();
   }
   HEXABLOCKGUI::currentDocGView->setAllSelection();
   MESSAGE("}");
}


// ============================================================== _allowVTKSelection
bool HexaBaseDialog::_allowVTKSelection( QObject* obj )
{

   MESSAGE("HexaBaseDialog::_allowVTKSelection(){");
   bool isOk = false;

   //	MESSAGE("*  getCurrentVtkView()->raise()");
   //	getCurrentVtkView()->raise();
   //	getCurrentVtkView()->setFocus();
   QVariant v  = obj->property("HexaWidgetType");
   HexaWidgetType wType = v.value<HexaWidgetType>();
   MESSAGE("*  HexaWidgetType property is " << wType );

   switch (wType){
   case VERTEX_TREE:
      HEXABLOCKGUI::currentDocGView->setVertexSelection(); isOk = true;
      break;
   case EDGE_TREE:
      HEXABLOCKGUI::currentDocGView->setEdgeSelection(); isOk = true;
      break;
   case QUAD_TREE:
      HEXABLOCKGUI::currentDocGView->setQuadSelection(); isOk = true;
      break;
   case HEXA_TREE:
      HEXABLOCKGUI::currentDocGView->setHexaSelection(); isOk = true;
      break;
      //    case VECTOR_TREE:
      //    case CYLINDER_TREE:
      //    case PIPE_TREE:
      //    case ELEMENTS_TREE:
      //    case CROSSELEMENTS_TREE: //selector = getPatternDataSelectionModel();
      //    case GROUP_TREE:      //selector = getGroupsSelectionModel();
      //    case LAW_TREE:
      //    case PROPAGATION_TREE: // selector = getMeshSelectionModel();
   default : MESSAGE("NOT YET");
   }
   MESSAGE("}");
   return isOk;
}

// ============================================================== _allowOCCSelection
bool HexaBaseDialog::_allowOCCSelection( QObject* obj )
{
   MESSAGE("HexaBaseDialog::_allowOCCCSelection(){");

   QVariant v  = obj->property("GeomWidgetType");
   GeomWidgetType wType = v.value<GeomWidgetType>();
   HEXABLOCKGUI::currentOccGView->setSelectionMode(wType);

   return true;
}

// ============================================================== _getSelector
QItemSelectionModel* HexaBaseDialog::_getSelector( QObject* obj )
{
   QItemSelectionModel* selector = NULL;
   MESSAGE("HexaBaseDialog::_getSelector(){");

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
   case HEXA_TREE: selector = getPatternDataSelectionModel(); break;
   case VECTOR_TREE:
   case CYLINDER_TREE:
   case PIPE_TREE:
   case ELEMENTS_TREE:
   case CROSSELEMENTS_TREE: selector = getPatternBuilderSelectionModel(); break;
   case GROUP_TREE: selector = getGroupsSelectionModel(); break;
   case LAW_TREE:
   case PROPAGATION_TREE: selector = getMeshSelectionModel(); break;
   default : MESSAGE("NOT YET");
   }
   MESSAGE("}");
   return selector;
}

// ============================================================== _onSelectionChanged
/*
 * Puts elements selected in the model (treeview) in the corresponding
 * line edit widget (the one that has the focus) of the current dialog box.
 */
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
               tr("%1: Bad type selected\nPlease select a %2.").arg(windowTitle()).arg(_strHexaWidgetType[wType]));
         return false;
   }

   //fill the lineEdit if selection is OK
   le->setText( selected.data().toString() );// namee
   le->setProperty("QModelIndex",  QVariant::fromValue(selected) );
   _index[le] = selected;

   QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(_currentObj);
   if (selected.isValid() && lineEdit != NULL)
      setFocusToNextField();

   MESSAGE("}");
   return true;
}

// ============================================================== _onSelectionChanged
/*
 * Puts elements selected in the model (treeview) in the corresponding list widget
 * of the current dialog box.
 */
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
            item->setData(LW_DATA_ROLE, isel.data(HEXA_DATA_ROLE));
            lw->addItem(item);
            updateButtonBox();
      }
   }
   MESSAGE("}");
   return true;
}

// ============================================================== onSelectionChanged
/*
 * Puts elements selected in the model in the corresponding widget (list widget or line edit)
 * of the current dialog box.
 */
void HexaBaseDialog::onSelectionChanged( const QItemSelection& sel, const QItemSelection& unsel )
{
   MESSAGE( "HexaBaseDialog::onSelectionChanged(){" );
   QString className = metaObject()->className();
   MESSAGE( "*  I am                          : " << className.toStdString() );
   MESSAGE( "*  sender is                     : " << sender() );
   MESSAGE( "*  patternDataSelectionModel    : " << getPatternDataSelectionModel() );
   MESSAGE( "*  patternBuilderSelectionModel : " << getPatternBuilderSelectionModel() );
   MESSAGE( "*  groupsSelectionModel         : " << getGroupsSelectionModel() );
   MESSAGE( "*  meshSelectionModel           : " << getMeshSelectionModel() );

   //	highlightSelectedAssocs();
   QItemSelectionModel* selector = dynamic_cast<QItemSelectionModel*>(sender());
   MESSAGE( "*  selector           : " << selector);

   foreach( const QModelIndex& isel, sel.indexes() ){
      MESSAGE("*  selected : " << isel.data().toString().toStdString());
   }
   foreach( const QModelIndex& iunsel, unsel.indexes() ){
      MESSAGE("*  unselected : " << iunsel.data().toString().toStdString());
   }

   if ( _selectionMutex ) return;

   bool selOk = false;
   QLineEdit*   aLineEdit   = NULL;
   QListWidget* aListWidget = NULL;
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

// ============================================================== showEvent
void HexaBaseDialog::showEvent( QShowEvent * event )
{
   MESSAGE("HexaBaseDialog::showEvent(){");
   QString className = metaObject()->className();
   MESSAGE( "*  I am                          : " << className.toStdString() );

   if (getDocumentModel() == NULL)
   {
       QDialog::showEvent ( event );
       return;
   }

   if ( _editMode == INFO_MODE ){
         getDocumentModel()->allowEdition();
   } else {
         getDocumentModel()->disallowEdition();
   }

   //Connect to salome selection signals
   if (HEXABLOCKGUI::selectionMgr() != NULL)
   {
//       disconnect(  HEXABLOCKGUI::selectionMgr() , SIGNAL(currentSelectionChanged()),
//               this, SLOT(onCurrentSelectionChanged()) );

       connect( HEXABLOCKGUI::selectionMgr(), SIGNAL(currentSelectionChanged()),
               this, SLOT(onCurrentSelectionChanged()), Qt::UniqueConnection );
   }

   //Connect to vtk window activation signals
   if (HEXABLOCKGUI::currentDocGView->getViewWindow() != NULL)
   {
//       disconnect( HEXABLOCKGUI::currentDocGView->getViewWindow()->getViewManager(),
//           SIGNAL( activated(SUIT_ViewManager*) ),
//           this, SLOT( onWindowActivated(SUIT_ViewManager*) ) );

       connect( HEXABLOCKGUI::currentDocGView->getViewWindow()->getViewManager(), SIGNAL( activated(SUIT_ViewManager*) ),
                  this, SLOT( onWindowActivated(SUIT_ViewManager*) ), Qt::UniqueConnection );
   }

   //connect to occ window activation signals
   if (HEXABLOCKGUI::currentOccGView->getViewWindow() != NULL)
   {
//       disconnect( HEXABLOCKGUI::currentOccGView->getViewWindow()->getViewManager(),
//               SIGNAL( activated(SUIT_ViewManager*) ),
//               this, SLOT( onWindowActivated(SUIT_ViewManager*) ) );

       connect( HEXABLOCKGUI::currentOccGView->getViewWindow()->getViewManager(),
               SIGNAL( activated(SUIT_ViewManager*) ),
               this, SLOT( onWindowActivated(SUIT_ViewManager*) ), Qt::UniqueConnection );
   }

   //connect model selection signals
   connectDocumentGraphicView();

   QDialog::showEvent ( event );
   MESSAGE("}");
}


// ============================================================== hideEvent
void HexaBaseDialog::hideEvent ( QHideEvent * event )
{
   MESSAGE("HexaBaseDialog::hideEvent(){");
   QString className = metaObject()->className();
   MESSAGE( "*  I am                          : " << className.toStdString() );
   if (getDocumentModel() == NULL) return;

   //Disconnection salome selection signals
   if (HEXABLOCKGUI::selectionMgr() != NULL)
       disconnect(  HEXABLOCKGUI::selectionMgr() , SIGNAL(currentSelectionChanged()),
               this, SLOT(onCurrentSelectionChanged()) );

   //Disconnect vtk window activation signals
   if (HEXABLOCKGUI::currentDocGView->getViewWindow() != NULL)
       disconnect( HEXABLOCKGUI::currentDocGView->getViewWindow()->getViewManager(),
               SIGNAL( activated(SUIT_ViewManager*) ),
               this, SLOT( onWindowActivated(SUIT_ViewManager*) ) );

   //Disconnect occ window activation signals
   if (HEXABLOCKGUI::currentOccGView->getViewWindow() != NULL)
       disconnect( HEXABLOCKGUI::currentOccGView->getViewWindow()->getViewManager(),
               SIGNAL( activated(SUIT_ViewManager*) ),
               this, SLOT( onWindowActivated(SUIT_ViewManager*) ) );

   //Disconnect model selection signals
   disconnectDocumentGraphicView();

   getDocumentModel()->allowEdition();

   QDialog::hideEvent( event );
   MESSAGE("}");
}

// ============================================================== updateButtonBox
void HexaBaseDialog::updateButtonBox()
{
}

// ============================================================== updateName
void HexaBaseDialog::updateName()
{
   if (!getDocumentModel()) return;
   if ( getPatternDataSelectionModel() == NULL )  return;
   const PatternDataModel*   patternDataModel = getPatternDataModel();
   if (patternDataModel == NULL) return;

   QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(sender());
   if (!lineEdit) return;
   QString newName = lineEdit->text();
   if ( newName.isEmpty() ) return;

   QVariant v = lineEdit->property("QModelIndex");
   if ( !v.isValid() ) return;

   QModelIndex index = v.value<QModelIndex>();
   if ( index.isValid() )
      getDocumentModel()->setName( patternDataModel->mapToSource(index), newName );
}

// ============================================================== updateDefaultName
void HexaBaseDialog::updateDefaultName(QLineEdit* name_field, QModelIndex& last)
{
   if (getDocumentModel() == NULL) return;
   if (name_field == NULL || getDocumentModel()->getHexaPtr(last) == NULL)
      return;

   char buffer [16];
   const char *defaultName = getDocumentModel()->getHexaPtr(last)->getNextName(buffer);
   name_field->setText(defaultName);
}

// ============================================================== selectElementOfModel
/*Selects in the model (treeview) elements selected in a listwidget,
 *  or an element in a line edit.*/

void HexaBaseDialog::selectElementOfModel()
{
   MESSAGE("HexaBaseDialog::selectElementOfModel()");
   if (getPatternDataSelectionModel() == NULL) return;

   QListWidget* currentListWidget = dynamic_cast<QListWidget*>( sender() );
   if ( !currentListWidget ) return;

   _selectionMutex = true;

   QList<QListWidgetItem *> sel = currentListWidget->selectedItems();
   QModelIndex index;
   getPatternDataSelectionModel()->clearSelection();
   foreach ( QListWidgetItem *item, sel ){
//      index = item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>(); //unsafe: index can change in the tree
      index = getPatternDataSelectionModel()->indexBy(HEXA_DATA_ROLE, item->data(LW_DATA_ROLE));
      if ( index.isValid() )
      {
          MESSAGE( "*  selecting the element : " << index.data().toString().toStdString() );
          getPatternDataSelectionModel()->select( index, QItemSelectionModel::Select );
      }
   }
   _selectionMutex = false;

   MESSAGE("}");
}

// ============================================================== _isLineOrListWidget
/*
 * Return true is the widget is a line or a list
 * false otherwise.
 */
bool HexaBaseDialog::_isLineOrListWidget(QObject *widget)
{
   if (widget == NULL) return false;

   QLineEdit   	*lineEdit = dynamic_cast<QLineEdit*>(widget);
   QListWidget *listWidget = dynamic_cast<QListWidget*>(widget);
   return (lineEdit != NULL) || (listWidget != NULL);

}//_isLineOrListWidget


// ============================================================== _highlightWidget
/*
 * Highlight the given widget with the given color.
 */
void HexaBaseDialog::_highlightWidget(QObject *obj, Qt::GlobalColor clr)
{
   if (!_isLineOrListWidget(obj)) return;

   QWidget *widget = dynamic_cast<QWidget*>(obj); //sure it's not NULL (_isLineOrListWidget(obj))
   QPalette palette1 = widget->palette();
   palette1.setColor(widget->backgroundRole(), clr);
   widget->setPalette(palette1);

}//_highlightWidget


// ============================================================== _updateCurrentObject
void HexaBaseDialog::_updateCurrentObject(QObject* obj)
{
   _highlightWidget(_currentObj, Qt::white);
   _currentObj = obj;
   _highlightWidget(obj, Qt::yellow);
}

// ============================================================== eventFilter
/*
 * Handles events from the treeview and the dialog boxes.
 */
bool HexaBaseDialog::eventFilter(QObject *obj, QEvent *event)
{
   //	highlightSelectedAssocs();
   QLineEdit   	*lineEdit = dynamic_cast<QLineEdit*>(obj);
   QListWidget *listWidget = dynamic_cast<QListWidget*>(obj);
   HexaBaseDialog  *dialog = dynamic_cast<HexaBaseDialog*>(obj);

   if ( event->type() == QEvent::KeyPress )
      {
         if ( ((QKeyEvent*)event)->key() == Qt::Key_Return)
            {
               setFocusToNextField();
               return true;
            }
      }

   if ( event->type() == QEvent::FocusOut ){
         QString className = obj->metaObject()->className();
         QString objectName = obj->objectName();
         MESSAGE("QEvent::FocusOut : I am "<< objectName.toStdString() );
         MESSAGE("QEvent::FocusOut : I am "<< className.toStdString() );
   }

   if ( event->type() == QEvent::FocusIn ){
         QString className  = obj->metaObject()->className();
         QString objectName = obj->objectName();
         MESSAGE("QEvent::FocusIn : I am "<< objectName.toStdString() );
         MESSAGE("QEvent::FocusIn : I am "<< className.toStdString() );

   }
   else return false;


   MESSAGE("HexaBaseDialog::eventFilter{");
   MESSAGE("*  QEvent::FocusIn");

   /* ON FOCUS ON A WIDGET*/

   QVariant v;
   QModelIndex index;

   QItemSelectionModel *selector = 0;

   //vtk selection
   if (getObjectViewType(obj) == HEXABLOCKGUI::VTK)
      _allowVTKSelection( obj );

   //geom selection
   if (getObjectViewType(obj) == HEXABLOCKGUI::OCC)
      _allowOCCSelection( obj );

   //If the widget is a geom line edit highlight its content in the occ view
   if (lineEdit != NULL && getPatternDataSelectionModel() != NULL &&
         getObjectViewType(obj) == HEXABLOCKGUI::OCC && !lineEdit->text().isEmpty())
      {
         //highlight element in the OCC View
         QMultiMap<QString, int> geomElts;
         DocumentModel::GeomObj geomObj;
         QVariant v = lineEdit->property("GeomObj");
         if ( v.isValid() )
            {
               geomObj = v.value<DocumentModel::GeomObj>();
               geomElts.insert( geomObj.entry, geomObj.subid.toInt() );
            }
         HEXABLOCKGUI::currentOccGView->highlight(geomElts);
      }

   //Depending of focused widget type, get the right selector for it
   selector = _getSelector( obj );
   if ( selector == NULL )
      {
         _updateCurrentObject(obj);
         return false;
      }

   if ( _currentObj != obj && (lineEdit || listWidget) )
      selector->clearSelection();

   _updateCurrentObject(obj);

   //If there is a current selection fill the widget with it
   if ( selector->hasSelection() ){
         //		QItemSelection currentSelection  = selector->selection();
         //		bool selOk = false;
         //		if ( lineEdit ){ //element can be from lineEdit
         //			selOk = _onSelectionChanged( currentSelection, lineEdit );
         //		} else if ( listWidget ){
         //			selOk = _onSelectionChanged( currentSelection, listWidget );
         //		}
         //		if ( !selOk &&  ( lineEdit || listWidget ) ){
         //			selector->clearSelection();
         //		}

   } else { //If the widget contains an hexa element, select it in model/view
         if ( lineEdit ){ //element can be from lineEdit
               MESSAGE("*  on QLineEdit");
               v = lineEdit->property("QModelIndex");
               if ( !v.isValid() ) {
                     //return QObject::eventFilter(obj, event);
                     return false;
               }
               index = v.value<QModelIndex>();

               _selectionMutex = true;
               MESSAGE("*  selecting the element : " << index.data().toString().toStdString());
               MESSAGE("*  with selector : " << selector);
               MESSAGE( "*  patternDataSelectionModel    : " << getPatternDataSelectionModel() );
               MESSAGE( "*  patternBuilderSelectionModel : " << getPatternBuilderSelectionModel() );
               MESSAGE( "*  groupsSelectionModel         : " << getGroupsSelectionModel() );
               MESSAGE( "*  meshSelectionModel           : " << getMeshSelectionModel() );
               selector->select( index, QItemSelectionModel::Clear );
               selector->select( index, QItemSelectionModel::Select );
               _selectionMutex = false;
         }

         if ( dialog ){ //element can be from a dialog box
               MESSAGE("*  on Dialog");
               v = dialog->property("QModelIndex");
               if ( !v.isValid() ) {
                     //return QObject::eventFilter(obj, event);
                     return false;
               }
               index = v.value<QModelIndex>();
               _selectionMutex = true;
               MESSAGE("*  selecting the element : " << index.data().toString().toStdString());
               selector->select( index, QItemSelectionModel::Clear );
               selector->select( index, QItemSelectionModel::Select );
               _selectionMutex = false;
         }

   }
   MESSAGE("eventFilter}");
   return false;
}

// ------------------------- VERTEX ----------------------------------
// ============================================================== Constructeur
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

// ============================================================== Destructeur
VertexDialog::~VertexDialog()
{
}


// ============================================================== _initInputWidget
void VertexDialog::_initInputWidget( Mode editmode )
{
   x_spb->setRange(VERTEX_COORD_MIN, VERTEX_COORD_MAX);
   y_spb->setRange(VERTEX_COORD_MIN, VERTEX_COORD_MAX);
   z_spb->setRange(VERTEX_COORD_MIN, VERTEX_COORD_MAX);

//   setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
   installEventFilter(this);
//   name_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
   name_le->installEventFilter(this);

   //connect( name_le, SIGNAL(returnPressed()), this, SLOT(updateName()));
}

// ============================================================== clear
void VertexDialog::clear()
{
      name_le->clear();

      modelUnregister(this);
}

// ============================================================== setValue
void VertexDialog::setValue(HEXA_NS::Vertex* v)
{
   MESSAGE("*  setValue : " << v);
   //0) Name
   name_le->setText( v->getName() );

   //1) Value (x,y,z)
   x_spb->setValue( v->getX() );
   y_spb->setValue( v->getY() );
   z_spb->setValue( v->getZ() );

   MESSAGE("*  patternDataSelectionModel  : " << getPatternDataSelectionModel() );
   if ( getPatternDataSelectionModel() != NULL ){
         QModelIndex iv = getPatternDataSelectionModel()->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(v) );
         MESSAGE("*  selecting the element : " << iv.data().toString().toStdString());

         setProperty( "QModelIndex",  QVariant::fromValue<QModelIndex>(iv) );
         name_le->setProperty( "QModelIndex",  QVariant::fromValue<QModelIndex>(iv) );
   }
   _value = v;
}


// ============================================================== getValue
HEXA_NS::Vertex* VertexDialog::getValue()
{
   return _value;
}


// ============================================================== apply
bool VertexDialog::apply(QModelIndex& result)
{
   if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
   _currentObj = NULL;

   SUIT_OverrideCursor wc;
   if ( !getDocumentModel() ) return false;
   if ( !getPatternDataSelectionModel() ) return false;
   const PatternDataModel* patternDataModel = getPatternDataModel();
   if ( !patternDataModel ) return false;

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
                  isOk = getDocumentModel()->updateVertex( iVertex, newX, newY, newZ );
         }
   } else if ( _editMode == NEW_MODE){
         iVertex = getDocumentModel()->addVertex( newX, newY, newZ );
         if ( iVertex.isValid() )
            {
               isOk = true;
            }
   }
   if (!isOk) {
         SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "VERTEX UPDATE/CONSTRUCTION" ) );
         return false;
   }

   QString newName = name_le->text();
   if ( !newName.isEmpty() )/*{*/
      getDocumentModel()->setName( iVertex, newName );

   updateDefaultName(name_le, iVertex);

   // to select/highlight result
   result = patternDataModel->mapFromSource(iVertex);

   //	updateDialogBoxName(name_le, result);
   //	const char *defaultName = getDocumentModel()->getHexaPtr(last)->getName();
   return isOk;
}


// ------------------------- EDGE ----------------------------------

// ============================================================== Constructeur
EdgeDialog::EdgeDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f ):
								                                                                                                                    HexaBaseDialog(parent, editmode, f),
								                                                                                                                    _value(0)
{
   _helpFileName = "gui_edge.html";
   setupUi( this );
   _initWidget(editmode);

   //	rb0->setFocusProxy( v0_le_rb0 );
   //	rb1->setFocusProxy( vex_le_rb1 );

   if  ( editmode == INFO_MODE ){
         setWindowTitle( tr("Edge Information") );
         rb1->hide();
   }
   else if ( editmode == UPDATE_MODE ){
         setWindowTitle( tr("Vertex Modification") );
   }

   rb0->click();
}


// ============================================================== Destructeur
EdgeDialog::~EdgeDialog()
{
}


// ============================================================== _initInputWidget
void EdgeDialog::_initInputWidget( Mode editmode )
{
   QRegExp rx("");
   QValidator *validator = new QRegExpValidator(rx, this);

//   setProperty( "HexaWidgetType",  QVariant::fromValue(EDGE_TREE) );
   installEventFilter(this);

//   name_le->setProperty( "HexaWidgetType",  QVariant::fromValue(EDGE_TREE) );
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

   v0_le_rb0->setReadOnly(true);
   v1_le_rb0->setReadOnly(true);
   vex_le_rb1->setReadOnly(true);
   vec_le_rb1->setReadOnly(true);

   //connect( name_le, SIGNAL(returnPressed()), this, SLOT(updateName()) );

}


// ============================================================== Clear
void EdgeDialog::clear()
{
   name_le->clear();

   v0_le_rb0->clear();
   modelUnregister(v0_le_rb0);

   v1_le_rb0->clear();
   modelUnregister(v1_le_rb0);

   vex_le_rb1->clear();
   modelUnregister(vex_le_rb1);

   vec_le_rb1->clear();
   modelUnregister(vec_le_rb1);

   modelUnregister(this);
}



// ============================================================== setValue
void EdgeDialog::setValue(HEXA_NS::Edge* e)
{
   HEXA_NS::Vertex* v0 = e->getVertex(0);
   HEXA_NS::Vertex* v1 = e->getVertex(1);
   //	HEXA_NS::EltBase* e0 = e;
   //	HEXA_NS::EltBase* v0 = e->getVertex(0);
   //	HEXA_NS::EltBase* v1 = e->getVertex(1);

   name_le->setText( e->getName() );
   v0_le_rb0->setText( v0->getName() );
   v1_le_rb0->setText( v1->getName() );

   if ( getPatternDataSelectionModel() ){
         QModelIndex ie  = getPatternDataSelectionModel()->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(e) );
         QModelIndex iv0 = getPatternDataSelectionModel()->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(v0) );
         QModelIndex iv1 = getPatternDataSelectionModel()->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(v1) );

         name_le->setProperty( "QModelIndex",  QVariant::fromValue(ie) );
         v0_le_rb0->setProperty( "QModelIndex",  QVariant::fromValue(iv0) );
         v1_le_rb0->setProperty( "QModelIndex",  QVariant::fromValue(iv1) );
   }
   _value = e;
}


// ============================================================== getValue
HEXA_NS::Edge* EdgeDialog::getValue()
{
   return _value;
}

// ============================================================== apply
bool EdgeDialog::apply(QModelIndex& result)
{
   if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
   _currentObj = NULL;

   SUIT_OverrideCursor wc;
   if ( !getDocumentModel() ) return false;
   const PatternDataModel*    patternDataModel = getPatternDataModel();
   const PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();
   if ( !patternDataModel || !patternBuilderModel ) return false;

   QModelIndex iEdge;

   if ( rb0->isChecked() ){
         QModelIndex iv0 = patternDataModel->mapToSource( _index[v0_le_rb0] );
         QModelIndex iv1 = patternDataModel->mapToSource( _index[v1_le_rb0] );
         if ( iv0.isValid()&& iv1.isValid() ){
               iEdge = getDocumentModel()->addEdgeVertices( iv0, iv1 );
         }
   } else if ( rb1->isChecked() ){
         QModelIndex ivex = patternDataModel->mapToSource( _index[vex_le_rb1] );
         QModelIndex ivec = patternBuilderModel->mapToSource( _index[vec_le_rb1] );
         if ( ivex.isValid() && ivec.isValid() ){
               iEdge = getDocumentModel()->addEdgeVector( ivex, ivec );
         }
   }

   if ( !iEdge.isValid() ){
         SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT BUILD EDGE" ) );
         return false;
   }
   _value  = iEdge.model()->data(iEdge, HEXA_DATA_ROLE).value<HEXA_NS::Edge*>();

   QString newName = name_le->text();
   if ( !newName.isEmpty() )/*{*/
      getDocumentModel()->setName( iEdge, newName );

   updateDefaultName(name_le, iEdge);

   result = patternDataModel->mapFromSource(iEdge);

   return true;
}


// ------------------------- QUAD ----------------------------------

// ============================================================== Constructeur
QuadDialog::QuadDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f ):
								                                                                                                                    HexaBaseDialog(parent, editmode, f),
								                                                                                                                    _value(0)
{
   _helpFileName = "gui_quadrangle.html";
   setupUi( this );
   _initWidget(editmode);
   //	rb0->setFocusProxy( v0_le_rb0 );
   //	rb1->setFocusProxy( e0_le_rb1 );
   rb0->click();

   if  ( editmode == INFO_MODE ){
         setWindowTitle( tr("Quad Information") );
   }
}

// ============================================================== Destructeur
QuadDialog::~QuadDialog()
{
}

// ============================================================== _initInputWidget
void QuadDialog::_initInputWidget( Mode editmode )
{
   QRegExp rx("");
   QValidator *validator = new QRegExpValidator(rx, this);

//   setProperty( "HexaWidgetType",  QVariant::fromValue(QUAD_TREE) );
   installEventFilter(this);

//   name_le->setProperty( "HexaWidgetType",  QVariant::fromValue(QUAD_TREE) );
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

   v0_le_rb0->setReadOnly(true);
   v1_le_rb0->setReadOnly(true);
   v2_le_rb0->setReadOnly(true);
   v3_le_rb0->setReadOnly(true);

   e0_le_rb1->setReadOnly(true);
   e1_le_rb1->setReadOnly(true);
   e2_le_rb1->setReadOnly(true);
   e3_le_rb1->setReadOnly(true);

   //connect( name_le, SIGNAL(returnPressed()), this, SLOT(updateName()) );
}


// ============================================================== clear
void QuadDialog::clear()
{
    //Clear the dialog and unregister it from the model
   QModelIndex invalidIndex;

   name_le->clear();

   v0_le_rb0->clear();
   modelUnregister(v0_le_rb0);

   v1_le_rb0->clear();
   modelUnregister(v1_le_rb0);

   v2_le_rb0->clear();
   modelUnregister(v2_le_rb0);

   v3_le_rb0->clear();
   modelUnregister(v3_le_rb0);

   e0_le_rb1->clear();
   modelUnregister(e0_le_rb1);

   e1_le_rb1->clear();
   modelUnregister(e1_le_rb1);

   e2_le_rb1->clear();
   modelUnregister(e2_le_rb1);

   e3_le_rb1->clear();
   modelUnregister(e3_le_rb1);

   modelUnregister(this);

}


// ============================================================== setValue
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

   if ( getPatternDataSelectionModel() ){
         QModelIndex iq = getPatternDataSelectionModel()->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(q) );

         QModelIndex iv0 = getPatternDataSelectionModel()->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(v0) );
         QModelIndex iv1 = getPatternDataSelectionModel()->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(v1) );
         QModelIndex iv2 = getPatternDataSelectionModel()->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(v2) );
         QModelIndex iv3 = getPatternDataSelectionModel()->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(v3) );

         QModelIndex ie0 = getPatternDataSelectionModel()->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(e0) );
         QModelIndex ie1 = getPatternDataSelectionModel()->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(e1) );
         QModelIndex ie2 = getPatternDataSelectionModel()->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(e2) );
         QModelIndex ie3 = getPatternDataSelectionModel()->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(e3) );

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

// ============================================================== getValue
HEXA_NS::Quad* QuadDialog::getValue()
{
   return _value;
}


// ============================================================== apply
bool QuadDialog::apply(QModelIndex& result)
{
   if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
   _currentObj = NULL;

   SUIT_OverrideCursor wc;
   if ( !getDocumentModel() ) return false;
   const PatternDataModel* patternDataModel = getPatternDataModel();
   if ( !patternDataModel ) return false;

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
               iQuad = getDocumentModel()->addQuadVertices( iv0, iv1, iv2, iv3 );
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
               iQuad = getDocumentModel()->addQuadEdges( ie0, ie1, ie2, ie3 );
         }
   }

   if ( !iQuad.isValid() ){
         SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT BUILD QUAD" ) );
         return false;
   }
   _value  = iQuad.model()->data(iQuad, HEXA_DATA_ROLE).value<HEXA_NS::Quad *>();

   QString newName = name_le->text();
   if ( !newName.isEmpty() )/*{*/
      getDocumentModel()->setName( iQuad, newName );

   updateDefaultName(name_le, iQuad);

   // to select/highlight result
   result = patternDataModel->mapFromSource(iQuad);

   return true;
}



// ------------------------- HEXA ----------------------------------

// ============================================================== HexaDialog

HexaDialog::HexaDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f ):
								                                                                                                                    HexaBaseDialog(parent, editmode, f),
								                                                                                                                    _value(0)
{
   _helpFileName = "gui_hexahedron.html";
   setupUi( this );
   _initWidget(editmode);
   //	quads_rb->setFocusProxy( quads_lw );
   //	vertices_rb->setFocusProxy( vertices_lw );
   quads_rb->click();
   //   quads_rb->setFocus();

   if  ( editmode == INFO_MODE ){
         setWindowTitle( tr("Hexahedron Information") );
   }
}

// ============================================================== Destructeur
HexaDialog::~HexaDialog()
{
}


// ============================================================== _initInputWidget
void HexaDialog::_initInputWidget( Mode editmode )
{
   QRegExp rx("");

//   setProperty( "HexaWidgetType",  QVariant::fromValue(HEXA_TREE) );
   installEventFilter(this);

//   name_le->setProperty( "HexaWidgetType",  QVariant::fromValue(HEXA_TREE) );
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
   }
   // edit name
   //connect( name_le, SIGNAL(returnPressed()), this, SLOT(updateName()) );
   // highlight item on model view (VTK) from listwidget
   connect( quads_lw,    SIGNAL(itemSelectionChanged()), this, SLOT(selectElementOfModel()), Qt::UniqueConnection );
   connect( vertices_lw, SIGNAL(itemSelectionChanged()), this, SLOT(selectElementOfModel()), Qt::UniqueConnection );

   if ( editmode != INFO_MODE)
   {
       connect( vertices_rb, SIGNAL(clicked()), vertices_lw, SLOT(setFocus()));
       connect( quads_rb, SIGNAL(clicked()), quads_lw, SLOT(setFocus()));
   }

   connect( vertices_rb, SIGNAL(clicked()), this, SLOT(refreshHighlight()), Qt::UniqueConnection);
   connect( quads_rb, SIGNAL(clicked()), this, SLOT(refreshHighlight()), Qt::UniqueConnection);
}

// ============================================================== clear
void HexaDialog::clear()
{
   name_le->clear();

   quads_lw->clear();
   modelUnregister(quads_lw);

   vertices_lw->clear();
   modelUnregister(vertices_lw);

   modelUnregister(this);
}

// ============================================================== getAssocsVTK
/*
 * Returns elements currently associated to vtk
 */
QModelIndexList HexaDialog::getAssocsVTK()
{
   QModelIndexList assocs;
   QModelIndex iQuad, iVertex;
   QListWidgetItem* item = NULL;

   const PatternDataModel* patternDataModel = getPatternDataModel();
   if (patternDataModel == NULL) return assocs;
   if (quads_rb->isChecked())
      {
         //ListWidget content
         for ( int r = 0; r < quads_lw->count(); ++r ){
               item = quads_lw->item(r);
               iQuad = patternDataModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() );
               if ( iQuad.isValid() )
                  assocs << iQuad;
         }
         return assocs;
      }
   else if (vertices_rb->isChecked())
      {
         //ListWidget content
         for ( int r = 0; r < vertices_lw->count(); ++r ){
               item = vertices_lw->item(r);
               iVertex = patternDataModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() );
               if ( iVertex.isValid() )
                  assocs << iVertex;
         }
         return assocs;
      }
   else return assocs;
}


// ============================================================== updateButtonBox
void HexaDialog::updateButtonBox()
{
   if ( quads_rb->isChecked() ){  // build from quads count() must be between [2,6]
         int nbQuads = quads_lw->count();
         //std::cout << "nbQuads => " <<  nbQuads << std::endl;
         if ( nbQuads >= 2 && nbQuads <= 6 ){
               //_applyCloseButton->setEnabled(true);
               _applyButton->setEnabled(true);
               //       _applyCloseButton->setFlat( false );
               //       _applyButton->setFlat( false );
         } else {
               //_applyCloseButton->setEnabled(false);
               _applyButton->setEnabled(false);
               //       _applyCloseButton->setFlat( true );
               //       _applyButton->setFlat( true );
         }
   } else if ( vertices_rb->isChecked() ){ // build from vertices count() must be equals to 8
         int nbVertices = vertices_lw->count();
         //std::cout << "nbVertices => " <<  nbVertices << std::endl;
         if ( nbVertices == 8 ){
               //_applyCloseButton->setEnabled(true);
               _applyButton->setEnabled(true);
               //       _applyCloseButton->setFlat( false );
               //       _applyButton->setFlat( false );
         } else {
               //_applyCloseButton->setEnabled(false);
               _applyButton->setEnabled(false);
               //       _applyCloseButton->setFlat( true );
               //       _applyButton->setFlat( true );
         }
   }
}

// ============================================================== deleteQuadItem
void HexaDialog::deleteQuadItem()
{
   delete quads_lw->currentItem();
   updateButtonBox();
}

// ============================================================== deleteVertexItem
void HexaDialog::deleteVertexItem()
{
   delete vertices_lw->currentItem();
   updateButtonBox();
}

// ============================================================== _setValueQuads
void HexaDialog::_setValueQuads( HEXA_NS::Hexa* h )
{
   QListWidgetItem *qItem = NULL;
   HEXA_NS::Quad   *q     = NULL;
   QModelIndex      qIndex;
   if (getPatternDataSelectionModel() == NULL) return;
   quads_lw->clear();
   for( int i = 0; i <= 5; ++i ){
         q      = h->getQuad(i);
         qIndex = getPatternDataSelectionModel()->indexBy( HEXA_ENTRY_ROLE, QString::number(reinterpret_cast<intptr_t>(q)) );
         qItem  = new QListWidgetItem( q->getName() );
         qItem->setData(  LW_QMODELINDEX_ROLE, QVariant::fromValue<QModelIndex>(qIndex) );
         qItem->setData(LW_DATA_ROLE, qIndex.data(HEXA_DATA_ROLE));
         quads_lw->addItem( qItem );
   }
}

// ============================================================== _setValueVertices
void HexaDialog::_setValueVertices( HEXA_NS::Hexa* h )
{
   QListWidgetItem *vItem = NULL;
   HEXA_NS::Vertex* v     = NULL;
   QModelIndex      vIndex;
   if (getPatternDataSelectionModel() == NULL) return;
   vertices_lw->clear();
   for( int i = 0; i <= 7; ++i ){
         v = h->getVertex(i);
         vIndex = getPatternDataSelectionModel()->indexBy( HEXA_ENTRY_ROLE, QString::number(reinterpret_cast<intptr_t>(v)) );
         vItem  = new QListWidgetItem( v->getName() );
         vItem->setData(  LW_QMODELINDEX_ROLE, QVariant::fromValue<QModelIndex>(vIndex) );
         vItem->setData(LW_DATA_ROLE, vIndex.data(HEXA_DATA_ROLE));
         vertices_lw->addItem( vItem );
   }
}

// ============================================================== setValue
void HexaDialog::setValue(HEXA_NS::Hexa* h)
{
   // 0) name
   name_le->setText( h->getName() );

   if ( getPatternDataSelectionModel()){
         QModelIndex hIndex = getPatternDataSelectionModel()->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(h) );
         _setValueVertices(h);
         _setValueQuads(h);
         name_le->setProperty( "QModelIndex",  QVariant::fromValue(hIndex) );
   }
   _value = h;
}

// ============================================================== getValue
HEXA_NS::Hexa* HexaDialog::getValue()
{
   return _value;
}

// ============================================================== apply
bool HexaDialog::apply(QModelIndex& result)
{
   if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
   _currentObj = NULL;

   SUIT_OverrideCursor wc;
   if ( !getDocumentModel() ) return false;
   const PatternDataModel* patternDataModel = getPatternDataModel();
   if ( !patternDataModel ) return false;

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

   if ( quads_rb->isChecked() and (iElts.count()>=2 and iElts.count()<=6) ){ // build from quads iElts.count() should be between [2,6]
         iHexa = getDocumentModel()->addHexaQuads( iElts );
   } else if ( vertices_rb->isChecked() and iElts.count()== 8 ){ // build from vertices
         iHexa = getDocumentModel()->addHexaVertices( iElts[0], iElts[1], iElts[2], iElts[3],
               iElts[4], iElts[5], iElts[6], iElts[7] );
   }

   if ( !iHexa.isValid() ){
         SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT BUILD HEXA" ) );
         return false;
   }
   _value  = iHexa.model()->data(iHexa, HEXA_DATA_ROLE).value<HEXA_NS::Hexa*>();

   QString newName = name_le->text();
   if ( !newName.isEmpty() )/*{*/
      getDocumentModel()->setName( iHexa, newName );

   updateDefaultName(name_le, iHexa);

   // to select/highlight result
   result = patternDataModel->mapFromSource(iHexa);

   return true;
}


// ------------------------- VECTOR ----------------------------------

// ============================================================== Constructeur

VectorDialog::VectorDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f ):
								                                                                                                                    HexaBaseDialog(parent, editmode, f),
								                                                                                                                    _value(0)
{
   _helpFileName = "gui_vector.html";
   setupUi( this );
   _initWidget(editmode);

   //	rb0->setFocusProxy( dx_spb_rb0 );
   //	rb1->setFocusProxy( v0_le_rb1 );
   rb0->click();
   //   setFocusProxy( rb0 );

   if ( editmode == INFO_MODE ){
         setWindowTitle( tr("Vector Information") );
         rb1->hide();
   }
}

// ============================================================== Destructeur
VectorDialog::~VectorDialog()
{
}

// ============================================================== _initInputWidget
void VectorDialog::_initInputWidget( Mode editmode )
{
   QRegExp rx("");
   QValidator *validator = new QRegExpValidator(rx, this);

//   setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );
   installEventFilter(this);

//   name_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );
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

   v0_le_rb1->setReadOnly(true);
   v1_le_rb1->setReadOnly(true);

   //connect( name_le, SIGNAL(returnPressed()), this, SLOT(updateName()) );
}

// ============================================================== clear
void VectorDialog::clear()
{
   name_le->clear();
//   dx_spb_rb0->clear();
//   dy_spb_rb0->clear();
//   dz_spb_rb0->clear();
   v0_le_rb1->clear();
   modelUnregister(v0_le_rb1);

   v1_le_rb1->clear();
   modelUnregister(v1_le_rb1);

   modelUnregister(this);
}

// ============================================================== setValue
void VectorDialog::setValue(HEXA_NS::Vector* v)
{
   name_le->setText( v->getName() );
   dx_spb_rb0->setValue( v->getDx() );
   dy_spb_rb0->setValue( v->getDy() );
   dz_spb_rb0->setValue( v->getDz() );

   if ( getPatternDataSelectionModel() ){
         QModelIndex ivec = getPatternDataSelectionModel()->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(v) );
         name_le->setProperty( "QModelIndex",  QVariant::fromValue(ivec) );
   }
   _value = v;
}

// ============================================================== getValue
HEXA_NS::Vector* VectorDialog::getValue()
{
   return _value;
}

// ============================================================== apply
bool VectorDialog::apply(QModelIndex& result)
{
   if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
   _currentObj = NULL;

   SUIT_OverrideCursor wc;
   if ( !getDocumentModel() ) return false;
   const PatternDataModel*    patternDataModel    = getPatternDataModel();
   const PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();
   if ( !patternDataModel || !patternBuilderModel) return false;

   QModelIndex iVector;

   if ( rb0->isChecked() ){ //scalar
         double dx = dx_spb_rb0->value();
         double dy = dy_spb_rb0->value();
         double dz = dz_spb_rb0->value();

         iVector = getDocumentModel()->addVector( dx, dy, dz );
   } else if ( rb1->isChecked() ){ //vertices
         QModelIndex iv0 = patternDataModel->mapToSource( _index[v0_le_rb1] );
         QModelIndex iv1 = patternDataModel->mapToSource( _index[v1_le_rb1] );

         if ( iv0.isValid()
               && iv1.isValid() ){
               iVector = getDocumentModel()->addVectorVertices( iv0, iv1 );
         }
   }

   if ( !iVector.isValid() ){
         SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT BUILD VECTOR" ) );
         return false;
   }

   _value  = iVector.model()->data(iVector, HEXA_DATA_ROLE).value<HEXA_NS::Vector *>();

   QString newName = name_le->text();
   if ( !newName.isEmpty() )/*{*/
      getDocumentModel()->setName( iVector, newName );

   updateDefaultName(name_le, iVector);

   // to select/highlight result
   result = patternBuilderModel->mapFromSource(iVector);

   return true;
}


// ============================================================== Constructeur
CylinderDialog::CylinderDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f )
: HexaBaseDialog(parent, editmode, f),
  _value(0)
{
   _helpFileName = "gui_cyl.html";
   setupUi( this );
   _initWidget(editmode);

   if ( editmode == INFO_MODE ){
         setWindowTitle( tr("Cylinder Information") );
   }
}

// ============================================================== Destructeur
CylinderDialog::~CylinderDialog()
{
}

// ============================================================== _initInputWidget
void CylinderDialog::_initInputWidget( Mode editmode )
{
   QRegExp rx("");
   QValidator *validator = new QRegExpValidator(rx, this);

//   setProperty( "HexaWidgetType",  QVariant::fromValue(CYLINDER_TREE) );
   installEventFilter(this);

//   name_le->setProperty( "HexaWidgetType",  QVariant::fromValue(CYLINDER_TREE) );
   name_le->installEventFilter(this);

   vex_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
   vec_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );

   vex_le->setValidator( validator );
   vec_le->setValidator( validator );

   vex_le->installEventFilter(this);
   vec_le->installEventFilter(this);

   vex_le->setReadOnly(true);
   vec_le->setReadOnly(true);

   //connect( name_le, SIGNAL(returnPressed()), this, SLOT(updateName()));
}

// ============================================================== clear
void CylinderDialog::clear()
{
   name_le->clear();

   vex_le->clear();
   modelUnregister(vex_le);

   vec_le->clear();
   modelUnregister(vec_le);

   r_spb->clear();
   h_spb->clear();

   modelUnregister(this);

}

// ============================================================== setValue
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

   if ( getPatternDataSelectionModel() ){
         QModelIndex iCyl       = getPatternDataSelectionModel()->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(c) );
         QModelIndex iBase      = getPatternDataSelectionModel()->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(base) );
         QModelIndex iDirection = getPatternDataSelectionModel()->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(direction) );

         name_le->setProperty( "QModelIndex",  QVariant::fromValue(iCyl) );
         vex_le->setProperty( "QModelIndex",  QVariant::fromValue(iBase) );
         vec_le->setProperty( "QModelIndex",  QVariant::fromValue(iDirection) );
   }
   _value = c;
}

// ============================================================== getValue
HEXA_NS::Cylinder* CylinderDialog::getValue()
{
   return _value;
}



// ============================================================== apply
bool CylinderDialog::apply(QModelIndex& result)
{
   if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
   _currentObj = NULL;

   SUIT_OverrideCursor wc;
   if ( !getDocumentModel() ) return false;
   const PatternDataModel*    patternDataModel    = getPatternDataModel();
   const PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();
   if ( !patternDataModel || !patternBuilderModel) return false;

   QModelIndex iCyl;
   QModelIndex ivex = patternDataModel->mapToSource( _index[vex_le] );
   QModelIndex ivec = patternBuilderModel->mapToSource( _index[vec_le] );
   double r = r_spb->value();
   double h = h_spb->value();

   if ( ivex.isValid()
         && ivec.isValid() ){
         iCyl = getDocumentModel()->addCylinder( ivex, ivec, r,  h );
   }

   if ( !iCyl.isValid() ){
         SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT ADD CYLINDER" ) );
         return false;
   }

   _value  = iCyl.model()->data(iCyl, HEXA_DATA_ROLE).value<HEXA_NS::Cylinder *>();

   QString newName = name_le->text();
   if (!newName.isEmpty()) /*{*/
      getDocumentModel()->setName( iCyl, newName );

   updateDefaultName(name_le, iCyl);

   // to select/highlight result
   result = patternBuilderModel->mapFromSource(iCyl);

   return true;
}


//------------------------------- PipeDialog -----------------------------------

// ============================================================== Constructeur

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

// ============================================================== Destructeur
PipeDialog::~PipeDialog()
{
}

// ============================================================== _initInputWidget
void PipeDialog::_initInputWidget( Mode editmode )
{
   QRegExp rx("");
   QValidator *validator = new QRegExpValidator(rx, this);


//   setProperty( "HexaWidgetType",  QVariant::fromValue(PIPE_TREE) );
   installEventFilter(this);

//   name_le->setProperty( "HexaWidgetType",  QVariant::fromValue(PIPE_TREE) );
   name_le->installEventFilter(this);

   //Vertex Field config
   vex_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
   vex_le->installEventFilter(this);
   vex_le->setValidator( validator );

   //Vector Field config
   vec_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );
   vec_le->installEventFilter(this);
   vec_le->setValidator( validator );


   if ( editmode == INFO_MODE ){
         ir_spb->setReadOnly(true);
         er_spb->setReadOnly(true);
         h_spb->setReadOnly(true);
   }

   vex_le->setReadOnly(true);
   vec_le->setReadOnly(true);

   //connect( name_le, SIGNAL(returnPressed()), this, SLOT(updateName()));
}


// ============================================================== clear
void PipeDialog::clear()
{
   name_le->clear();

   vex_le->clear();
   modelUnregister(vex_le);

   vec_le->clear();
   modelUnregister(vec_le);

   modelUnregister(this);
}

// ============================================================== setValue
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

   if ( getPatternDataSelectionModel() ){
         QModelIndex iPipe      = getPatternDataSelectionModel()->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(p) );
         QModelIndex iBase      = getPatternDataSelectionModel()->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(base) );
         QModelIndex iDirection = getPatternDataSelectionModel()->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(direction) );

         name_le->setProperty( "QModelIndex",  QVariant::fromValue(iPipe) );
         vex_le->setProperty( "QModelIndex",  QVariant::fromValue(iBase) );
         vec_le->setProperty( "QModelIndex",  QVariant::fromValue(iDirection) );
   }
   _value = p;
}

// ============================================================== getValue
HEXA_NS::Pipe* PipeDialog::getValue()
{
   return _value;
}


// ============================================================== apply
bool PipeDialog::apply(QModelIndex& result)
{
   if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
   _currentObj = NULL;

   SUIT_OverrideCursor wc;
   if ( !getDocumentModel() ) return false;
   const PatternDataModel*    patternDataModel    = getPatternDataModel();
   const PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();
   if ( !patternDataModel || !patternBuilderModel)    return false;

   QModelIndex iPipe;
   QModelIndex ivex = patternDataModel->mapToSource( _index[vex_le] );
   QModelIndex ivec = patternBuilderModel->mapToSource( _index[vec_le] );
   double ir = ir_spb->value();
   double er = er_spb->value();
   double h  = h_spb->value();

   if (ir >= er) {
         SUIT_MessageBox::information( this, tr( "CANNOT ADD PIPE" ), tr( "External radius must be greather than Internal radius!" ) );
         return false;
   }


   if ( ivex.isValid()
         && ivec.isValid() ){
         iPipe = getDocumentModel()->addPipe( ivex, ivec, ir, er, h );
   }

   if ( !iPipe.isValid() ){
         SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT ADD PIPE" ) );
         return false;
   }
   _value  = iPipe.model()->data(iPipe, HEXA_DATA_ROLE).value<HEXA_NS::Pipe *>();

   QString newName = name_le->text();
   if ( !newName.isEmpty() )/*{*/
      getDocumentModel()->setName( iPipe, newName );

   updateDefaultName(name_le, iPipe);

   // to select/highlight result
   result = patternBuilderModel->mapFromSource(iPipe);

   return true;
}


// ------------------------- MakeGridDialog ----------------------------------
//                  ( Cartesian, Cylindrical, Spherical )

// ============================================================== Constructeur

MakeGridDialog::MakeGridDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f )
: HexaBaseDialog(parent, editmode, f)
{
   setupUi( this );
   _initWidget(editmode);
   rb0->click();// Default : cartesian grid
   uniform_rb->click();
   //	rb0->setFocusProxy( vex_le_rb0 );
   //	rb1->setFocusProxy( center_le_rb1 );
   //	rb2->setFocusProxy( vex_le_rb2 );
   //   setFocusProxy( rb0 );

   _helpFileName = "creategrids.html#guicartgrid";
   connect( rb0, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
   connect( rb1, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
   connect( rb2, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
}


// ============================================================== Destructeur
MakeGridDialog::~MakeGridDialog()
{
}

// ============================================================== _initInputWidget
void MakeGridDialog::_initInputWidget( Mode editmode )
{

   QRegExp rx("");
   QValidator *validator = new QRegExpValidator(rx, this);

//   setProperty( "HexaWidgetType",  QVariant::fromValue(ELEMENTS_TREE) );
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
         radius_lw->setItemDelegate(new HexaPositiveDoubleSpinBoxDelegate(radius_lw));
         radius_lw->setEditTriggers(QAbstractItemView::DoubleClicked);

         angle_lw->setItemDelegate(new HexaAngleDoubleSpinBoxDelegate(angle_lw));
         angle_lw->setEditTriggers(QAbstractItemView::DoubleClicked);

         height_lw->setItemDelegate(new HexaPositiveDoubleSpinBoxDelegate(height_lw));
         height_lw->setEditTriggers(QAbstractItemView::DoubleClicked);

         connect( add_radius_pb, SIGNAL(clicked()), this, SLOT(addRadiusItem()) );
         connect( del_radius_pb, SIGNAL(clicked()), this, SLOT(delRadiusItem()) );
         connect( add_angle_pb, SIGNAL(clicked()), this, SLOT(addAngleItem()) );
         connect( del_angle_pb, SIGNAL(clicked()), this, SLOT(delAngleItem()) );
         connect( add_height_pb, SIGNAL(clicked()), this, SLOT(addHeightItem()) );
         connect( del_height_pb, SIGNAL(clicked()), this, SLOT(delHeightItem()) );
   }

   vex_le_rb0->setReadOnly(true);
   vec_le_rb0->setReadOnly(true);
   vex_le_rb2->setReadOnly(true);
   center_le_rb1->setReadOnly(true);
   base_le_rb1->setReadOnly(true);
   height_le_rb1->setReadOnly(true);

}


// ============================================================== clear
void MakeGridDialog::clear()
{
   vex_le_rb0->clear();
   modelUnregister(vex_le_rb0);

   vec_le_rb0->clear();
   modelUnregister(vec_le_rb0);

   base_le_rb1->clear();
   modelUnregister(base_le_rb1);

   center_le_rb1->clear();
   modelUnregister(center_le_rb1);

   height_le_rb1->clear();
   modelUnregister(height_le_rb1);

   random_param_w->clear();
   modelUnregister(random_param_w);

   radius_lw->clear();
   modelUnregister(radius_lw);

   angle_lw->clear();
   modelUnregister(angle_lw);

   height_lw->clear();
   modelUnregister(height_lw);

   vex_le_rb2->clear();
   modelUnregister(vex_le_rb2);

   modelUnregister(this);
}

// ============================================================== updateHelpFileName
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


// ============================================================== updateButtonBox
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

// ============================================================== addRadiusItem
void MakeGridDialog::addRadiusItem()
{
   QListWidgetItem* previousItem = radius_lw->currentItem();
   QListWidgetItem* newItem      = new QListWidgetItem();

   double defaultValue = 1.;
   if ( previousItem )
      defaultValue = previousItem->data(Qt::EditRole).toDouble();

   newItem->setData(  Qt::EditRole, QVariant(defaultValue) );
   newItem->setFlags( newItem->flags () | Qt::ItemIsEditable);
   radius_lw->addItem(newItem);

   updateButtonBox();
}

// ============================================================== delRadiusItem
void MakeGridDialog::delRadiusItem()
{
   delete radius_lw->currentItem();
   updateButtonBox();
}

// ============================================================== addAngleItem
void MakeGridDialog::addAngleItem()
{
   QListWidgetItem* previousItem = angle_lw->currentItem();
   QListWidgetItem* newItem      = new QListWidgetItem();

   double defaultValue = 180.;
   if ( previousItem )
      defaultValue = previousItem->data(Qt::EditRole).toDouble();

   newItem->setData(  Qt::EditRole, QVariant(defaultValue) );
   newItem->setFlags( newItem->flags () | Qt::ItemIsEditable);
   angle_lw->addItem(newItem);

   updateButtonBox();
}

// ============================================================== delAngleItem
void MakeGridDialog::delAngleItem()
{
   //std::cout << "delAngleItem()" << std::endl;
   delete angle_lw->currentItem();
   updateButtonBox();
}

// ============================================================== addHeightItem
void MakeGridDialog::addHeightItem()
{
   QListWidgetItem* previousItem = height_lw->currentItem();
   QListWidgetItem* newItem      = new QListWidgetItem();

   double defaultValue = 1.;
   if ( previousItem )
      defaultValue = previousItem->data(Qt::EditRole).toDouble();

   newItem->setData(  Qt::EditRole, QVariant(defaultValue) );
   newItem->setFlags( newItem->flags () | Qt::ItemIsEditable);
   height_lw->addItem(newItem);

   updateButtonBox();
}

// ============================================================== delHeightItem
void MakeGridDialog::delHeightItem()
{
   //std::cout << "delHeightItem()" << std::endl;
   delete height_lw->currentItem();
   updateButtonBox();
}

// ============================================================== apply
bool MakeGridDialog::apply(QModelIndex& result)
{
   if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
   _currentObj = NULL;

   SUIT_OverrideCursor wc;
   if ( !getDocumentModel() ) return false;
   const PatternDataModel*    patternDataModel    = getPatternDataModel();
   const PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();
   if ( !patternDataModel || !patternBuilderModel)    return false;

   QModelIndex iNewElts;
   if ( rb0->isChecked() ){ //cartesian
         QModelIndex ivex_rb0 = patternDataModel->mapToSource( _index[vex_le_rb0] );
         QModelIndex ivec_rb0 = patternBuilderModel->mapToSource( _index[vec_le_rb0] );
         long nx = nx_spb_rb0->value();
         long ny = ny_spb_rb0->value();
         long nz = nz_spb_rb0->value();

         if ( ivex_rb0.isValid()
               && ivec_rb0.isValid() ){
               iNewElts = getDocumentModel()->makeCartesian( ivex_rb0, ivec_rb0, nx, ny, nz );
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

                     iNewElts = getDocumentModel()->makeCylindrical( icenter_rb1,
                           ibase_rb1, iheight_rb1,
                           dr, da, dl, nr, na, nl, fill );
               }
               if ( random_rb->isChecked() ){
                     QListWidgetItem* item = NULL;
                     //				QDoubleSpinBox*   spb = NULL;

                     QList<double> radius;
                     QList<double> angles;
                     QList<double> heights;

                     double somme = 0.;
                     for ( int r = 0; r < angle_lw->count(); ++r){
                           item = angle_lw->item(r);
                           angles << item->data(Qt::EditRole).toDouble();
                           somme += item->data(Qt::EditRole).toDouble();
                     }
                     if (somme > 360.01)
                        {
                           SUIT_MessageBox::information( 0,
                                 tr("HEXA_INFO"),
                                 tr("The sum of the picked angles has to be \nless or equal than %1 degrees.").arg(360));
                           return false;
                        }

                     for ( int r = 0; r < radius_lw->count(); ++r){
                           item = radius_lw->item(r);
                           //std::cout << "radius : " << item->data(Qt::EditRole).toDouble()<< std::endl;
                           radius << item->data(Qt::EditRole).toDouble();
                     }

                     for ( int r = 0; r < height_lw->count(); ++r){
                           item = height_lw->item(r);
                           heights << item->data(Qt::EditRole).toDouble();
                     }

                     iNewElts =  getDocumentModel()->makeCylindricals(
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
               iNewElts = getDocumentModel()->makeSpherical( ivex_rb2, radius, nb, k );
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



// ============================================================== Constructeur

MakeCylinderDialog::MakeCylinderDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f )
: HexaBaseDialog(parent, editmode, f)
{
   _helpFileName = "gui_blocks_for_cyl_pipe.html#make-cylinder";
   setupUi( this );
   _initWidget(editmode);
   //   setFocusProxy( cyl_le );
}

// ============================================================== Destructeur
MakeCylinderDialog::~MakeCylinderDialog()
{
}

void MakeCylinderDialog::_initInputWidget( Mode editmode )
{
   QRegExp rx("");
   QValidator *validator = new QRegExpValidator(rx, this);

//   setProperty( "HexaWidgetType",  QVariant::fromValue(ELEMENTS_TREE) );
   installEventFilter(this);

   cyl_le->setProperty( "HexaWidgetType",  QVariant::fromValue(CYLINDER_TREE) );
   vec_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );
   cyl_le->setValidator( validator );
   vec_le->setValidator( validator );
   cyl_le->installEventFilter(this);
   vec_le->installEventFilter(this);

   cyl_le->setReadOnly(true);
   vec_le->setReadOnly(true);
}

// ============================================================== clear
void MakeCylinderDialog::clear()
{
   cyl_le->clear();
   modelUnregister(cyl_le);

   vec_le->clear();
   modelUnregister(vec_le);

   modelUnregister(this);
}


// ============================================================== apply
bool MakeCylinderDialog::apply(QModelIndex& result)
{
   if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
   _currentObj = NULL;

   SUIT_OverrideCursor wc;
   if ( !getDocumentModel() ) return false;
   const PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();
   if ( !patternBuilderModel ) return false;

   QModelIndex iElts;
   QModelIndex icyl = patternBuilderModel->mapToSource( _index[cyl_le] );
   QModelIndex ivec = patternBuilderModel->mapToSource( _index[vec_le] );
   double nr = nr_spb->value();
   double na = na_spb->value();
   double nl = nl_spb->value();

   if ( icyl.isValid()
         && ivec.isValid() ){
         iElts = getDocumentModel()->makeCylinder( icyl, ivec, nr, na, nl );
   }

   if ( !iElts.isValid() ){
         SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE CYLINDER" ) );
         return false;
   }

   // to select/highlight result
   result = patternBuilderModel->mapFromSource(iElts);

   return true;
}


//---------------------------------- MakePipeDialog -----------------------------

// ============================================================== Constructeur

MakePipeDialog::MakePipeDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f )
: HexaBaseDialog(parent, editmode, f)
{
   _helpFileName = "gui_blocks_for_cyl_pipe.html#make-pipe";
   setupUi( this );
   _initWidget(editmode);
   //   setFocusProxy( pipe_le );
}

// ============================================================== Destructeur
MakePipeDialog::~MakePipeDialog()
{
}

// ============================================================== _initInputWidget
void MakePipeDialog::_initInputWidget( Mode editmode )
{
   QRegExp rx("");
   QValidator *validator = new QRegExpValidator(rx, this);

//   setProperty( "HexaWidgetType",  QVariant::fromValue(ELEMENTS_TREE) );
   installEventFilter(this);

   pipe_le->setProperty( "HexaWidgetType", QVariant::fromValue(PIPE_TREE) );
   vec_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );

   pipe_le->setValidator( validator );
   vec_le->setValidator( validator );

   pipe_le->installEventFilter(this);
   vec_le->installEventFilter(this);

   pipe_le->setReadOnly(true);
   vec_le->setReadOnly(true);



}

// ============================================================== clear
void MakePipeDialog::clear()
{
   pipe_le->clear();
   modelUnregister(pipe_le);

   vec_le->clear();
   modelUnregister(vec_le);

   modelUnregister(this);
}


// ============================================================== apply
bool MakePipeDialog::apply(QModelIndex& result)
{
   if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
   _currentObj = NULL;

   SUIT_OverrideCursor wc;
   if ( !getDocumentModel() ) return false;
   const PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();
   if ( !patternBuilderModel ) return false;

   QModelIndex iElts;
   QModelIndex ipipe = patternBuilderModel->mapToSource( _index[pipe_le] );
   QModelIndex ivec  = patternBuilderModel->mapToSource( _index[vec_le] );
   double nr = nr_spb->value();
   double na = na_spb->value();
   double nl = nl_spb->value();

   if ( ipipe.isValid()
         && ivec.isValid() ){
         iElts = getDocumentModel()->makePipe( ipipe, ivec, nr, na, nl );
   }
   if ( !iElts.isValid() ){
         SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE PIPE" ) );
         return false;
   }

   // to select/highlight result
   result = patternBuilderModel->mapFromSource(iElts);

   return true;
}

//---------------------------------- MakeCylindersDialog -----------------------------

// ============================================================== Constructeur

MakeCylindersDialog::MakeCylindersDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f )
: HexaBaseDialog(parent, editmode, f)
{
   _helpFileName = "gui_blocks_for_cyl_pipe.html#make-cylinders";
   setupUi( this );
   _initWidget(editmode);
   //   setFocusProxy( cyl1_le );
}

// ============================================================== Destructeur
MakeCylindersDialog::~MakeCylindersDialog()
{
}

void MakeCylindersDialog::_initInputWidget( Mode editmode )
{
   QRegExp rx("");
   QValidator *validator = new QRegExpValidator(rx, this);

//   setProperty( "HexaWidgetType",  QVariant::fromValue(CROSSELEMENTS_TREE) );
   installEventFilter(this);

   cyl1_le->setProperty( "HexaWidgetType",  QVariant::fromValue(CYLINDER_TREE) );
   cyl2_le->setProperty( "HexaWidgetType",  QVariant::fromValue(CYLINDER_TREE) );

   cyl1_le->setValidator( validator );
   cyl2_le->setValidator( validator );

   cyl1_le->installEventFilter(this);
   cyl2_le->installEventFilter(this);

   cyl1_le->setReadOnly(true);
   cyl2_le->setReadOnly(true);
}

// ============================================================== clear
void MakeCylindersDialog::clear()
{
   cyl1_le->clear();
   modelUnregister(cyl1_le);

   cyl2_le->clear();
   modelUnregister(cyl2_le);

   modelUnregister(this);
}


// ============================================================== apply
bool MakeCylindersDialog::apply(QModelIndex& result)
{
   if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
   _currentObj = NULL;

   SUIT_OverrideCursor wc;
   if ( !getDocumentModel() ) return false;
   const PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();
   if ( !patternBuilderModel ) return false;

   QModelIndex iCrossElts;
   QModelIndex icyl1 = patternBuilderModel->mapToSource( _index[cyl1_le] );
   QModelIndex icyl2 = patternBuilderModel->mapToSource( _index[cyl2_le] );

   if ( icyl1.isValid()
         && icyl2.isValid() ){
         iCrossElts = getDocumentModel()->makeCylinders( icyl1, icyl2 );
   }

   if ( !iCrossElts.isValid() ){
         SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE CYLINDERS" ) );
         return false;
   }

   // to select/highlight result
   result = patternBuilderModel->mapFromSource(iCrossElts);

   return true;
}



//---------------------------------- MakePipesDialog -----------------------------

// ============================================================== Constructeur

MakePipesDialog::MakePipesDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f )
: HexaBaseDialog(parent, editmode, f)
{
   _helpFileName = "gui_blocks_for_cyl_pipe.html#make-pipes";
   setupUi( this );
   _initWidget(editmode);
   //   setFocusProxy( pipe1_le );
}

// ============================================================== Destructeur
MakePipesDialog::~MakePipesDialog()
{
}

// ============================================================== _initInputWidget
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

   pipe1_le->setReadOnly(true);
   pipe2_le->setReadOnly(true);
}


// ============================================================== clear
void MakePipesDialog::clear()
{
   pipe1_le->clear();
   modelUnregister(pipe1_le);

   pipe2_le->clear();
   modelUnregister(pipe2_le);

   modelUnregister(this);
}


// ============================================================== apply
bool MakePipesDialog::apply(QModelIndex& result)
{
   if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
   _currentObj = NULL;

   SUIT_OverrideCursor wc;
   if ( !getDocumentModel() ) return false;
   const PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();
   if ( !patternBuilderModel ) return false;

   QModelIndex iCrossElts;
   QModelIndex ipipe1 = patternBuilderModel->mapToSource( _index[pipe1_le] );
   QModelIndex ipipe2 = patternBuilderModel->mapToSource( _index[pipe2_le] );

   if ( ipipe1.isValid()
         && ipipe2.isValid() ){
         iCrossElts = getDocumentModel()->makePipes( ipipe1, ipipe2 );
   }
   if ( !iCrossElts.isValid() ){
         SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE PIPES" ) );
         return false;
   }
   result = patternBuilderModel->mapFromSource(iCrossElts);

   return true;
}


//---------------------------------- RemoveHexaDialog -----------------------------

// ============================================================== Constructeur

RemoveHexaDialog::RemoveHexaDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f )
: HexaBaseDialog(parent, editmode, f)
{
   _helpFileName = "gui_remove.html";
   setupUi( this );
   _initWidget(editmode);
   //   setFocusProxy( hexa_le );
}

// ============================================================== Destructeur
RemoveHexaDialog::~RemoveHexaDialog()
{
}

// ============================================================== _initInputWidget
void RemoveHexaDialog::_initInputWidget( Mode editmode )
{
   QRegExp rx("");
   QValidator *validator = new QRegExpValidator(rx, this);

   hexa_le->setProperty( "HexaWidgetType",  QVariant::fromValue(HEXA_TREE) );
   hexa_le->setValidator( validator );
   hexa_le->installEventFilter(this);
   hexa_le->setReadOnly(true);
}

// ============================================================== clear
void RemoveHexaDialog::clear()
{
   hexa_le->clear();
   modelUnregister(hexa_le);

   modelUnregister(this);
}


// ============================================================== apply
bool RemoveHexaDialog::apply(QModelIndex& result)
{
   if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
   _currentObj = NULL;

   SUIT_OverrideCursor wc;
   if ( !getDocumentModel() ) return false;
   const PatternDataModel*    patternDataModel = getPatternDataModel();
   if ( !patternDataModel ) return false;

   QModelIndex ihexa = patternDataModel->mapToSource( _index[hexa_le] );

   bool removed = false;
   if ( ihexa.isValid() ){
         if ( connected_cb->isChecked() ){
               removed = getDocumentModel()->removeConnectedHexa( ihexa );
         } else {
               removed = getDocumentModel()->removeHexa( ihexa );
         }
   }

   if ( removed == false ){
         SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT REMOVE HEXA" ) );
         return false;
   }

   clear();

   return true;
}


//---------------------------------- PrismQuadDialog -----------------------------

// ============================================================== Constructeur

PrismQuadDialog::PrismQuadDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f )
: HexaBaseDialog(parent, editmode, f)
{
   _helpFileName = "gui_prism_join_quad.html#prism-quadrangles";
   setupUi( this );
   _initWidget(editmode);
   //	setFocusProxy( quads_lw );
   //	connect( regular_rb, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
   //	connect( irregular_rb, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
   regular_rb->click();
}


// ============================================================== Destructeur
PrismQuadDialog::~PrismQuadDialog()
{
}

// ============================================================== getAssocsVTK
/*
 * Returns elements currently associated to vtk
 */
QModelIndexList PrismQuadDialog::getAssocsVTK()
{
   QModelIndexList assocs;
   QModelIndex iQuad;
   QListWidgetItem* item = NULL;

   //ListWidget content
   const PatternDataModel* patternDataModel = getPatternDataModel();
   if ( !patternDataModel ) return assocs;
   for ( int r = 0; r < quads_lw->count(); ++r ){
         item = quads_lw->item(r);
         iQuad = patternDataModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() );
         if ( iQuad.isValid() )
            assocs << iQuad;
   }
   return assocs;
}


// ============================================================== _initInputWidget
void PrismQuadDialog::_initInputWidget( Mode editmode )
{
   QRegExp rx("");
   QValidator *validator = new QRegExpValidator(rx, this);

//   setProperty( "HexaWidgetType",  QVariant::fromValue(ELEMENTS_TREE) );
   installEventFilter(this);

   vec_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );
   quads_lw->setProperty( "HexaWidgetType",  QVariant::fromValue(QUAD_TREE) );

   vec_le->setValidator( validator );

   vec_le->installEventFilter(this);
   quads_lw->installEventFilter(this);

   if ( editmode != INFO_MODE ){
         QShortcut* delQuadShortcut   = new QShortcut( QKeySequence(Qt::Key_X), quads_lw );
         delQuadShortcut->setContext( Qt::WidgetShortcut );
         connect( delQuadShortcut, SIGNAL(activated()), this, SLOT(removeQuad()) );
   }

   vec_le->setReadOnly(true);

   connect( quads_lw,    SIGNAL(itemSelectionChanged()), this, SLOT(selectElementOfModel()), Qt::UniqueConnection );
   height_lw->setItemDelegate(new HexaPositiveDoubleSpinBoxDelegate(height_lw));
   height_lw->setEditTriggers(QAbstractItemView::DoubleClicked);
   connect( add_height_pb, SIGNAL(clicked()), this, SLOT(addHeightItem()) );
   connect( del_height_pb, SIGNAL(clicked()), this, SLOT(delHeightItem()) );
}

// ============================================================== clear
void PrismQuadDialog::clear()
{
    quads_lw->clear();
    modelUnregister(quads_lw);

    vec_le->clear();
    modelUnregister(vec_le);

    modelUnregister(this);
}

// ============================================================== removeQuad
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

// ============================================================== addHeightItem
void PrismQuadDialog::addHeightItem()
{
   QListWidgetItem* previousItem = height_lw->currentItem();
   QListWidgetItem* newItem      = new QListWidgetItem();

   double defaultValue = 1.;
   if ( previousItem )
      defaultValue = previousItem->data(Qt::EditRole).toDouble();

   newItem->setData(  Qt::EditRole, QVariant(defaultValue) );
   newItem->setFlags( newItem->flags () | Qt::ItemIsEditable);
   height_lw->addItem(newItem);

   //	updateButtonBox();
}

// ============================================================== delHeightItem
void PrismQuadDialog::delHeightItem()
{
   delete height_lw->currentItem();
   //	updateButtonBox();
}



// ============================================================== apply
bool PrismQuadDialog::apply(QModelIndex& result)
{
   if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
   _currentObj = NULL;

   SUIT_OverrideCursor wc;
   if ( !getDocumentModel() ) return false;
   const PatternDataModel*    patternDataModel    = getPatternDataModel();
   const PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();
   if ( !patternDataModel || !patternBuilderModel)    return false;

   /////
   QModelIndexList iquads = getIndexList(quads_lw);
   QModelIndex ivec  = patternBuilderModel->mapToSource( _index[vec_le] );

   QModelIndex iElts;
   if ( ivec.isValid() ){
         if (regular_rb->isChecked())
            {
               int nb = nb_spb->value();
               if ( iquads.count() == 1 ){
                     iElts = getDocumentModel()->prismQuad( iquads[0], ivec, nb );
               } else if ( iquads.count() > 1 ){
                     iElts = getDocumentModel()->prismQuads( iquads, ivec, nb );
               }
            }
         else //irregular_rb is checked
            {
               QVector<double> heights;
               QListWidgetItem* item = NULL;
               unsigned int nbItems = height_lw->count();

               for ( int r = 0; r < nbItems; ++r){
                     item = height_lw->item(r);
                     heights << item->data(Qt::EditRole).toDouble();
               }
               if (iquads.count() == 0 || heights.size() == 0)
                  {
                     SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ),
                           tr( "Parameters are invalid!" ) );
                     return false;
                  }
               iElts = getDocumentModel()->prismQuads( iquads, ivec, heights.toStdVector()/*, nb*/ );
            }
   }

   if ( !iElts.isValid() ){
         SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT PRISM QUAD(S)" ) );
         return false;
   }

   result = patternBuilderModel->mapFromSource(iElts);

   return true;
}

//---------------------------------- JoinQuadDialog -----------------------------

// ============================================================== Constructeur

JoinQuadDialog::JoinQuadDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f )
: HexaBaseDialog(parent, editmode, f)
{
   _helpFileName = "gui_prism_join_quad.html#join-quadrangles";
   setupUi( this );
   _initWidget(editmode);
   //   setFocusProxy( quads_lw );
}


// ============================================================== Destructeur
JoinQuadDialog::~JoinQuadDialog()
{
}

// ============================================================== getAssocsVTK
/*
 * Returns elements currently associated to vtk
 */
QModelIndexList JoinQuadDialog::getAssocsVTK()
{
   QModelIndexList assocs;
   QModelIndex iQuad;
   QListWidgetItem* item = NULL;

   if (getPatternDataSelectionModel() == NULL) return assocs;

   //ListWidget content
   const PatternDataModel* patternDataModel = getPatternDataModel();
   if ( !patternDataModel ) return assocs;
   for ( int r = 0; r < quads_lw->count(); ++r ){
         item = quads_lw->item(r);
         //		iQuad = patternDataModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() ); //unsafe
         iQuad = getPatternDataSelectionModel()->indexBy(HEXA_DATA_ROLE, item->data(LW_DATA_ROLE));
         if ( iQuad.isValid() ) assocs << iQuad;
   }
   return assocs;
}

// ============================================================== _initInputWidget
void JoinQuadDialog::_initInputWidget( Mode editmode )
{
   QRegExp rx("");
   QValidator *validator = new QRegExpValidator(rx, this);

//   setProperty( "HexaWidgetType",  QVariant::fromValue(ELEMENTS_TREE) );
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

   vex0_le->setReadOnly(true);
   vex1_le->setReadOnly(true);
   vex2_le->setReadOnly(true);
   vex3_le->setReadOnly(true);
   quad_dest_le->setReadOnly(true);

   _currentObj = quads_lw;

   connect( quads_lw,    SIGNAL(itemSelectionChanged()), this, SLOT(selectElementOfModel()), Qt::UniqueConnection );
}


// ============================================================== clear
void JoinQuadDialog::clear()
{
   quads_lw->clear();
   modelUnregister(quads_lw);

   vex0_le->clear();
   modelUnregister(vex0_le);

   vex2_le->clear();
   modelUnregister(vex2_le);

   quad_dest_le->clear();
   modelUnregister(quad_dest_le);

   vex1_le->clear();
   modelUnregister(vex1_le);

   vex3_le->clear();
   modelUnregister(vex3_le);

   modelUnregister(this);
}


// ============================================================== removeQuad
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


// ============================================================== apply
bool JoinQuadDialog::apply(QModelIndex& result)
{
   if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
   _currentObj = NULL;

   SUIT_OverrideCursor wc;
   if ( !getDocumentModel() ) return false;
   const PatternDataModel*    patternDataModel    = getPatternDataModel();
   const PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();
   if ( !patternDataModel || !patternBuilderModel)    return false;

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
               iElts = getDocumentModel()->joinQuad( iquads[0], iquaddest,
                     ivex0, ivex1, ivex2, ivex3,
                     nb );
         } else if ( iquads.count() > 1 ){
               iElts = getDocumentModel()->joinQuads( iquads, iquaddest,
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



// ------------------------- MergeDialog ----------------------------------

// ============================================================== Constructeur

MergeDialog::MergeDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f )
: HexaBaseDialog(parent, editmode, f)
{
   setupUi( this );
   _initWidget(editmode);
   //	rb0->setFocusProxy( v0_le_rb0 );
   //	rb1->setFocusProxy( e0_le_rb1 );
   //	rb2->setFocusProxy( q0_le_rb2 );
   //   setFocusProxy( rb0 );
   rb0->click();

   _helpFileName = "gui_merge_elmts.html#merge-two-vertices";
   connect( rb0, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
   connect( rb1, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
   connect( rb2, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );

}

// ============================================================== Destructeur
MergeDialog::~MergeDialog()
{
}

// ============================================================== _initInputWidget
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

   v0_le_rb0->setReadOnly(true);
   v1_le_rb0->setReadOnly(true);

   v0_le_rb1->setReadOnly(true);
   v1_le_rb1->setReadOnly(true);
   e0_le_rb1->setReadOnly(true);
   e1_le_rb1->setReadOnly(true);

   v0_le_rb2->setReadOnly(true);
   v1_le_rb2->setReadOnly(true);
   v2_le_rb2->setReadOnly(true);
   v3_le_rb2->setReadOnly(true);
   q0_le_rb2->setReadOnly(true);
   q1_le_rb2->setReadOnly(true);


}


// ============================================================== clear
void MergeDialog::clear()
{
   v0_le_rb0->clear();
   modelUnregister(v0_le_rb0);

   v1_le_rb0->clear();
   modelUnregister(v1_le_rb0);

   e0_le_rb1->clear();
   modelUnregister(e0_le_rb1);

   e1_le_rb1->clear();
   modelUnregister(e1_le_rb1);

   v0_le_rb1->clear();
   modelUnregister(v0_le_rb1);

   v1_le_rb1->clear();
   modelUnregister(v1_le_rb1);

   q0_le_rb2->clear();
   modelUnregister(q0_le_rb2);

   q1_le_rb2->clear();
   modelUnregister(q1_le_rb2);

   v0_le_rb2->clear();
   modelUnregister(v0_le_rb2);

   v1_le_rb2->clear();
   modelUnregister(v1_le_rb2);

   v2_le_rb2->clear();
   modelUnregister(v2_le_rb2);

   v3_le_rb2->clear();
   modelUnregister(v3_le_rb2);

   modelUnregister(this);
}

// ============================================================== updateHelpFileName
void MergeDialog::updateHelpFileName()
{
   if ( sender() == rb0 ){
         _helpFileName = "gui_merge_elmts.html#merge-two-vertices";
   } else if ( sender() == rb1 ){
         _helpFileName = "gui_merge_elmts.html#merge-two-edges";
   } else if ( sender() == rb2 ){
         _helpFileName = "gui_merge_elmts.html#merge-two-quadrangles";
   }
}


// ============================================================== apply
bool MergeDialog::apply(QModelIndex& result)
{
   if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
   _currentObj = NULL;

   if (getDocumentModel() == NULL) return false;
   SUIT_OverrideCursor wc;
   const PatternDataModel*    patternDataModel = getPatternDataModel();
   if ( !patternDataModel ) return false;

   bool merged = false;

   if ( rb0->isChecked() ){
         QModelIndex iv0 = patternDataModel->mapToSource( _index[v0_le_rb0] );
         QModelIndex iv1 = patternDataModel->mapToSource( _index[v1_le_rb0] );

         if ( iv0.isValid()
               && iv1.isValid() ){
               merged = getDocumentModel()->mergeVertices( iv0, iv1 );
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
               merged =  getDocumentModel()->mergeEdges( ie0, ie1, iv0, iv1);
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
               merged =  getDocumentModel()->mergeQuads( iq0, iq1,
                     iv0, iv1, iv2, iv3 );
         }
   }

   if ( merged == false ){
         SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MERGE" ) );
         return false;
   }

   return true;
}


// ------------------------- DisconnectDialog ----------------------------------

// ============================================================== Constructeur

DisconnectDialog::DisconnectDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f )
: HexaBaseDialog(parent, editmode, f)
{
   setupUi( this );
   _initWidget(editmode);
   //	rb0->setFocusProxy( v_le_rb0 );
   //	rb1->setFocusProxy( e_le_rb1 );
   //	rb2->setFocusProxy( q_le_rb2 );
   //	rb3->setFocusProxy( d_edges_lw);
   rb0->click();
   //   setFocusProxy( rb0 );

   _helpFileName = "gui_disc_elmts.html#disconnect-a-vertex";

   connect( rb0, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
   connect( rb1, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
   connect( rb2, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
   connect( rb3, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
}

// ============================================================== Destructeur
DisconnectDialog::~DisconnectDialog()
{
}

// ============================================================== _initInputWidget
void DisconnectDialog::_initInputWidget( Mode editmode )
{
   QRegExp rx("");
   QValidator *validator = new QRegExpValidator(rx, this);

//   setProperty( "HexaWidgetType",  QVariant::fromValue(ELEMENTS_TREE) );
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

   //edges
   d_edges_lw->setProperty( "HexaWidgetType",  QVariant::fromValue(EDGE_TREE) );
   hexas_lw->setProperty( "HexaWidgetType",  QVariant::fromValue(HEXA_TREE) );
   d_edges_lw->installEventFilter(this);
   hexas_lw->installEventFilter(this);

   QShortcut* delEdgeShortcut = new QShortcut(QKeySequence(/*Qt::Key_Delete*/Qt::Key_X/*Qt::Key_Alt*//*Qt::Key_Space*/), d_edges_lw);
   QShortcut* delHexaShortcut = new QShortcut(QKeySequence(/*Qt::Key_Delete*/Qt::Key_X/*Qt::Key_Alt*//*Qt::Key_Space*/), hexas_lw);
   delEdgeShortcut->setContext( Qt::WidgetShortcut );
   delHexaShortcut->setContext( Qt::WidgetShortcut );

   v_le_rb0->setReadOnly(true);
   h_le_rb0->setReadOnly(true);

   e_le_rb1->setReadOnly(true);
   h_le_rb1->setReadOnly(true);

   q_le_rb2->setReadOnly(true);
   h_le_rb2->setReadOnly(true);

   connect( d_edges_lw,   SIGNAL(itemSelectionChanged()), this, SLOT(selectElementOfModel()), Qt::UniqueConnection );
   connect( hexas_lw,   SIGNAL(itemSelectionChanged()), this, SLOT(selectElementOfModel()), Qt::UniqueConnection );
   connect( delEdgeShortcut, SIGNAL(activated()), this, SLOT(deleteEdgeItem()) );
   connect( delHexaShortcut, SIGNAL(activated()), this, SLOT(deleteHexaItem()) );

}

// =============================================== getAssocsVTK
/*
 * Returns elements in the list
 */
QModelIndexList DisconnectDialog::getAssocsVTK()
{
   QModelIndexList iElts;
   QModelIndex index;
   QListWidgetItem* item = NULL;

   const PatternDataModel* patternDataModel = getPatternDataModel();
   if (patternDataModel == NULL) return iElts;
   if (rb3->isChecked())
   {
       //ListWidget content
       if (HEXABLOCKGUI::currentDocGView->getSelectionMode() == EDGE_TREE)
       {
           for ( int r = 0; r < d_edges_lw->count(); ++r )
           {
               item = d_edges_lw->item(r);
               index = patternDataModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() );
               if ( index.isValid() )
                   iElts << index;
           }
       }
       else if (HEXABLOCKGUI::currentDocGView->getSelectionMode() == HEXA_TREE)
       {
           for ( int r = 0; r < hexas_lw->count(); ++r )
           {
               item = hexas_lw->item(r);
               index = patternDataModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() );
               if ( index.isValid() )
                   iElts << index;
           }
       }
       return iElts;
   }
   else
       return iElts;
}

// ============================================================== clear
void DisconnectDialog::clear()
{
   v_le_rb0->clear();
   modelUnregister(v_le_rb0);

   h_le_rb0->clear();
   modelUnregister(h_le_rb0);

   e_le_rb1->clear();
   modelUnregister(e_le_rb1);

   h_le_rb1->clear();
   modelUnregister(h_le_rb1);

   d_edges_lw->clear();
   modelUnregister(d_edges_lw);

   hexas_lw->clear();
   modelUnregister(hexas_lw);

   q_le_rb2->clear();
   modelUnregister(q_le_rb2);

   h_le_rb2->clear();
   modelUnregister(h_le_rb2);

   modelUnregister(this);
}

// ============================================================== deleteEdgeItem
void DisconnectDialog::deleteEdgeItem()
{
   delete d_edges_lw->currentItem();
}

// ============================================================== deleteHexaItem
void DisconnectDialog::deleteHexaItem()
{
   delete hexas_lw->currentItem();
}

// ============================================================== updateHelpFileName
void DisconnectDialog::updateHelpFileName()
{
   if ( sender() == rb0 ){
         _helpFileName = "gui_disc_elmts.html#disconnect-a-vertex";
   } else if ( sender() == rb1 ){
         _helpFileName = "gui_disc_elmts.html#disconnect-an-edge";
   } else if ( sender() == rb2 ){
         _helpFileName = "gui_disc_elmts.html#disconnect-a-quadrangle";
   } else if (sender() == rb3 ){
         _helpFileName = "gui_disc_elmts.html#disconnect-edges";
   }
}

// ============================================================== apply
bool DisconnectDialog::apply(QModelIndex& result)
{
   if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
   _currentObj = NULL;
   if (getDocumentModel() == NULL) return false;
   const PatternDataModel*    patternDataModel    = getPatternDataModel();
   const PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();
   if ( !patternDataModel || !patternBuilderModel)    return false;

   SUIT_OverrideCursor wc;
   QModelIndex iElts;

   if ( rb0->isChecked() ){
         QModelIndex ihexa = patternDataModel->mapToSource( _index[h_le_rb0] );
         QModelIndex ivex  = patternDataModel->mapToSource( _index[v_le_rb0] );

         if ( ihexa.isValid()
               && ivex.isValid() ){
               iElts = getDocumentModel()->disconnectVertex( ihexa, ivex );
         }
   } else if ( rb1->isChecked() ){
         QModelIndex ihexa = patternDataModel->mapToSource( _index[h_le_rb1] );
         QModelIndex iedge = patternDataModel->mapToSource( _index[e_le_rb1] );

         if ( ihexa.isValid()
               && iedge.isValid() ){
               iElts = getDocumentModel()->disconnectEdge( ihexa, iedge );
         }
   } else if ( rb2->isChecked() ){
         QModelIndex ihexa = patternDataModel->mapToSource( _index[h_le_rb2] );
         QModelIndex iquad = patternDataModel->mapToSource( _index[q_le_rb2] );

         if ( ihexa.isValid()
               && iquad.isValid() ){
               iElts = getDocumentModel()->disconnectQuad( ihexa, iquad );
         }
   } else if ( rb3->isChecked() ){ //

         QModelIndex iedge, ihexa;
         QModelIndexList iedges, ihexas;
         QListWidgetItem* item = NULL;

         //Liste des edges
         for (int r = 0; r < d_edges_lw->count(); ++r){
               item = d_edges_lw->item(r);
               iedge = patternDataModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() );
               if ( iedge.isValid() )
                  iedges << iedge;
         }

         //Liste des hexas
         for (int r = 0; r < hexas_lw->count(); ++r){
               item = hexas_lw->item(r);
               ihexa = patternDataModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() );
               if ( ihexa.isValid() )
                  ihexas << ihexa;
         }

         iElts = getDocumentModel()->disconnectEdges( ihexas, iedges );
   }

   if ( !iElts.isValid() ){
         SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT DISCONNECT" ) );
         return false;
   }

   result = patternBuilderModel->mapFromSource(iElts);

   return true;
}




// ------------------------- CutEdgeDialog ----------------------------------

// ============================================================== Constructeur

CutEdgeDialog::CutEdgeDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f ):
								                                                                                                                  HexaBaseDialog(parent, editmode, f)
{
   _helpFileName = "gui_cut_hexa.html";
   setupUi( this );
   _initWidget(editmode);
   //   setFocusProxy( e_le );
}

// ============================================================== Destructeur
CutEdgeDialog::~CutEdgeDialog()
{
}

// ============================================================== _initInputWidget
void CutEdgeDialog::_initInputWidget( Mode editmode )
{
   QRegExp rx("");
   QValidator *validator = new QRegExpValidator(rx, this);

//   setProperty( "HexaWidgetType",  QVariant::fromValue(ELEMENTS_TREE) );
   installEventFilter(this);

   e_le->setProperty( "HexaWidgetType",  QVariant::fromValue(EDGE_TREE) );
   e_le->setValidator( validator );
   e_le->installEventFilter(this);

   e_le->setReadOnly(true);
}

// ============================================================== clear
void CutEdgeDialog::clear()
{
   e_le->clear();
   modelUnregister(e_le);

   modelUnregister(this);
}


// ============================================================== apply
bool CutEdgeDialog::apply(QModelIndex& result)
{
   if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
   _currentObj = NULL;

   SUIT_OverrideCursor wc;
   if ( !getDocumentModel() ) return false;
   const PatternDataModel*    patternDataModel    = getPatternDataModel();
   const PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();
   if ( !patternDataModel || !patternBuilderModel)    return false;

   int nbCut = nb_cut_spb->value();
   QModelIndex iedge = patternDataModel->mapToSource( _index[e_le] );

   QModelIndex iElts;
   if ( iedge.isValid() ){
         iElts = getDocumentModel()->cutEdge( iedge, nbCut );
   }

   if ( !iElts.isValid() ){
         SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT CUT EDGE" ) );
         return false;
   }

   result = patternBuilderModel->mapFromSource(iElts);

   //Update the line edit
   QVariant invalid;
   e_le->setProperty("QModelIndex",  invalid );

   QModelIndex invalidIndex;
   _index[e_le] = invalidIndex;

   clear();

   return true;
}

// // ------------------------- MakeTransformationDialog ----------------------------------

// ============================================================== Constructeur

MakeTransformationDialog::MakeTransformationDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f )
: HexaBaseDialog(parent, editmode, f)
{
   setupUi( this );
   _initWidget(editmode);
   //	rb0->setFocusProxy( elts_le_rb0 );
   //	rb1->setFocusProxy( elts_le_rb1 );
   //	rb2->setFocusProxy( elts_le_rb2 );
   //	setFocusProxy( rb0 );
   rb0->click();

   _helpFileName = "gui_make_elmts.html#make-elements-by-translation";
   connect( rb0, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
   connect( rb1, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
   connect( rb2, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
}

// ============================================================== Destructeur
MakeTransformationDialog::~MakeTransformationDialog()
{
}

// ============================================================== _initInputWidget
void MakeTransformationDialog::_initInputWidget( Mode editmode )
{
   QRegExp rx("");
   QValidator *validator = new QRegExpValidator(rx, this);

//   setProperty( "HexaWidgetType",  QVariant::fromValue(ELEMENTS_TREE) );
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

   vec_le_rb0->setReadOnly(true);
   elts_le_rb0->setReadOnly(true);

   vex_le_rb1->setReadOnly(true);
   elts_le_rb1->setReadOnly(true);

   vex_le_rb2->setReadOnly(true);
   vec_le_rb2->setReadOnly(true);
   elts_le_rb2->setReadOnly(true);

}

// ============================================================== clear
void MakeTransformationDialog::clear()
{
    elts_le_rb0->clear();
    modelUnregister(elts_le_rb0);

    vec_le_rb0->clear();
    modelUnregister(vec_le_rb0);

    elts_le_rb1->clear();
    modelUnregister(elts_le_rb1);

    vex_le_rb1->clear();
    modelUnregister(vex_le_rb1);

    elts_le_rb2->clear();
    modelUnregister(elts_le_rb2);

    vex_le_rb2->clear();
    modelUnregister(vex_le_rb2);

    vec_le_rb2->clear();
    modelUnregister(vec_le_rb2);

    modelUnregister(this);
}


// ============================================================== updateHelpFileName
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

// ============================================================== apply
bool MakeTransformationDialog::apply(QModelIndex& result)
{
   if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
   _currentObj = NULL;

   SUIT_OverrideCursor wc;
   if (getDocumentModel() == NULL) return false;
   const PatternDataModel*    patternDataModel    = getPatternDataModel();
   const PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();
   if ( !patternDataModel || !patternBuilderModel)    return false;

   QModelIndex iNewElts;

   if ( rb0->isChecked() ){
         QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb0] );
         QModelIndex  ivec = patternBuilderModel->mapToSource( _index[vec_le_rb0] );

         if ( ielts.isValid()
               && ivec.isValid() )
            iNewElts = getDocumentModel()->makeTranslation( ielts, ivec );

   } else if ( rb1->isChecked() ){
         QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb1] );
         QModelIndex  ivex = patternDataModel->mapToSource( _index[vex_le_rb1] );
         double          k = k_spb->value();

         if ( ielts.isValid()
               && ivex.isValid() )
            iNewElts = getDocumentModel()->makeScale( ielts, ivex, k );

   } else if ( rb2->isChecked() ){
         QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb2] );
         QModelIndex  ivex = patternDataModel->mapToSource( _index[vex_le_rb2] );
         QModelIndex  ivec = patternBuilderModel->mapToSource( _index[vec_le_rb2] );
         double      angle = angle_spb->value();

         if ( ielts.isValid()
               && ivex.isValid()
               && ivec.isValid() )
            iNewElts = getDocumentModel()->makeRotation( ielts, ivex, ivec, angle );
   }

   if ( !iNewElts.isValid() ){
         SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE TRANSFORMATION" ) );
         return false;
   }

   result = patternBuilderModel->mapFromSource(iNewElts);

   return true;
}

// // ------------------------- MakeSymmetryDialog ----------------------------------

// ============================================================== Constructeur

MakeSymmetryDialog::MakeSymmetryDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f ):
								                                                                                                                  HexaBaseDialog(parent, editmode, f)
{
   setupUi( this );
   _initWidget(editmode);
   //	rb0->setFocusProxy( elts_le_rb0 );
   //	rb1->setFocusProxy( elts_le_rb1 );
   //	rb2->setFocusProxy( elts_le_rb2 );
   rb0->click();
   //     setFocusProxy( rb0 );

   _helpFileName = "gui_make_symmetry.html#make-elements-by-point-symmetry";
   connect( rb0, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
   connect( rb1, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
   connect( rb2, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
}

// ============================================================== Destructeur
MakeSymmetryDialog::~MakeSymmetryDialog()
{
}

// ============================================================== _initInputWidget
void MakeSymmetryDialog::_initInputWidget( Mode editmode )
{
   QRegExp rx("");
   QValidator *validator = new QRegExpValidator(rx, this);

//   setProperty( "HexaWidgetType",  QVariant::fromValue(ELEMENTS_TREE) );
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

   vex_le_rb0->setReadOnly(true);
   elts_le_rb0->setReadOnly(true);
   vex_le_rb1->setReadOnly(true);
   vec_le_rb1->setReadOnly(true);
   elts_le_rb1->setReadOnly(true);
   vex_le_rb2->setReadOnly(true);
   vec_le_rb2->setReadOnly(true);
   elts_le_rb2->setReadOnly(true);
}

// ============================================================== clear
void MakeSymmetryDialog::clear()
{
    elts_le_rb0->clear();
    modelUnregister(elts_le_rb0);

    vex_le_rb0->clear();
    modelUnregister(vex_le_rb0);

    elts_le_rb1->clear();
    modelUnregister(elts_le_rb1);

    vex_le_rb1->clear();
    modelUnregister(vex_le_rb1);

    vec_le_rb1->clear();
    modelUnregister(vec_le_rb1);

    elts_le_rb2->clear();
    modelUnregister(elts_le_rb2);

    vex_le_rb2->clear();
    modelUnregister(vex_le_rb2);

    vec_le_rb2->clear();
    modelUnregister(vec_le_rb2);

    modelUnregister(this);
}

// ============================================================== updateHelpFileName
void MakeSymmetryDialog::updateHelpFileName()
{
   if ( sender() == rb0 ){
         _helpFileName = "gui_make_symmetry.html#make-elements-by-point-symmetry";
   } else if ( sender() == rb1 ){
         _helpFileName = "gui_make_symmetry.html#make-elements-by-line-symmetry";
   } else if ( sender() == rb2 ){
         _helpFileName = "gui_make_symmetry.html#make-elements-by-plane-symmetry";
   }
}

// ============================================================== apply
bool MakeSymmetryDialog::apply(QModelIndex& result)
{
   if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
   _currentObj = NULL;
   if (getDocumentModel() == NULL) return false;
   const PatternDataModel*    patternDataModel    = getPatternDataModel();
   const PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();
   if ( !patternDataModel || !patternBuilderModel)    return false;

   SUIT_OverrideCursor wc;
   QModelIndex iNewElts;

   if ( rb0->isChecked() ){
         QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb0] );
         QModelIndex  ivex = patternDataModel->mapToSource( _index[vex_le_rb0] );

         if ( ielts.isValid()
               && ivex.isValid() )
            iNewElts = getDocumentModel()->makeSymmetryPoint( ielts, ivex );


   } else if ( rb1->isChecked() ){
         QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb1] );
         QModelIndex  ivex = patternDataModel->mapToSource( _index[vex_le_rb1] );
         QModelIndex  ivec = patternBuilderModel->mapToSource( _index[vec_le_rb1] );

         if ( ielts.isValid()
               && ivex.isValid()
               && ivec.isValid() )
            iNewElts = getDocumentModel()->makeSymmetryLine( ielts, ivex, ivec );

   } else if ( rb2->isChecked() ){
         QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb2] );
         QModelIndex  ivex = patternDataModel->mapToSource( _index[vex_le_rb2] );
         QModelIndex  ivec = patternBuilderModel->mapToSource( _index[vec_le_rb2] );

         if ( ielts.isValid()
               && ivex.isValid()
               && ivec.isValid() )
            iNewElts = getDocumentModel()->makeSymmetryPlane( ielts, ivex, ivec );
   }

   if ( !iNewElts.isValid() ){
         SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE TRANSFORMATION" ) );
         return false;
   }

   result = patternBuilderModel->mapFromSource(iNewElts);

   return true;
}

// // ------------------------- PerformTransformationDialog ----------------------------------
// ============================================================== Constructeur

PerformTransformationDialog::PerformTransformationDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f ):
										                                                                                                                  HexaBaseDialog(parent, editmode, f)
{
   setupUi( this );
   _initWidget(editmode);
   //	rb0->setFocusProxy( elts_le_rb0 );
   //	rb1->setFocusProxy( elts_le_rb1 );
   //	rb2->setFocusProxy( elts_le_rb2 );
   //   setFocusProxy( rb0 );
   rb0->click();

   _helpFileName = "gui_modify_elmts.html#modify-elements-by-translation";
   connect( rb0, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
   connect( rb1, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
   connect( rb2, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
}

// ============================================================== Destructeur
PerformTransformationDialog::~PerformTransformationDialog()
{
}

// ============================================================== _initInputWidget
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

   vec_le_rb0->setReadOnly(true);
   elts_le_rb0->setReadOnly(true);
   vex_le_rb1->setReadOnly(true);
   elts_le_rb1->setReadOnly(true);
   vex_le_rb2->setReadOnly(true);
   vec_le_rb2->setReadOnly(true);
   elts_le_rb2->setReadOnly(true);

}

// ============================================================== clear
void PerformTransformationDialog::clear()
{
    vec_le_rb0->clear();
    modelUnregister(vec_le_rb0);

    elts_le_rb0->clear();
    modelUnregister(elts_le_rb0);

    vex_le_rb1->clear();
    modelUnregister(vex_le_rb1);

    elts_le_rb1->clear();
    modelUnregister(elts_le_rb1);

    vex_le_rb2->clear();
    modelUnregister(vex_le_rb2);

    vec_le_rb2->clear();
    modelUnregister(vec_le_rb2);

    elts_le_rb2->clear();
    modelUnregister(elts_le_rb2);

    modelUnregister(this);
}

// ============================================================== updateHelpFileName
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


// ============================================================== apply
bool PerformTransformationDialog::apply(QModelIndex& result)
{
   if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
   _currentObj = NULL;

   SUIT_OverrideCursor wc;
   if (getDocumentModel() == NULL) return false;
   const PatternDataModel*    patternDataModel    = getPatternDataModel();
   const PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();
   if ( !patternDataModel || !patternBuilderModel)    return false;

   bool performed = false;

   if ( rb0->isChecked() ){
         QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb0] );
         QModelIndex  ivec = patternBuilderModel->mapToSource( _index[vec_le_rb0] );

         if ( ielts.isValid()
               && ivec.isValid() )
            performed = getDocumentModel()->performTranslation( ielts, ivec );

   } else if ( rb1->isChecked() ){
         QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb1] );
         QModelIndex  ivex = patternDataModel->mapToSource( _index[vex_le_rb1] );
         double          k = k_spb->value();

         if ( ielts.isValid()
               && ivex.isValid() )
            performed = getDocumentModel()->performScale( ielts, ivex, k );

   } else if ( rb2->isChecked() ){
         QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb2] );
         QModelIndex  ivex = patternDataModel->mapToSource( _index[vex_le_rb2] );
         QModelIndex  ivec = patternBuilderModel->mapToSource( _index[vec_le_rb2] );
         double      angle = angle_spb->value();

         if ( ielts.isValid()
               && ivex.isValid()
               && ivec.isValid() )
            performed = getDocumentModel()->performRotation( ielts, ivex, ivec, angle );
   }

   if ( performed == false){
         SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT PERFORM TRANSFORMATION" ) );
         return false;
   }


   return true;
}


// // ------------------------- PerformSymmetryDialog ----------------------------------

// ============================================================== Constructeur

PerformSymmetryDialog::PerformSymmetryDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f )
: HexaBaseDialog(parent, editmode, f)
{
   setupUi( this );
   _initWidget( editmode );
   //	rb0->setFocusProxy( elts_le_rb0 );
   //	rb1->setFocusProxy( elts_le_rb1 );
   //	rb2->setFocusProxy( elts_le_rb2 );
   //   setFocusProxy( rb0 );
   rb0->click();

   _helpFileName = "gui_modify_symmetry.html#modify-elements-by-point-symmetry";
   connect( rb0, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
   connect( rb1, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
   connect( rb2, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
}

// ============================================================== Destructeur
PerformSymmetryDialog::~PerformSymmetryDialog()
{
}

// ============================================================== _initInputWidget
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

   vex_le_rb0->setReadOnly(true);
   elts_le_rb0->setReadOnly(true);
   vex_le_rb1->setReadOnly(true);
   vec_le_rb1->setReadOnly(true);
   elts_le_rb1->setReadOnly(true);
   vex_le_rb2->setReadOnly(true);
   vec_le_rb2->setReadOnly(true);
   elts_le_rb2->setReadOnly(true);

}

// ============================================================== clear
void PerformSymmetryDialog::clear()
{
   vex_le_rb0->clear();
   modelUnregister(vex_le_rb0);

   elts_le_rb0->clear();
   modelUnregister(elts_le_rb0);

   elts_le_rb1->clear();
   modelUnregister(elts_le_rb1);

   vex_le_rb1->clear();
   modelUnregister(vex_le_rb1);

   vec_le_rb1->clear();
   modelUnregister(vec_le_rb1);

   elts_le_rb2->clear();
   modelUnregister(elts_le_rb2);

   vex_le_rb2->clear();
   modelUnregister(vex_le_rb2);

   vec_le_rb2->clear();
   modelUnregister(vec_le_rb2);

   modelUnregister(this);
}

// ============================================================== updateHelpFileName
void PerformSymmetryDialog::updateHelpFileName()
{
   if ( sender() == rb0 ){
         _helpFileName = "gui_modify_symmetry.html#modify-elements-by-point-symmetry";
   } else if ( sender() == rb1 ){
         _helpFileName = "gui_modify_symmetry.html#modify-elements-by-line-symmetry";
   } else if ( sender() == rb2 ){
         _helpFileName = "gui_modify_symmetry.html#modify-elements-by-plane-symmetry";
   }
}

// ============================================================== apply
bool PerformSymmetryDialog::apply(QModelIndex& result)
{
   if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
   _currentObj = NULL;

   SUIT_OverrideCursor wc;
   if (getDocumentModel() == NULL) return false;
   const PatternDataModel*    patternDataModel    = getPatternDataModel();
   const PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();
   if ( !patternDataModel || !patternBuilderModel)    return false;

   bool performed = false;

   if ( rb0->isChecked() ){
         QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb0] );
         QModelIndex  ivex = patternDataModel->mapToSource( _index[vex_le_rb0] );

         if ( ielts.isValid()
               && ivex.isValid() )
            performed = getDocumentModel()->performSymmetryPoint( ielts, ivex );

   } else if ( rb1->isChecked() ){
         QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb1] );
         QModelIndex  ivex = patternDataModel->mapToSource( _index[vex_le_rb1] );
         QModelIndex  ivec = patternBuilderModel->mapToSource( _index[vec_le_rb1] );

         if ( ielts.isValid()
               && ivex.isValid()
               && ivec.isValid() )
            performed = getDocumentModel()->performSymmetryLine( ielts, ivex, ivec );

   } else if ( rb2->isChecked() ){
         QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb2] );
         QModelIndex  ivex = patternDataModel->mapToSource( _index[vex_le_rb2] );
         QModelIndex  ivec = patternBuilderModel->mapToSource( _index[vec_le_rb2] );

         if ( ielts.isValid()
               && ivex.isValid()
               && ivec.isValid() )
            performed = getDocumentModel()->performSymmetryPlane( ielts, ivex, ivec );
   }

   if ( performed == false ){
         SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT PERFORM SYMMETRY" ) );
         return false;
   }

   return true;
}



// // ------------------------- EdgeAssocDialog ----------------------------------

// ============================================================== Constructeur

EdgeAssocDialog::EdgeAssocDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f ):
										                                                                                                                  HexaBaseDialog( parent, editmode, f )
{
   _helpFileName ="gui_asso_quad_to_geom.html#one-edge";
   setupUi( this );
   _initWidget(editmode);
   _initViewManager();
   //	setFocusProxy( edges_lw );

   myLine.nullify();

   _currentParameter = 0.;
   single_rb->click();
}

// ============================================================== Destructeur
EdgeAssocDialog::~EdgeAssocDialog()
{
   //   disconnect( delEdgeShortcut, SIGNAL(activated()), this, SLOT(deleteEdgeItem()) );
   //   disconnect( delLineShortcut, SIGNAL(activated()), this, SLOT(deleteLineItem()) );
   disconnect( HEXABLOCKGUI::selectionMgr(), SIGNAL(currentSelectionChanged()), this, SLOT(addLine()) );
   disconnect( pstart_spb, SIGNAL(valueChanged(double)), this, SLOT( pstartChanged(double)) );
   disconnect( pend_spb,   SIGNAL(valueChanged(double)), this, SLOT( pendChanged(double)) );
}

// ============================================================== close
void EdgeAssocDialog::close()
{
   HEXABLOCKGUI::assocInProgress = false;
   HexaBaseDialog::close();
}

// ============================================================== _initInputWidget
void EdgeAssocDialog::_initInputWidget( Mode editmode )
{
   QRegExp rx("");
   QValidator *validator = new QRegExpValidator(rx, this);

   //model
   first_vex_le->setProperty( "HexaWidgetType", QVariant::fromValue(VERTEX_TREE) );
   first_vex_le->installEventFilter(this);
   first_vex_le->setValidator( validator );

   single_edge_le->setProperty( "HexaWidgetType", QVariant::fromValue(EDGE_TREE) );
   single_edge_le->installEventFilter(this);

   edges_lw->setProperty( "HexaWidgetType", QVariant::fromValue(EDGE_TREE) );
   edges_lw->installEventFilter(this);


   //geom
   lines_lw->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_EDGE) );
   lines_lw->installEventFilter(this);

   single_line_le->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_EDGE) );
   single_line_le->installEventFilter(this);


   QShortcut* delEdgeShortcut = new QShortcut(QKeySequence(/*Qt::Key_Delete*/Qt::Key_X), edges_lw);
   QShortcut* delLineShortcut = new QShortcut(QKeySequence(/*Qt::Key_Delete*/Qt::Key_X), lines_lw);
   delLineShortcut->setContext( Qt::WidgetWithChildrenShortcut );
   delEdgeShortcut->setContext( Qt::WidgetWithChildrenShortcut );

   pend_spb->setValue(1.);

   first_vex_le->setReadOnly(true);
   single_edge_le->setReadOnly(true);
   single_line_le->setReadOnly(true);

   connect( delEdgeShortcut, SIGNAL(activated()), this, SLOT(deleteEdgeItem()) );
   connect( delLineShortcut, SIGNAL(activated()), this, SLOT(deleteLineItem()) );
   connect( pstart_spb, SIGNAL(valueChanged(double)), this, SLOT( pstartChanged(double)) );
   connect( pend_spb,   SIGNAL(valueChanged(double)), this, SLOT( pendChanged(double)) );
   connect( edges_lw,   SIGNAL(itemSelectionChanged()), this, SLOT(selectElementOfModel()), Qt::UniqueConnection );
   connect( lines_lw,   SIGNAL(itemSelectionChanged()), this, SLOT(selectElementOfGeom()), Qt::UniqueConnection );
   connect( single_rb,   SIGNAL(clicked()), this, SLOT( refreshHighlight()), Qt::UniqueConnection );
   connect( multiple_rb,   SIGNAL(clicked()), this, SLOT(refreshHighlight()), Qt::UniqueConnection );
   connect( single_rb,   SIGNAL(clicked()), this, SLOT( updateHelpFileName() ) );
   connect( multiple_rb,   SIGNAL(clicked()), this, SLOT( updateHelpFileName() ) );
}

// ============================================================== updateHelpFileName
void EdgeAssocDialog::updateHelpFileName()
{
   if ( sender() == single_rb ){
         _helpFileName = "gui_asso_quad_to_geom.html#one-edge";
   } else if ( sender() == multiple_rb ){
         _helpFileName = "gui_asso_quad_to_geom.html#line";
   }
}


// ============================================================== getAssocsVTK
/*
 * Returns elements currently being associated in vtk side
 */
QModelIndexList EdgeAssocDialog::getAssocsVTK()
{
   QModelIndexList assocs;
   QModelIndex iEdge;
   QListWidgetItem* item = NULL;

   if (getPatternDataSelectionModel() == NULL) return assocs;
   if (single_rb->isChecked())
      {
         //LineEdit content
         QVariant v = single_edge_le->property("QModelIndex");
         if ( !v.isValid() ) return assocs;
         assocs << v.value<QModelIndex>();
         return assocs;
      }
   else if (multiple_rb->isChecked())
      {
         //ListWidget content
         const PatternDataModel* patternDataModel = getPatternDataModel();
         if ( !patternDataModel ) return assocs;
         for ( int r = 0; r < edges_lw->count(); ++r){
               item = edges_lw->item(r);
               iEdge = patternDataModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() );
               if ( iEdge.isValid() ) assocs << iEdge;
         }
         return assocs;
      }
   else return assocs;
}

// ============================================================== getAssocsGEOM
/*
 * Returns elements currently being associated in geom side
 */
QMultiMap<QString, int> EdgeAssocDialog::getAssocsGEOM()
{
   QMultiMap<QString, int> assocs;
   QListWidgetItem* item = NULL;
   DocumentModel::GeomObj geomObj;

   if (single_rb->isChecked())
      {
         //LineEdit content
         QVariant v = single_line_le->property("GeomObj");
         if ( !v.isValid() ) return assocs;
         geomObj = v.value<DocumentModel::GeomObj>();
         assocs.insert( geomObj.entry, geomObj.subid.toInt() );
         return assocs;
      }
   else if (multiple_rb->isChecked())
      {
         //ListWidget content
         unsigned int nbEdges = lines_lw->count();
         for ( int r = 0; r < nbEdges; ++r){
               item = lines_lw->item(r);
               geomObj = item->data(LW_ASSOC_ROLE).value<DocumentModel::GeomObj>();
               assocs.insert( geomObj.entry, geomObj.subid.toInt() );
         }
         return assocs;
      }
   else return assocs;
}

// ============================================================== selectElementOfGeom

/*Highlight in the OCC view selected elements in a listwidget,
 *  or an element in a line edit.*/

void EdgeAssocDialog::selectElementOfGeom()
{
   MESSAGE("HexaBaseDialog::selectElementOfGeom()");

   QListWidget* currentListWidget = dynamic_cast<QListWidget*>( sender() );
   if ( !currentListWidget ) return;

   _selectionMutex = true;

   QMultiMap<QString, int> geomElts;
   DocumentModel::GeomObj geomObj;
   QList<QListWidgetItem *> sel = currentListWidget->selectedItems();
   foreach ( QListWidgetItem *item, sel ){
      geomObj = item->data(LW_ASSOC_ROLE).value<DocumentModel::GeomObj>();
      geomElts.insert( geomObj.entry, geomObj.subid.toInt() );
   }
   _selectionMutex = false;

   //highlight geom selected elts
   if (geomElts.size() > 0 && getPatternDataSelectionModel() != NULL)
       HEXABLOCKGUI::currentOccGView->highlight(geomElts);

   MESSAGE("}");
}

// ============================================================== clear
void EdgeAssocDialog::clear()
{
   first_vex_le->clear();
   modelUnregister(first_vex_le);

   edges_lw->clear();
   modelUnregister(edges_lw);

   lines_lw->clear();

   single_edge_le->clear();
   modelUnregister(single_edge_le);

   single_line_le->clear();
   QVariant invalid;
   single_line_le->setProperty("GeomObj", invalid);

   modelUnregister(this);

//   single_edge_le->setText("");
//   single_line_le->setText("");
}

// ============================================================== deleteEdgeItem
void EdgeAssocDialog::deleteEdgeItem()
{
   delete edges_lw->currentItem();
}

// ============================================================== deleteLineItem
void EdgeAssocDialog::deleteLineItem()
{
   delete lines_lw->currentItem();
}


// ============================================================== setGeomEngine
void EdgeAssocDialog::setGeomEngine( GEOM::GEOM_Gen_var geomEngine )
{
   _geomEngine = geomEngine;
}

// ============================================================== onWindowActivated
void EdgeAssocDialog::onWindowActivated(SUIT_ViewManager* vm)
{
   QString vmType = vm->getType();
   if ( (vmType == SVTK_Viewer::Type()) || (vmType == VTKViewer_Viewer::Type()) ){

         if (single_rb->isChecked())
            single_edge_le->setFocus();
//         else if (multiple_rb->isChecked())
//            {
//               if (close_cb->isChecked())
//                  first_vex_le->setFocus();
//               else
//                  edges_lw->setFocus();
//            }
   }
   else if ( vmType == OCCViewer_Viewer::Type() ){
         globalSelection2(HEXABLOCKGUI::currentOccGView->getViewWindow()); // close local contexts, if any
//         localSelection(GEOM::GEOM_Object::_nil(), TopAbs_EDGE);
         localSelection(HEXABLOCKGUI::currentOccGView->getViewWindow(), TopAbs_EDGE);

         if (single_rb->isChecked())
            single_line_le->setFocus();
         else if (multiple_rb->isChecked())
            lines_lw->setFocus();
   }
}

// ============================================================== createOperation
GEOM::GEOM_IOperations_ptr EdgeAssocDialog::createOperation()
{
   return _geomEngine->GetIBasicOperations(getStudyId());
}

// ============================================================== execute
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

// ============================================================== pstartChanged
void EdgeAssocDialog::pstartChanged( double val )
{
   MESSAGE("pstartChanged("<<val<<") { ");
   QListWidgetItem* lineItem = lines_lw->item( 0 );
   if (!lineItem) return;
   //   MESSAGE("data : "<<lineItem->text().toStdString());
   _currentLine = lineItem->data(LW_GEOM_OBJ_ROLE).value<GEOM::GeomObjPtr>();
   _currentParameter = pstart_spb->value();
//   displayPreview(true);
   MESSAGE("}pstartChanged()");
}

// ============================================================== pendChanged
void EdgeAssocDialog::pendChanged( double val )
{
   MESSAGE("pendChanged("<<val<<") { ");
   //   if ( _lastLine->_is_nil() ) return;
   QListWidgetItem* lineItem = lines_lw->item( lines_lw->count()-1 );
   if (!lineItem) return;
   //   MESSAGE("data : "<<lineItem->text().toStdString());
   _currentLine      = lineItem->data(LW_GEOM_OBJ_ROLE).value<GEOM::GeomObjPtr>();
   _currentParameter = pend_spb->value();
//   displayPreview(true);
   MESSAGE("}pendChanged()");
}

// ============================================================== onCurrentSelectionChanged
void EdgeAssocDialog::onCurrentSelectionChanged()
{
   if ( !isVisible() )
      {
         highlightSelectedAssocs();
         return;
      }

   SUIT_ViewWindow* window = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();

   bool isOCC = (window && window->getViewManager()->getType() == OCCViewer_Viewer::Type());
   if (!isOCC || !getDocumentModel())
      {
         highlightSelectedAssocs();
         return;
      }

   GEOM::GeomObjPtr aSelectedObject = getSelected(TopAbs_EDGE);
   DocumentModel::GeomObj * aLine = getDocumentModel()->convertToGeomObj(aSelectedObject);
   if ( aLine == NULL )
      {
         highlightSelectedAssocs();
         return;
      }

   if (single_rb->isChecked()) {
         single_line_le->setText(aLine->name);
         single_line_le->setProperty("GeomObj", QVariant::fromValue<DocumentModel::GeomObj>(*aLine));
         myLine = aSelectedObject;
         setFocusToNextField();
   }
   else if( multiple_rb->isChecked() ) {
         QList<QListWidgetItem *> twice = lines_lw->findItems( aLine->name, Qt::MatchExactly);
         if ( twice.count() == 0 ){
               QListWidgetItem* item  = new QListWidgetItem( aLine->name );
               item->setData(  LW_ASSOC_ROLE, QVariant::fromValue<DocumentModel::GeomObj>(*aLine) );
               item->setData(  LW_GEOM_OBJ_ROLE, QVariant::fromValue<GEOM::GeomObjPtr>(aSelectedObject) );
               lines_lw->addItem(item);
         }
   }
   highlightSelectedAssocs();
}

// ============================================================== apply
bool EdgeAssocDialog::apply(QModelIndex& result)
{
   if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
   _currentObj = NULL;

   SUIT_OverrideCursor wc;
   bool assocOk = false;

   highlightSelectedAssocs();

   if (!getDocumentModel()) return false;
   const PatternDataModel* patternDataModel = getPatternDataModel();
   if ( !patternDataModel ) return false;

   QModelIndex     iEdge;
   QModelIndexList iEdges;
   QListWidgetItem* item = NULL;
   QList<DocumentModel::GeomObj> assocs;
   DocumentModel::GeomObj aLine;
   DocumentModel::GeomObj *aLinePtr;

   if (single_rb->isChecked()){ //Single edge and/or line association

         iEdge = patternDataModel->mapToSource( _index[single_edge_le] );
         GEOM::GeomObjPtr aSelectedObject = myLine;
         aLinePtr = getDocumentModel()->convertToGeomObj(aSelectedObject);
         if  ( !iEdge.isValid() || aLinePtr == NULL)
            {
               SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE EDGE ASSOCIATION" ) );
               highlightSelectedAssocs();
               return false;
            }
         aLinePtr->start = pstart_spb->value();
         aLinePtr->end   = pend_spb->value();
         getDocumentModel()->addAssociation(iEdge, *aLinePtr);
         assocOk = true;
   }
   else { //Multiple edge and/or line association

         // edges
         iEdges = getIndexList(edges_lw);
         if  (iEdges.count() == 0 || lines_lw->count() == 0)
            {
               SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE EDGE ASSOCIATION" ) );
               highlightSelectedAssocs();
               return false;
            }

         // lines
         for (int r = 0; r < lines_lw->count(); ++r){
               item = lines_lw->item(r);
               aLine = item->data(LW_ASSOC_ROLE).value<DocumentModel::GeomObj>();
               MESSAGE(" assocs => " << aLine.name.toStdString() );
               MESSAGE(" assocs => " << aLine.brep.toStdString() );
               assocs << aLine;
         }

         if ( close_cb->isChecked() ){ //closed line
               QModelIndex iFirstVertex = patternDataModel->mapToSource( _index[first_vex_le] );
               if  ( !iFirstVertex.isValid() )
                  {
                     SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE EDGE ASSOCIATION" ) );
                     highlightSelectedAssocs();
                     return false;
                  }
               bool inv = inverse_cb->isChecked();
               assocOk = getDocumentModel()->associateClosedLine( iFirstVertex, iEdges, assocs, pstart_spb->value(), inv );
         } else
            //opened line
            assocOk = getDocumentModel()->associateOpenedLine( iEdges, assocs, pstart_spb->value(), pend_spb->value() );
   }

   if ( !assocOk ){
         SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE EDGE ASSOCIATION" ) );
         highlightSelectedAssocs();
         return false;
   }

   highlightSelectedAssocs();
   return true;
}

// ------------------------- QuadAssocDialog ----------------------------------
// ============================================================== Constructeur
QuadAssocDialog::QuadAssocDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f ):
								                                                                                                                  HexaBaseDialog(parent, editmode, f)
{
   _helpFileName = "gui_asso_quad_to_geom.html#associate-to-a-face-or-a-shell-of-the-geometry";
   setupUi( this );
   _initWidget(editmode);
   _initViewManager();
   //	setFocusProxy( quad_le );
}

// ============================================================== Destructeur
QuadAssocDialog::~QuadAssocDialog()
{
   disconnect( _delFaceShortcut, SIGNAL(activated()), this, SLOT(deleteFaceItem()) );
   disconnect( HEXABLOCKGUI::selectionMgr(), SIGNAL(currentSelectionChanged()), this, SLOT(addFace()) );
   delete _delFaceShortcut;
}

// ============================================================== close
void QuadAssocDialog::close()
{
   HEXABLOCKGUI::assocInProgress = false;
   HexaBaseDialog::close();
}

// ============================================================== _initInputWidget
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

   quad_le->setReadOnly(true);

   connect( _delFaceShortcut, SIGNAL(activated()), this, SLOT(deleteFaceItem()) );
   connect( faces_lw,   SIGNAL(itemSelectionChanged()), this, SLOT(selectElementOfGeom()), Qt::UniqueConnection );
}

// ============================================================== getAssocsVTK
/*
 * Returns elements currently being associated in vtk side
 */
QModelIndexList QuadAssocDialog::getAssocsVTK()
{
   QModelIndexList assocs;
   QModelIndex iQuad;

   //LineEdit content
   QVariant v = quad_le->property("QModelIndex");
   if ( !v.isValid() )
      return assocs;
   assocs << v.value<QModelIndex>();
   return assocs;
}

// ============================================================== getAssocsGEOM
/*
 * Returns elements currently being associated in geom side
 */
QMultiMap<QString, int> QuadAssocDialog::getAssocsGEOM()
{
   QMultiMap<QString, int> assocs;
   QListWidgetItem* item = NULL;
   DocumentModel::GeomObj geomObj;

   //ListWidget content
   unsigned int nbFaces = faces_lw->count();
   for ( int r = 0; r < nbFaces; ++r){
         item = faces_lw->item(r);
         geomObj = item->data(LW_ASSOC_ROLE).value<DocumentModel::GeomObj>();
         assocs.insert( geomObj.entry, geomObj.subid.toInt() );
   }
   return assocs;
}

// ============================================================== selectElementOfGeom
/*Highlight in the OCC view selected elements in a listwidget,
 *  or an element in a line edit.*/

void QuadAssocDialog::selectElementOfGeom()
{
   MESSAGE("HexaBaseDialog::selectElementOfGeom()");

   QListWidget* currentListWidget = dynamic_cast<QListWidget*>( sender() );
   if ( !currentListWidget ) return;

   _selectionMutex = true;

   QMultiMap<QString, int> geomElts;
   DocumentModel::GeomObj geomObj;
   QList<QListWidgetItem *> sel = currentListWidget->selectedItems();
   foreach ( QListWidgetItem *item, sel ){
      geomObj = item->data(LW_ASSOC_ROLE).value<DocumentModel::GeomObj>();
      geomElts.insert( geomObj.entry, geomObj.subid.toInt() );
   }
   _selectionMutex = false;

   //highlight geom selected elts
   if (geomElts.size() > 0 && getPatternDataSelectionModel() != NULL)
       HEXABLOCKGUI::currentOccGView->highlight(geomElts);

   MESSAGE("}");
}

void QuadAssocDialog::clear()
{
   quad_le->clear();
   modelUnregister(quad_le);

   faces_lw->clear();

   modelUnregister(this);
}

// ============================================================== onCurrentSelectionChanged
void QuadAssocDialog::onCurrentSelectionChanged()
{
   if ( !isVisible() )
      {
         highlightSelectedAssocs();
         return;
      }

   SUIT_ViewWindow* window = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();

   bool isOCC = (window && window->getViewManager()->getType() == OCCViewer_Viewer::Type());
   //   bool isVTK = (window && window->getViewManager()->getType() == SVTK_Viewer::Type());
   if (!isOCC || !getDocumentModel())
      {
         //		if (HEXABLOCKGUI::currentOccView != NULL) HEXABLOCKGUI::currentOccView->setFocus();
         highlightSelectedAssocs();
         return;
      }

   GEOM::GeomObjPtr aSelectedObject = getSelected(TopAbs_FACE);
   DocumentModel::GeomObj* aFace = getDocumentModel()->convertToGeomObj(aSelectedObject);
   if (aFace == NULL)
      {
         highlightSelectedAssocs();
         return;
      }

   QList<QListWidgetItem *> twice = faces_lw->findItems( aFace->name, Qt::MatchExactly);
   if ( twice.count() == 0 ){
         QListWidgetItem* item = new QListWidgetItem( aFace->name );
         item->setData(  LW_ASSOC_ROLE, QVariant::fromValue<DocumentModel::GeomObj>(*aFace) );
         faces_lw->addItem(item);
         //     faces_lw->setCurrentRow( faces_lw->count() - 1 );
   }

   highlightSelectedAssocs();
}

// ============================================================== onWindowActivated
void QuadAssocDialog::onWindowActivated(SUIT_ViewManager* vm)
{
   QString vmType = vm->getType();
   if ( (vmType == SVTK_Viewer::Type()) || (vmType == VTKViewer_Viewer::Type()) )
      quad_le->setFocus();
   else if ( vmType == OCCViewer_Viewer::Type() ){
         globalSelection2(HEXABLOCKGUI::currentOccGView->getViewWindow()); // close local contexts, if any
//         localSelection(GEOM::GEOM_Object::_nil(), TopAbs_FACE);
         localSelection(HEXABLOCKGUI::currentOccGView->getViewWindow(), TopAbs_FACE);
         faces_lw->setFocus();
   }
}

// ============================================================== deleteFaceItem
void QuadAssocDialog::deleteFaceItem()
{
   delete faces_lw->currentItem();
}


// ============================================================== apply
bool QuadAssocDialog::apply(QModelIndex& result)
{
   highlightSelectedAssocs();
   if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
   _currentObj = NULL;

   SUIT_OverrideCursor wc;
   if ( !getDocumentModel() ) return false;
   if ( !getPatternDataSelectionModel() ) return false;
   const PatternDataModel* patternDataModel = getPatternDataModel();
   if ( !patternDataModel )           return false;

   // quad
   QModelIndex iQuad = patternDataModel->mapToSource( _index[quad_le] );
   if  (!iQuad.isValid()|| faces_lw->count() == 0)
      {
         SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE QUAD ASSOCIATION" ) );
         highlightSelectedAssocs();
         return false;
      }
   // faces
   QListWidgetItem* item = NULL;
   DocumentModel::GeomObj aFace;
   for ( int r = 0; r < faces_lw->count(); ++r ){
         item = faces_lw->item(r);
         aFace = item->data(LW_ASSOC_ROLE).value<DocumentModel::GeomObj>();
         getDocumentModel()->addAssociation( iQuad, aFace );
   }

   result = patternDataModel->mapFromSource(iQuad);

   highlightSelectedAssocs();
   return true;
}



// ------------------------- GroupDialog ----------------------------------

// ============================================================== Constructeur

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

// ============================================================== Destructeur
GroupDialog::~GroupDialog()
{
}

// ============================================================== getAssocsVTK
/*
 * Returns elements currently being associated in vtk side
 */
QModelIndexList GroupDialog::getAssocsVTK()
{
   QModelIndexList assocs;
   QModelIndex iItem;
   QListWidgetItem* item = NULL;
   if (getPatternDataSelectionModel() == NULL) return assocs;
   //ListWidget content
   const PatternDataModel* patternDataModel = getPatternDataModel();
   if ( !patternDataModel ) return assocs;
   for ( int r = 0; r < eltBase_lw->count(); ++r){
         item = eltBase_lw->item(r);
         iItem = patternDataModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() );
         if ( iItem.isValid() ) assocs << iItem;
   }
   return assocs;
}

// ============================================================== _initInputWidget
void GroupDialog::_initInputWidget( Mode editmode )
{
//   setProperty( "HexaWidgetType",  QVariant::fromValue(GROUP_TREE) );
   installEventFilter(this);

//   name_le->setProperty( "HexaWidgetType",  QVariant::fromValue(GROUP_TREE) );
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

   onKindChanged( kind_cb->currentIndex() );
   eltBase_lw->installEventFilter(this);

   if ( editmode != INFO_MODE ){
         QShortcut* delEltShortcut = new QShortcut( QKeySequence(Qt::Key_X), eltBase_lw );
         delEltShortcut->setContext( Qt::WidgetShortcut );
         connect(delEltShortcut,   SIGNAL(activated()), this, SLOT(removeEltBase()));
         connect(kind_cb,  SIGNAL(activated(int)), this, SLOT(onKindChanged(int)) );
   }

   connect(eltBase_lw,    SIGNAL(itemSelectionChanged()), this, SLOT(selectElementOfModel()), Qt::UniqueConnection);
}

// ============================================================== clear
void GroupDialog::clear()
{
   name_le->clear();

   eltBase_lw->clear();
   modelUnregister(eltBase_lw);

   modelUnregister(this);
}

// ============================================================== onKindChanged
void GroupDialog::onKindChanged(int index)
{
   //   onKind
   MESSAGE("GroupDialog::onKindChanged(" << index << ") ");
   switch ( kind_cb->itemData(index).toInt() ){
   case HEXA_NS::HexaCell:
   case HEXA_NS::HexaNode:
      eltBase_lw->setProperty("HexaWidgetType", QVariant::fromValue(HEXA_TREE));
      break;

   case HEXA_NS::QuadCell:
   case HEXA_NS::QuadNode:
      eltBase_lw->setProperty("HexaWidgetType", QVariant::fromValue(QUAD_TREE));
      break;

   case HEXA_NS::EdgeCell:
   case HEXA_NS::EdgeNode: eltBase_lw->setProperty("HexaWidgetType", QVariant::fromValue(EDGE_TREE));
   break;

   case HEXA_NS::VertexNode:
      eltBase_lw->setProperty("HexaWidgetType", QVariant::fromValue(VERTEX_TREE));
      break;

   default:Q_ASSERT(false);
   }
   eltBase_lw->clear();
   eltBase_lw->setFocus();
}

// ============================================================== setValue
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

   if ( !getPatternDataSelectionModel() ) return;
   if ( !getGroupsSelectionModel() ) return;

   QModelIndex iGroup = getGroupsSelectionModel()->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(g) );
   name_le->setProperty( "QModelIndex",  QVariant::fromValue(iGroup) );


   for ( int nr = 0; nr < g->countElement(); ++nr ){
         eltBase = g->getElement( nr );
         switch ( g->getKind() ){
         case HEXA_NS::HexaCell: case HEXA_NS::HexaNode: v = QVariant::fromValue( (HEXA_NS::Hexa *)eltBase ); break;
         case HEXA_NS::QuadCell: case HEXA_NS::QuadNode: v = QVariant::fromValue( (HEXA_NS::Quad *)eltBase ); break;
         case HEXA_NS::EdgeCell: case HEXA_NS::EdgeNode: v = QVariant::fromValue( (HEXA_NS::Edge *)eltBase ); break;
         case HEXA_NS::VertexNode: v = QVariant::fromValue( (HEXA_NS::Vertex *)eltBase ); break;
         }
         iEltBase  = getPatternDataSelectionModel()->indexBy( HEXA_DATA_ROLE, v);
         if ( iEltBase.isValid() ){
               item = new QListWidgetItem( eltBase->getName() );
               item->setData(  LW_QMODELINDEX_ROLE, QVariant::fromValue<QModelIndex>(iEltBase) );
               item->setData(LW_DATA_ROLE, iEltBase.data(HEXA_DATA_ROLE));
               eltBase_lw->addItem( item );
         }
   }

   _value = g;
}

// ============================================================== getValue
HEXA_NS::Group* GroupDialog::getValue()
{
   return _value;
}


// ============================================================== removeEltBase
void GroupDialog::removeEltBase()
{
   QListWidgetItem *item = eltBase_lw->currentItem();

   if (item) {
         int r = eltBase_lw->row(item);
         eltBase_lw->takeItem(r);
         delete item;
   }

}

// ============================================================== apply
bool GroupDialog::apply(QModelIndex& result)
{
   if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
   _currentObj = NULL;

   SUIT_OverrideCursor wc;

   if ( !getDocumentModel() ) return false;
   const PatternDataModel* patternDataModel = getPatternDataModel();
   const GroupsModel*      groupsModel = getGroupsModel();
   if (patternDataModel == NULL || groupsModel == NULL) return false;

   QString               grpName = name_le->text();
   DocumentModel::Group  grpKind = static_cast<DocumentModel::Group>( kind_cb->itemData( kind_cb->currentIndex() ).toInt());
   QModelIndex iGrp;
   if ( _value == NULL ){ // create group
         iGrp = getDocumentModel()->addGroup( grpName, grpKind );
   } else {
         QModelIndexList iGrps = getDocumentModel()->match(
               getDocumentModel()->index(0, 0),
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
   getDocumentModel()->clearGroupElement(iGrp);
   for ( int r = 0; r < eltBase_lw->count(); ++r){
         item     = eltBase_lw->item(r);
         iEltBase = patternDataModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() );
         if ( iEltBase.isValid() )
            eltAdded = getDocumentModel()->addGroupElement( iGrp, iEltBase );
         //    if ( eltAdded == false ){
         //      //SUIT_MessageBox::information( this, tr( "HEXA_INFO" ), tr( "ELEMENT NOT ADDED : %1" ).arg( iEltBase.data().toString() ));
         //    }
   }

   QString newName = name_le->text();
   if ( !newName.isEmpty() )/*{*/
      getDocumentModel()->setName( iGrp, newName );

   HEXA_NS::Group* hGroup = getDocumentModel()->data(iGrp, HEXA_DATA_ROLE).value<HEXA_NS::Group *>();
   if (hGroup != NULL)
      {
         char buffer [16];
         name_le->setText(hGroup->getNextName(buffer));
      }

   result = groupsModel->mapFromSource(iGrp);

   return true;
}



// ------------------------- LawDialog ----------------------------------

// ============================================================== Constructeur

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

// ============================================================== Destructeur
LawDialog::~LawDialog()
{
}

// ============================================================== _initInputWidget
void LawDialog::_initInputWidget( Mode editmode )
{
   QRegExp rx("");

//   setProperty( "HexaWidgetType",  QVariant::fromValue(LAW_TREE) );
   installEventFilter(this);

//   name_le->setProperty( "HexaWidgetType",  QVariant::fromValue(LAW_TREE) );
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

// ============================================================== clear
void LawDialog::clear()
{
   name_le->clear();

   modelUnregister(this);
}

// ============================================================== setValue
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

// ============================================================== getValue
HEXA_NS::Law* LawDialog::getValue()
{
   return _value;
}


// ============================================================== apply
bool LawDialog::apply(QModelIndex& result)
{
   if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
   _currentObj = NULL;

   SUIT_OverrideCursor wc;
   if ( !getDocumentModel() ) return false;

   const MeshModel* meshModel = getMeshModel();
   if (meshModel == NULL) return false;

   QString lawName = name_le->text();
   int     nbnodes = nb_nodes_spb->value();
   double  coeff   = coeff_spb->value();
   DocumentModel::KindLaw  lawKind = static_cast<DocumentModel::KindLaw>( kind_cb->itemData( kind_cb->currentIndex() ).toInt());

   QModelIndex iLaw;
   if ( _value == NULL ){ // create Law
         iLaw = getDocumentModel()->addLaw( lawName, nbnodes );
   } else {
         QModelIndexList iLaws = getDocumentModel()->match(
               getDocumentModel()->index(0, 0),
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
   bool setOk = getDocumentModel()->setLaw( iLaw, nbnodes, coeff, lawKind );
   if ( !setOk ){
         SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT ADD LAW" ) );
         return false;
   }

   QString newName = name_le->text();
   if ( !newName.isEmpty() )/*{*/
      getDocumentModel()->setName( iLaw, newName );

   HEXA_NS::Law* hLaw = getDocumentModel()->data(iLaw, HEXA_DATA_ROLE).value<HEXA_NS::Law *>();
   if (hLaw != NULL)
      {
         char buffer [16];
         name_le->setText(hLaw->getNextName(buffer));
      }

   result = meshModel->mapFromSource(iLaw);

   return true;
}




// ------------------------- PropagationDialog ----------------------------------

// ============================================================== Constructeur

PropagationDialog::PropagationDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f )
: HexaBaseDialog(parent, editmode, f),
  _value(NULL)
{
   _helpFileName = "gui_propag.html";
   setupUi( this );
   _initWidget(editmode);
   //   setFocusProxy( law_le );
   //	setFocusProxy( propagations_lw );

   if ( editmode == INFO_MODE ){
         setWindowTitle( tr("Propagation Information") );
         if (groupBox_2)
            delete groupBox_2;
   } else if ( editmode == UPDATE_MODE ){
         setWindowTitle( tr("Propagation Modification") );
         if (groupBox_2)
            delete groupBox_2;
   } else if ( editmode == NEW_MODE ){
         setWindowTitle( tr("Propagation(s) Setting") );
   }
}


// ============================================================== Destructeur
PropagationDialog::~PropagationDialog()
{
}


// ============================================================== _initInputWidget
void PropagationDialog::_initInputWidget( Mode editmode )
{
   QRegExp rx("");
   QValidator *validator = new QRegExpValidator(rx, this);

//   setProperty( "HexaWidgetType",  QVariant::fromValue(PROPAGATION_TREE) );
   installEventFilter(this);


   law_le->setProperty( "HexaWidgetType",  QVariant::fromValue(LAW_TREE) );
   law_le->installEventFilter(this);
   law_le->setValidator( validator );

   propagations_lw->setProperty( "HexaWidgetType",  QVariant::fromValue(PROPAGATION_TREE) );
   propagations_lw->installEventFilter(this);

   QShortcut* delPropagationShortcut = new QShortcut(QKeySequence(/*Qt::Key_Delete*/Qt::Key_X/*Qt::Key_Alt*//*Qt::Key_Space*/), propagations_lw);
   delPropagationShortcut->setContext( Qt::WidgetShortcut );

   law_le->setReadOnly(true);

   connect( delPropagationShortcut, SIGNAL(activated()), this, SLOT(deletePropagationItem()) );
   connect( propagations_lw,   SIGNAL(itemSelectionChanged()), this, SLOT(selectElementOfModel()), Qt::UniqueConnection );
}


// ============================================================== clear
void PropagationDialog::clear()
{
   propagations_lw->clear();
   modelUnregister(propagations_lw);

   law_le->clear();
   modelUnregister(law_le);

   modelUnregister(this);
}

// ============================================================== deletePropagationItem
void PropagationDialog::deletePropagationItem()
{
   delete propagations_lw->currentItem();
}


// ============================================================== setValue
void PropagationDialog::setValue(HEXA_NS::Propagation* p)
{
   if (getMeshSelectionModel() == NULL) return;
   HEXA_NS::Law* l = p->getLaw();
   bool way = p->getWay();

   // propagation
   QModelIndex ip = getMeshSelectionModel()->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(p) );
   setProperty( "QModelIndex",  QVariant::fromValue<QModelIndex>(ip) );

   // law on propagation
   if ( l != NULL ){
         law_le->setText( l->getName() );
         QModelIndex il = getMeshSelectionModel()->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(l) );
         law_le->setProperty( "QModelIndex",  QVariant::fromValue<QModelIndex>(il) );
   }

   // way of propagation
   way_cb->setChecked(way);

   _value = p;
}

// ============================================================== selectElementOfModel

/*Selects in the model (treeview) elements selected in a listwidget,
 *  or an element in a line edit.*/

void PropagationDialog::selectElementOfModel()
{
   MESSAGE("HexaBaseDialog::selectElementOfModel()");
   if (!getMeshSelectionModel()) return;

   QListWidget* currentListWidget = dynamic_cast<QListWidget*>( sender() );
   if ( !currentListWidget ) return;

   _selectionMutex = true;

   QList<QListWidgetItem *> sel = currentListWidget->selectedItems();
   QModelIndex index;
   getMeshSelectionModel()->clearSelection();
   foreach ( QListWidgetItem *item, sel ){
      index = item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>();
      if ( index.isValid() )
         {
            MESSAGE( "*  selecting the element : " << index.data().toString().toStdString() );
            getMeshSelectionModel()->select( index, QItemSelectionModel::Select );
         }
   }
   _selectionMutex = false;

   MESSAGE("}");
}


// ============================================================== getValue
HEXA_NS::Propagation* PropagationDialog::getValue()
{
   return _value;
}


// ============================================================== apply
bool PropagationDialog::apply(QModelIndex& result)
{
   if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
   _currentObj = NULL;

   bool isOk = false;
   SUIT_OverrideCursor wc;
   if ( !getDocumentModel() ) return false;
   const MeshModel* meshModel = getMeshModel();
   if (meshModel == NULL) return false;

   bool way = way_cb->isChecked();
   QListWidgetItem* item = NULL;

   QModelIndex iPropagation;
   QModelIndex iLaw = meshModel->mapToSource( law_le->property("QModelIndex").value<QModelIndex>() );
   if (!iLaw.isValid() ){
         SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT SET PROPAGATION" ) );
         return false;
   }
   MESSAGE("iLaw.isValid()         => " << iLaw.isValid() );

   for (int r = 0; r < propagations_lw->count(); ++r){
         item = propagations_lw->item(r);
         iPropagation = meshModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() );
         if ( !iPropagation.isValid() ){
               SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT SET PROPAGATION" ) );
               return false;
         }
         MESSAGE("iPropagation.isValid() => " << iPropagation.isValid() );

         //fill it and select it
         isOk = getDocumentModel()->setPropagation( iPropagation, iLaw, way );
         if ( !isOk ){
               SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT SET PROPAGATION" ) );
               return false;
         }

         result = meshModel->mapFromSource(iPropagation);
   }

   return true;
}


// ------------------------- ComputeMeshDialog ----------------------------------

// ============================================================== Constructeur

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

}

// ============================================================== Destructeur
ComputeMeshDialog::~ComputeMeshDialog()
{
}


// ============================================================== _initInputWidget
void ComputeMeshDialog::_initInputWidget( Mode editmode )
{
}

// ============================================================== setDocumentModel
void ComputeMeshDialog::setDocumentModel(DocumentModel* m)
{
//   HexaBaseDialog::setDocumentModel(m);
   if (m == NULL) return;
   _name->setText(m->getName());
}

// ============================================================== clear
void ComputeMeshDialog::clear()
{
    modelUnregister(this);
}

// ============================================================== apply
bool ComputeMeshDialog::apply(QModelIndex& result)
{
   if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
   _currentObj = NULL;

   SUIT_OverrideCursor wc;
   if (getDocumentModel() == NULL) return false;
   QString command = QString("import hexablock ; %1 = hexablock.mesh(\"%2\", \"%1\", %3, \"%4\")")
    								                                  .arg( _name->text() )
    								                                  .arg( getDocumentModel()->getName() )
    								                                  .arg( _dim->value() )
    								                                  .arg( _fact->text() );
   //std::cout << "command: " << command.toStdString() << std::endl;

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
//       .arg( getDocumentModel()->documentEntry() )
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



// ------------------------- ReplaceHexaDialog ----------------------------------

// ============================================================== Constructeur

ReplaceHexaDialog::ReplaceHexaDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f ):
								                                                                                                                  HexaBaseDialog(parent, editmode, f)
{
   _helpFileName = "gui_replace_hexa.html";
   setupUi( this );
   _initWidget(editmode);
   //	setFocusProxy( quads_lw );
}

// ============================================================== Destructeur
ReplaceHexaDialog::~ReplaceHexaDialog()
{
}

// ============================================================== getAssocsVTK
/*
 * Returns elements currently associated to vtk
 */
QModelIndexList ReplaceHexaDialog::getAssocsVTK()
{
   QModelIndexList assocs;
   QModelIndex iQuad;
   QListWidgetItem* item = NULL;

   //ListWidget content
   const PatternDataModel* patternDataModel = getPatternDataModel();
   if ( !patternDataModel ) return assocs;
   for ( int r = 0; r < quads_lw->count(); ++r ){
         item = quads_lw->item(r);
         //		iQuad = patternDataModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() ); //unsafe
         iQuad = getPatternDataSelectionModel()->indexBy(HEXA_DATA_ROLE, item->data(LW_DATA_ROLE));
         if ( iQuad.isValid() ) assocs << iQuad;
   }
   return assocs;
}

// ============================================================== _initInputWidget
void ReplaceHexaDialog::_initInputWidget( Mode editmode )
{
   QRegExp rx("");

//   setProperty( "HexaWidgetType",  QVariant::fromValue(ELEMENTS_TREE) );
   installEventFilter(this);

   c1_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
   c2_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
   c3_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );

   p1_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
   p2_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
   p3_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );

//   name_le->installEventFilter(this);
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

   c1_le->setReadOnly(true);
   c2_le->setReadOnly(true);
   c3_le->setReadOnly(true);

   p1_le->setReadOnly(true);
   p2_le->setReadOnly(true);
   p3_le->setReadOnly(true);

   connect(quads_lw,    SIGNAL(itemSelectionChanged()),
           this, SLOT(selectElementOfModel()), Qt::UniqueConnection);
}

// ============================================================== clear
void ReplaceHexaDialog::clear()
{
//    name_le->clear();

    quads_lw->clear();
    modelUnregister(quads_lw);

    p1_le->clear();
    modelUnregister(p1_le);

    p2_le->clear();
    modelUnregister(p2_le);

    p3_le->clear();
    modelUnregister(p3_le);

    c1_le->clear();
    modelUnregister(c1_le);

    c2_le->clear();
    modelUnregister(c2_le);

    c3_le->clear();
    modelUnregister(c3_le);

    modelUnregister(this);
}

// ============================================================== updateButtonBox
void ReplaceHexaDialog::updateButtonBox()
{
   int nbQuad = quads_lw->count();

   if ( nbQuad > 0 ){
         //_applyCloseButton->setEnabled(true);
         _applyButton->setEnabled(true);
   } else {
         //_applyCloseButton->setEnabled(false);
         _applyButton->setEnabled(false);
   }
}

// ============================================================== deleteQuadItem
void ReplaceHexaDialog::deleteQuadItem()
{
   delete quads_lw->currentItem();
   updateButtonBox();
}

// ============================================================== apply
bool ReplaceHexaDialog::apply(QModelIndex& result)
{
   if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
   _currentObj = NULL;

   SUIT_OverrideCursor wc;
   if ( !getDocumentModel() ) return false;
   const PatternDataModel* patternDataModel = getPatternDataModel();
   const PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();
   if ( !patternDataModel || !patternBuilderModel)    return false;

   QModelIndex ielts; //result

   QListWidgetItem* item = NULL;
   QModelIndexList iquads;
   QModelIndex     iquad;
   for ( int r = 0; r < quads_lw->count(); ++r){
         item = quads_lw->item(r);
         iquad = patternDataModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() );
         //std::cout << "iquad => " << iquad.data().toString().toStdString() << std::endl;
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
         ielts = getDocumentModel()->replace( iquads,
               ip1, ic1,
               ip2, ic2,
               ip3, ic3 );
   }

   if ( !ielts.isValid() ){
         SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT REPLACE HEXA" ) );
         return false;
   }
   _value  = ielts.model()->data(ielts, HEXA_DATA_ROLE).value<HEXA_NS::Elements*>();

//   QString newName = name_le->text();
//   if ( !newName.isEmpty() )/*{*/
//      getDocumentModel()->setName( ielts, newName );
//
//   updateDefaultName(name_le, ielts);

   result = patternBuilderModel->mapFromSource(ielts);

   //update the list (indexes)
   for ( int r = 0; r < quads_lw->count(); ++r ){
         item = quads_lw->item(r);
         iquad = getPatternDataSelectionModel()->indexBy(HEXA_DATA_ROLE, item->data(LW_DATA_ROLE));
         item->setData(  LW_QMODELINDEX_ROLE, QVariant::fromValue<QModelIndex>(iquad) );
         item->setData(LW_DATA_ROLE, iquad.data(HEXA_DATA_ROLE));
   }

   return true;
}




// ------------------------- QuadRevolutionDialog ----------------------------------

// ============================================================== Constructeur

QuadRevolutionDialog::QuadRevolutionDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f ):
								                                                                                                                  HexaBaseDialog(parent, editmode, f)
{
   _helpFileName = "gui_quad_revolution.html";
   setupUi( this );
   _initWidget(editmode);
   //	setFocusProxy( quads_lw );
}

// ============================================================== getAssocsVTK
/*
 * Returns elements currently associated to vtk
 */
QModelIndexList QuadRevolutionDialog::getAssocsVTK()
{
   QModelIndexList assocs;
   QModelIndex iQuad;
   QListWidgetItem* item = NULL;
   if (getPatternDataSelectionModel() == NULL) return assocs;

   //ListWidget content
   const PatternDataModel* patternDataModel = getPatternDataModel();
   if ( !patternDataModel ) return assocs;
   for ( int r = 0; r < quads_lw->count(); ++r ){
         item = quads_lw->item(r);
         iQuad = patternDataModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() );
         if ( iQuad.isValid() )
            assocs << iQuad;
   }
   return assocs;
}

// ============================================================== Destructeur
QuadRevolutionDialog::~QuadRevolutionDialog()
{
}

// ============================================================== _initInputWidget
void QuadRevolutionDialog::_initInputWidget( Mode editmode )
{
   QRegExp rx("");
   QValidator *validator = new QRegExpValidator(rx, this);

//   setProperty( "HexaWidgetType",  QVariant::fromValue(ELEMENTS_TREE) );
   installEventFilter(this);

   center_pt_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
   axis_vec_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );
   quads_lw->setProperty( "HexaWidgetType",  QVariant::fromValue(QUAD_TREE) );

   center_pt_le->setValidator( validator );
   axis_vec_le->setValidator( validator );
   //   quads_lw->setValidator( validator );

//   name_le->installEventFilter(this);
   center_pt_le->installEventFilter(this);
   axis_vec_le->installEventFilter(this);
   quads_lw->installEventFilter(this);

   if ( editmode == NEW_MODE ){
         angles_lw->setItemDelegate( new HexaAngleDoubleSpinBoxDelegate(angles_lw) );
         angles_lw->setEditTriggers( QAbstractItemView::DoubleClicked );

         QShortcut* delQuadShortcut = new QShortcut( QKeySequence(Qt::Key_X), quads_lw );
         delQuadShortcut->setContext( Qt::WidgetShortcut );
         connect( delQuadShortcut, SIGNAL(activated()), this, SLOT(delQuadItem()) );
         connect( add_angle_pb, SIGNAL(clicked()), this, SLOT(addAngleItem()));
         connect( del_angle_pb, SIGNAL(clicked()), this, SLOT(delAngleItem()));
         //     connect(clear_pb, SIGNAL(clicked()), this, SLOT(clearQuads()));
   }

   center_pt_le->setReadOnly(true);
   axis_vec_le->setReadOnly(true);

   connect(quads_lw,    SIGNAL(itemSelectionChanged()), this, SLOT(selectElementOfModel()), Qt::UniqueConnection);
}

// ============================================================== clear
void QuadRevolutionDialog::clear()
{
//    name_le->clear();

    quads_lw->clear();
    modelUnregister(quads_lw);

    center_pt_le->clear();
    modelUnregister(center_pt_le);

    axis_vec_le->clear();
    modelUnregister(axis_vec_le);

    angles_lw->clear();
    modelUnregister(angles_lw);

    modelUnregister(this);
}


// ============================================================== updateButtonBox
void QuadRevolutionDialog::updateButtonBox() //CS_TODO? : check center, axis
{
   int nbQuads  = quads_lw->count();
   int nbAngles = angles_lw->count();

   if ( nbQuads>0 && nbAngles> 0 ){
         //_applyCloseButton->setEnabled(true);
         _applyButton->setEnabled(true);
   } else {
         //_applyCloseButton->setEnabled(false);
         _applyButton->setEnabled(false);
   }
}


// ============================================================== addAngleItem
void QuadRevolutionDialog::addAngleItem() //CS_TODO
{
   QListWidgetItem* previousItem = angles_lw->currentItem();
   QListWidgetItem* newItem      = new QListWidgetItem();

   double defaultValue = 180.;
   if ( previousItem )
      defaultValue =  previousItem->data(Qt::EditRole).toDouble();

   newItem->setData(  Qt::EditRole, QVariant(defaultValue) );
   newItem->setFlags( newItem->flags () | Qt::ItemIsEditable);
   angles_lw->addItem(newItem);

   updateButtonBox();
}

// ============================================================== delAngleItem
void QuadRevolutionDialog::delAngleItem()
{
   delete angles_lw->currentItem();
   updateButtonBox();
}

// ============================================================== delQuadItem
void QuadRevolutionDialog::delQuadItem()
{
   delete quads_lw->currentItem();
   updateButtonBox();
}

// ============================================================== apply
bool QuadRevolutionDialog::apply(QModelIndex& result)
{
   if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
   _currentObj = NULL;

   SUIT_OverrideCursor wc;
   if ( !getDocumentModel() ) return false;
   const PatternDataModel* patternDataModel = getPatternDataModel();
   const PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();
   if ( !patternDataModel || !patternBuilderModel)    return false;

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
         angles << item->data(Qt::EditRole).toDouble();
   }

   if ( icenter_pt.isValid() && iaxis_vec.isValid() ){
         ielts = getDocumentModel()->revolutionQuads( istartquads, icenter_pt, iaxis_vec, angles );
   }

   if ( !ielts.isValid() ){
         SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE QUAD REVOLUTION" ) );
         return false;
   }
   _value  = ielts.model()->data(ielts, HEXA_DATA_ROLE).value<HEXA_NS::Elements*>();

//   QString newName = name_le->text();
//   if ( !newName.isEmpty() )/*{*/
//      getDocumentModel()->setName( ielts, newName );
//
//   updateDefaultName(name_le, ielts);

   result = patternBuilderModel->mapFromSource(ielts);

   return true;
}



// ------------------------- MakeHemiSphereDialog ----------------------------------

// ============================================================== Constructeur

MakeHemiSphereDialog::MakeHemiSphereDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f ):
								                                                                                                                  HexaBaseDialog(parent, editmode, f)
{
   _helpFileName = "gui_hemisphere.html";
   setupUi( this );
   _initWidget(editmode);
}

// ============================================================== Destructeur
MakeHemiSphereDialog::~MakeHemiSphereDialog()
{
}

// ============================================================== _initInputWidget
void MakeHemiSphereDialog::_initInputWidget( Mode editmode )
{
   QRegExp rx("");
   QValidator *validator = new QRegExpValidator(rx, this);

//   setProperty( "HexaWidgetType",  QVariant::fromValue(ELEMENTS_TREE) );
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


//   name_le->installEventFilter(this);
   sphere_center_le->installEventFilter(this);
   cross_pt_le->installEventFilter(this);
   hole_axis_le->installEventFilter(this);
   cross_vec_le->installEventFilter(this);
   radial_vec_le->installEventFilter(this);

   sphere_center_le->setReadOnly(true);
   cross_pt_le->setReadOnly(true);
   hole_axis_le->setReadOnly(true);
   cross_vec_le->setReadOnly(true);
   radial_vec_le->setReadOnly(true);
}

// ============================================================== clear
void MakeHemiSphereDialog::clear()
{
    modelUnregister(this);
}

// ============================================================== apply
bool MakeHemiSphereDialog::apply(QModelIndex& result)
{
   if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
   _currentObj = NULL;

   SUIT_OverrideCursor wc;
   if ( !getDocumentModel() ) return false;
   const PatternDataModel* patternDataModel = getPatternDataModel();
   const PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();
   if ( !patternDataModel || !patternBuilderModel) return false;

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
                     iElts = getDocumentModel()->makePartRind( icenter, ivecx, ivecz,
                           radext, radint, radhole,
                           iplorig,      angle,
                           nrad, nang, nhaut );
               } else {
                     iElts = getDocumentModel()->makeRind( icenter,
                           ivecx, ivecz,
                           radext, radint, radhole,
                           iplorig,
                           nrad, nang, nhaut );
               }
         } else { // sphere
               if ( radial_partial_cb->isChecked() ){
                     double angle = radial_angle_spb->value();
                     iElts = getDocumentModel()->makePartSphere( icenter, ivecx, ivecz,
                           radext, radhole,
                           iplorig, angle,
                           nrad, nang, nhaut );
               } else {
                     iElts = getDocumentModel()->makeSphere( icenter,
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

//   QString newName = name_le->text();
//   if ( !newName.isEmpty() )/*{*/
//      getDocumentModel()->setName( iElts, newName );
//
//   updateDefaultName(name_le, iElts);

   result = patternBuilderModel->mapFromSource(iElts);

   return true;
}
// ==============================================================


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
//	 if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
//	 _currentObj = NULL;
//
//   SUIT_OverrideCursor wc;
//   if ( !getDocumentModel() ) return false;
//   if ( !getPatternBuilderSelectionModel() ) return false;
//   if ( !getPatternDataSelectionModel() )    return false;
//   const PatternBuilderModel* patternBuilderModel = dynamic_cast<const PatternBuilderModel*>( getPatternBuilderSelectionModel()->model() );
//   const PatternDataModel* patternDataModel = dynamic_cast<const PatternDataModel*>( getPatternDataSelectionModel()->model() );
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
//       iElts = getDocumentModel()->makePartRind( icenter, ivecx, ivecz,
//                                         radext, radint, radhole,
//                                         iplorig,      angle,
//                                         nrad, nang, nhaut );
//     } else {
//       iElts = getDocumentModel()->makeRind( icenter,
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
//   if (!newName.isEmpty()) getDocumentModel()->setName( iElts, newName );
//
//   return true;
// }

// QVariant v = lineEdit->property("index");
//           if ( v.isValid() ){
//             MESSAGE("*  get index ... " );
//             QModelIndex i = v.value<QModelIndex>();
//             _selectionMutex = true;
//             getPatternDataSelectionModel()->select( i, QItemSelectionModel::Clear );
//             getPatternDataSelectionModel()->select( i, QItemSelectionModel::Select );
//             _selectionMutex = false;
//           }
