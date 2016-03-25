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

#include <OCCViewer_ViewWindow.h>
#include "HEXABLOCKGUI_DocumentPanel.hxx"
#include "HEXABLOCKGUI_VtkDocumentGraphicView.hxx"
#include "HEXABLOCKGUI_OccGraphicView.hxx"
#include "HEXABLOCKGUI_SalomeTools.hxx"

#include "Hex.hxx"
#include <iostream>
#include <QFlags>
#include <QKeyEvent>
#include <QStyledItemDelegate>

#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#ifndef DISABLE_PYCONSOLE
#include <PyConsole_Console.h>
#endif
#include <SalomeApp_Tools.h>

#include <Standard_GUID.hxx>
#include <TDF_Label.hxx>

#include <OCCViewer_ViewManager.h>

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

#include <GEOMBase.h>

#include <utilities.h>

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
Q_DECLARE_METATYPE(DocumentModel::GeomObj);
Q_DECLARE_METATYPE(TopAbs_ShapeEnum);
Q_DECLARE_METATYPE(TopoDS_Shape);

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
        sb->setMaximum(SPINBOX_DOUBLE_MAX);       //10e9
        sb->setDecimals(NB_DECIMALS);
        return sb;
    }
};

// ======================================================== Constructeur
HexaBaseDialog::HexaBaseDialog( QWidget * parent, Mode editmode, Qt::WindowFlags f ):
                          QDialog(parent, f),
                          _editMode( editmode ),
                          _currentObj(NULL),
                          _applyButton(NULL),
                          debugEdgeAssoc(false),
                          autoFocusSwitch(true)
{
    _strHexaWidgetType[VERTEX_TREE] = tr( "VERTEX" );
    _strHexaWidgetType[EDGE_TREE]   = tr( "EDGE" );
    _strHexaWidgetType[QUAD_TREE]   = tr( "QUAD" );
    _strHexaWidgetType[HEXA_TREE]   = tr( "HEXA" );

    _strHexaWidgetType[VECTOR_TREE]   = tr( "VECTOR" );
//    _strHexaWidgetType[CYLINDER_TREE] = tr( "CYLINDER" );
//    _strHexaWidgetType[PIPE_TREE]     = tr( "PIPE" );
    _strHexaWidgetType[ELEMENTS_TREE] = tr( "ELEMENTS" );
    _strHexaWidgetType[CROSSELEMENTS_TREE]= tr( "CROSSELEMENTS" );

    //geom
    _strHexaWidgetType[GEOMSHAPE_TREE] = tr( "GEOMSHAPE" );
    _strHexaWidgetType[GEOMPOINT_TREE]   = tr( "GEOMPOINT" );
    _strHexaWidgetType[GEOMEDGE_TREE]   = tr( "GEOMEDGE" );
    _strHexaWidgetType[GEOMFACE_TREE]   = tr( "GEOMFACE" );

    _strHexaWidgetType[GROUP_TREE]      = tr( "GROUP" );
    _strHexaWidgetType[LAW_TREE]        = tr( "LAW" );
    _strHexaWidgetType[PROPAGATION_TREE]= tr( "PROPAGATION" );
}

// ============================================================= getIndexList
QModelIndexList HexaBaseDialog::getIndexList(QListWidget* itemsList, bool mapToSource)
{
    QModelIndexList iItems;
    QModelIndex     iItem;
    QListWidgetItem* item = NULL;

    const PatternDataModel*    patternDataModel = getPatternDataModel();
    if (patternDataModel == NULL || itemsList == NULL) return iItems;

    unsigned int nbItems = itemsList->count();
    for ( int r = 0; r < nbItems; ++r){
        item = itemsList->item(r);
        if (mapToSource)
            iItem = patternDataModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() );
        else
            iItem = item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>();
        iItems << iItem;
    }
    return iItems;
}


// ================================================================ computeAndSetDimension
void HexaBaseDialog::computeAndSetDimension(const QModelIndex& elt)
{
    // * Check if everything is OK for the computing
    DocumentModel* docModel = getDocumentModel();
    if (docModel == NULL || _currentObj == NULL)
        return;

    int selectedType = elt.data(HEXA_TREE_ROLE).toInt();
    if (selectedType != EDGE_TREE && selectedType != GEOMEDGE_TREE)
        return;

    QListWidget*    list = dynamic_cast<QListWidget*>(_currentObj);
    QDoubleSpinBox* spb  = dynamic_cast<QDoubleSpinBox*>(_currentObj);

    if (list == NULL && spb == NULL)
        return;

    // * Compute the value of the dimension
    double value = 0.;

    if (_currentObj->property("Radius").isValid())
        value = docModel->getRadius(elt);
    else if (_currentObj->property("Angle").isValid())
        value = docModel->getAngle(elt);
    else if (_currentObj->property("Length").isValid())
        value = docModel->getLength(elt);

    if (value == 0.)
        return;

    // * Set the value to the field (radius, length or height)
    if (list != NULL)
    {
        QListWidgetItem* item = list->currentItem();
        if (item != NULL)
        {
            if (value != 0.)
            {
                item->setText(QString::number(value));
                list->editItem(item);
            }
        }
    }
    else if (spb != NULL)
    {
        spb->setValue(value);
        spb->setFocus();
        spb->selectAll();
    }
}

// ============================================================= resetSizeAndShow
void HexaBaseDialog::resetSizeAndShow(QDockWidget* parent)
{
    if (parent == NULL)
        return;

    //force the dialog to fit its contain
//    setMinimumWidth(sizeHint().width());

    //set the dialog in the dockwidget
    if (parent->widget())
        parent->widget()->close();
    parent->setWidget(this);
    parent->setWindowTitle(windowTitle());
    parent->show();

//    setMinimumWidth(MIN_WIDTH);
}

QString HexaBaseDialog::getErrorMsg()
{
    DocumentModel* docModel = getDocumentModel();
    Hex::Hex* hex = docModel->getHexaRoot();
    int nbErrorMsg = hex->sizeofMessage();
    QString msg, newLine = "\n";
    for (int i = 1; i < nbErrorMsg; ++i)
        msg +=  newLine + hex->getMessageLine(i);

    return msg;
}

//unlink the widget from the model
void HexaBaseDialog::modelUnregister(QWidget* widget)
{
    if (widget == NULL) return;

    if (widget->property("GeomWidgetType").isValid())
    {
        widget->setProperty("GeomObj", QVariant());
        widget->setProperty("TopoDS_Shape", QVariant());
    }

    widget->setProperty("HexaData", QVariant());
    widget->setProperty("QModelIndex",  QVariant());
    _index[widget] = QModelIndex();

    HexaBaseDialog* diag = dynamic_cast<HexaBaseDialog*>(widget);
    if (diag != NULL)
        diag->clearWidgetsIndexes();
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
//    connect( docGView->getPatternBuilderSelectionModel(), SIGNAL( selectionChanged( const QItemSelection &, const QItemSelection &) ),
//            this, SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ), Qt::UniqueConnection );
    connect( docGView->getPatternGeomSelectionModel(), SIGNAL( selectionChanged( const QItemSelection &, const QItemSelection &) ),
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

//    disconnect(docGView->getPatternBuilderSelectionModel(), SIGNAL( selectionChanged( const QItemSelection &, const QItemSelection &) ),
//            this, SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );

    disconnect(docGView->getPatternGeomSelectionModel(), SIGNAL( selectionChanged( const QItemSelection &, const QItemSelection &) ),
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

// ============================================================= clearCurrentObjectFocus
void HexaBaseDialog::clearCurrentObjectFocus()
{
    QWidget* currentWidget = dynamic_cast<QWidget*>(_currentObj);
    if (currentWidget != NULL)
    {
        currentWidget->clearFocus();
        _currentObj = NULL;
    }
}

// ============================================================= setFocusToNextField
//Sets focus to the next field of the current object
void HexaBaseDialog::setFocusToNextField()
{
    activateWindow ();

    if (!HEXABLOCKGUI::assocInProgress && autoFocusSwitch)
    {
        _highlightWidget(_currentObj, Qt::white);
        focusNextChild ();
    }
}

// ============================================================== _initButtonBox
QGroupBox* HexaBaseDialog::_initButtonBox( Mode editmode )
{
    if ( editmode == INFO_MODE )
        return NULL;

    QGroupBox* buttonBox = new QGroupBox();
    buttonBox->setMinimumWidth(BUTTON_BOX_MIN_WIDTH);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    QHBoxLayout* buttonsHLayout = new QHBoxLayout();
    _applyButton      = new QPushButton(tr("Apply"));
    QPushButton* closeButton = new QPushButton(tr("Close"));
    QPushButton* helpButton  = new QPushButton(tr("Help"));

    connect( _applyButton, SIGNAL(clicked()), this, SLOT(apply()), Qt::UniqueConnection );
    connect( closeButton, SIGNAL(clicked()), this, SLOT(close()), Qt::UniqueConnection );
    connect( helpButton, SIGNAL(clicked()), this, SLOT(onHelpRequested()), Qt::UniqueConnection );

    buttonsHLayout->addWidget( _applyButton );
    buttonsHLayout->addWidget( closeButton );
    buttonsHLayout->addStretch(1);
    buttonsHLayout->addWidget( helpButton );
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

// ============================================================== apply
bool HexaBaseDialog::apply()
{
    QModelIndex iNew;
    bool applied = apply(iNew);
    if ( applied )
    {
        _selectAndHighlight( iNew );
        if (!HEXABLOCKGUI::assocInProgress)
            getDocumentModel()->updateGeomTree();
    }
    return applied;
}

void HexaBaseDialog::clearVTKSelection()
{
    HEXABLOCKGUI::currentDocGView->clearSelection();
    _highlightWidget(_currentObj, Qt::white);
}

void HexaBaseDialog::clearOCCSelection()
{
	HEXABLOCKGUI::currentOccGView->clearSelection();
}

// ============================================================== close
void HexaBaseDialog::close()
{
    //reset the data selection pattern (forget all selections of the current context)
    getPatternDataSelectionModel()->reset();
//    getPatternBuilderSelectionModel()->reset();
    getPatternGeomSelectionModel()->reset();

    //Clear vtk selection
	clearVTKSelection();

    //Clear occ selection
	clearOCCSelection();

    //Close the dialog box
    if (parentWidget())
    	parentWidget()->close();

    getPatternDataSelectionModel()->setInfoMode(true);
}

// ============================================================== onHelpRequested
void HexaBaseDialog::onHelpRequested()
{
    LightApp_Application* app = (LightApp_Application*)( SUIT_Session::session()->activeApplication() );
    if ( app )
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
    QMultiMap<QString, int> geomAssocs = getAssocsGEOM();
    QModelIndexList vtkAssocs = getAssocsVTK();

//    HEXABLOCKGUI::selectionMgr()->clearSelected();
    //highlight geom selected elts
    if (geomAssocs.size() > 0)
        HEXABLOCKGUI::currentOccGView->highlight(geomAssocs, false);

    //highlight vtk selected elts
    if (!vtkAssocs.isEmpty())
        HEXABLOCKGUI::currentDocGView->highlight(vtkAssocs);
}

// ============================================================== refreshHighlight
void HexaBaseDialog::refreshHighlight()
{
    HEXABLOCKGUI::selectionMgr()->clearSelected();
    highlightSelectedAssocs();
}

bool HexaBaseDialog::isDimensionType(const QObject* obj)
{
    if (obj == NULL)
        return false;

    return (obj->property("Radius").isValid()) ||
            (obj->property("Angle").isValid()) ||
            (obj->property("Length").isValid());
}

// ============================================================== getObjectViewType
HEXABLOCKGUI::ViewType HexaBaseDialog::getObjectViewType(QObject* obj)
{
    if (obj == NULL)
        return HEXABLOCKGUI::UNKNOWN;

    QVariant v1 = obj->property("GeomWidgetType");
    QVariant v2 = obj->property("HexaWidgetType");

    if (v1.isValid() && v2.isValid() && isDimensionType(obj))
        return HEXABLOCKGUI::VTK_OCC;
    else if (v1.isValid())
        return HEXABLOCKGUI::OCC;
    else if (v2.isValid())
        return HEXABLOCKGUI::VTK;
    else
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
    if ( getDocumentModel() )
        getDocumentModel()->disallowEdition();
}

// ============================================================== _disallowSelection
void HexaBaseDialog::_disallowSelection()
{
    if ( getDocumentModel() ){
        getDocumentModel()->allowEdition();
    }
    HEXABLOCKGUI::currentDocGView->setAllSelection();
}

// ============================================================== _allowVTKSelection
bool HexaBaseDialog::_allowVTKSelection( QObject* obj )
{
    bool isOk = false;

    QVariant v  = obj->property("HexaWidgetType");
    HexaWidgetType wType = v.value<HexaWidgetType>();

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
    default: HEXABLOCKGUI::currentDocGView->setAllSelection();
    }
    return isOk;
}

// ============================================================== _allowOCCSelection
bool HexaBaseDialog::_allowOCCSelection( QObject* obj )
{
    QVariant v  = obj->property("GeomWidgetType");
    GeomWidgetType wType = v.value<GeomWidgetType>();
    HEXABLOCKGUI::currentOccGView->setSelectionMode(wType);

    return true;
}

// ============================================================== _getSelector
QItemSelectionModel* HexaBaseDialog::_getSelector( QObject* obj )
{
    if (obj == NULL)
        return NULL;

    QItemSelectionModel* selector = NULL;

    HexaWidgetType wtype;
    QVariant v  = obj->property("HexaWidgetType");
    QVariant v2 = obj->property("GeomWidgetType");
    if ( !v.isValid() || (v2.isValid() && isDimensionType(obj)) )
        return NULL;

    wtype = v.value<HexaWidgetType>();

    switch (wtype){
    case VERTEX_TREE:
    case EDGE_TREE:
    case QUAD_TREE:
    case HEXA_TREE:
    case VECTOR_TREE: selector = getPatternDataSelectionModel(); break;
//    case CYLINDER_TREE:
//    case PIPE_TREE:
//    case ELEMENTS_TREE:
//    case CROSSELEMENTS_TREE: selector = getPatternBuilderSelectionModel(); break;
    case GEOMPOINT_TREE:
    case GEOMEDGE_TREE:
    case GEOMFACE_TREE: selector = getPatternGeomSelectionModel(); break;
    case GROUP_TREE: selector = getGroupsSelectionModel(); break;
    case LAW_TREE:
    case PROPAGATION_TREE: selector = getMeshSelectionModel(); break;
    default : ;
    }

    return selector;
}

// ============================================================== getGeomObj
DocumentModel::GeomObj* HexaBaseDialog::getGeomObj(const QModelIndex& index)
{
    HEXA_NS::NewShape* aSh  = getDocumentModel()->getHexaPtr<HEXA_NS::NewShape*>(index);
    DocumentModel::GeomObj* geomObj = NULL;
    if (aSh != NULL)
    {
        geomObj = new DocumentModel::GeomObj;
        geomObj->shapeName = aSh->getName();
        geomObj->subId = QString::number(-1);
    }
    else
    {
        HEXA_NS::EltBase*  aSShElt = getDocumentModel()->getHexaPtr(index);
        HEXA_NS::SubShape* aSSh = dynamic_cast<HEXA_NS::SubShape*>(aSShElt);
        if (aSSh != NULL)
        {
            geomObj = new DocumentModel::GeomObj;
            QString shapeName;
            if (aSSh->getParentShape() != NULL)
                shapeName = aSSh->getParentShape()->getName();
            geomObj->shapeName = shapeName;
            geomObj->subId = QString::number(aSSh->getIdent());
        }
    }
    return geomObj;
}

// ============================================================== _onSelectionChanged
/*
 * Puts elements selected in the model (treeview) in the corresponding
 * line edit widget (the one that has the focus) of the current dialog box.
 */
bool HexaBaseDialog::_onSelectionChanged( const QItemSelection& sel, QLineEdit*  le )
{
    QModelIndexList l = sel.indexes();
    if ( l.count() == 0 ) return false;

    // mono selection mode: we just get the first selected element
    QModelIndex selected = l[0];

    // we just return if the selection is not valid or the selection and the
    // line content are the same
    if ( !selected.isValid() || le->property("HexaData") == selected.data(HEXA_DATA_ROLE) )
        return false;

    // type of selection
    int selType = selected.data(HEXA_TREE_ROLE).toInt();

    // type of widget
    QVariant v = le->property("HexaWidgetType");
    if ( !v.isValid() ) return false;
    HexaWidgetType wType = v.value<HexaWidgetType>();

    // check selection compatibility between selection and widget
    if ( selType != wType ){
        SUIT_MessageBox::information( 0, tr("HEXA_INFO"),
                tr("%1: Bad type selected\nPlease select a %2.").arg(windowTitle()).arg(_strHexaWidgetType[wType]));
        return false;
    }
    if (le->property("GeomWidgetType").isValid())
    {
        DocumentModel::GeomObj* geomObj = getGeomObj(selected);
        if (geomObj != NULL)
        {
            le->setProperty("GeomObj", QVariant::fromValue<DocumentModel::GeomObj>(*geomObj));

            DocumentModel* docModel = getDocumentModel();
            QString objId =  geomObj->shapeName + "," + geomObj->subId;
            HEXA_NS::SubShape* ssh = docModel->getGeomPtr(objId);
            if (ssh != NULL)
            {
                TopoDS_Shape shape = ssh->getShape();
                if (!shape.IsNull())
                    le->setProperty("TopoDS_Shape", QVariant::fromValue<TopoDS_Shape>(shape));
            }
            setCurrentGeomObj(geomObj);
        }
    }

    //fill the lineEdit if selection is OK
    le->setText( selected.data().toString() );// name
    le->setProperty("QModelIndex",  QVariant::fromValue(selected));
    le->setProperty("HexaData", selected.data(HEXA_DATA_ROLE));
    _index[le] = selected;

    QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(_currentObj);
    if (selected.isValid() && lineEdit != NULL)
        setFocusToNextField();

    return true;
}

// ============================================================== _onSelectionChanged
/*
 * Puts elements selected in the model (treeview) in the corresponding list widget
 * of the current dialog box.
 */
bool HexaBaseDialog::_onSelectionChanged( const QItemSelection& sel, QListWidget* lw )
{
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
            if (lw->property("GeomWidgetType").isValid())
            {
                DocumentModel::GeomObj* geomObj = getGeomObj(isel);
                if (geomObj != NULL)
                    item->setData(LW_ASSOC_ROLE, QVariant::fromValue<DocumentModel::GeomObj>(*geomObj));
            }
            lw->addItem(item);
            updateButtonBox();
        }
    }
    return true;
}

// ============================================================== onSelectionChanged
/*
 * Puts elements selected in the model in the corresponding widget (list widget or line edit)
 * of the current dialog box.
 */
void HexaBaseDialog::onSelectionChanged( const QItemSelection& sel, const QItemSelection& unsel )
{
    QModelIndexList l = sel.indexes();

    if ( l.count() == 0 )
        return;
    QModelIndex selected = l[0];

    // * no edition for Info Dialogs
    if ( _editMode == INFO_MODE || _currentObj == NULL || !selected.isValid())
        return;

    if (isDimensionType(_currentObj))
    {
        // ** set the dimension of the selected object in the editor **/
        int selectedType = selected.data(HEXA_TREE_ROLE).toInt();
        if (selectedType == EDGE_TREE || selectedType == GEOMEDGE_TREE)
            computeAndSetDimension(selected);
        return;
    }

    QLineEdit*   aLineEdit   = dynamic_cast<QLineEdit*>(_currentObj);
    QListWidget* aListWidget = dynamic_cast<QListWidget*>(_currentObj);

    // * fill the lineedit with selection
    if ( aLineEdit)
    {
        _onSelectionChanged( sel, aLineEdit );
        return;
    }

    // * fill the listWidget with selection
    if (aListWidget)
        _onSelectionChanged( sel, aListWidget );
}

// ============================================================== showEvent
void HexaBaseDialog::showEvent( QShowEvent * event )
{
    if ( _editMode == INFO_MODE )
        getDocumentModel()->allowEdition();
    else
        getDocumentModel()->disallowEdition();

    //Connect to salome selection signals
    if (HEXABLOCKGUI::selectionMgr() != NULL)
    {
        connect( HEXABLOCKGUI::selectionMgr(), SIGNAL(currentSelectionChanged()),
                this, SLOT(onCurrentSelectionChanged()), Qt::UniqueConnection );
    }

    //connect model selection signals
    connectDocumentGraphicView();

    QDialog::showEvent ( event );
}


// ============================================================== hideEvent
void HexaBaseDialog::hideEvent ( QHideEvent * event )
{
    //Disconnection salome selection signals
    if (HEXABLOCKGUI::selectionMgr() != NULL)
        disconnect(  HEXABLOCKGUI::selectionMgr() , SIGNAL(currentSelectionChanged()),
                this, SLOT(onCurrentSelectionChanged()) );

    //Disconnect vtk window activation signals
//    if (HEXABLOCKGUI::currentDocGView->getViewWindow() != NULL)
//        disconnect( HEXABLOCKGUI::currentDocGView->getViewWindow()->getViewManager(),
//                SIGNAL( activated(SUIT_ViewManager*) ),
//                this, SLOT( onWindowActivated(SUIT_ViewManager*) ) );

    //Disconnect occ window activation signals
//    if (HEXABLOCKGUI::currentOccGView->getViewWindow() != NULL)
//        disconnect( HEXABLOCKGUI::currentOccGView->getViewWindow()->getViewManager(),
//                SIGNAL( activated(SUIT_ViewManager*) ),
//                this, SLOT( onWindowActivated(SUIT_ViewManager*) ) );

    //Disconnect model selection signals
    disconnectDocumentGraphicView();
    getDocumentModel()->allowEdition();

    QDialog::hideEvent( event );
}

// ============================================================== updateButtonBox
void HexaBaseDialog::updateButtonBox()
{
}

// ============================================================== updateName
void HexaBaseDialog::updateName()
{
    const PatternDataModel*   patternDataModel = getPatternDataModel();

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
void HexaBaseDialog::updateDefaultName(QLineEdit* name_field, HEXA_NS::EnumElt type)
{
    if (name_field == NULL) return;

    HEXA_NS::Document* doc = getDocumentModel()->getHexaDocument();
    if (doc == NULL) return;

    name_field->setText(doc->getNextName(type).c_str());
}

// ============================================================== selectElementOfModel
/*Selects in the model (treeview) elements selected in a listwidget,
 *  or an element in a line edit.*/
void HexaBaseDialog::selectElementOfModel()
{
    if (getPatternDataSelectionModel() == NULL) return;

    QListWidget* currentListWidget = dynamic_cast<QListWidget*>( sender() );
    if ( !currentListWidget ) return;

    QList<QListWidgetItem *> sel = currentListWidget->selectedItems();
    QModelIndex index;
    getPatternDataSelectionModel()->clearSelection();
    foreach ( QListWidgetItem *item, sel ){
        //index = item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>(); //unsafe: index can change in the tree
        index = getPatternDataSelectionModel()->indexBy(HEXA_DATA_ROLE, item->data(LW_DATA_ROLE));
        if ( index.isValid() )
            getPatternDataSelectionModel()->select( index, QItemSelectionModel::SelectCurrent );
    }
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
    QDoubleSpinBox* spb = dynamic_cast<QDoubleSpinBox*>(obj);
    if (!_isLineOrListWidget(obj) && spb == NULL)
        return;

    QWidget *widget = dynamic_cast<QWidget*>(obj);
    QPalette palette1 = widget->palette();
    palette1.setColor(QPalette::Active, widget->backgroundRole(), clr);
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
    QLineEdit   	*lineEdit = dynamic_cast<QLineEdit*>(obj);
    QListWidget     *listWidget = dynamic_cast<QListWidget*>(obj);

    // * Enter key press ------
    if ( event->type() == QEvent::KeyPress &&
            ((QKeyEvent*)event)->key() == Qt::Key_Return)
    {
        setFocusToNextField();
        return true;
    }

    // * Focus out from a list widget ------
    if ( event->type() == QEvent::FocusOut && listWidget != NULL)
    {
        QItemSelectionModel * selectionModel = listWidget->selectionModel();
        selectionModel->clearSelection();
    }

    if ( event->type() != QEvent::FocusIn )
        return false;

    // * Focus In ------
    HEXABLOCKGUI::ViewType vtype = getObjectViewType(obj);
    if (vtype == HEXABLOCKGUI::VTK_OCC)
    {
        _allowVTKSelection(obj);
        _allowOCCSelection(obj);
    }
    else if (vtype == HEXABLOCKGUI::VTK)
        _allowVTKSelection( obj );
    else if (vtype == HEXABLOCKGUI::OCC)
        _allowOCCSelection( obj );

    //Depending on the focused widget type, get the right selector for it
    QVariant v;
    QModelIndex index;
    QItemSelectionModel* selector = _getSelector( obj );
    if ( selector == NULL )
    {
        _updateCurrentObject(obj);
        return false;
    }

    if ( _currentObj != obj && (lineEdit == NULL || listWidget == NULL) )
        selector->clearSelection();

    _updateCurrentObject(obj);

    //If the widget contains an element, select it in model/view
    if ( lineEdit != NULL ){ //element can be from lineEdit
        v = lineEdit->property("HexaData");
        if ( !v.isValid() )
            return QObject::eventFilter(obj, event);
        index = ((SelectionModel*)selector)->indexBy( HEXA_DATA_ROLE, v);
        if (index.isValid())
            selector->select( index, QItemSelectionModel::SelectCurrent );
    }

    return QObject::eventFilter(obj, event);
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

    if ( editmode  == NEW_MODE ){
        setWindowTitle( tr("Vertex Construction") );
    } else if ( editmode == UPDATE_MODE ){
        setWindowTitle( tr("Vertex Modification") );
    }
    else if ( editmode == INFO_MODE){
        setWindowTitle( tr("Vertex Information") );
    }
}

// ============================================================== Destructeur
VertexDialog::~VertexDialog()
{
}

// ============================================================== _initInputWidget
void VertexDialog::_initInputWidget( Mode editmode )
{
//    x_spb->setRange(VERTEX_COORD_MIN, VERTEX_COORD_MAX);
//    y_spb->setRange(VERTEX_COORD_MIN, VERTEX_COORD_MAX);
//    z_spb->setRange(VERTEX_COORD_MIN, VERTEX_COORD_MAX);
    QDoubleValidator *doubleValidator = new QDoubleValidator(widget_2);
    x_spb->setValidator(doubleValidator);
    y_spb->setValidator(doubleValidator);
    z_spb->setValidator(doubleValidator);

    //   setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
    installEventFilter(this);
    //   name_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
    name_le->installEventFilter(this);

    if (editmode == INFO_MODE)
    {
        name_le->setReadOnly(true);
        x_spb->setReadOnly(true);
        y_spb->setReadOnly(true);
        z_spb->setReadOnly(true);
    }

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
    //0) Name
    name_le->setText( v->getName() );

    //1) Value (x,y,z)
//    x_spb->setValue( v->getX() );
//    y_spb->setValue( v->getY() );
//    z_spb->setValue( v->getZ() );
    x_spb->setText( QString::number(v->getX()) );
    y_spb->setText( QString::number(v->getY()) );
    z_spb->setText( QString::number(v->getZ()) );

    if ( getPatternDataSelectionModel() != NULL ){
        QModelIndex iv = getPatternDataSelectionModel()->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(v) );

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

    if ( !getDocumentModel() ) return false;
    if ( !getPatternDataSelectionModel() ) return false;
    const PatternDataModel* patternDataModel = getPatternDataModel();
    if ( !patternDataModel ) return false;

    QModelIndex iVertex;

    QString xStr = x_spb->text();
    QString yStr = y_spb->text();
    QString zStr = z_spb->text();
    bool isOk = false, validArgs = !xStr.isEmpty() && !yStr.isEmpty() && !zStr.isEmpty();

    double newX, newY, newZ;
    if (validArgs)
    {
        newX = xStr.toDouble();
        newY = yStr.toDouble();
        newZ = zStr.toDouble();
    }

    if ( _editMode == UPDATE_MODE && validArgs){
        QVariant v = property("QModelIndex");
        if ( v.isValid() ){
            iVertex = patternDataModel->mapToSource( v.value<QModelIndex>() );
            if ( iVertex.isValid() )
                isOk = getDocumentModel()->updateVertex( iVertex, newX, newY, newZ );
        }
    } else if ( _editMode == NEW_MODE && validArgs){
        iVertex = getDocumentModel()->addVertex( newX, newY, newZ );
        if ( iVertex.isValid() )
            isOk = true;
    }
    if (!isOk) {
        SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "VERTEX UPDATE/CONSTRUCTION" ) + "\n" + getErrorMsg() );
        return false;
    }

    QString newName = name_le->text();
    if ( !newName.isEmpty() )
        getDocumentModel()->setName( iVertex, newName );

    //the default name in the dialog box
    HEXA_NS::Vertex* v = getDocumentModel()->getHexaPtr<HEXA_NS::Vertex*>(iVertex);
    if (v != NULL)
        updateDefaultName(name_le, v->getType());

    // to select/highlight result
    result = patternDataModel->mapFromSource(iVertex);

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

    if  ( editmode == INFO_MODE ){
        setWindowTitle( tr("Edge Information") );
        rb1->hide();
    }
    else if ( editmode == UPDATE_MODE ){
        setWindowTitle( tr("Edge Modification") );
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

    if (editmode == INFO_MODE)
        name_le->setReadOnly(true);

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

    if ( !getDocumentModel() ) return false;
    const PatternDataModel*    patternDataModel = getPatternDataModel();
//    const PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();
    if ( !patternDataModel /*|| !patternBuilderModel*/ ) return false;

    QModelIndex iEdge;

    if ( rb0->isChecked() ){
        QModelIndex iv0 = patternDataModel->mapToSource( _index[v0_le_rb0] );
        QModelIndex iv1 = patternDataModel->mapToSource( _index[v1_le_rb0] );
        if ( iv0.isValid()&& iv1.isValid() ){
            iEdge = getDocumentModel()->addEdgeVertices( iv0, iv1 );
        }
    } else if ( rb1->isChecked() ){
        QModelIndex ivex = patternDataModel->mapToSource( _index[vex_le_rb1] );
        QModelIndex ivec = patternDataModel->mapToSource( _index[vec_le_rb1] );
//        QModelIndex ivec = patternBuilderModel->mapToSource( _index[vec_le_rb1] );
        if ( ivex.isValid() && ivec.isValid() ){
            iEdge = getDocumentModel()->addEdgeVector( ivex, ivec );
        }
    }

    if ( !iEdge.isValid() ){
        SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT BUILD EDGE" )  + "\n" + getErrorMsg() );
        return false;
    }
    _value  = iEdge.model()->data(iEdge, HEXA_DATA_ROLE).value<HEXA_NS::Edge*>();

    QString newName = name_le->text();
    if ( !newName.isEmpty() )/*{*/
        getDocumentModel()->setName( iEdge, newName );

    //update the default name in the dialog box
    if (_value != NULL)
        updateDefaultName(name_le, _value->getType());

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

    installEventFilter(this);
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

    if (editmode == INFO_MODE)
        name_le->setReadOnly(true);
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
        SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT BUILD QUAD" )  + "\n" + getErrorMsg());
        return false;
    }
    _value  = iQuad.model()->data(iQuad, HEXA_DATA_ROLE).value<HEXA_NS::Quad *>();

    QString newName = name_le->text();
    if ( !newName.isEmpty() )/*{*/
        getDocumentModel()->setName( iQuad, newName );

    //the default name in the dialog box
    if (_value != NULL)
        updateDefaultName(name_le, _value->getType());

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
    quads_rb->click();

    if  ( editmode == INFO_MODE ){
        setWindowTitle( tr("Hexahedron Information") );
        quads_lw->viewport()->setAttribute( Qt::WA_TransparentForMouseEvents );
        vertices_lw->viewport()->setAttribute( Qt::WA_TransparentForMouseEvents );
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

    installEventFilter(this);
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

    if (editmode == INFO_MODE)
        name_le->setReadOnly(true);
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
        int nbQuads = quads_lw->count();
        for ( int r = 0; r < nbQuads; ++r ){
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
        int nbVertices = vertices_lw->count();
        for ( int r = 0; r < nbVertices; ++r ){
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
    if ( _editMode == INFO_MODE )
        return;

    if ( quads_rb->isChecked() ){  // build from quads count() must be between [2,6]
        int nbQuads = quads_lw->count();
        if ( nbQuads >= 2 && nbQuads <= 6 ){
            _applyButton->setEnabled(true);
        } else {
            _applyButton->setEnabled(false);
        }
    } else if ( vertices_rb->isChecked() ){ // build from vertices count() must be equals to 8
        int nbVertices = vertices_lw->count();
        if ( nbVertices == 8 ){
            _applyButton->setEnabled(true);
        } else {
            _applyButton->setEnabled(false);
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
    int nbItems = currentLw->count();
    for ( int r = 0; r < nbItems; ++r){
        item = currentLw->item(r);
        iElt = patternDataModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() );
        if ( iElt.isValid() )
            iElts << iElt;
    }

    nbItems = iElts.count();
    if ( quads_rb->isChecked() && (nbItems>=2 && nbItems<=6) ){ // build from quads iElts.count() should be between [2,6]
        iHexa = getDocumentModel()->addHexaQuads( iElts );
    } else if ( vertices_rb->isChecked() && nbItems== 8 ){ // build from vertices
        iHexa = getDocumentModel()->addHexaVertices( iElts[0], iElts[1], iElts[2], iElts[3],
                iElts[4], iElts[5], iElts[6], iElts[7] );
    }

    if ( !iHexa.isValid() ){
        SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT BUILD HEXA" ) + "\n" + getErrorMsg() );
        return false;
    }
    _value  = iHexa.model()->data(iHexa, HEXA_DATA_ROLE).value<HEXA_NS::Hexa*>();

    QString newName = name_le->text();
    if ( !newName.isEmpty() )/*{*/
        getDocumentModel()->setName( iHexa, newName );

    //update the default name in the dialog box
    if (_value != NULL)
        updateDefaultName(name_le, _value->getType());

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

    rb0->click();

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

    installEventFilter(this);
    name_le->installEventFilter(this);

    v0_le_rb1->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
    v0_le_rb1->setValidator( validator );
    v0_le_rb1->installEventFilter(this);

    v1_le_rb1->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
    v1_le_rb1->setValidator( validator );
    v1_le_rb1->installEventFilter(this);

    if ( editmode == INFO_MODE ){
        name_le->setReadOnly(true);
        dx_spb_rb0->setReadOnly(true);
        dy_spb_rb0->setReadOnly(true);
        dz_spb_rb0->setReadOnly(true);
    }

    v0_le_rb1->setReadOnly(true);
    v1_le_rb1->setReadOnly(true);
}

// ============================================================== clear
void VectorDialog::clear()
{
    name_le->clear();
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

    if ( !getDocumentModel() ) return false;
    const PatternDataModel*    patternDataModel    = getPatternDataModel();
//    const PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();
    if ( !patternDataModel /*|| !patternBuilderModel*/) return false;

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
        SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT BUILD VECTOR" ) + "\n" + getErrorMsg());
        return false;
    }

    _value  = iVector.model()->data(iVector, HEXA_DATA_ROLE).value<HEXA_NS::Vector *>();

    QString newName = name_le->text();
    if ( !newName.isEmpty() )/*{*/
        getDocumentModel()->setName( iVector, newName );

    //update the default name in the dialog box
    if (_value != NULL)
        updateDefaultName(name_le, _value->getType());

    // to select/highlight result
    result = patternDataModel->mapFromSource(iVector);
//    result = patternBuilderModel->mapFromSource(iVector);

    return true;
}

// ------------------------- MakeGridDialog ----------------------------------
// ============================================================== Constructeur

MakeGridDialog::MakeGridDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f )
: HexaBaseDialog(parent, editmode, f)
{
    setupUi( this );
    _initWidget(editmode);
    rb0->click();

    _helpFileName = "creategrids.html#guicartgridsimple";
    connect( rb0, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
    connect( rb0, SIGNAL(clicked()), this, SLOT(clearVTKSelection()) );
    connect( rb0, SIGNAL(clicked()), this, SLOT(clearCurrentObjectFocus()) );

    connect( rb1, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
    connect( rb1, SIGNAL(clicked()), this, SLOT(clearVTKSelection()) );
    connect( rb1, SIGNAL(clicked()), this, SLOT(clearCurrentObjectFocus()) );

    connect( rb2, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
    connect( rb2, SIGNAL(clicked()), this, SLOT(clearVTKSelection()) );
    connect( rb2, SIGNAL(clicked()), this, SLOT(clearCurrentObjectFocus()) );
}

// ============================================================== Destructeur
MakeGridDialog::~MakeGridDialog()
{
}

// ============================================================== _initInputWidget
void MakeGridDialog::_initInputWidget( Mode editmode )
{
    center_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
    axis_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );
    base_le->setProperty( "HexaWidgetType", QVariant::fromValue(VECTOR_TREE) );
    vec_le->setProperty( "HexaWidgetType", QVariant::fromValue(VECTOR_TREE) );

    center_le->setReadOnly(true);
    axis_le->setReadOnly(true);
    base_le->setReadOnly(true);
    vec_le->setReadOnly(true);

    installEventFilter(this);
    center_le->installEventFilter(this);
    axis_le->installEventFilter(this);
    base_le->installEventFilter(this);
    vec_le->installEventFilter(this);

    radius_lw->setItemDelegate(new HexaPositiveDoubleSpinBoxDelegate(radius_lw));
    radius_lw->setEditTriggers(QAbstractItemView::DoubleClicked);

    angle_lw->setItemDelegate(new HexaPositiveDoubleSpinBoxDelegate(angle_lw));
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

// ============================================================== clear
void MakeGridDialog::clear()
{
    center_le->clear();
    modelUnregister(center_le);

    axis_le->clear();
    modelUnregister(axis_le);

    base_le->clear();
    modelUnregister(base_le);

    vec_le->clear();
    modelUnregister(vec_le);

    modelUnregister(this);
}

// ============================================================== updateHelpFileName
void MakeGridDialog::updateHelpFileName()
{
    if ( sender() == rb0 ){
        _helpFileName = "creategrids.html#guicartgridsimple";
    } else if ( sender() == rb1 ){
        _helpFileName = "creategrids.html#guicartgriduniform";
    } else if ( sender() == rb2 ){
        _helpFileName = "creategrids.html#guicartgridcustom";
    }
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
}

// ============================================================== delRadiusItem
void MakeGridDialog::delRadiusItem()
{
    delete radius_lw->currentItem();
}

// ============================================================== addAngleItem
void MakeGridDialog::addAngleItem()
{
    QListWidgetItem* previousItem = angle_lw->currentItem();
    QListWidgetItem* newItem      = new QListWidgetItem();

    double defaultValue = 1.;
    if ( previousItem )
        defaultValue = previousItem->data(Qt::EditRole).toDouble();

    newItem->setData(  Qt::EditRole, QVariant(defaultValue) );
    newItem->setFlags( newItem->flags () | Qt::ItemIsEditable);
    angle_lw->addItem(newItem);
}

// ============================================================== delAngleItem
void MakeGridDialog::delAngleItem()
{
    delete angle_lw->currentItem();
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
}

// ============================================================== delHeightItem
void MakeGridDialog::delHeightItem()
{
    delete height_lw->currentItem();
}

// ============================================================== apply
bool MakeGridDialog::apply(QModelIndex& result)
{
    if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
    _currentObj = NULL;

    DocumentModel* docModel = getDocumentModel();
    PatternDataModel*    patternDataModel    = getPatternDataModel();
//    PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();

    QModelIndex iNewElts;
    if ( rb0->isChecked() )
    {
        long nx = nx_spb->value();
        long ny = ny_spb->value();
        long nz = nz_spb->value();
        iNewElts = docModel->makeCartesianTop(nx, ny, nz);
    }
    else if ( rb1->isChecked() )
    {
        QModelIndex icenter = patternDataModel->mapToSource( _index[center_le] );
        QModelIndex iaxis   = patternDataModel->mapToSource( _index[axis_le] );
        QModelIndex ibase   = patternDataModel->mapToSource( _index[base_le] );
        QModelIndex ivec    = patternDataModel->mapToSource( _index[vec_le] );
//        QModelIndex iaxis   = patternBuilderModel->mapToSource( _index[axis_le] );
//        QModelIndex ibase   = patternBuilderModel->mapToSource( _index[base_le] );
//        QModelIndex ivec    = patternBuilderModel->mapToSource( _index[vec_le] );

        if ( icenter.isValid() && iaxis.isValid() && ibase.isValid() && ivec.isValid() )
        {
            double lx = lx_spb->value();
            double ly = ly_spb->value();
            double lz = lz_spb->value();
            double nx = nx_spb->value();
            double ny = ny_spb->value();
            double nz = nz_spb->value();

            iNewElts = docModel->makeCartesianUni( icenter, ibase, ivec, iaxis,
                                                   lx, ly, lz, nx, ny, nz);
        }

    } else if ( rb2->isChecked() )
    {
        QModelIndex icenter = patternDataModel->mapToSource( _index[center_le] );
        QModelIndex iaxis   = patternDataModel->mapToSource( _index[axis_le] );
        QModelIndex ibase   = patternDataModel->mapToSource( _index[base_le] );
        QModelIndex ivec    = patternDataModel->mapToSource( _index[vec_le] );
//        QModelIndex iaxis   = patternBuilderModel->mapToSource( _index[axis_le] );
//        QModelIndex ibase   = patternBuilderModel->mapToSource( _index[base_le] );
//        QModelIndex ivec    = patternBuilderModel->mapToSource( _index[vec_le] );

        if ( icenter.isValid() && iaxis.isValid() && ibase.isValid() && ivec.isValid() )
        {
            QListWidgetItem* item = NULL;

            vector<double> radius;
            vector<double> angles;
            vector<double> heights;

//            double somme = 0.;
            int nbAngles = angle_lw->count();
            for ( int r = 0; r < nbAngles; ++r){
                item = angle_lw->item(r);
                double itemValue = item->data(Qt::EditRole).toDouble();
                angles.push_back(itemValue);
//                somme += itemValue;
            }
//            if (somme > 360.01)
//            {
//                SUIT_MessageBox::information( 0,
//                        tr("HEXA_INFO"),
//                        tr("The sum of the picked angles has to be \nless or equal than %1 degrees.").arg(360));
//                return false;
//            }

            int nbRadius = radius_lw->count();
            for ( int r = 0; r < nbRadius; ++r){
                item = radius_lw->item(r);
                radius.push_back(item->data(Qt::EditRole).toDouble());
            }

            int nbHeight = height_lw->count();
            for ( int r = 0; r < nbHeight; ++r){
                item = height_lw->item(r);
                heights.push_back(item->data(Qt::EditRole).toDouble());
            }

            iNewElts =  docModel->makeCartesian( icenter, ibase, ivec, iaxis,
                                                 radius, angles, heights);
        }
    }

    if ( !iNewElts.isValid() )
    {
        SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE GRID" ) + "\n" + getErrorMsg() );
        return false;
    }

    result = patternDataModel->mapFromSource(iNewElts);
//    result = patternBuilderModel->mapFromSource( iNewElts );

    return true;
}

// ============================================================== Constructeur

MakeCylinderDialog::MakeCylinderDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f )
: HexaBaseDialog(parent, editmode, f)
{
    setupUi( this );
    _helpFileName = "gui_blocks_for_cyl_pipe.html#guicylinder";
    connect( rb0, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
    connect( rb1, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
    connect( rb2, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
    _initWidget(editmode);

    rb0->click();
}

// ============================================================== Destructeur
MakeCylinderDialog::~MakeCylinderDialog()
{
}

void MakeCylinderDialog::_initInputWidget( Mode editmode )
{
    installEventFilter(this);

    rb0->installEventFilter(this);
    rb1->installEventFilter(this);
    rb2->installEventFilter(this);

    origin_le->setProperty( "HexaWidgetType", QVariant::fromValue(VERTEX_TREE));
    origin_le->installEventFilter(this);
    origin_le->setReadOnly(true);

    axis_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );
    axis_le->installEventFilter(this);
    axis_le->setReadOnly(true);

    base_le->setProperty( "HexaWidgetType", QVariant::fromValue(VECTOR_TREE));
    base_le->installEventFilter(this);
    base_le->setReadOnly(true);

    ext_radius_spb->setProperty( "HexaWidgetType", QVariant::fromValue(EDGE_TREE));
    ext_radius_spb->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_EDGE));
    ext_radius_spb->setProperty("Radius", QVariant::fromValue(true));
    ext_radius_spb->installEventFilter(this);

    int_radius_spb->setProperty( "HexaWidgetType", QVariant::fromValue(EDGE_TREE));
    int_radius_spb->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_EDGE));
    int_radius_spb->setProperty("Radius", QVariant::fromValue(true));
    int_radius_spb->installEventFilter(this);

    angle_spb->setProperty( "HexaWidgetType", QVariant::fromValue(EDGE_TREE));
    angle_spb->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_EDGE));
    angle_spb->setProperty("Angle", QVariant::fromValue(true));
    angle_spb->installEventFilter(this);

    height_spb->setProperty( "HexaWidgetType", QVariant::fromValue(EDGE_TREE));
    height_spb->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_EDGE));
    height_spb->setProperty("Length", QVariant::fromValue(true));
    height_spb->installEventFilter(this);

    radius_lw->setProperty( "HexaWidgetType", QVariant::fromValue(EDGE_TREE));
    radius_lw->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_EDGE));
    radius_lw->setProperty("Radius", QVariant::fromValue(true));
    radius_lw->installEventFilter(this);


    angle_lw->setProperty( "HexaWidgetType", QVariant::fromValue(EDGE_TREE));
    angle_lw->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_EDGE));
    angle_lw->setProperty("Angle", QVariant::fromValue(true));
    angle_lw->installEventFilter(this);

    height_lw->setProperty( "HexaWidgetType", QVariant::fromValue(EDGE_TREE));
    height_lw->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_EDGE));
    height_lw->setProperty("Length", QVariant::fromValue(true));
    height_lw->installEventFilter(this);

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

// ============================================================== updateHelpFileName
void MakeCylinderDialog::updateHelpFileName()
{
    if ( sender() == rb0 ){
        _helpFileName = "gui_blocks_for_cyl_pipe.html#guicylindersimple";
    } else if ( sender() == rb1 ){
        _helpFileName = "gui_blocks_for_cyl_pipe.html#guicylinderuniform";
    } else if ( sender() == rb2 ){
        _helpFileName = "gui_blocks_for_cyl_pipe.html#guicylindercustom";
    }
}

// ============================================================== addRadiusItem
void MakeCylinderDialog::addRadiusItem()
{
    QListWidgetItem* previousItem = radius_lw->currentItem();
    QListWidgetItem* newItem      = new QListWidgetItem();

    double defaultValue = 1.;
    if ( previousItem )
        defaultValue = previousItem->data(Qt::EditRole).toDouble();

    newItem->setData(  Qt::EditRole, QVariant(defaultValue) );
    newItem->setFlags( newItem->flags () | Qt::ItemIsEditable);
    radius_lw->addItem(newItem);
}

// ============================================================== delRadiusItem
void MakeCylinderDialog::delRadiusItem()
{
    delete radius_lw->currentItem();
}

// ============================================================== addAngleItem
void MakeCylinderDialog::addAngleItem()
{
    QListWidgetItem* previousItem = angle_lw->currentItem();
    QListWidgetItem* newItem      = new QListWidgetItem();

    double defaultValue = 180.;
    if ( previousItem )
        defaultValue = previousItem->data(Qt::EditRole).toDouble();

    newItem->setData(  Qt::EditRole, QVariant(defaultValue) );
    newItem->setFlags( newItem->flags () | Qt::ItemIsEditable);
    angle_lw->addItem(newItem);
}

// ============================================================== delAngleItem
void MakeCylinderDialog::delAngleItem()
{
    delete angle_lw->currentItem();
}

// ============================================================== addHeightItem
void MakeCylinderDialog::addHeightItem()
{
    QListWidgetItem* previousItem = height_lw->currentItem();
    QListWidgetItem* newItem      = new QListWidgetItem();

    double defaultValue = 1.;
    if ( previousItem )
        defaultValue = previousItem->data(Qt::EditRole).toDouble();

    newItem->setData(  Qt::EditRole, QVariant(defaultValue) );
    newItem->setFlags( newItem->flags () | Qt::ItemIsEditable);
    height_lw->addItem(newItem);
}

// ============================================================== delHeightItem
void MakeCylinderDialog::delHeightItem()
{
    delete height_lw->currentItem();
}

// ============================================================== clear
void MakeCylinderDialog::clear()
{
    origin_le->clear();
    modelUnregister(origin_le);

    axis_le->clear();
    modelUnregister(axis_le);

    base_le->clear();
    modelUnregister(base_le);

    modelUnregister(this);
}

// ============================================================== apply
bool MakeCylinderDialog::apply(QModelIndex& result)
{
    if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
    _currentObj = NULL;

    DocumentModel* docModel = getDocumentModel();
    PatternDataModel* patternDataModel = getPatternDataModel();
//    PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();

    QModelIndex iorigin = patternDataModel->mapToSource( _index[origin_le] );
    QModelIndex iaxis = patternDataModel->mapToSource( _index[axis_le] );
    QModelIndex ibase = patternDataModel->mapToSource( _index[base_le] );
//    QModelIndex iaxis = patternBuilderModel->mapToSource( _index[axis_le] );
//    QModelIndex ibase = patternBuilderModel->mapToSource( _index[base_le] );
    double rext = ext_radius_spb->value();
    double rint = int_radius_spb->value();
    double angle = angle_spb->value();
    double height = height_spb->value();
    double nr = nr_spb->value();
    double na = na_spb->value();
    double nh = nh_spb->value();

    QModelIndex iElts;
    if (rb0->isChecked())
        iElts = docModel->makeCylinderTop(nr, na, nh);
    else if (rb1->isChecked())
    {
        if ( iorigin.isValid() && iaxis.isValid() && ibase.isValid())
                iElts = docModel->makeCylinderUni(iorigin, ibase, iaxis, rint,
                                                  rext, angle, height, nr, na, nh);
    }
    else if (rb2->isChecked())
    {
        QListWidgetItem* item = NULL;

        vector<double> radius;
        vector<double> angles;
        vector<double> heights;

//        double somme = 0.;
        int nbAngles = angle_lw->count();
        for ( int r = 0; r < nbAngles; ++r){
            item = angle_lw->item(r);
            double itemValue = item->data(Qt::EditRole).toDouble();
            angles.push_back(itemValue);
//            somme += itemValue;
        }
//        if (somme > 360.01)
//        {
//            SUIT_MessageBox::information( 0,
//                    tr("HEXA_INFO"),
//                    tr("The sum of the picked angles has to be \nless or equal than %1 degrees.").arg(360));
//            return false;
//        }

        int nbRadius = radius_lw->count();
        for ( int r = 0; r < nbRadius; ++r){
            item = radius_lw->item(r);
            radius.push_back(item->data(Qt::EditRole).toDouble());
        }

        int nbHeight = height_lw->count();
        for ( int r = 0; r < nbHeight; ++r){
            item = height_lw->item(r);
            heights.push_back(item->data(Qt::EditRole).toDouble());
        }

        if ( iorigin.isValid() && iaxis.isValid() && ibase.isValid())
                    iElts = docModel->makeCylinder( iorigin, ibase, iaxis, radius, angles, heights);
    }


    if ( !iElts.isValid() ){
        SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE CYLINDER" ) + "\n" + getErrorMsg() );
        return false;
    }

    result = patternDataModel->mapFromSource(iElts);
//    result = patternBuilderModel->mapFromSource(iElts);

    return true;
}

//---------------------------------- MakePipeDialog -----------------------------
// ============================================================== Constructeur

MakePipeDialog::MakePipeDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f )
: HexaBaseDialog(parent, editmode, f)
{
	setupUi( this );
	_helpFileName = "gui_blocks_for_cyl_pipe.html#guipipe";
	connect( rb0, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
	connect( rb1, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
	connect( rb2, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
    _initWidget(editmode);
    rb0->click();
}

// ============================================================== Destructeur
MakePipeDialog::~MakePipeDialog()
{
}

// ============================================================== _initInputWidget
void MakePipeDialog::_initInputWidget( Mode editmode )
{
    origin_le->setProperty( "HexaWidgetType", QVariant::fromValue(VERTEX_TREE) );
    axis_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );
    base_le->setProperty( "HexaWidgetType", QVariant::fromValue(VECTOR_TREE) );

    installEventFilter(this);
    origin_le->installEventFilter(this);
    axis_le->installEventFilter(this);
    base_le->installEventFilter(this);

    rb0->installEventFilter(this);
    rb1->installEventFilter(this);
    rb2->installEventFilter(this);

    origin_le->setReadOnly(true);
    axis_le->setReadOnly(true);
    base_le->setReadOnly(true);

    ext_radius_spb->setProperty( "HexaWidgetType", QVariant::fromValue(EDGE_TREE));
    ext_radius_spb->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_EDGE));
    ext_radius_spb->setProperty("Radius", QVariant::fromValue(true));
    ext_radius_spb->installEventFilter(this);

    int_radius_spb->setProperty( "HexaWidgetType", QVariant::fromValue(EDGE_TREE));
    int_radius_spb->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_EDGE));
    int_radius_spb->setProperty("Radius", QVariant::fromValue(true));
    int_radius_spb->installEventFilter(this);

    angle_spb->setProperty( "HexaWidgetType", QVariant::fromValue(EDGE_TREE));
    angle_spb->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_EDGE));
    angle_spb->setProperty("Angle", QVariant::fromValue(true));
    angle_spb->installEventFilter(this);

    height_spb->setProperty( "HexaWidgetType", QVariant::fromValue(EDGE_TREE));
    height_spb->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_EDGE));
    height_spb->setProperty("Length", QVariant::fromValue(true));
    height_spb->installEventFilter(this);

    radius_lw->setProperty( "HexaWidgetType", QVariant::fromValue(EDGE_TREE));
    radius_lw->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_EDGE));
    radius_lw->setProperty("Radius", QVariant::fromValue(true));
    radius_lw->installEventFilter(this);

    angle_lw->setProperty( "HexaWidgetType", QVariant::fromValue(EDGE_TREE));
    angle_lw->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_EDGE));
    angle_lw->setProperty("Angle", QVariant::fromValue(true));
    angle_lw->installEventFilter(this);

    height_lw->setProperty( "HexaWidgetType", QVariant::fromValue(EDGE_TREE));
    height_lw->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_EDGE));
    height_lw->setProperty("Length", QVariant::fromValue(true));
    height_lw->installEventFilter(this);

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


// ============================================================== updateHelpFileName
void MakePipeDialog::updateHelpFileName()
{
    if ( sender() == rb0 ){
        _helpFileName = "gui_blocks_for_cyl_pipe.html#guipipesimple";
    } else if ( sender() == rb1 ){
        _helpFileName = "gui_blocks_for_cyl_pipe.html#guipipeuniform";
    } else if ( sender() == rb2 ){
        _helpFileName = "gui_blocks_for_cyl_pipe.html#guipipecustom";
    }
}

// ============================================================== addRadiusItem
void MakePipeDialog::addRadiusItem()
{
    QListWidgetItem* previousItem = radius_lw->currentItem();
    QListWidgetItem* newItem      = new QListWidgetItem();

    double defaultValue = 1.;
    if ( previousItem )
        defaultValue = previousItem->data(Qt::EditRole).toDouble();

    newItem->setData(  Qt::EditRole, QVariant(defaultValue) );
    newItem->setFlags( newItem->flags () | Qt::ItemIsEditable);
    radius_lw->addItem(newItem);
}

// ============================================================== delRadiusItem
void MakePipeDialog::delRadiusItem()
{
    delete radius_lw->currentItem();
}

// ============================================================== addAngleItem
void MakePipeDialog::addAngleItem()
{
    QListWidgetItem* previousItem = angle_lw->currentItem();
    QListWidgetItem* newItem      = new QListWidgetItem();

    double defaultValue = 180.;
    if ( previousItem )
        defaultValue = previousItem->data(Qt::EditRole).toDouble();

    newItem->setData(  Qt::EditRole, QVariant(defaultValue) );
    newItem->setFlags( newItem->flags () | Qt::ItemIsEditable);
    angle_lw->addItem(newItem);
}

// ============================================================== delAngleItem
void MakePipeDialog::delAngleItem()
{
    delete angle_lw->currentItem();
}

// ============================================================== addHeightItem
void MakePipeDialog::addHeightItem()
{
    QListWidgetItem* previousItem = height_lw->currentItem();
    QListWidgetItem* newItem      = new QListWidgetItem();

    double defaultValue = 1.;
    if ( previousItem )
        defaultValue = previousItem->data(Qt::EditRole).toDouble();

    newItem->setData(  Qt::EditRole, QVariant(defaultValue) );
    newItem->setFlags( newItem->flags () | Qt::ItemIsEditable);
    height_lw->addItem(newItem);
}

// ============================================================== delHeightItem
void MakePipeDialog::delHeightItem()
{
    delete height_lw->currentItem();
}

// ============================================================== clear
void MakePipeDialog::clear()
{
    origin_le->clear();
    modelUnregister(origin_le);

    axis_le->clear();
    modelUnregister(axis_le);

    base_le->clear();
    modelUnregister(base_le);

    modelUnregister(this);
}

// ============================================================== apply
bool MakePipeDialog::apply(QModelIndex& result)
{
    if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
    _currentObj = NULL;

    DocumentModel* docModel = getDocumentModel();
    PatternDataModel* patternDataModel = getPatternDataModel();
//    PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();

    QModelIndex iorigin = patternDataModel->mapToSource( _index[origin_le] );
    QModelIndex iaxis = patternDataModel->mapToSource( _index[axis_le] );
    QModelIndex ibase = patternDataModel->mapToSource( _index[base_le] );
//    QModelIndex iaxis  = patternBuilderModel->mapToSource( _index[axis_le] );
//    QModelIndex ibase = patternBuilderModel->mapToSource( _index[base_le] );
    double rext = ext_radius_spb->value();
    double rint = int_radius_spb->value();
    double angle = angle_spb->value();
    double height = height_spb->value();
    int nr = nr_spb->value();
    int na = na_spb->value();
    int nh = nh_spb->value();

    QModelIndex iElts;

    if (rb0->isChecked())
        iElts = docModel->makePipeTop(nr, na, nh);
    else if (rb1->isChecked())
    {
        if ( iorigin.isValid() && iaxis.isValid() && ibase.isValid())
            iElts = docModel->makePipeUni(iorigin, ibase, iaxis, rint, rext, angle, height,
                                          nr, na, nh);
    }
    else if (rb2->isChecked())
    {
        QListWidgetItem* item = NULL;

        vector<double> radius;
        vector<double> angles;
        vector<double> heights;

//        double somme = 0.;
        int nbAngles = angle_lw->count();
        for ( int r = 0; r < nbAngles; ++r){
            item = angle_lw->item(r);
            double itemValue = item->data(Qt::EditRole).toDouble();
            angles.push_back(itemValue);
//            somme += itemValue;
        }
//        if (somme > 360.01)
//        {
//            SUIT_MessageBox::information( 0,
//                    tr("HEXA_INFO"),
//                    tr("The sum of the picked angles has to be \nless or equal than %1 degrees.").arg(360));
//            return false;
//        }

        int nbRadius = radius_lw->count();
        for ( int r = 0; r < nbRadius; ++r){
            item = radius_lw->item(r);
            radius.push_back(item->data(Qt::EditRole).toDouble());
        }

        int nbHeight = height_lw->count();
        for ( int r = 0; r < nbHeight; ++r){
            item = height_lw->item(r);
            heights.push_back(item->data(Qt::EditRole).toDouble());
        }
        if ( iorigin.isValid() && iaxis.isValid() && ibase.isValid())
            iElts = docModel->makePipe( iorigin, ibase, iaxis, radius, angles, heights );
    }

    if ( !iElts.isValid() ){
        SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE PIPE" ) + "\n" + getErrorMsg() );
        return false;
    }

    // to select/highlight result
    result = patternDataModel->mapFromSource(iElts);
//    result = patternBuilderModel->mapFromSource(iElts);

    return true;
}

//---------------------------------- MakeCylindersDialog -----------------------------
// ============================================================== Constructeur

MakeCylindersDialog::MakeCylindersDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f )
: HexaBaseDialog(parent, editmode, f)
{
    _helpFileName = "gui_blocks_for_cyl_pipe.html#guicylinders";
    setupUi( this );
    _initWidget(editmode);
}

// ============================================================== Destructeur
MakeCylindersDialog::~MakeCylindersDialog()
{
}

void MakeCylindersDialog::_initInputWidget( Mode editmode )
{
    center_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
    center2_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
    direction_le->setProperty( "HexaWidgetType", QVariant::fromValue(VECTOR_TREE) );
    direction2_le->setProperty( "HexaWidgetType", QVariant::fromValue(VECTOR_TREE) );

    radius_spb->setProperty("HexaWidgetType", QVariant::fromValue(EDGE_TREE));
    radius_spb->setProperty("GeomWidgetType", QVariant::fromValue(TopAbs_EDGE));
    radius_spb->setProperty("Radius", QVariant::fromValue(true));
    radius_spb->installEventFilter(this);

    height_spb->setProperty("HexaWidgetType", QVariant::fromValue(EDGE_TREE));
    height_spb->setProperty("GeomWidgetType", QVariant::fromValue(TopAbs_EDGE));
    height_spb->setProperty("Length", QVariant::fromValue(true));
    height_spb->installEventFilter(this);

    radius2_spb->setProperty("HexaWidgetType", QVariant::fromValue(EDGE_TREE));
    radius2_spb->setProperty("GeomWidgetType", QVariant::fromValue(TopAbs_EDGE));
    radius2_spb->setProperty("Radius", QVariant::fromValue(true));
    radius2_spb->installEventFilter(this);

    height2_spb->setProperty("HexaWidgetType", QVariant::fromValue(EDGE_TREE));
    height2_spb->setProperty("GeomWidgetType", QVariant::fromValue(TopAbs_EDGE));
    height2_spb->setProperty("Length", QVariant::fromValue(true));
    height2_spb->installEventFilter(this);

    installEventFilter(this);
    center_le->installEventFilter(this);
    center2_le->installEventFilter(this);
    direction_le->installEventFilter(this);
    direction2_le->installEventFilter(this);

    center_le->setReadOnly(true);
    center2_le->setReadOnly(true);
    direction_le->setReadOnly(true);
    direction2_le->setReadOnly(true);
}

// ============================================================== clear
void MakeCylindersDialog::clear()
{
    center_le->clear();
    modelUnregister(center_le);

    center2_le->clear();
    modelUnregister(center2_le);

    direction_le->clear();
    modelUnregister(direction_le);

    direction2_le->clear();
    modelUnregister(direction2_le);

    modelUnregister(this);
}

// ============================================================== apply
bool MakeCylindersDialog::apply(QModelIndex& result)
{
    if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
    _currentObj = NULL;

    DocumentModel* docModel = getDocumentModel();
    PatternDataModel* patternDataModel = getPatternDataModel();
//    PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();

    QModelIndex iElts;
    QModelIndex icenter = patternDataModel->mapToSource( _index[center_le] );
    QModelIndex icenter2 = patternDataModel->mapToSource( _index[center2_le] );
    QModelIndex idir = patternDataModel->mapToSource( _index[direction_le] );
    QModelIndex idir2 = patternDataModel->mapToSource( _index[direction2_le] );
//    QModelIndex idir = patternBuilderModel->mapToSource( _index[direction_le] );
//    QModelIndex idir2 = patternBuilderModel->mapToSource( _index[direction2_le] );

    if ( icenter.isValid() && icenter2.isValid() && idir.isValid() && idir2.isValid()){
        double r1 = radius_spb->value();
        double h1 = height_spb->value();
        double r2 = radius2_spb->value();
        double h2 = height2_spb->value();
        iElts = docModel->makeCylinders( icenter, idir, r1, h1, icenter2, idir2, r2, h2 );
    }

    if ( !iElts.isValid() ){
        SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE CYLINDERS" ) + "\n" + getErrorMsg() );
        return false;
    }

    result = patternDataModel->mapFromSource(iElts);
//    result = patternBuilderModel->mapFromSource(iElts);

    return true;
}

//---------------------------------- MakePipesDialog -----------------------------
// ============================================================== Constructeur

MakePipesDialog::MakePipesDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f )
: HexaBaseDialog(parent, editmode, f)
{
    _helpFileName = "gui_blocks_for_cyl_pipe.html#guipipes";
    setupUi( this );
    _initWidget(editmode);
}

// ============================================================== Destructeur
MakePipesDialog::~MakePipesDialog()
{
}

// ============================================================== _initInputWidget
void MakePipesDialog::_initInputWidget( Mode editmode )
{
    origin_le->setProperty( "HexaWidgetType", QVariant::fromValue(VERTEX_TREE) );
    origin2_le->setProperty( "HexaWidgetType", QVariant::fromValue(VERTEX_TREE) );
    dir_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );
    dir2_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );

    ext_radius_spb->setProperty("HexaWidgetType", QVariant::fromValue(EDGE_TREE));
    ext_radius_spb->setProperty("GeomWidgetType", QVariant::fromValue(TopAbs_EDGE));
    ext_radius_spb->setProperty("Radius", QVariant::fromValue(true));
    ext_radius_spb->installEventFilter(this);

    int_radius_spb->setProperty("HexaWidgetType", QVariant::fromValue(EDGE_TREE));
    int_radius_spb->setProperty("GeomWidgetType", QVariant::fromValue(TopAbs_EDGE));
    int_radius_spb->setProperty("Radius", QVariant::fromValue(true));
    int_radius_spb->installEventFilter(this);

    height_spb->setProperty("HexaWidgetType", QVariant::fromValue(EDGE_TREE));
    height_spb->setProperty("GeomWidgetType", QVariant::fromValue(TopAbs_EDGE));
    height_spb->setProperty("Length", QVariant::fromValue(true));
    height_spb->installEventFilter(this);

    ext_radius2_spb->setProperty("HexaWidgetType", QVariant::fromValue(EDGE_TREE));
    ext_radius2_spb->setProperty("GeomWidgetType", QVariant::fromValue(TopAbs_EDGE));
    ext_radius2_spb->setProperty("Radius", QVariant::fromValue(true));
    ext_radius2_spb->installEventFilter(this);

    int_radius2_spb->setProperty("HexaWidgetType", QVariant::fromValue(EDGE_TREE));
    int_radius2_spb->setProperty("GeomWidgetType", QVariant::fromValue(TopAbs_EDGE));
    int_radius2_spb->setProperty("Radius", QVariant::fromValue(true));
    int_radius2_spb->installEventFilter(this);

    height2_spb->setProperty("HexaWidgetType", QVariant::fromValue(EDGE_TREE));
    height2_spb->setProperty("GeomWidgetType", QVariant::fromValue(TopAbs_EDGE));
    height2_spb->setProperty("Length", QVariant::fromValue(true));
    height2_spb->installEventFilter(this);

    origin_le->installEventFilter(this);
    origin2_le->installEventFilter(this);
    dir_le->installEventFilter(this);
    dir2_le->installEventFilter(this);

    origin_le->setReadOnly(true);
    origin2_le->setReadOnly(true);
    dir_le->setReadOnly(true);
    dir2_le->setReadOnly(true);
}

// ============================================================== clear
void MakePipesDialog::clear()
{
    origin_le->clear();
    modelUnregister(origin_le);

    origin2_le->clear();
    modelUnregister(origin2_le);

    dir_le->clear();
    modelUnregister(dir_le);

    dir2_le->clear();
    modelUnregister(dir2_le);

    modelUnregister(this);
}

// ============================================================== apply
bool MakePipesDialog::apply(QModelIndex& result)
{
    if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
    _currentObj = NULL;

    DocumentModel* docModel = getDocumentModel();
    PatternDataModel* patternDataModel = getPatternDataModel();
//    PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();

    QModelIndex iorigin1 = patternDataModel->mapToSource( _index[origin_le] );
    QModelIndex iorigin2 = patternDataModel->mapToSource( _index[origin2_le] );
    QModelIndex idir1 = patternDataModel->mapToSource( _index[dir_le] );
    QModelIndex idir2 = patternDataModel->mapToSource( _index[dir2_le] );
//    QModelIndex idir1 = patternBuilderModel->mapToSource( _index[dir_le] );
//    QModelIndex idir2 = patternBuilderModel->mapToSource( _index[dir2_le] );
    double rint1 = int_radius_spb->value();
    double rext1 = ext_radius_spb->value();
    double height1 = height_spb->value();
    double rint2 = int_radius2_spb->value();
    double rext2 = ext_radius2_spb->value();
    double height2 = height_spb->value();

    QModelIndex iElts;

    if ( iorigin1.isValid() && iorigin2.isValid() && idir1.isValid() && idir2.isValid())
        iElts = docModel->makePipes( iorigin1, idir1, rint1, rext1, height1,
                                     iorigin2, idir2, rint2, rext2, height2);

    if ( !iElts.isValid() ){
        SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE PIPES" ) + "\n" + getErrorMsg() );
        return false;
    }

    result = patternDataModel->mapFromSource(iElts);
//    result = patternBuilderModel->mapFromSource(iElts);

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
    autoFocusSwitch = false;
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
        SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT REMOVE HEXA" ) + "\n" + getErrorMsg() );
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
	setupUi( this );
    _helpFileName = "gui_prism_join_quad.html#guiextrudequads";
    connect( extrudeTop_rb, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
    connect( extrudeUni_rb, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
    connect( extrude_rb, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
    _initWidget(editmode);
    extrudeTop_rb->click();
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
    int nbQuads = quads_lw->count();
    for ( int r = 0; r < nbQuads; ++r ){
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
    quads_lw->setProperty( "HexaWidgetType",  QVariant::fromValue(QUAD_TREE) );
    axis_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );

    length_spb->setProperty("HexaWidgetType", QVariant::fromValue(EDGE_TREE));
    length_spb->setProperty("GeomWidgetType", QVariant::fromValue(TopAbs_EDGE));
    length_spb->setProperty("Length", QVariant::fromValue(true));
    length_spb->installEventFilter(this);


    height_lw->setProperty("HexaWidgetType", QVariant::fromValue(EDGE_TREE));
    height_lw->setProperty("GeomWidgetType", QVariant::fromValue(TopAbs_EDGE));
    height_lw->setProperty("Length", QVariant::fromValue(true));
    height_lw->installEventFilter(this);

    installEventFilter(this);
    quads_lw->installEventFilter(this);
    axis_le->installEventFilter(this);

    extrudeTop_rb->installEventFilter(this);
    extrudeUni_rb->installEventFilter(this);
    extrude_rb->installEventFilter(this);

    axis_le->setReadOnly(true);

    QShortcut* delQuadShortcut   = new QShortcut( QKeySequence(Qt::Key_X), quads_lw );
    delQuadShortcut->setContext( Qt::WidgetShortcut );
    height_lw->setItemDelegate(new HexaPositiveDoubleSpinBoxDelegate(height_lw));
    height_lw->setEditTriggers(QAbstractItemView::DoubleClicked);

    connect( delQuadShortcut, SIGNAL(activated()), this, SLOT(removeQuad()) );
    connect( quads_lw,    SIGNAL(itemSelectionChanged()), this, SLOT(selectElementOfModel()), Qt::UniqueConnection );
    connect( add_height_pb, SIGNAL(clicked()), this, SLOT(addHeightItem()) );
    connect( del_height_pb, SIGNAL(clicked()), this, SLOT(delHeightItem()) );
}

// ============================================================== updateHelpFileName
void PrismQuadDialog::updateHelpFileName()
{
    if ( sender() == extrudeTop_rb ){
        _helpFileName = "gui_prism_join_quad.html#guiextrudequadssimple";
    } else if ( sender() == extrudeUni_rb ){
        _helpFileName = "gui_prism_join_quad.html#guiextrudequadsuniform";
    } else if ( sender() == extrude_rb ){
        _helpFileName = "gui_prism_join_quad.html#guiextrudequadscustom";
    }
}

// ============================================================== clear
void PrismQuadDialog::clear()
{
    quads_lw->clear();
    modelUnregister(quads_lw);

    axis_le->clear();
    modelUnregister(axis_le);

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
}

// ============================================================== delHeightItem
void PrismQuadDialog::delHeightItem()
{
    delete height_lw->currentItem();
}

// ============================================================== apply
bool PrismQuadDialog::apply(QModelIndex& result)
{
    if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
    _currentObj = NULL;

    DocumentModel* docModel = getDocumentModel();
    PatternDataModel* patternDataModel = getPatternDataModel();
//    PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();

    QModelIndexList iquads = getIndexList(quads_lw);
    QModelIndex iaxis = patternDataModel->mapToSource( _index[axis_le] );
//    QModelIndex iaxis  = patternBuilderModel->mapToSource( _index[axis_le] );
    double length = length_spb->value();
    int nb = nb_spb->value();

    vector<double> heights;
    QListWidgetItem* item = NULL;
    int nbItems = height_lw->count();

    for ( int r = 0; r < nbItems; ++r){
        item = height_lw->item(r);
        heights.push_back( item->data(Qt::EditRole).toDouble() );
    }

    QModelIndex iElts;
    int nbQuads = iquads.count();

    if (nbQuads == 1 && iquads[0].isValid())
    {
        if (extrudeTop_rb->isChecked())
            iElts = docModel->extrudeQuadTop(iquads[0], nb);
        else if (extrudeUni_rb->isChecked() && iaxis.isValid())
            iElts = docModel->extrudeQuadUni(iquads[0], iaxis, length, nb);
        else if (extrude_rb->isChecked() && iaxis.isValid())
            iElts = docModel->extrudeQuad(iquads[0], iaxis, heights);
    }
    else if (nbQuads > 1)
    {
        if (extrudeTop_rb->isChecked())
            iElts = docModel->extrudeQuadsTop(iquads, nb);
        else if (extrudeUni_rb->isChecked() && iaxis.isValid())
            iElts = docModel->extrudeQuadsUni(iquads, iaxis, length, nb);
        else if (extrude_rb->isChecked() && iaxis.isValid())
            iElts = docModel->extrudeQuads(iquads, iaxis, heights);
    }

    if ( !iElts.isValid() ){
        SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT PRISM QUAD(S)" ) + "\n" + getErrorMsg() );
        return false;
    }

    result = patternDataModel->mapFromSource(iElts);
//    result = patternBuilderModel->mapFromSource(iElts);

    return true;
}

//---------------------------------- JoinQuadDialog -----------------------------
// ============================================================== Constructeur

JoinQuadDialog::JoinQuadDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f )
: HexaBaseDialog(parent, editmode, f)
{
	setupUi( this );
	_helpFileName = "gui_prism_join_quad.html#guijoinquads";
	connect( joinUni_rb, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
	connect( join_rb, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
    _initWidget(editmode);
    joinUni_rb->click();
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
    int nbQuads = quads_lw->count();
    for ( int r = 0; r < nbQuads; ++r ){
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
    quad_dest_le->setProperty( "HexaWidgetType",  QVariant::fromValue(QUAD_TREE) );
    quads_lw->setProperty( "HexaWidgetType",  QVariant::fromValue(QUAD_TREE) );
    vex0_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
    vex1_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
    vex2_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
    vex3_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );

    height_lw->setProperty("HexaWidgetType", QVariant::fromValue(EDGE_TREE));
    height_lw->setProperty("GeomWidgetType", QVariant::fromValue(TopAbs_EDGE));
    height_lw->setProperty("Length", QVariant::fromValue(true));
    height_lw->installEventFilter(this);

    installEventFilter(this);
    quad_dest_le->installEventFilter(this);
    quads_lw->installEventFilter(this);
    vex0_le->installEventFilter(this);
    vex1_le->installEventFilter(this);
    vex2_le->installEventFilter(this);
    vex3_le->installEventFilter(this);

    joinUni_rb->installEventFilter(this);
    join_rb->installEventFilter(this);

    QShortcut* delQuadShortcut = new QShortcut( QKeySequence(Qt::Key_X), quads_lw );
    delQuadShortcut->setContext( Qt::WidgetShortcut );
    connect( delQuadShortcut,   SIGNAL(activated()), this, SLOT(removeQuad()) );

    quad_dest_le->setReadOnly(true);
    vex0_le->setReadOnly(true);
    vex1_le->setReadOnly(true);
    vex2_le->setReadOnly(true);
    vex3_le->setReadOnly(true);

    _currentObj = quads_lw;
    height_lw->setItemDelegate(new HexaPositiveDoubleSpinBoxDelegate(height_lw));
    height_lw->setEditTriggers(QAbstractItemView::DoubleClicked);

    connect( quads_lw,    SIGNAL(itemSelectionChanged()), this, SLOT(selectElementOfModel()), Qt::UniqueConnection );

    connect( add_height_pb, SIGNAL(clicked()), this, SLOT(addHeightItem()) );
    connect( del_height_pb, SIGNAL(clicked()), this, SLOT(delHeightItem()) );
}

// ============================================================== updateHelpFileName
void JoinQuadDialog::updateHelpFileName()
{
    if ( sender() == joinUni_rb ){
        _helpFileName = "gui_prism_join_quad.html#guijoinquadsuniform";
    } else if ( sender() == join_rb ){
        _helpFileName = "gui_prism_join_quad.html#guijoinquadscustom";
    }
}

// ============================================================== addHeightItem
void JoinQuadDialog::addHeightItem()
{
    QListWidgetItem* previousItem = height_lw->currentItem();
    QListWidgetItem* newItem      = new QListWidgetItem();

    double defaultValue = 1.;
    if ( previousItem )
        defaultValue = previousItem->data(Qt::EditRole).toDouble();

    newItem->setData(  Qt::EditRole, QVariant(defaultValue) );
    newItem->setFlags( newItem->flags () | Qt::ItemIsEditable);
    height_lw->addItem(newItem);
}

// ============================================================== delHeightItem
void JoinQuadDialog::delHeightItem()
{
    delete height_lw->currentItem();
}

// ============================================================== clear
void JoinQuadDialog::clear()
{
    quad_dest_le->clear();
    modelUnregister(quad_dest_le);

    quads_lw->clear();
    modelUnregister(quads_lw);

    vex0_le->clear();
    modelUnregister(vex0_le);

    vex1_le->clear();
    modelUnregister(vex1_le);

    vex2_le->clear();
    modelUnregister(vex2_le);

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

// ============================================================== apply
bool JoinQuadDialog::apply(QModelIndex& result)
{
    if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
    _currentObj = NULL;

    DocumentModel* docModel = getDocumentModel();
    PatternDataModel*    patternDataModel    = getPatternDataModel();
//    PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();

    QModelIndexList iquads;
    QModelIndex     iquad;
    QListWidgetItem* item = NULL;
    int nbQuads = quads_lw->count();
    for ( int r = 0; r < nbQuads; ++r){
        item = quads_lw->item(r);
        iquad = patternDataModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() );
        if ( iquad.isValid() )
            iquads << iquad;
    }
    QModelIndex iquaddest = patternDataModel->mapToSource( _index[quad_dest_le] );
    QModelIndex ivex0 = patternDataModel->mapToSource( _index[vex0_le] );
    QModelIndex ivex1 = patternDataModel->mapToSource( _index[vex1_le] );
    QModelIndex ivex2 = patternDataModel->mapToSource( _index[vex2_le] );
    QModelIndex ivex3 = patternDataModel->mapToSource( _index[vex3_le] );
    double nb = nb_spb->value();

    vector<double> heights;
    int nbHeight = height_lw->count();
    for ( int r = 0; r < nbHeight; ++r){
        item = height_lw->item(r);
        heights.push_back(item->data(Qt::EditRole).toDouble());
    }

    QModelIndex iElts;
    if (nbQuads == 1 && iquads[0].isValid() && iquaddest.isValid() && ivex0.isValid() &&
        ivex1.isValid() && ivex2.isValid() && ivex3.isValid())
    {
        if (joinUni_rb->isChecked())
            iElts = docModel->joinQuadUni(iquads[0], iquaddest, ivex0, ivex1, ivex2, ivex3, nb);
        else if (join_rb->isChecked())
            iElts = docModel->joinQuad(iquads[0], iquaddest, ivex0, ivex1, ivex2, ivex3, heights);
    }
    else if (nbQuads > 1 && iquaddest.isValid() && ivex0.isValid() &&
            ivex1.isValid() && ivex2.isValid() && ivex3.isValid())
    {
        if (joinUni_rb->isChecked())
            iElts = docModel->joinQuadsUni(iquads, iquaddest, ivex0, ivex1, ivex2, ivex3, nb);
        else if (join_rb->isChecked())
            iElts = docModel->joinQuads(iquads, iquaddest, ivex0, ivex1, ivex2, ivex3, heights);
    }

    if ( !iElts.isValid() ){
        SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT JOIN QUAD(S)" ) + "\n" + getErrorMsg() );
        return false;
    }
    result = patternDataModel->mapFromSource(iElts);
//    result = patternBuilderModel->mapFromSource(iElts);

    return true;
}

// ------------------------- MergeDialog ----------------------------------
// ============================================================== Constructeur

MergeDialog::MergeDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f )
: HexaBaseDialog(parent, editmode, f)
{
    setupUi( this );
    _initWidget(editmode);
    rb0->click();

    _helpFileName = "gui_merge_elmts.html#merge-two-vertices";
    connect( rb0, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
    connect( rb0, SIGNAL(clicked()), this, SLOT(clearVTKSelection()) );
    connect( rb0, SIGNAL(clicked()), this, SLOT(clearCurrentObjectFocus()) );

    connect( rb1, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
    connect( rb1, SIGNAL(clicked()), this, SLOT(clearVTKSelection()) );
    connect( rb1, SIGNAL(clicked()), this, SLOT(clearCurrentObjectFocus()) );

    connect( rb2, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
    connect( rb2, SIGNAL(cliked()), this, SLOT(clearVTKSelection()) );
    connect( rb2, SIGNAL(clicked()), this, SLOT(clearCurrentObjectFocus()) );
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
        SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MERGE" ) + "\n" + getErrorMsg() );
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
    rb0->click();

    _helpFileName = "gui_disc_elmts.html#disconnect-a-vertex";

    connect( rb0, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
    connect( rb0, SIGNAL(clicked()), this, SLOT(clearVTKSelection()) );
    connect( rb0, SIGNAL(clicked()), this, SLOT(clearCurrentObjectFocus()) );

    connect( rb1, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
    connect( rb1, SIGNAL(clicked()), this, SLOT(clearVTKSelection()) );
    connect( rb1, SIGNAL(clicked()), this, SLOT(clearCurrentObjectFocus()) );

    connect( rb2, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
    connect( rb2, SIGNAL(clicked()), this, SLOT(clearVTKSelection()) );
    connect( rb2, SIGNAL(clicked()), this, SLOT(clearCurrentObjectFocus()) );

    connect( rb3, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
    connect( rb3, SIGNAL(clicked()), this, SLOT(clearVTKSelection()) );
    connect( rb3, SIGNAL(clicked()), this, SLOT(clearCurrentObjectFocus()) );

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
    autoFocusSwitch = false;

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
    if (patternDataModel == NULL || !rb3->isChecked())
    	return iElts;
    //ListWidget content
    if (HEXABLOCKGUI::currentDocGView->getSelectionMode() == EDGE_TREE)
    {
        int nbEdges = d_edges_lw->count();
    	for ( int r = 0; r < nbEdges; ++r )
    	{
    		item = d_edges_lw->item(r);
    		index = patternDataModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() );
    		if ( index.isValid() )
    			iElts << index;
    	}
    }
    else if (HEXABLOCKGUI::currentDocGView->getSelectionMode() == HEXA_TREE)
    {
        int nbHexas = hexas_lw->count();
    	for ( int r = 0; r < nbHexas; ++r )
    	{
    		item = hexas_lw->item(r);
    		index = patternDataModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() );
    		if ( index.isValid() )
    			iElts << index;
    	}
    }
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
//    const PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();
    if ( !patternDataModel /*|| !patternBuilderModel*/)    return false;

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
        int nbEdges = d_edges_lw->count();
        for (int r = 0; r < nbEdges; ++r){
            item = d_edges_lw->item(r);
            iedge = patternDataModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() );
            if ( iedge.isValid() )
                iedges << iedge;
        }

        //Liste des hexas
        int nbHexas = hexas_lw->count();
        for (int r = 0; r < nbHexas; ++r){
            item = hexas_lw->item(r);
            ihexa = patternDataModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() );
            if ( ihexa.isValid() )
                ihexas << ihexa;
        }

        iElts = getDocumentModel()->disconnectEdges( ihexas, iedges );
    }

    if ( !iElts.isValid() ){
        SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT DISCONNECT" ) + "\n" + getErrorMsg() );
        return false;
    }

    result = patternDataModel->mapFromSource(iElts);
//    result = patternBuilderModel->mapFromSource(iElts);

    return true;
}


// ------------------------- CutEdgeDialog ----------------------------------
// ============================================================== Constructeur

CutEdgeDialog::CutEdgeDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f ):
HexaBaseDialog(parent, editmode, f)
{
    setupUi( this );
    _helpFileName = "gui_cut_hexa.html";
    connect( cutUni_rb, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
    connect( cut_rb, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
    _initWidget(editmode);

    cutUni_rb->click();
}

// ============================================================== Destructeur
CutEdgeDialog::~CutEdgeDialog()
{
}

// ============================================================== _initInputWidget
void CutEdgeDialog::_initInputWidget( Mode editmode )
{
    installEventFilter(this);

    e_le->setProperty( "HexaWidgetType",  QVariant::fromValue(EDGE_TREE) );
    e_le->installEventFilter(this);

    cutUni_rb->installEventFilter(this);
    cut_rb->installEventFilter(this);

    e_le->setReadOnly(true);

    height_lw->setProperty("HexaWidgetType", QVariant::fromValue(EDGE_TREE));
    height_lw->setProperty("GeomWidgetType", QVariant::fromValue(TopAbs_EDGE));
    height_lw->setProperty("Length", QVariant::fromValue(true));
    height_lw->installEventFilter(this);

    height_lw->setItemDelegate(new HexaPositiveDoubleSpinBoxDelegate(height_lw));
    height_lw->setEditTriggers(QAbstractItemView::DoubleClicked);

    connect( add_height_pb, SIGNAL(clicked()), this, SLOT(addHeightItem()) );
    connect( del_height_pb, SIGNAL(clicked()), this, SLOT(delHeightItem()) );
}

// ============================================================== updateHelpFileName
void CutEdgeDialog::updateHelpFileName()
{
    if ( sender() == cutUni_rb ){
        _helpFileName = "gui_cut_hexa.html#guicuthexauniform";
    } else if ( sender() == cut_rb ){
        _helpFileName = "gui_cut_hexa.html#guicuthexacustom";
    }
}

// ============================================================== clear
void CutEdgeDialog::clear()
{
    e_le->clear();
    modelUnregister(e_le);

    modelUnregister(this);
}

// ============================================================== addHeightItem
void CutEdgeDialog::addHeightItem()
{
    QListWidgetItem* previousItem = height_lw->currentItem();
    QListWidgetItem* newItem      = new QListWidgetItem();

    double defaultValue = 1.;
    if ( previousItem )
        defaultValue = previousItem->data(Qt::EditRole).toDouble();

    newItem->setData(  Qt::EditRole, QVariant(defaultValue) );
    newItem->setFlags( newItem->flags () | Qt::ItemIsEditable);
    height_lw->addItem(newItem);
}

// ============================================================== delHeightItem
void CutEdgeDialog::delHeightItem()
{
    delete height_lw->currentItem();
}

// ============================================================== apply
bool CutEdgeDialog::apply(QModelIndex& result)
{
    if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
    _currentObj = NULL;

    DocumentModel* docModel = getDocumentModel();
    PatternDataModel*    patternDataModel    = getPatternDataModel();
//    PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();

    QModelIndex iElts;
    QModelIndex iedge = patternDataModel->mapToSource( _index[e_le] );

    if (cutUni_rb->isChecked() && iedge.isValid())
    {
        int nbCut = nb_cut_spb->value();
        iElts = docModel->cutUni(iedge, nbCut);
    }
    else if (cut_rb->isChecked() && iedge.isValid())
    {
        vector<double> heights;
        QListWidgetItem* item = NULL;
        int nbItems = height_lw->count();

        for ( int r = 0; r < nbItems; ++r){
            item = height_lw->item(r);
            heights.push_back( item->data(Qt::EditRole).toDouble() );
        }

        iElts = docModel->cut(iedge, heights);
    }

    if ( !iElts.isValid() ){
        SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT CUT EDGE" ) + "\n" + getErrorMsg() );
        return false;
    }

    result = patternDataModel->mapFromSource(iElts);
//    result = patternBuilderModel->mapFromSource(iElts);

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

    installEventFilter(this);
    rb0->installEventFilter(this);
    rb1->installEventFilter(this);
    rb2->installEventFilter(this);

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

    angle_spb->setProperty("HexaWidgetType", QVariant::fromValue(EDGE_TREE));
    angle_spb->setProperty("GeomWidgetType", QVariant::fromValue(TopAbs_EDGE));
    angle_spb->setProperty("Angle", QVariant::fromValue(true));
    angle_spb->installEventFilter(this);

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

    if (getDocumentModel() == NULL) return false;
    const PatternDataModel*    patternDataModel    = getPatternDataModel();
//    const PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();
    if ( !patternDataModel /*|| !patternBuilderModel*/)    return false;

    QModelIndex iNewElts;

    if ( rb0->isChecked() ){
        QModelIndex ielts = patternDataModel->mapToSource( _index[elts_le_rb0] );
        QModelIndex ivec = patternDataModel->mapToSource( _index[vec_le_rb0] );
//        QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb0] );
//        QModelIndex  ivec = patternBuilderModel->mapToSource( _index[vec_le_rb0] );

        if ( ielts.isValid()
                && ivec.isValid() )
            iNewElts = getDocumentModel()->makeTranslation( ielts, ivec );

    } else if ( rb1->isChecked() ){
//        QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb1] );
        QModelIndex ielts = patternDataModel->mapToSource( _index[elts_le_rb1] );
        QModelIndex  ivex = patternDataModel->mapToSource( _index[vex_le_rb1] );
        double          k = k_spb->value();

        if ( ielts.isValid()
                && ivex.isValid() )
            iNewElts = getDocumentModel()->makeScale( ielts, ivex, k );

    } else if ( rb2->isChecked() ){
//        QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb2] );
        QModelIndex ielts = patternDataModel->mapToSource( _index[elts_le_rb2] );
        QModelIndex  ivex = patternDataModel->mapToSource( _index[vex_le_rb2] );
//        QModelIndex  ivec = patternBuilderModel->mapToSource( _index[vec_le_rb2] );
        QModelIndex ivec = patternDataModel->mapToSource( _index[vec_le_rb2] );
        double      angle = angle_spb->value();

        if ( ielts.isValid()
                && ivex.isValid()
                && ivec.isValid() )
            iNewElts = getDocumentModel()->makeRotation( ielts, ivex, ivec, angle );
    }

    if ( !iNewElts.isValid() ){
        SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE TRANSFORMATION" ) + "\n" + getErrorMsg() );
        return false;
    }

    result = patternDataModel->mapFromSource(iNewElts);
    if (result.isValid())
    {
        MESSAGE("======> Result is valid!");
        HEXA_NS::Elements* elts = getDocumentModel()->getHexaPtr<HEXA_NS::Elements*>(result);
        MESSAGE("======>   " << elts->getName());
    }
    else
    {
        MESSAGE("======> Result is not valid!");
    }
//    result = patternBuilderModel->mapFromSource(iNewElts);

    return true;
}

// // ------------------------- MakeSymmetryDialog ----------------------------------
// ============================================================== Constructeur

MakeSymmetryDialog::MakeSymmetryDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f ):
HexaBaseDialog(parent, editmode, f)
{
    setupUi( this );
    _initWidget(editmode);
    rb0->click();

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
//    const PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();
    if ( !patternDataModel /*|| !patternBuilderModel*/)    return false;

    QModelIndex iNewElts;

    if ( rb0->isChecked() ){
//        QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb0] );
        QModelIndex ielts = patternDataModel->mapToSource( _index[elts_le_rb0] );
        QModelIndex  ivex = patternDataModel->mapToSource( _index[vex_le_rb0] );

        if ( ielts.isValid()
                && ivex.isValid() )
            iNewElts = getDocumentModel()->makeSymmetryPoint( ielts, ivex );


    } else if ( rb1->isChecked() ){
//        QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb1] );
        QModelIndex ielts = patternDataModel->mapToSource( _index[elts_le_rb1] );
        QModelIndex  ivex = patternDataModel->mapToSource( _index[vex_le_rb1] );
//        QModelIndex  ivec = patternBuilderModel->mapToSource( _index[vec_le_rb1] );
        QModelIndex ivec = patternDataModel->mapToSource( _index[vec_le_rb1] );

        if ( ielts.isValid()
                && ivex.isValid()
                && ivec.isValid() )
            iNewElts = getDocumentModel()->makeSymmetryLine( ielts, ivex, ivec );

    } else if ( rb2->isChecked() ){
//        QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb2] );
        QModelIndex ielts = patternDataModel->mapToSource(_index[elts_le_rb2]);
        QModelIndex  ivex = patternDataModel->mapToSource( _index[vex_le_rb2] );
//        QModelIndex  ivec = patternBuilderModel->mapToSource( _index[vec_le_rb2] );
        QModelIndex ivec = patternDataModel->mapToSource(_index[vec_le_rb2]);

        if ( ielts.isValid()
                && ivex.isValid()
                && ivec.isValid() )
            iNewElts = getDocumentModel()->makeSymmetryPlane( ielts, ivex, ivec );
    }

    if ( !iNewElts.isValid() ){
        SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE TRANSFORMATION" ) + "\n" + getErrorMsg() );
        return false;
    }

//    result = patternBuilderModel->mapFromSource(iNewElts);
    result = patternDataModel->mapFromSource(iNewElts);

    return true;
}

// // ------------------------- PerformTransformationDialog ----------------------------------
// ============================================================== Constructeur
PerformTransformationDialog::PerformTransformationDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f ):
HexaBaseDialog(parent, editmode, f)
{
    setupUi( this );
    _initWidget(editmode);
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

    installEventFilter(this);
    rb0->installEventFilter(this);
    rb1->installEventFilter(this);
    rb2->installEventFilter(this);

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

    angle_spb->setProperty("HexaWidgetType", QVariant::fromValue(EDGE_TREE));
    angle_spb->setProperty("GeomWidgetType", QVariant::fromValue(TopAbs_EDGE));
    angle_spb->setProperty("Angle", QVariant::fromValue(true));
    angle_spb->installEventFilter(this);

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

    if (getDocumentModel() == NULL) return false;
    const PatternDataModel*    patternDataModel    = getPatternDataModel();
//    const PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();
    if ( !patternDataModel /*|| !patternBuilderModel*/)    return false;

    bool performed = false;

    if ( rb0->isChecked() ){
//        QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb0] );
        QModelIndex ielts = patternDataModel->mapToSource(_index[elts_le_rb0]);
//        QModelIndex  ivec = patternBuilderModel->mapToSource( _index[vec_le_rb0] );
        QModelIndex ivec = patternDataModel->mapToSource(_index[vec_le_rb0]);

        if ( ielts.isValid()
                && ivec.isValid() )
            performed = getDocumentModel()->performTranslation( ielts, ivec );

    } else if ( rb1->isChecked() ){
//        QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb1] );
        QModelIndex ielts = patternDataModel->mapToSource(_index[elts_le_rb1]);
        QModelIndex  ivex = patternDataModel->mapToSource( _index[vex_le_rb1] );
        double          k = k_spb->value();

        if ( ielts.isValid()
                && ivex.isValid() )
            performed = getDocumentModel()->performScale( ielts, ivex, k );

    } else if ( rb2->isChecked() ){
//        QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb2] );
        QModelIndex ielts = patternDataModel->mapToSource(_index[elts_le_rb2]);
        QModelIndex  ivex = patternDataModel->mapToSource( _index[vex_le_rb2] );
//        QModelIndex  ivec = patternBuilderModel->mapToSource( _index[vec_le_rb2] );
        QModelIndex ivec = patternDataModel->mapToSource(_index[vec_le_rb2]);
        double      angle = angle_spb->value();

        if ( ielts.isValid()
                && ivex.isValid()
                && ivec.isValid() )
            performed = getDocumentModel()->performRotation( ielts, ivex, ivec, angle );
    }

    if ( performed == false){
        SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT PERFORM TRANSFORMATION" ) + "\n" + getErrorMsg() );
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

    if (getDocumentModel() == NULL) return false;
    const PatternDataModel*    patternDataModel    = getPatternDataModel();
//    const PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();
    if ( !patternDataModel /*|| !patternBuilderModel*/)    return false;

    bool performed = false;

    if ( rb0->isChecked() ){
//        QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb0] );
        QModelIndex ielts = patternDataModel->mapToSource(_index[elts_le_rb0]);
        QModelIndex  ivex = patternDataModel->mapToSource( _index[vex_le_rb0] );

        if ( ielts.isValid()
                && ivex.isValid() )
            performed = getDocumentModel()->performSymmetryPoint( ielts, ivex );

    } else if ( rb1->isChecked() ){
//        QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb1] );
        QModelIndex ielts = patternDataModel->mapToSource(_index[elts_le_rb1]);
        QModelIndex  ivex = patternDataModel->mapToSource( _index[vex_le_rb1] );
//        QModelIndex  ivec = patternBuilderModel->mapToSource( _index[vec_le_rb1] );
        QModelIndex ivec = patternDataModel->mapToSource(_index[vec_le_rb1]);

        if ( ielts.isValid()
                && ivex.isValid()
                && ivec.isValid() )
            performed = getDocumentModel()->performSymmetryLine( ielts, ivex, ivec );

    } else if ( rb2->isChecked() ){
//        QModelIndex ielts = patternBuilderModel->mapToSource( _index[elts_le_rb2] );
        QModelIndex ielts = patternDataModel->mapToSource(_index[elts_le_rb2]);
        QModelIndex  ivex = patternDataModel->mapToSource( _index[vex_le_rb2] );
//        QModelIndex  ivec = patternBuilderModel->mapToSource( _index[vec_le_rb2] );
        QModelIndex ivec = patternDataModel->mapToSource(_index[vec_le_rb2]);

        if ( ielts.isValid()
                && ivex.isValid()
                && ivec.isValid() )
            performed = getDocumentModel()->performSymmetryPlane( ielts, ivex, ivec );
    }

    if ( performed == false ){
        SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT PERFORM SYMMETRY" ) + "\n" + getErrorMsg() );
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
    myLine = NULL;
    single_rb->click();
}

// ============================================================== Destructeur
EdgeAssocDialog::~EdgeAssocDialog()
{
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
    lines_lw->setProperty("HexaWidgetType", QVariant::fromValue(GEOMEDGE_TREE) );
    lines_lw->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_EDGE) );
    lines_lw->installEventFilter(this);

    single_line_le->setProperty( "HexaWidgetType", QVariant::fromValue(GEOMEDGE_TREE) );
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
        int nbEdges = edges_lw->count();
        for ( int r = 0; r < nbEdges; ++r){
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
        assocs.insert( geomObj.shapeName, geomObj.subId.toInt() );
        return assocs;
    }
    else if (multiple_rb->isChecked())
    {
        //ListWidget content
        unsigned int nbEdges = lines_lw->count();
        for ( int r = 0; r < nbEdges; ++r){
            item = lines_lw->item(r);
            geomObj = item->data(LW_ASSOC_ROLE).value<DocumentModel::GeomObj>();
            assocs.insert( geomObj.shapeName, geomObj.subId.toInt() );
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
    QListWidget* currentListWidget = dynamic_cast<QListWidget*>( sender() );
    if ( !currentListWidget )
        return;

    QModelIndex index;
    QList<QListWidgetItem *> sel = currentListWidget->selectedItems();
    PatternGeomSelectionModel* pgsm = getPatternGeomSelectionModel();
    pgsm->clearSelection();
    if (sel.count() == 0)
        return;

    QListWidgetItem *item = sel[0];
//    index = item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>(); //unsafe: index can change in the tree
    index = pgsm->indexBy(HEXA_DATA_ROLE, item->data(LW_DATA_ROLE));
    if ( index.isValid() )
        pgsm->select( index, QItemSelectionModel::SelectCurrent );
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
    modelUnregister(single_line_le);

    modelUnregister(this);
}

// ============================================================== onWindowActivated
void EdgeAssocDialog::onWindowActivated(SUIT_ViewManager* vm)
{
    QString vmType = vm->getType();
    if ( (vmType == SVTK_Viewer::Type()) || (vmType == VTKViewer_Viewer::Type()) )
    {
        if (single_rb->isChecked())
            single_edge_le->setFocus();
        else if (multiple_rb->isChecked() && focusWidget() != first_vex_le )
            edges_lw->setFocus();
    }
    else if ( vmType == OCCViewer_Viewer::Type() ){
        if (single_rb->isChecked())
            single_line_le->setFocus();
        else if (multiple_rb->isChecked() && focusWidget() != first_vex_le)
            lines_lw->setFocus();
    }
}

// ============================================================== apply
bool EdgeAssocDialog::apply(QModelIndex& result)
{
    if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
    _currentObj = NULL;

    bool assocOk = false;

    PatternDataModel* patternDataModel = getPatternDataModel();
    PatternDataSelectionModel* pdsm = getPatternDataSelectionModel();
    PatternGeomSelectionModel* pgsm = getPatternGeomSelectionModel();

    QModelIndex     iEdge, iGeomEdge;
    QModelIndexList iEdges;
    QListWidgetItem* item = NULL;
    QList<DocumentModel::GeomObj> assocs;
    DocumentModel::GeomObj aLine;
    HEXA_NS::SubShape* ssh;
    HEXA_NS::EdgeShape* sh;
    HEXA_NS::NewShapes shapes;
    HEXA_NS::IntVector subIds;
    QString id;
    QModelIndexList edges, lines;

    if (single_rb->isChecked()){ //Single edge and/or line association

        iEdge = patternDataModel->mapToSource( _index[single_edge_le] );
        if  ( !iEdge.isValid() || myLine == NULL)
        {
            SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE EDGE ASSOCIATION" ) );
            return false;
        }
        id = myLine->shapeName+","+myLine->subId;
        ssh = getDocumentModel()->getGeomPtr(id);
        sh  = dynamic_cast<HEXA_NS::EdgeShape*>(ssh);
        iGeomEdge = pgsm->indexBy(HEXA_DATA_ROLE, QVariant::fromValue(sh));
        if (!iGeomEdge.isValid())
        {
            SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE EDGE ASSOCIATION" ) );
            return false;
        }
        assocOk = getDocumentModel()->addEdgeAssociation(iEdge, iGeomEdge, pstart_spb->value(), pend_spb->value());
        edges << _index[single_edge_le];
        lines << iGeomEdge;
    }
    else { //Multiple edge and/or line association

        // edges
        iEdges = getIndexList(edges_lw, true);
        int nbLines = lines_lw->count();
        if  (iEdges.count() == 0 || nbLines == 0)
        {
            SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE EDGE ASSOCIATION" ) );
            return false;
        }
        edges = getIndexList(edges_lw, false);
        // lines
        for (int r = 0; r < nbLines; ++r){
            item = lines_lw->item(r);
            aLine = item->data(LW_ASSOC_ROLE).value<DocumentModel::GeomObj>();
            id = aLine.shapeName+","+aLine.subId;
            ssh = getDocumentModel()->getGeomPtr(id);
            sh  = dynamic_cast<HEXA_NS::EdgeShape*>(ssh);
            if (sh == NULL)
            {
                SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE EDGE ASSOCIATION" ) );
                return false;
            }
            shapes.push_back(sh->getParentShape());
            subIds.push_back(sh->getIdent());
            iGeomEdge = pgsm->indexBy(HEXA_DATA_ROLE, QVariant::fromValue(sh));
            if (iGeomEdge.isValid())
                lines << iGeomEdge;
        }

        if ( close_cb->isChecked() ){ //closed line
            QModelIndex iFirstVertex = patternDataModel->mapToSource( _index[first_vex_le] );
            if  ( !iFirstVertex.isValid() )
            {
                SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE EDGE ASSOCIATION" ) );
                return false;
            }
            bool inv = inverse_cb->isChecked();
            assocOk = getDocumentModel()->associateClosedLine( iFirstVertex, iEdges, shapes, subIds, pstart_spb->value(), inv );

        } else
            //opened line
            assocOk = getDocumentModel()->associateOpenedLine( iEdges, shapes, subIds, pstart_spb->value(), pend_spb->value() );
    }

    if ( !assocOk ){
        SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE EDGE ASSOCIATION" ) + "\n" + getErrorMsg() );
        return false;
    }

    // highlight associated items in the trees
    pdsm->unhighlightTreeItems();
    pgsm->unhighlightTreeItems();
    pdsm->highlightTreeItems(edges);
    pgsm->highlightTreeItems(lines);

    // highlight associated items in the views
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

    faces_lw->setProperty( "HexaWidgetType", QVariant::fromValue(GEOMFACE_TREE) );
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
        assocs.insert( geomObj.shapeName, geomObj.subId.toInt() );
    }
    return assocs;
}

// ============================================================== selectElementOfGeom
/*Highlight in the OCC view selected elements in a listwidget,
 *  or an element in a line edit.*/

void QuadAssocDialog::selectElementOfGeom()
{
    QListWidget* currentListWidget = dynamic_cast<QListWidget*>( sender() );
    if ( !currentListWidget )
        return;

    QModelIndex index;
    QList<QListWidgetItem *> sel = currentListWidget->selectedItems();
    PatternGeomSelectionModel* pgsm = getPatternGeomSelectionModel();
    pgsm->clearSelection();
    if (sel.count() == 0)
        return;
    QListWidgetItem *item = sel[0];
//    index = item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>();
    index = pgsm->indexBy(HEXA_DATA_ROLE, item->data(LW_DATA_ROLE));
    if ( index.isValid() )
        pgsm->select( index, QItemSelectionModel::SelectCurrent );
}

void QuadAssocDialog::clear()
{
    quad_le->clear();
    modelUnregister(quad_le);

    faces_lw->clear();

    modelUnregister(this);
}

// ============================================================== onWindowActivated
void QuadAssocDialog::onWindowActivated(SUIT_ViewManager* vm)
{
    QString vmType = vm->getType();
    if ( (vmType == SVTK_Viewer::Type()) || (vmType == VTKViewer_Viewer::Type()) )
        quad_le->setFocus();
    else if ( vmType == OCCViewer_Viewer::Type() ){
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
    if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
    _currentObj = NULL;

    PatternDataModel* patternDataModel = getPatternDataModel();
    PatternDataSelectionModel* pdsm = getPatternDataSelectionModel();
    PatternGeomSelectionModel* pgsm = getPatternGeomSelectionModel();


    QModelIndex iQuad, iGeomFace;
    HEXA_NS::SubShape* ssh;
    HEXA_NS::FaceShape* sh;
    QString id;

    // quad
    iQuad = patternDataModel->mapToSource( _index[quad_le] );

    // faces
    QListWidgetItem* item = NULL;
    DocumentModel::GeomObj aFace;
    QModelIndexList quads, faces;
    int nbFaces = faces_lw->count();
    for ( int r = 0; r < nbFaces; ++r ){
        item = faces_lw->item(r);
        aFace = item->data(LW_ASSOC_ROLE).value<DocumentModel::GeomObj>();
        id = aFace.shapeName+","+aFace.subId;
        ssh = getDocumentModel()->getGeomPtr(id);
        sh  = dynamic_cast<HEXA_NS::FaceShape*>(ssh);
        if (sh == NULL)
        {
            SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE QUAD ASSOCIATION" ) );
            return false;
        }
        iGeomFace = pgsm->indexBy(HEXA_DATA_ROLE, QVariant::fromValue(sh));
        if (!getDocumentModel()->addQuadAssociation(iQuad, iGeomFace))
        {
            SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE QUAD ASSOCIATION" ) + "\n" + getErrorMsg() );
            return false;
        }
        else if (iGeomFace.isValid())
            faces << iGeomFace;
    }
    result = iQuad;

    if (iQuad.isValid())
        quads << _index[quad_le];

    // highlight associated items in the trees
    pdsm->highlightTreeItems(quads);
    pgsm->highlightTreeItems(faces);

    // highlight associated items in the views
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
    int nbElts = eltBase_lw->count();
    for ( int r = 0; r < nbElts; ++r){
        item = eltBase_lw->item(r);
        iItem = patternDataModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() );
        if ( iItem.isValid() ) assocs << iItem;
    }
    return assocs;
}

// ============================================================== _initInputWidget
void GroupDialog::_initInputWidget( Mode editmode )
{
    installEventFilter(this);
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

    if ( editmode == INFO_MODE)
    {
        name_le->setReadOnly(true);
        kind_cb->setEnabled(false);
        eltBase_lw->viewport()->setAttribute( Qt::WA_TransparentForMouseEvents );
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


    int nbElts = g->countElement();
    for ( int nr = 0; nr < nbElts; ++nr ){
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
        SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT ADD GROUP" ) + "\n" + getErrorMsg() );
        return false;
    }

    //fill it and select it
    QModelIndex iEltBase;
    QListWidgetItem* item = NULL;
    bool eltAdded = false;
    getDocumentModel()->clearGroupElement(iGrp);
    int nbElts = eltBase_lw->count();
    for ( int r = 0; r < nbElts; ++r){
        item     = eltBase_lw->item(r);
        iEltBase = patternDataModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() );
        if ( iEltBase.isValid() )
            eltAdded = getDocumentModel()->addGroupElement( iGrp, iEltBase );
    }

    QString newName = name_le->text();
    if ( !newName.isEmpty() )
        getDocumentModel()->setName( iGrp, newName );

    HEXA_NS::Group* hGroup = getDocumentModel()->data(iGrp, HEXA_DATA_ROLE).value<HEXA_NS::Group *>();
    if (hGroup != NULL)
    {
        QString groupName = QString::fromStdString(hGroup->getNextName());
        name_le->setText(groupName);
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
    else if ( editmode == INFO_MODE){
        setWindowTitle( tr("Law Information") );
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
    QMap<HEXA_NS::KindLaw, QString>::ConstIterator iKind, iEnd;
    for( iKind = strKind.constBegin(), iEnd = strKind.constEnd(); iKind != iEnd; ++iKind )
        kind_cb->addItem( iKind.value(), QVariant(iKind.key()) );

    if (editmode == INFO_MODE)
    {
        name_le->setReadOnly(true);
        nb_nodes_spb->setReadOnly(true);
        coeff_spb->setReadOnly(true);
        kind_cb->setEnabled(false);
    }
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
        SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT ADD LAW" ) + "\n" + getErrorMsg() );
        return false;
    }
    //fill it and select it
    bool setOk = getDocumentModel()->setLaw( iLaw, nbnodes, coeff, lawKind );
    if ( !setOk ){
        SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT ADD LAW" ) + "\n" + getErrorMsg() );
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

    installEventFilter(this);


    law_le->setProperty( "HexaWidgetType",  QVariant::fromValue(LAW_TREE) );
    law_le->installEventFilter(this);
    law_le->setValidator( validator );

    propagations_lw->setProperty( "HexaWidgetType",  QVariant::fromValue(PROPAGATION_TREE) );
    propagations_lw->installEventFilter(this);

    QShortcut* delPropagationShortcut = new QShortcut(QKeySequence(/*Qt::Key_Delete*/Qt::Key_X/*Qt::Key_Alt*//*Qt::Key_Space*/), propagations_lw);
    delPropagationShortcut->setContext( Qt::WidgetShortcut );

    law_le->setReadOnly(true);

    if ( editmode == INFO_MODE)
        way_cb->setEnabled(false);

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
    if (!getMeshSelectionModel()) return;

    QListWidget* currentListWidget = dynamic_cast<QListWidget*>( sender() );
    if ( !currentListWidget ) return;

    QList<QListWidgetItem *> sel = currentListWidget->selectedItems();
    QModelIndex index;
    getMeshSelectionModel()->clearSelection();
    foreach ( QListWidgetItem *item, sel ){
        //index = item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>(); //unsafe: index can change in the tree
        index = getMeshSelectionModel()->indexBy(HEXA_DATA_ROLE, item->data(LW_DATA_ROLE));
        if ( index.isValid() )
            getMeshSelectionModel()->select( index, QItemSelectionModel::SelectCurrent );
    }
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

    int nbPropagations = propagations_lw->count();
    for (int r = 0; r < nbPropagations; ++r){
        item = propagations_lw->item(r);
        iPropagation = meshModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() );
        if ( !iPropagation.isValid() ){
            SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT SET PROPAGATION" ) );
            return false;
        }

        //fill it and select it
        isOk = getDocumentModel()->setPropagation( iPropagation, iLaw, way );
        if ( !isOk ){
            SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT SET PROPAGATION" ) + "\n" + getErrorMsg() );
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
    setupUi( this );
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

    if (getDocumentModel() == NULL) return false;
#ifndef DISABLE_PYCONSOLE
    QString command = QString("import hexablock ; %1 = hexablock.mesh(\"%2\", \"%1\", %3, \"%4\")")
    								                                          .arg( _name->text() )
    								                                          .arg( getDocumentModel()->getName() )
    								                                          .arg( _dim->value() )
    								                                          .arg( _fact->text() );

    SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
    PyConsole_Console* pyConsole = app->pythonConsole();

    if ( pyConsole )
        pyConsole->exec( command );
    else
        return false;
#else
    return false;
#endif

    return true;
}

// ------------------------- ReplaceHexaDialog ----------------------------------
// ============================================================== Constructeur
ReplaceHexaDialog::ReplaceHexaDialog( QWidget* parent, Mode editmode, Qt::WindowFlags f ):
HexaBaseDialog(parent, editmode, f)
{
    _helpFileName = "gui_replace_hexa.html";
    setupUi( this );
    _initWidget(editmode);
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
    int nbQuads = quads_lw->count();
    for ( int r = 0; r < nbQuads; ++r ){
        item = quads_lw->item(r);
        //iQuad = patternDataModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() ); //unsafe
        iQuad = getPatternDataSelectionModel()->indexBy(HEXA_DATA_ROLE, item->data(LW_DATA_ROLE));
        if ( iQuad.isValid() )
            assocs << iQuad;
    }

    nbQuads = quads_lw_2->count();
    for( int i = 0; i < nbQuads; ++i)
    {
        item = quads_lw_2->item(i);
        iQuad = getPatternDataSelectionModel()->indexBy(HEXA_DATA_ROLE, item->data(LW_DATA_ROLE));
        if (iQuad.isValid())
            assocs << iQuad;
    }

    return assocs;
}

// ============================================================== _initInputWidget
void ReplaceHexaDialog::_initInputWidget( Mode editmode )
{
    QRegExp rx("");

    installEventFilter(this);

    c1_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
    c2_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );

    p1_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
    p2_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );

    c1_le->installEventFilter(this);
    c2_le->installEventFilter(this);

    p1_le->installEventFilter(this);
    p2_le->installEventFilter(this);

    quads_lw->setProperty( "HexaWidgetType",  QVariant::fromValue(QUAD_TREE) );
    quads_lw->installEventFilter(this);

    quads_lw_2->setProperty("HexaWidgetType", QVariant::fromValue(QUAD_TREE));
    quads_lw_2->installEventFilter(this);

    if ( editmode == NEW_MODE ){
        QShortcut* delQuadShortcut = new QShortcut( QKeySequence(Qt::Key_X), quads_lw );
        QShortcut* delQuadShortcut2 = new QShortcut( QKeySequence(Qt::Key_X), quads_lw_2 );
        delQuadShortcut->setContext( Qt::WidgetShortcut );
        delQuadShortcut2->setContext( Qt::WidgetShortcut );
        connect( delQuadShortcut, SIGNAL(activated()), this, SLOT(deleteQuadItem()) );
        connect( quads_lw, SIGNAL(currentRowChanged(int)), this, SLOT(updateButtonBox(int)) );
        connect( delQuadShortcut2, SIGNAL(activated()), this, SLOT(deleteQuadItem2()));
        connect( quads_lw_2, SIGNAL(currentRowChanged(int)), this, SLOT(updateButtonBox(int)) );
    }

    c1_le->setReadOnly(true);
    c2_le->setReadOnly(true);

    p1_le->setReadOnly(true);
    p2_le->setReadOnly(true);

    connect(quads_lw,    SIGNAL(itemSelectionChanged()),
            this, SLOT(selectElementOfModel()), Qt::UniqueConnection);

    connect(quads_lw_2, SIGNAL(itemSelectionChanged()),
            this, SLOT(selectElementOfModel()), Qt::UniqueConnection);
}

// ============================================================== clear
void ReplaceHexaDialog::clear()
{
    quads_lw->clear();
    modelUnregister(quads_lw);

    quads_lw_2->clear();
    modelUnregister(quads_lw_2);

    p1_le->clear();
    modelUnregister(p1_le);

    p2_le->clear();
    modelUnregister(p2_le);

    c1_le->clear();
    modelUnregister(c1_le);

    c2_le->clear();
    modelUnregister(c2_le);

    modelUnregister(this);
}

// ============================================================== updateButtonBox
void ReplaceHexaDialog::updateButtonBox()
{
    int nbQuad = quads_lw->count();

    if ( nbQuad > 0 ){
        _applyButton->setEnabled(true);
    } else {
        _applyButton->setEnabled(false);
    }
}

// ============================================================== deleteQuadItem
void ReplaceHexaDialog::deleteQuadItem()
{
    delete quads_lw->currentItem();
    updateButtonBox();
}

// ============================================================== deleteQuadItem2
void ReplaceHexaDialog::deleteQuadItem2()
{
    delete quads_lw_2->currentItem();
    updateButtonBox();
}

// ============================================================== apply
bool ReplaceHexaDialog::apply(QModelIndex& result)
{
    if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
    _currentObj = NULL;

    if ( !getDocumentModel() ) return false;
    const PatternDataModel* patternDataModel = getPatternDataModel();
//    const PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();
    if ( !patternDataModel /*|| !patternBuilderModel*/)    return false;

    QModelIndex ielts; //result

    QListWidgetItem* item = NULL;
    QModelIndexList iquads_source;
    QModelIndex     iquad;
    int nbQuads = quads_lw->count();
    for ( int r = 0; r < nbQuads; ++r){
        item = quads_lw->item(r);
        iquad = patternDataModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() );
        if ( iquad.isValid() )
            iquads_source << iquad;
    }

    QModelIndexList iquads_dest;
    nbQuads = quads_lw_2->count();
    for (int i = 0; i < nbQuads; ++i)
    {
        item = quads_lw_2->item(i);
        iquad = patternDataModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() );
        if (iquad.isValid())
            iquads_dest << iquad;
    }

    QModelIndex ip1_source = patternDataModel->mapToSource( _index[p1_le] );
    QModelIndex ip2_source = patternDataModel->mapToSource( _index[p2_le] );

    QModelIndex ic1_dest = patternDataModel->mapToSource( _index[c1_le] );
    QModelIndex ic2_dest = patternDataModel->mapToSource( _index[c2_le] );

    bool ipts_ok = ip1_source.isValid() && ip2_source.isValid() &&
                      ic1_dest.isValid() && ic2_dest.isValid();

    if (ipts_ok)
    {
            ielts = getDocumentModel()->replace( iquads_source, iquads_dest,
                                                 ip1_source, ic1_dest,
                                                 ip2_source, ic2_dest );
    }

    if ( !ielts.isValid() ){
        SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT REPLACE HEXA" ) + "\n" + getErrorMsg() );
        return false;
    }
    _value  = ielts.model()->data(ielts, HEXA_DATA_ROLE).value<HEXA_NS::Elements*>();
//    result = patternBuilderModel->mapFromSource(ielts);
    result = patternDataModel->mapFromSource(ielts);

    //update the list (indexes)
    for ( int r = 0; r < nbQuads; ++r ){
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
    setupUi( this );
    _helpFileName = "gui_quad_revolution.html#guiquadsrevolution";
    connect( revolutionUni_rb, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
    connect( revolution_rb, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
    _initWidget(editmode);
    revolutionUni_rb->click();
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
    int nbQuads = quads_lw->count();
    for ( int r = 0; r < nbQuads; ++r ){
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
    quads_lw->setProperty( "HexaWidgetType",  QVariant::fromValue(QUAD_TREE) );
    center_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
    axis_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );

    angle_spb->setProperty("HexaWidgetType", QVariant::fromValue(EDGE_TREE));
    angle_spb->setProperty("GeomWidgetType", QVariant::fromValue(TopAbs_EDGE));
    angle_spb->setProperty("Angle", QVariant::fromValue(true));
    angle_spb->installEventFilter(this);

    angles_lw->setProperty("HexaWidgetType", QVariant::fromValue(EDGE_TREE));
    angles_lw->setProperty("GeomWidgetType", QVariant::fromValue(TopAbs_EDGE));
    angles_lw->setProperty("Angle", QVariant::fromValue(true));
    angles_lw->installEventFilter(this);

    installEventFilter(this);
    revolutionUni_rb->installEventFilter(this);
    revolution_rb->installEventFilter(this);

    quads_lw->installEventFilter(this);
    center_le->installEventFilter(this);
    axis_le->installEventFilter(this);

    center_le->setReadOnly(true);
    axis_le->setReadOnly(true);

    angles_lw->setItemDelegate( new HexaAngleDoubleSpinBoxDelegate(angles_lw) );
    angles_lw->setEditTriggers( QAbstractItemView::DoubleClicked );

    QShortcut* delQuadShortcut = new QShortcut( QKeySequence(Qt::Key_X), quads_lw );
    delQuadShortcut->setContext( Qt::WidgetShortcut );
    connect( delQuadShortcut, SIGNAL(activated()), this, SLOT(delQuadItem()) );
    connect( add_angle_pb, SIGNAL(clicked()), this, SLOT(addAngleItem()));
    connect( del_angle_pb, SIGNAL(clicked()), this, SLOT(delAngleItem()));

    connect(quads_lw,    SIGNAL(itemSelectionChanged()), this, SLOT(selectElementOfModel()), Qt::UniqueConnection);
}

// ============================================================== updateHelpFileName
void QuadRevolutionDialog::updateHelpFileName()
{
    if ( sender() == revolutionUni_rb ){
        _helpFileName = "gui_quad_revolution.html#guiquadsrevolutionuniform";
    } else if ( sender() == revolution_rb ){
        _helpFileName = "gui_quad_revolution.html#guiquadsrevolutioncustom";
    }
}

// ============================================================== clear
void QuadRevolutionDialog::clear()
{
    quads_lw->clear();
    modelUnregister(quads_lw);

    center_le->clear();
    modelUnregister(center_le);

    axis_le->clear();
    modelUnregister(axis_le);

    modelUnregister(this);
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

    DocumentModel* docModel = getDocumentModel();
    PatternDataModel* patternDataModel = getPatternDataModel();
//    PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();

    QListWidgetItem* item = NULL;

    QModelIndexList istartquads;
    QModelIndex     iquad;
    int nbQuads = quads_lw->count();
    for ( int r = 0; r < nbQuads; ++r){
        item = quads_lw->item(r);
        iquad = patternDataModel->mapToSource( item->data(LW_QMODELINDEX_ROLE).value<QModelIndex>() );
        if ( iquad.isValid() )
            istartquads << iquad;
    }

    QModelIndex icenter = patternDataModel->mapToSource( _index[center_le] );
    QModelIndex iaxis = patternDataModel->mapToSource(_index[axis_le]);
//    QModelIndex iaxis   = patternBuilderModel->mapToSource( _index[axis_le] );
    int angle = angle_spb->value();
    int nbre = nbre_spb->value();

    vector<double> angles;
    int nbAngles = angles_lw->count();
    for ( int r = 0; r < nbAngles; ++r){
        item = angles_lw->item(r);
        angles.push_back(item->data(Qt::EditRole).toDouble());
    }

    QModelIndex iElts; //result
    if (nbQuads == 1 && istartquads[0].isValid() && icenter.isValid() && iaxis.isValid())
    {
        if (revolutionUni_rb->isChecked())
            iElts = docModel->revolutionQuadUni(istartquads[0], icenter, iaxis, angle, nbre);
        else if (revolution_rb->isChecked())
            iElts = docModel->revolutionQuad(istartquads[0], icenter, iaxis, angles);
    }
    else if (nbQuads > 1 && icenter.isValid() && iaxis.isValid())
    {
        if (revolutionUni_rb->isChecked())
            iElts = docModel->revolutionQuadsUni(istartquads, icenter, iaxis, angle, nbre);
        else if (revolution_rb->isChecked())
            iElts = docModel->revolutionQuads(istartquads, icenter, iaxis, angles);
    }

    if ( !iElts.isValid() ){
        SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE QUAD REVOLUTION" ) + "\n" + getErrorMsg() );
        return false;
    }
    _value  = iElts.model()->data(iElts, HEXA_DATA_ROLE).value<HEXA_NS::Elements*>();
    result = patternDataModel->mapFromSource(iElts);
//    result = patternBuilderModel->mapFromSource(iElts);

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
    sphere_rb->click();

    connect( sphereTop_rb, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
    connect( sphereUni_rb, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
    connect( sphere2_rb,   SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
    connect( sphere_rb,    SIGNAL(clicked()), this, SLOT(clearVTKSelection()) );
    connect( sphere_rb,    SIGNAL(clicked()), this, SLOT(clearCurrentObjectFocus()) );

    connect( sphericalTop_rb, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
    connect( sphericalUni_rb, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
    connect( spherical2_rb,   SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
    connect( spherical_rb,    SIGNAL(clicked()), this, SLOT(clearVTKSelection()) );
    connect( spherical_rb,    SIGNAL(clicked()), this, SLOT(clearCurrentObjectFocus()) );

    connect( rindTop_rb, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
    connect( rindUni_rb, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
    connect( rind2_rb,   SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
    connect( rind_rb,    SIGNAL(clicked()), this, SLOT(clearVTKSelection()) );
    connect( rind_rb,    SIGNAL(clicked()), this, SLOT(clearCurrentObjectFocus()) );
}

// ============================================================== Destructeur
MakeHemiSphereDialog::~MakeHemiSphereDialog()
{
}

// ============================================================== _initInputWidget
void MakeHemiSphereDialog::_initInputWidget( Mode editmode )
{
    center_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );
    hole_axis_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );
    base_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VECTOR_TREE) );
    vplan_le->setProperty( "HexaWidgetType",  QVariant::fromValue(VERTEX_TREE) );

    sphere_radext_spb->setProperty( "HexaWidgetType", QVariant::fromValue(EDGE_TREE) );
    sphere_radext_spb->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_EDGE) );
    sphere_radext_spb->setProperty("Radius", QVariant::fromValue(true));

    sphere_radint_spb->setProperty( "HexaWidgetType", QVariant::fromValue(EDGE_TREE) );
    sphere_radint_spb->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_EDGE) );
    sphere_radint_spb->setProperty("Radius", QVariant::fromValue(true));

    hole_rad_spb->setProperty( "HexaWidgetType", QVariant::fromValue(EDGE_TREE) );
    hole_rad_spb->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_EDGE) );
    hole_rad_spb->setProperty("Radius", QVariant::fromValue(true));

    radial_angle_spb->setProperty( "HexaWidgetType", QVariant::fromValue(EDGE_TREE) );
    radial_angle_spb->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_EDGE) );
    radial_angle_spb->setProperty("Angle", QVariant::fromValue(true));

    radius_lw_1->setProperty( "HexaWidgetType", QVariant::fromValue(EDGE_TREE) );
    radius_lw_1->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_EDGE) );
    radius_lw_1->setProperty("Radius", QVariant::fromValue(true));

    radius_lw_2->setProperty( "HexaWidgetType", QVariant::fromValue(EDGE_TREE) );
    radius_lw_2->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_EDGE) );
    radius_lw_2->setProperty("Radius", QVariant::fromValue(true));

    height_lw->setProperty( "HexaWidgetType", QVariant::fromValue(EDGE_TREE) );
    height_lw->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_EDGE) );
    height_lw->setProperty("Length", QVariant::fromValue(true));

    angle_lw->setProperty( "HexaWidgetType", QVariant::fromValue(EDGE_TREE) );
    angle_lw->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_EDGE) );
    angle_lw->setProperty("Angle", QVariant::fromValue(true));

    installEventFilter(this);
    center_le->installEventFilter(this);
    hole_axis_le->installEventFilter(this);
    base_le->installEventFilter(this);
    vplan_le->installEventFilter(this);

    sphere_radext_spb->installEventFilter(this);
    sphere_radint_spb->installEventFilter(this);
    hole_rad_spb->installEventFilter(this);
    radial_angle_spb->installEventFilter(this);
    radius_lw_1->installEventFilter(this);
    radius_lw_2->installEventFilter(this);
    height_lw->installEventFilter(this);
    angle_lw->installEventFilter(this);

    sphere_rb->installEventFilter(this);
    sphereTop_rb->installEventFilter(this);
    sphereUni_rb->installEventFilter(this);
    sphere2_rb->installEventFilter(this);
    rind_rb->installEventFilter(this);
    rindTop_rb->installEventFilter(this);
    rindUni_rb->installEventFilter(this);
    rind2_rb->installEventFilter(this);
    spherical_rb->installEventFilter(this);
    sphericalTop_rb->installEventFilter(this);
    sphericalUni_rb->installEventFilter(this);
    spherical2_rb->installEventFilter(this);

    center_le->setReadOnly(true);
    hole_axis_le->setReadOnly(true);
    base_le->setReadOnly(true);
    vplan_le->setReadOnly(true);

    radius_lw_1->setItemDelegate(new HexaPositiveDoubleSpinBoxDelegate(radius_lw_1));
    radius_lw_1->setEditTriggers(QAbstractItemView::DoubleClicked);

    radius_lw_2->setItemDelegate(new HexaPositiveDoubleSpinBoxDelegate(radius_lw_2));
    radius_lw_2->setEditTriggers(QAbstractItemView::DoubleClicked);

    angle_lw->setItemDelegate(new HexaAngleDoubleSpinBoxDelegate(angle_lw));
    angle_lw->setEditTriggers(QAbstractItemView::DoubleClicked);

    height_lw->setItemDelegate(new HexaPositiveDoubleSpinBoxDelegate(height_lw));
    height_lw->setEditTriggers(QAbstractItemView::DoubleClicked);

    connect( add_radius_pb_1, SIGNAL(clicked()), this, SLOT(addRadiusItem1()) );
    connect( del_radius_pb_1, SIGNAL(clicked()), this, SLOT(delRadiusItem1()) );

    connect( add_radius_pb_2, SIGNAL(clicked()), this, SLOT(addRadiusItem2()) );
    connect( del_radius_pb_2, SIGNAL(clicked()), this, SLOT(delRadiusItem2()) );

    connect( add_angle_pb, SIGNAL(clicked()), this, SLOT(addAngleItem()) );
    connect( del_angle_pb, SIGNAL(clicked()), this, SLOT(delAngleItem()) );

    connect( add_height_pb, SIGNAL(clicked()), this, SLOT(addHeightItem()) );
    connect( del_height_pb, SIGNAL(clicked()), this, SLOT(delHeightItem()) );
}

// ============================================================== updateHelpFileName
void MakeHemiSphereDialog::updateHelpFileName()
{
    if ( sender() == sphereTop_rb || sender() == rindTop_rb ){
        _helpFileName = "gui_hemisphere.html#guisphereandrindsimple";
    } else if ( sender() == sphereUni_rb ){
        _helpFileName = "gui_hemisphere.html#guisphereuniform";
    } else if ( sender() == sphere2_rb ){
        _helpFileName = "gui_hemisphere.html#guispherecustom";
    } else if ( sender() == rindUni_rb ){
        _helpFileName = "gui_hemisphere.html#guirinduniform";
    } else if ( sender() == rind2_rb ){
        _helpFileName = "gui_hemisphere.html#guirindcustom";
    } else if ( sender() == sphericalTop_rb ){
        _helpFileName = "gui_hemisphere.html#guiconcentricsimple";
    } else if ( sender() == sphericalUni_rb ){
        _helpFileName = "gui_hemisphere.html#guiconcentricuniform";
    } else if ( sender() == spherical2_rb ){
        _helpFileName = "gui_hemisphere.html#guiconcentriccustom";
    }
}

// ============================================================== addRadiusItem
void MakeHemiSphereDialog::addRadiusItem1()
{
    QListWidgetItem* previousItem = radius_lw_1->currentItem();
    QListWidgetItem* newItem      = new QListWidgetItem();

    double defaultValue = 1.;
    if ( previousItem )
        defaultValue = previousItem->data(Qt::EditRole).toDouble();

    newItem->setData(  Qt::EditRole, QVariant(defaultValue) );
    newItem->setFlags( newItem->flags () | Qt::ItemIsEditable);
    radius_lw_1->addItem(newItem);
}

// ============================================================== delRadiusItem
void MakeHemiSphereDialog::delRadiusItem1()
{
    delete radius_lw_1->currentItem();
}

// ============================================================== addRadiusItem
void MakeHemiSphereDialog::addRadiusItem2()
{
    QListWidgetItem* previousItem = radius_lw_2->currentItem();
    QListWidgetItem* newItem      = new QListWidgetItem();

    double defaultValue = 1.;
    if ( previousItem )
        defaultValue = previousItem->data(Qt::EditRole).toDouble();

    newItem->setData(  Qt::EditRole, QVariant(defaultValue) );
    newItem->setFlags( newItem->flags () | Qt::ItemIsEditable);
    radius_lw_2->addItem(newItem);
}

// ============================================================== delRadiusItem
void MakeHemiSphereDialog::delRadiusItem2()
{
    delete radius_lw_2->currentItem();
}

// ============================================================== addAngleItem
void MakeHemiSphereDialog::addAngleItem()
{
    QListWidgetItem* previousItem = angle_lw->currentItem();
    QListWidgetItem* newItem      = new QListWidgetItem();

    double defaultValue = 180.;
    if ( previousItem )
        defaultValue = previousItem->data(Qt::EditRole).toDouble();

    newItem->setData(  Qt::EditRole, QVariant(defaultValue) );
    newItem->setFlags( newItem->flags () | Qt::ItemIsEditable);
    angle_lw->addItem(newItem);
}

// ============================================================== delAngleItem
void MakeHemiSphereDialog::delAngleItem()
{
    delete angle_lw->currentItem();
}

// ============================================================== addHeightItem
void MakeHemiSphereDialog::addHeightItem()
{
    QListWidgetItem* previousItem = height_lw->currentItem();
    QListWidgetItem* newItem      = new QListWidgetItem();

    double defaultValue = 1.;
    if ( previousItem )
        defaultValue = previousItem->data(Qt::EditRole).toDouble();

    newItem->setData(  Qt::EditRole, QVariant(defaultValue) );
    newItem->setFlags( newItem->flags () | Qt::ItemIsEditable);
    height_lw->addItem(newItem);
}

// ============================================================== delHeightItem
void MakeHemiSphereDialog::delHeightItem()
{
    delete height_lw->currentItem();
}

// ============================================================== clear
void MakeHemiSphereDialog::clear()
{
    center_le->clear();
    modelUnregister(center_le);

    hole_axis_le->clear();
    modelUnregister(hole_axis_le);

    base_le->clear();
    modelUnregister(base_le);

    vplan_le->clear();
    modelUnregister(vplan_le);

    modelUnregister(this);
}

// ============================================================== apply
bool MakeHemiSphereDialog::apply(QModelIndex& result)
{
    if (_currentObj != NULL) _highlightWidget(_currentObj, Qt::white);
    _currentObj = NULL;

    DocumentModel* docModel = getDocumentModel();
    PatternDataModel* patternDataModel = getPatternDataModel();
//    PatternBuilderModel* patternBuilderModel = getPatternBuilderModel();

    QModelIndex iElts;
    QModelIndex icenter = patternDataModel->mapToSource( _index[center_le] );
    QModelIndex ivplan  = patternDataModel->mapToSource( _index[vplan_le] );
    QModelIndex ivecx = patternDataModel->mapToSource(_index[base_le]);
    QModelIndex ivecz = patternDataModel->mapToSource(_index[hole_axis_le]);
//    QModelIndex ivecx   = patternBuilderModel->mapToSource( _index[base_le] );
//    QModelIndex ivecz   = patternBuilderModel->mapToSource( _index[hole_axis_le] );

    double radhole  = hole_rad_spb->value();
    double radext   = sphere_radext_spb->value();
    double radint   = sphere_radint_spb->value();
    double radang = radial_angle_spb->value();

    int crit  =  crit_spb->value();
    int nb    = nbre_spb->value();
    int nrad  = ngrid_rad_spb->value();
    int nang  = ngrid_ang_spb->value();
    int nhaut = ngrid_height_spb->value();

    //radius, angles and heights collection
    QListWidgetItem* item = NULL;

    vector<double> radius1;
    vector<double> radius2;
    vector<double> angles;
    vector<double> heights;

    //angles collection
//    double somme = 0.;
    int nbAngles = angle_lw->count();
    for ( int r = 0; r < nbAngles; ++r){
        item = angle_lw->item(r);
        double itemValue = item->data(Qt::EditRole).toDouble();
        angles.push_back(itemValue);
//        somme += itemValue;
    }

    //radius1 collection
    int nbRadius = radius_lw_1->count();
    for ( int r = 0; r < nbRadius; ++r){
        item = radius_lw_1->item(r);
        radius1.push_back(item->data(Qt::EditRole).toDouble());
    }

    //radius2 collection
    nbRadius = radius_lw_2->count();
    for ( int r = 0; r < nbRadius; ++r){
        item = radius_lw_2->item(r);
        radius2.push_back(item->data(Qt::EditRole).toDouble());
    }

    //heights collection
    int nbHeight = height_lw->count();
    for ( int r = 0; r < nbHeight; ++r){
        item = height_lw->item(r);
        heights.push_back(item->data(Qt::EditRole).toDouble());
    }

    if (sphere_rb->isChecked())
    {
        if (sphereTop_rb->isChecked())
            iElts = docModel->makeSphereTop( nrad, nang, nhaut );
        else if (sphereUni_rb->isChecked())
        {
            if (icenter.isValid() && ivplan.isValid() && ivecx.isValid() && ivecz.isValid())
                iElts = docModel->makeSphereUni(icenter, ivecx, ivecz, radhole, radext, radang,
                                                ivplan, nrad, nang, nhaut);
        }
        else if (sphere2_rb->isChecked())
        {
//            if (somme > 360.01)
//            {
//                SUIT_MessageBox::information( 0,
//                        tr("HEXA_INFO"),
//                        tr("The sum of the picked angles has to be \nless or equal than %1 degrees.").arg(360));
//                return false;
//            }
            if (icenter.isValid() && ivecx.isValid() && ivecz.isValid())
                iElts = docModel->makeSphere(icenter, ivecx, ivecz, radius2, angles, heights);
        }
    }
    else if (spherical_rb->isChecked())
    {
        if (sphericalTop_rb->isChecked())
            iElts = docModel->makeSphericalTop(nb, crit);
        else if (sphericalUni_rb->isChecked())
        {
            if (icenter.isValid() && ivecx.isValid() && ivecz.isValid())
                iElts = docModel->makeSphericalUni(icenter, ivecx, ivecz, radhole, nb, crit);
        }
        else if (spherical2_rb->isChecked())
        {
            if (icenter.isValid() && ivecx.isValid() && ivecz.isValid())
                iElts = docModel->makeSpherical(icenter, ivecx, ivecz, radius1, crit);
        }
    }
    else if (rind_rb->isChecked())
    {
        if (rindTop_rb->isChecked())
            iElts = docModel->makeRindTop(nrad, nang, nhaut);
        else if (rindUni_rb->isChecked())
        {
            if (icenter.isValid() && ivecx.isValid() && ivecz.isValid() && ivplan.isValid())
                iElts = docModel->makeRindUni(icenter, ivecx, ivecz,
                                              radhole, radint, radext, radang,
                                              ivplan, nrad, nang, nhaut);
        }
        else if (rind2_rb->isChecked())
        {
//            if (somme > 360.01)
//            {
//                SUIT_MessageBox::information( 0,
//                        tr("HEXA_INFO"),
//                        tr("The sum of the picked angles has to be \nless or equal than %1 degrees.").arg(360));
//                return false;
//            }
            if (icenter.isValid() && ivecx.isValid() && ivecz.isValid())
                iElts = docModel->makeRind(icenter, ivecx, ivecz, radius2, angles, heights);
        }
    }

    if ( !iElts.isValid() ){
        SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE RIND" ) + "\n" + getErrorMsg() );
        return false;
    }
    _value  = iElts.model()->data(iElts, HEXA_DATA_ROLE).value<HEXA_NS::Elements*>();
    result = patternDataModel->mapFromSource(iElts);
//    result = patternBuilderModel->mapFromSource(iElts);

    return true;
}

// ------------------------- ModelInfoDialog ----------------------------------
ModelInfoDialog::ModelInfoDialog(QWidget* parent, Qt::WindowFlags wf):
                 HexaBaseDialog(parent, INFO_MODE, wf)
{
    setupUi( this );
    setWindowTitle(HEXABLOCKGUI::tr("MODEL_INFO"));
}

void ModelInfoDialog::updateInfo()
{
    DocumentModel* docModel = getDocumentModel();
    if (docModel == NULL) return;

    //Model name
    model_name_le->setText(docModel->getName());

    //Nb of elements in the model
    total_vertices_le->setText(QString::number(docModel->getNbrElt(HEXA_NS::EL_VERTEX)));
    total_edges_le->setText(QString::number(docModel->getNbrElt(HEXA_NS::EL_EDGE)));
    total_quads_le->setText(QString::number(docModel->getNbrElt(HEXA_NS::EL_QUAD)));
    total_hexas_le->setText(QString::number(docModel->getNbrElt(HEXA_NS::EL_HEXA)));

    ///Nb of used elements in the model
    used_vertices_le->setText(QString::number(docModel->getNbrUsedElt(HEXA_NS::EL_VERTEX)));
    used_edges_le->setText(QString::number(docModel->getNbrUsedElt(HEXA_NS::EL_EDGE)));
    used_quads_le->setText(QString::number(docModel->getNbrUsedElt(HEXA_NS::EL_QUAD)));
    used_hexas_le->setText(QString::number(docModel->getNbrUsedElt(HEXA_NS::EL_HEXA)));

    //Nb of unused elements in the model
    unused_vertices_le->setText(QString::number(docModel->getNbrUnusedElt(HEXA_NS::EL_VERTEX)));
    unused_edges_le->setText(QString::number(docModel->getNbrUnusedElt(HEXA_NS::EL_EDGE)));
    unused_quads_le->setText(QString::number(docModel->getNbrUnusedElt(HEXA_NS::EL_QUAD)));
    unused_hexas_le->setText(QString::number(docModel->getNbrUnusedElt(HEXA_NS::EL_HEXA)));
}

void ModelInfoDialog::showEvent( QShowEvent * event )
{
    updateInfo();
    QDialog::showEvent ( event );
}

// ============================================================== hideEvent
void ModelInfoDialog::hideEvent ( QHideEvent * event )
{
    QDialog::hideEvent( event );
}

// ------------------------- AddShapeDialog ----------------------------------
AddShapeDialog::AddShapeDialog(QWidget* parent, Mode editmode, Qt::WindowFlags wf):
                 HexaBaseDialog(parent, editmode, wf)
{
    _helpFileName = "gui_add_geometry.html#guiaddgeometry";
    setupUi( this );
    _initWidget(editmode);
    setWindowTitle(HEXABLOCKGUI::tr("ADD_SHAPE"));
}

// ============================================================== Destructeur
AddShapeDialog::~AddShapeDialog()
{
    disconnect( HEXABLOCKGUI::selectionMgr(), SIGNAL(currentSelectionChanged()),
            this, SLOT(onCurrentSelectionChanged()) );
}

// ============================================================== close
void AddShapeDialog::close()
{
    HEXABLOCKGUI::assocInProgress = false;
    HexaBaseDialog::close();
}

// ============================================================== _initInputWidget
void AddShapeDialog::_initInputWidget( Mode editmode )
{
    shape_le->setProperty( "HexaWidgetType",  QVariant::fromValue(GEOMSHAPE_TREE) );
    shape_le->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_SHAPE) );
    shape_le->installEventFilter(this);
    shape_le->setReadOnly(true);
}

void AddShapeDialog::clear()
{
    shape_le->clear();
    shape_le->setProperty("GeomObj", QVariant());

    modelUnregister(this);
}

// ============================================================== onCurrentSelectionChanged
void AddShapeDialog::onCurrentSelectionChanged()
{
       SALOME_ListIO selectedObjects;
       HEXABLOCKGUI::currentDocGView->getSelected(selectedObjects);
       if (selectedObjects.IsEmpty())
           return;

       // * extract the TopoDS_Shape of the selected object
       TopoDS_Shape selectedTopoShape = GEOMBase::GetTopoFromSelection( selectedObjects );
       if (selectedTopoShape.IsNull())
           return;
       shape_le->setProperty("TopoDS_Shape", QVariant::fromValue<TopoDS_Shape>(selectedTopoShape));

       // * extract the name of the selected shape
       QString name;
       GEOMBase::GetNameOfSelectedIObjects( selectedObjects, name, true );
       shape_le->setText(name);
}

// ============================================================== onWindowActivated
void AddShapeDialog::onWindowActivated(SUIT_ViewManager* vm)
{
    QString vmType = vm->getType();
    if ( vmType == OCCViewer_Viewer::Type() )
        shape_le->setFocus();
}

// ============================================================== apply
bool AddShapeDialog::apply(QModelIndex& result)
{
    bool assocOk;
    QString shapeName = shape_le->text();
    QVariant v = shape_le->property("TopoDS_Shape");
    if ( !v.isValid() || shapeName.isEmpty())
        assocOk = false;
    else
    {
        TopoDS_Shape shape = v.value<TopoDS_Shape>();
        if (shape.IsNull())
            assocOk = false;
        else
            assocOk = getDocumentModel()->addShape(shape, shapeName);
    }

    if ( !assocOk ){
        SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), HEXABLOCKGUI::tr("ADD_SHAPE_FAILED") + "\n" + getErrorMsg() );
        return false;
    }

    return true;
}
