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

#include "MyBasicGUI_PointDlg.hxx"

#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include "MyDlgRef.hxx"


#include <QApplication>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QMenu>
#include <QTimer>
#include <QVector3D>

#include <gp_Pnt.hxx>
#include <TopoDS_Shape.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <TopExp.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>


#include <VTKViewer_ViewModel.h>
#include <SUIT_MessageBox.h>
#include <SUIT_OverrideCursor.h>
#include <SalomeApp_Tools.h>
#include <SalomeApp_Study.h>

#include "HEXABLOCKGUI_DocumentModel.hxx"
#include "HEXABLOCKGUI_DocumentSelectionModel.hxx"
#include "HEXABLOCKGUI_DocumentItem.hxx"
#include "HEXABLOCKGUI_SalomeTools.hxx"
#include "HEXABLOCKGUI.hxx"
#include "HEXABLOCKGUI_VtkDocumentGraphicView.hxx"

#define PARAM_VALUE 0
#define COORD_VALUE 1
#define LENGTH_VALUE 2

#define GEOM_POINT_XYZ    0
#define GEOM_POINT_REF    1
#define GEOM_POINT_EDGE   2
#define GEOM_POINT_INTINT 3
#define GEOM_POINT_SURF   4

#define SPACING 6
#define MARGIN  9

using namespace HEXABLOCK::GUI;

Q_DECLARE_METATYPE(HEXABLOCK::GUI::HexaTreeRole);
Q_DECLARE_METATYPE(TopAbs_ShapeEnum);
Q_DECLARE_METATYPE(TopoDS_Shape);

//=================================================================================
// class    : MyBasicGUI_PointDlg()
// purpose  : Constructs a MyBasicGUI_PointDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
MyBasicGUI_PointDlg::MyBasicGUI_PointDlg(QWidget* parent, Qt::WindowFlags fl)
: MyGEOMBase_Skeleton(parent, fl),
  myBusy (false)
{
    SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
    QPixmap image0 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_POINT")));
    QPixmap image1 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_POINT_EDGE")));
    QPixmap image2 (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));
    QPixmap image3 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_POINT_REF")));
    QPixmap image4 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_POINT_LINES")));
    QPixmap image5 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_POINT_FACE")));
    QPixmap image6 (aResMgr->loadPixmap("GEOM", tr("ICO_LINE")));
    QPixmap image7 (aResMgr->loadPixmap("GEOM", tr("ICO_WIRE")));

    setWindowTitle( tr("Vertex Association") );
//    setMinimumWidth(260);
//    setMinimumHeight(400);

    mainFrame()->GroupConstructors->setTitle(tr("GEOM_POINTS"));
    mainFrame()->RadioButton1->setIcon(image0);
    mainFrame()->RadioButton2->setIcon(image3);
    mainFrame()->RadioButton3->setIcon(image1);
    mainFrame()->RadioButton4->show();
    mainFrame()->RadioButton4->setIcon(image4);
    mainFrame()->RadioButton5->show();
    mainFrame()->RadioButton5->setIcon(image5);

    myParamGroup = new QGroupBox(centralWidget());
    myParamCoord = new QButtonGroup(myParamGroup);
    QHBoxLayout* boxLayout = new QHBoxLayout(myParamGroup);
    boxLayout->setMargin(MARGIN); boxLayout->setSpacing(SPACING);

    QRadioButton* btn = new QRadioButton(tr("By Param"), myParamGroup);
    myParamCoord->addButton(btn, PARAM_VALUE);
    boxLayout->addWidget(btn);

    btn = new QRadioButton(tr("By Length"), myParamGroup);
    myParamCoord->addButton(btn, LENGTH_VALUE);
    boxLayout->addWidget(btn);

    btn = new QRadioButton(tr("By Coords"), myParamGroup);
    myParamCoord->addButton(btn, COORD_VALUE);
    boxLayout->addWidget(btn);

    myParamCoord->setExclusive(true);
    myParamCoord->button(PARAM_VALUE)->setChecked(true);

    GroupXYZ = new MyDlgRef_3Spin(centralWidget());
    GroupXYZ->GroupBox1->setTitle(tr("GEOM_COORDINATES"));
    GroupXYZ->TextLabel1->setText(tr("GEOM_X"));
    GroupXYZ->TextLabel2->setText(tr("GEOM_Y"));
    GroupXYZ->TextLabel3->setText(tr("GEOM_Z"));
    coordsInputValue[GEOM_POINT_XYZ]  = new QVector3D(0, 0, 0);
    coordsInputValue[GEOM_POINT_EDGE] = new QVector3D(0, 0, 0);
    coordsInputValue[GEOM_POINT_SURF] = new QVector3D(0, 0, 0);

    GroupOnCurve = new MyDlgRef_2Sel1Spin(centralWidget());
    GroupOnCurve->GroupBox1->setTitle(tr("GEOM_POINT_ON_EDGE"));
    GroupOnCurve->TextLabel1->setText(tr("GEOM_EDGE"));
    GroupOnCurve->TextLabel2->setText(tr("GEOM_START_POINT"));
    GroupOnCurve->TextLabel3->setText(tr("GEOM_PARAMETER"));
    paramInputValue[PARAM_VALUE] = 0.5;
    paramInputValue[LENGTH_VALUE] = 0.5;

    GroupOnSurface = new MyDlgRef_1Sel2Spin(centralWidget());
    GroupOnSurface->GroupBox1->setTitle(tr("GEOM_POINT_ON_FACE"));
    GroupOnSurface->TextLabel1->setText(tr("GEOM_FACE"));
    GroupOnSurface->TextLabel2->setText(tr("GEOM_UPARAMETER"));
    GroupOnSurface->TextLabel3->setText(tr("GEOM_VPARAMETER"));

    GroupRefPoint = new MyDlgRef_1Sel3Spin(centralWidget());
    GroupRefPoint->GroupBox1->setTitle(tr("GEOM_REF_POINT"));
    GroupRefPoint->TextLabel1->setText(tr("GEOM_POINT"));
    GroupRefPoint->TextLabel2->setText(tr("GEOM_DX"));
    GroupRefPoint->TextLabel3->setText(tr("GEOM_DY"));
    GroupRefPoint->TextLabel4->setText(tr("GEOM_DZ"));

    /* popup menu for line intersect buttons */
    QIcon ico_line = QIcon(image6);
    QIcon ico_wire = QIcon(image7);
    // * menu for line1
    myBtnPopup = new QMenu(this);
    action_line1_edge = myBtnPopup->addAction(ico_line, tr("GEOM_EDGE"));
    action_line1_wire = myBtnPopup->addAction(ico_wire, tr("GEOM_WIRE"));

    // * menu for line 2
    myBtnPopup2 = new QMenu(this);
    action_line2_edge = myBtnPopup2->addAction(ico_line, tr("GEOM_EDGE"));
    action_line2_wire = myBtnPopup2->addAction(ico_wire, tr("GEOM_WIRE"));

    GroupLineIntersection = new MyDlgRef_2Sel(centralWidget());
    GroupLineIntersection->GroupBox1->setTitle(tr("GEOM_LINE_INTERSECTION"));
    GroupLineIntersection->TextLabel1->setText(tr("GEOM_LINE1"));
    GroupLineIntersection->TextLabel2->setText(tr("GEOM_LINE2"));
    GroupLineIntersection->PushButton1->setIcon(image2);
    GroupLineIntersection->PushButton1->setMenu(myBtnPopup);
    GroupLineIntersection->PushButton2->setIcon(image2);
    GroupLineIntersection->PushButton2->setMenu(myBtnPopup2);

    myCoordGrp = new QGroupBox(tr("GEOM_COORDINATES_RES"), centralWidget());
    QGridLayout* myCoordGrpLayout = new QGridLayout(myCoordGrp);
    myCoordGrpLayout->addWidget(new QLabel(tr("GEOM_X"), myCoordGrp), 0, 0);
    myX = new QLineEdit(myCoordGrp);
    myCoordGrpLayout->addWidget(myX, 0, 1);
    myCoordGrpLayout->addWidget(new QLabel(tr("GEOM_Y"), myCoordGrp), 1, 0);
    myY = new QLineEdit(myCoordGrp);
    myCoordGrpLayout->addWidget(myY, 1, 1);
    myCoordGrpLayout->addWidget(new QLabel(tr("GEOM_Z"), myCoordGrp), 2, 0);
    myZ = new QLineEdit(myCoordGrp);
    myCoordGrpLayout->addWidget(myZ, 2, 1);

    QVBoxLayout* layout = new QVBoxLayout(centralWidget());
    layout->setMargin(0); layout->setSpacing(6);
    layout->addWidget(myParamGroup);
    layout->addWidget(GroupXYZ);
    layout->addWidget(GroupOnCurve);
    layout->addWidget(GroupOnSurface);
    layout->addWidget(GroupRefPoint);
    layout->addWidget(GroupLineIntersection);
    layout->addWidget(myCoordGrp);

    myX->setReadOnly(true);
    myY->setReadOnly(true);
    myZ->setReadOnly(true);

    myX->setEnabled(false);
    myY->setEnabled(false);
    myZ->setEnabled(false);

    QPalette aPal = myX->palette();
    aPal.setColor(QPalette::Disabled, QPalette::Text, QColor(0, 0, 0));
    myX->setPalette(aPal);
    myY->setPalette(aPal);
    myZ->setPalette(aPal);

    _helpFileName = "gui_asso_quad_to_geom.html#associate-to-a-vertex-of-the-geometry";
    _initWidget( _editMode );
    mainFrame()->RadioButton1->click();
}

//=================================================================================
// function : ~MyBasicGUI_PointDlg()
// purpose  : Destructor
//=================================================================================
MyBasicGUI_PointDlg::~MyBasicGUI_PointDlg()
{
}

void MyBasicGUI_PointDlg::_initInputWidget( HexaBaseDialog::Mode editmode )
{
    // *** Init input widgets ***/
    initSpinBox(GroupXYZ->SpinBox_DX, COORD_MIN, COORD_MAX);
    initSpinBox(GroupXYZ->SpinBox_DY, COORD_MIN, COORD_MAX);
    initSpinBox(GroupXYZ->SpinBox_DZ, COORD_MIN, COORD_MAX);
    GroupXYZ->SpinBox_DX->setValue(0.0);
    GroupXYZ->SpinBox_DY->setValue(0.0);
    GroupXYZ->SpinBox_DZ->setValue(0.0);

    initSpinBox(GroupRefPoint->SpinBox_DX, COORD_MIN, COORD_MAX);
    initSpinBox(GroupRefPoint->SpinBox_DY, COORD_MIN, COORD_MAX);
    initSpinBox(GroupRefPoint->SpinBox_DZ, COORD_MIN, COORD_MAX);
    GroupRefPoint->SpinBox_DX->setValue(0.0);
    GroupRefPoint->SpinBox_DY->setValue(0.0);
    GroupRefPoint->SpinBox_DZ->setValue(0.0);

    initSpinBox(GroupOnCurve->SpinBox_DX, 0., 1.);
    GroupOnCurve->SpinBox_DX->setValue(paramInputValue[PARAM_VALUE]);

    initSpinBox(GroupOnSurface->SpinBox_DX, 0., 1.);
    GroupOnSurface->SpinBox_DX->setValue(0.5);
    initSpinBox(GroupOnSurface->SpinBox_DY, 0., 1.);
    GroupOnSurface->SpinBox_DY->setValue(0.5);

    // * vtk input widget
    mainFrame()->_vertex_le->setReadOnly(true);
    mainFrame()->_vertex_le->setProperty( "HexaWidgetType",  QVariant::fromValue(HEXABLOCK::GUI::VERTEX_TREE) );
    mainFrame()->_vertex_le->installEventFilter(this);

    // * geom input widgets
    // point with ref
    GroupRefPoint->LineEdit1->setReadOnly(true);
    GroupRefPoint->LineEdit1->setProperty( "HexaWidgetType", QVariant::fromValue(GEOMPOINT_TREE) );
    GroupRefPoint->LineEdit1->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_VERTEX) );
    GroupRefPoint->LineEdit1->installEventFilter(this);

    // point on curve
    //  - Line
    GroupOnCurve->LineEdit1->setReadOnly(true);
    GroupOnCurve->LineEdit1->setProperty( "HexaWidgetType", QVariant::fromValue(GEOMEDGE_TREE) );
    GroupOnCurve->LineEdit1->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_EDGE) );
    GroupOnCurve->LineEdit1->installEventFilter(this);

    //  - Start point
    GroupOnCurve->LineEdit2->setReadOnly(true);
    GroupOnCurve->LineEdit2->setProperty( "HexaWidgetType", QVariant::fromValue(GEOMPOINT_TREE) );
    GroupOnCurve->LineEdit2->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_VERTEX) );
    GroupOnCurve->LineEdit2->installEventFilter(this);

    // point of 2 lines intersection
    //  - Line1
    GroupLineIntersection->LineEdit1->setReadOnly(true);
    GroupLineIntersection->LineEdit1->setProperty( "HexaWidgetType", QVariant::fromValue(GEOMEDGE_TREE) );
    GroupLineIntersection->LineEdit1->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_EDGE) );
    GroupLineIntersection->LineEdit1->installEventFilter(this);

    // - Line2
    GroupLineIntersection->LineEdit2->setReadOnly(true);
    GroupLineIntersection->LineEdit2->setProperty( "HexaWidgetType", QVariant::fromValue(GEOMEDGE_TREE) );
    GroupLineIntersection->LineEdit2->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_EDGE) );
    GroupLineIntersection->LineEdit2->installEventFilter(this);

    //point on surface
    GroupOnSurface->LineEdit1->setReadOnly(true);
    GroupOnSurface->LineEdit1->setProperty( "HexaWidgetType", QVariant::fromValue(GEOMFACE_TREE) );
    GroupOnSurface->LineEdit1->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_FACE) );
    GroupOnSurface->LineEdit1->installEventFilter(this);

    // * connect signals
    connect(this,           SIGNAL(constructorsClicked(int)), this, SLOT(ConstructorsClicked(int)));
    connect(myParamCoord,   SIGNAL(buttonClicked(int)), this, SLOT(ClickParamCoord(int)));
    connect(myBtnPopup, SIGNAL(triggered(QAction*)), this, SLOT(onBtnPopup(QAction*)));
    connect(myBtnPopup2, SIGNAL(triggered(QAction*)), this, SLOT(onBtnPopup(QAction*)));

    // ref point
    connect(GroupRefPoint->LineEdit1, SIGNAL( textChanged(const QString&) ), this, SLOT(onUpdateResults(const QString&)));

    //point on curve
    connect(GroupOnCurve->LineEdit1, SIGNAL( textChanged(const QString&) ), this, SLOT(onUpdateResults(const QString&)));
    connect(GroupOnCurve->LineEdit2, SIGNAL( textChanged(const QString&) ), this, SLOT(onUpdateResults(const QString&)));

    connect(GroupOnCurve->SpinBox_DX,   SIGNAL(valueChanged(double)), this, SLOT(onParamValueChanged(double)));
    connect(GroupOnCurve->SpinBox_DX,  SIGNAL(valueChanged(const QString&)), this, SLOT(onUpdateResults(const QString&)));

    // intersection point
    connect(GroupLineIntersection->LineEdit1, SIGNAL( textChanged(const QString&) ), this, SLOT(onUpdateResults(const QString&)));
    connect(GroupLineIntersection->LineEdit2, SIGNAL( textChanged(const QString&) ), this, SLOT(onUpdateResults(const QString&)));

    // point on surface
    connect(GroupOnSurface->SpinBox_DX, SIGNAL(valueChanged(const QString&)), this, SLOT(onUpdateResults(const QString&)));
    connect(GroupOnSurface->SpinBox_DY, SIGNAL(valueChanged(const QString&)), this, SLOT(onUpdateResults(const QString&)));
    connect(GroupOnSurface->LineEdit1,  SIGNAL( textChanged(const QString&) ), this, SLOT(onUpdateResults(const QString&))); //surface: Face

    // Coords X, Y, Z
    connect(GroupXYZ->SpinBox_DX,       SIGNAL(valueChanged(double)), this, SLOT(onXCoordChanged(double)));
    connect(GroupXYZ->SpinBox_DY,       SIGNAL(valueChanged(double)), this, SLOT(onYCoordChanged(double)));
    connect(GroupXYZ->SpinBox_DZ,       SIGNAL(valueChanged(double)), this, SLOT(onZCoordChanged(double)));

    connect(GroupXYZ->SpinBox_DX,  SIGNAL(valueChanged(const QString&)), this, SLOT(onUpdateResults(const QString&)));
    connect(GroupXYZ->SpinBox_DY,  SIGNAL(valueChanged(const QString&)), this, SLOT(onUpdateResults(const QString&)));
    connect(GroupXYZ->SpinBox_DZ,  SIGNAL(valueChanged(const QString&)), this, SLOT(onUpdateResults(const QString&)));

    // DX, DY, DZ
    connect(GroupRefPoint->SpinBox_DX,  SIGNAL(valueChanged(const QString&)), this, SLOT(onUpdateResults(const QString&)));
    connect(GroupRefPoint->SpinBox_DY,  SIGNAL(valueChanged(const QString&)), this, SLOT(onUpdateResults(const QString&)));
    connect(GroupRefPoint->SpinBox_DZ,  SIGNAL(valueChanged(const QString&)), this, SLOT(onUpdateResults(const QString&)));

    // radio buttons
    connect( mainFrame()->RadioButton1, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
    connect( mainFrame()->RadioButton2, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
    connect( mainFrame()->RadioButton3, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
    connect( mainFrame()->RadioButton4, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
    connect( mainFrame()->RadioButton5, SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );

    connect( mainFrame()->RadioButton1, SIGNAL(clicked()), this, SLOT( refreshHighlight()) );
    connect( mainFrame()->RadioButton2, SIGNAL(clicked()), this, SLOT(refreshHighlight()) );
    connect( mainFrame()->RadioButton1, SIGNAL(clicked()), this, SLOT( refreshHighlight()) );
    connect( mainFrame()->RadioButton2, SIGNAL(clicked()), this, SLOT(refreshHighlight()) );
    connect( mainFrame()->RadioButton1, SIGNAL(clicked()), this, SLOT( refreshHighlight()) );

    connect( myParamCoord->button(PARAM_VALUE), SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
    connect( myParamCoord->button(LENGTH_VALUE), SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );
    connect( myParamCoord->button(COORD_VALUE), SIGNAL(clicked()), this, SLOT(updateHelpFileName()) );

}

void MyBasicGUI_PointDlg::showEvent ( QShowEvent * event )
{
    HexaBaseDialog::showEvent(event);
    if (getConstructorId() == GEOM_POINT_XYZ)
        HEXABLOCKGUI::currentOccGView->setSelectionMode(TopAbs_VERTEX);
}
//===============================================================
// function : updateHelpFileName()
// purpose  : update the help file according to the current panel
//===============================================================
void MyBasicGUI_PointDlg::updateHelpFileName()
{
    if ( sender() == mainFrame()->RadioButton1 ){
        _helpFileName = "gui_asso_quad_to_geom.html#associate-to-a-vertex-of-the-geometry";
    } else if ( sender() == mainFrame()->RadioButton2 ){
        _helpFileName = "gui_asso_vertex_to_geom.html#by-a-reference";
    } else if ( sender() == mainFrame()->RadioButton3 ){
        _helpFileName = "gui_asso_vertex_to_geom.html#by-an-edge-and-a-parameter";
    } else if ( sender() == mainFrame()->RadioButton4 ){
        _helpFileName = "gui_asso_vertex_to_geom.html#by-intersection-of-two-lines-or-wires";
    } else if ( sender() == mainFrame()->RadioButton5 ){
        _helpFileName = "gui_asso_vertex_to_geom.html#by-a-face-and-two-parameters";
    } else if (sender() == myParamCoord->button(PARAM_VALUE)){
        if (mainFrame()->RadioButton3->isChecked()){
            _helpFileName = "gui_asso_vertex_to_geom.html#by-an-edge-and-a-parameter";
        } else if (mainFrame()->RadioButton5->isChecked()){
            _helpFileName = "gui_asso_vertex_to_geom.html#by-a-face-and-two-parameters";
        }
    } else if (sender() == myParamCoord->button(LENGTH_VALUE)){
        _helpFileName = "gui_asso_vertex_to_geom.html#by-an-edge-and-a-length";
    } else if (sender() == myParamCoord->button(COORD_VALUE)){
        if (mainFrame()->RadioButton3->isChecked()){
            _helpFileName = "gui_asso_vertex_to_geom.html#by-an-edge-and-coordinates";
        } else if (mainFrame()->RadioButton5->isChecked()){
            _helpFileName = "gui_asso_vertex_to_geom.html#by-a-face-and-coordinates";
        }
    }
}

//=================================================================================
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//=================================================================================
void MyBasicGUI_PointDlg::SetDoubleSpinBoxStep(double step)
{
    GroupOnCurve->SpinBox_DX->setSingleStep(step);
    GroupXYZ->SpinBox_DX->setSingleStep(step);
    GroupXYZ->SpinBox_DY->setSingleStep(step);
    GroupXYZ->SpinBox_DZ->setSingleStep(step);
    GroupRefPoint->SpinBox_DX->setSingleStep(step);
    GroupRefPoint->SpinBox_DY->setSingleStep(step);
    GroupRefPoint->SpinBox_DZ->setSingleStep(step);
}


//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void MyBasicGUI_PointDlg::ConstructorsClicked(int constructorId)
{
	clearVTKSelection();
	clearOCCSelection();

    switch (constructorId) {
    case GEOM_POINT_XYZ:
    {
        HEXABLOCKGUI::currentOccGView->setSelectionMode(TopAbs_VERTEX);
        GroupRefPoint->hide();
        GroupOnCurve->hide();
        GroupLineIntersection->hide();
        GroupOnSurface->hide();
        myCoordGrp->hide();
        myParamGroup->hide();
        GroupXYZ->show();
        break;
    }
    case GEOM_POINT_REF:
    {
        myParamGroup->hide();
        GroupXYZ->hide();
        GroupOnCurve->hide();
        GroupLineIntersection->hide();
        GroupOnSurface->hide();
        GroupRefPoint->show();
        myCoordGrp->show();
        break;
    }
    case GEOM_POINT_EDGE:
    {
        GroupRefPoint->hide();
        GroupLineIntersection->hide();
        GroupOnSurface->hide();
        myParamGroup->show();
        myParamCoord->button(LENGTH_VALUE)->show();
        myParamCoord->button(PARAM_VALUE)->setChecked(true);
        GroupOnCurve->show();
        myCoordGrp->show();
        updateParamCoord(false);
        break;
    }
    case GEOM_POINT_INTINT:
    {
        myParamGroup->hide();
        GroupXYZ->hide();
        GroupRefPoint->hide();
        GroupOnCurve->hide();
        GroupOnSurface->hide();
        myCoordGrp->show();
        GroupLineIntersection->show();
        break;
    }
    case GEOM_POINT_SURF:
    {
        GroupRefPoint->hide();
        GroupOnCurve->hide();
        GroupLineIntersection->hide();
        myParamGroup->show();
        myParamCoord->button(LENGTH_VALUE)->hide();
        myParamCoord->button(PARAM_VALUE)->setChecked(true);
        GroupOnSurface->show();
        myCoordGrp->show();
        updateParamCoord(false);
        break;
    }
    }

    updateInputs(constructorId);
}

//=================================================================================
// funcion  : getParameter()
// purpose  :
//=================================================================================
double MyBasicGUI_PointDlg::getParameter() const
{
    return GroupOnCurve->SpinBox_DX->value();
}

//=================================================================================
// funcion  : getUParameter()
// purpose  :
//=================================================================================
double MyBasicGUI_PointDlg::getUParameter() const
{
    return GroupOnSurface->SpinBox_DX->value();
}

//=================================================================================
// funcion  : getVParameter()
// purpose  :
//=================================================================================
double MyBasicGUI_PointDlg::getVParameter() const
{
    return GroupOnSurface->SpinBox_DY->value();
}

void MyBasicGUI_PointDlg::onParamValueChanged(double newValue)
{
    if (getConstructorId() == GEOM_POINT_EDGE)
        paramInputValue[myParamCoord->checkedId()] = newValue;
}

// ============================================================== onSelectionChanged
/*
 * Puts elements selected in the model in the corresponding widget (list widget or line edit)
 * of the current dialog box.
 */
void MyBasicGUI_PointDlg::onSelectionChanged( const QItemSelection& sel, const QItemSelection& unsel )
{
    QModelIndexList l = sel.indexes();
    if ( l.count() == 0)
        return;

    QModelIndex selected = l[0];
    int selectedType = selected.data(HEXA_TREE_ROLE).toInt();
    // fill the coordinates of the selected point
    if ( getConstructorId() == GEOM_POINT_XYZ && selectedType == GEOMPOINT_TREE)
    {
        DocumentModel::GeomObj* geomObj = getGeomObj(selected);
        if (geomObj == NULL)
            return;

        DocumentModel* docModel = getDocumentModel();
        QString objId =  geomObj->shapeName + "," + geomObj->subId;
        HEXA_NS::SubShape* ssh = docModel->getGeomPtr(objId);
        if (ssh == NULL)
            return;
        TopoDS_Shape shape = ssh->getShape();
        if (shape.IsNull() || shape.ShapeType() != TopAbs_VERTEX)
            return;
        TopoDS_Vertex vertex = TopoDS::Vertex(shape);
        gp_Pnt pnt = BRep_Tool::Pnt(vertex);
        OnPointSelected(pnt);
        return;
    }

    QLineEdit*   aLineEdit   = NULL;
    QListWidget* aListWidget = NULL;

    // * put selection in the current line edit
    aLineEdit = dynamic_cast<QLineEdit*>(_currentObj);
    if ( aLineEdit != NULL )
    {
        _onSelectionChanged( sel, aLineEdit );
        return;
    }

    // * put selection in the current list widget
    aListWidget = dynamic_cast<QListWidget*>(_currentObj);
    if ( aListWidget )
        _onSelectionChanged( sel, aListWidget );
}


bool MyBasicGUI_PointDlg::apply(QModelIndex& result)
{
    PatternGeomSelectionModel* pgsm = getPatternGeomSelectionModel();
    PatternDataSelectionModel* pdsm = getPatternDataSelectionModel();
    PatternDataModel* pdm = getPatternDataModel();
    DocumentModel* docModel = getDocumentModel();

    // get the selected vertex in the vtk view
    QVariant v = mainFrame()->_vertex_le->property("QModelIndex");
    if ( !v.isValid() )
    {
        SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE VERTEX ASSOCIATION" ) );
        return false;
    }
    QModelIndex iVertex = pdm->mapToSource( v.value<QModelIndex>() );

    // get the selected vertex in the geometry (occ view)
    TopoDS_Vertex geomVertex = computeGeomVertex();

    // control
    if (!iVertex.isValid() || geomVertex.IsNull())
    {
        SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE VERTEX ASSOCIATION" ) );
        return false;
    }

    // * set association
    gp_Pnt aPnt = BRep_Tool::Pnt(geomVertex);
    if (!docModel->setVertexAssociation(iVertex, aPnt.X(), aPnt.Y(), aPnt.Z()))
    {
        SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "CANNOT MAKE VERTEX ASSOCIATION" ) );
        return false;
    }

    // * update data in the occ view and the geom tree
    HEXABLOCKGUI::currentOccGView->addVertexToCloud(geomVertex);
    docModel->updateGeomTree();

    // highlight associated items
    QModelIndex iGeomVertex = docModel->getVertexAssociation(iVertex);
    QModelIndexList list1, list2;
    result = v.value<QModelIndex>();
    list1 << result;
    list2 << iGeomVertex;
    pdsm->highlightTreeItems(list1); // data tree
    pgsm->highlightTreeItems(list2); // geom tree
    setFocus();
    _updateCurrentObject(NULL);
    HEXABLOCKGUI::currentOccGView->setSelectionMode(TopAbs_VERTEX);
    HEXABLOCKGUI::currentDocGView->highlight(list1); // vtk view
    HEXABLOCKGUI::currentOccGView->highlight(list1); // occ view

    return true;
}


void MyBasicGUI_PointDlg::onXCoordChanged(double newValue)
{
    int constructorId = getConstructorId();
    if (constructorId != GEOM_POINT_XYZ && constructorId != GEOM_POINT_EDGE &&
            constructorId != GEOM_POINT_SURF)
        return;
    coordsInputValue[constructorId]->setX(newValue);
}

void MyBasicGUI_PointDlg::onYCoordChanged(double newValue)
{
    int constructorId = getConstructorId();
    if (constructorId != GEOM_POINT_XYZ && constructorId != GEOM_POINT_EDGE &&
            constructorId != GEOM_POINT_SURF)
        return;
    coordsInputValue[constructorId]->setY(newValue);
}

void MyBasicGUI_PointDlg::onZCoordChanged(double newValue)
{
    int constructorId = getConstructorId();
    if (constructorId != GEOM_POINT_XYZ && constructorId != GEOM_POINT_EDGE &&
            constructorId != GEOM_POINT_SURF)
        return;
    coordsInputValue[constructorId]->setZ(newValue);

}

void MyBasicGUI_PointDlg::onUpdateResults(const QString& data)
{
    if (getConstructorId() == GEOM_POINT_XYZ)
        return;

    // * compute the geom vertex
    TopoDS_Vertex vertex = computeGeomVertex(true);

    // * update results
    if (vertex.IsNull())
    {
        myX->setText("");
        myY->setText("");
        myZ->setText("");
        return;
    }
    gp_Pnt pnt = BRep_Tool::Pnt(vertex);
    myX->setText(MyDlgRef::PrintDoubleValue(pnt.X(), 6));
    myY->setText(MyDlgRef::PrintDoubleValue(pnt.Y(), 6));
    myZ->setText(MyDlgRef::PrintDoubleValue(pnt.Z(), 6));
}

TopoDS_Vertex MyBasicGUI_PointDlg::computeGeomVertex(bool preview)
{
    TopoDS_Vertex vertex;
    int constructorId = getConstructorId();

    //display an emply vertex to erase the current preview
    HEXABLOCKGUI::currentOccGView->displayPreview(vertex);

    // compute the point
    switch (constructorId) {
    case GEOM_POINT_XYZ:
        vertex = computeGeomPointXYZ();
        break;
    case GEOM_POINT_REF:
        vertex = computeGeomPointRef();
        break;
    case GEOM_POINT_EDGE:
        vertex = computeGeomPointEdge();
        break;
    case GEOM_POINT_INTINT:
        vertex = computeGeomPointInt();
        break;
    case GEOM_POINT_SURF:
        vertex = computeGeomPointSurf();
        break;
    default:
        break;
    }

    // * display preview of computed point
    if (preview)
        HEXABLOCKGUI::currentOccGView->displayPreview(vertex);

    return vertex;
}

TopoDS_Vertex MyBasicGUI_PointDlg::computeGeomPointXYZ()
{
    TopoDS_Vertex vertex;

    if (getConstructorId() != GEOM_POINT_XYZ)
        return vertex;

    // - X, Y, Z Coords
    double x = GroupXYZ->SpinBox_DX->value();
    double y = GroupXYZ->SpinBox_DY->value();
    double z = GroupXYZ->SpinBox_DZ->value();

    // - compute the resulting vertex
    vertex = makePoint(x, y, z);

    return vertex;
}

TopoDS_Vertex MyBasicGUI_PointDlg::computeGeomPointRef()
{
    TopoDS_Vertex vertex;

    if (getConstructorId() != GEOM_POINT_REF)
        return vertex;

    // get parameters from widgets
    double dx = GroupRefPoint->SpinBox_DX->value();
    double dy = GroupRefPoint->SpinBox_DY->value();
    double dz = GroupRefPoint->SpinBox_DZ->value();

    QVariant v = GroupRefPoint->LineEdit1->property("TopoDS_Shape");
    if ( !v.isValid() )
        return vertex;
    TopoDS_Shape shape = v.value<TopoDS_Shape>();

    // compute the resulting point
    vertex = makePointWithReference(shape, dx, dy, dz);

    return vertex;
}

TopoDS_Vertex MyBasicGUI_PointDlg::computeGeomPointEdge()
{
    TopoDS_Vertex vertex;

    if (getConstructorId() != GEOM_POINT_EDGE)
        return vertex;

    // * get inputs values

    // - the edge
    QVariant v1 = GroupOnCurve->LineEdit1->property("TopoDS_Shape");
    if ( !v1.isValid() )
        return vertex;
    TopoDS_Shape edgeShape = v1.value<TopoDS_Shape>();

    bool isParam =  myParamCoord->checkedId() == PARAM_VALUE;
    bool isLength = myParamCoord->checkedId() == LENGTH_VALUE;
    bool isCoord  = myParamCoord->checkedId() == COORD_VALUE;
    if (isParam)
    {
    	// - param
    	double param = GroupOnCurve->SpinBox_DX->value();

    	// - compute the resulting vertex
    	vertex = makePointOnCurve(edgeShape, param);
    }
    else if (isLength)
    {
    	// - the starting point
    	QVariant v2 = GroupOnCurve->LineEdit2->property("TopoDS_Shape");
    	if ( !v2.isValid() )
    		return vertex;
    	TopoDS_Shape pointShape = v2.value<TopoDS_Shape>();

    	// - length
    	double length = GroupOnCurve->SpinBox_DX->value();

    	// - compute the resulting vertex
    	vertex = makePointOnCurveByLength(edgeShape, pointShape,length);
    }
    else if (isCoord)
    {
    	// - X, Y, Z Coords
    	double x = GroupXYZ->SpinBox_DX->value();
    	double y = GroupXYZ->SpinBox_DY->value();
    	double z = GroupXYZ->SpinBox_DZ->value();

    	// - compute the resulting vertex
    	vertex = makePointOnCurveByCoord(edgeShape, x, y, z);
    }

    return vertex;
}

TopoDS_Vertex MyBasicGUI_PointDlg::computeGeomPointInt()
{
    TopoDS_Vertex vertex;

    if (getConstructorId() != GEOM_POINT_INTINT)
        return vertex;

    QVariant v;

    // * get inputs values

    // - get edge 1
    v = GroupLineIntersection->LineEdit1->property("TopoDS_Shape");
    if ( !v.isValid() )
    	return vertex;
    TopoDS_Shape line1 = v.value<TopoDS_Shape>();

    // - get edge 2
    v = GroupLineIntersection->LineEdit2->property("TopoDS_Shape");
    if ( !v.isValid() )
    	return vertex;
    TopoDS_Shape line2 = v.value<TopoDS_Shape>();

    // * compute the resulting vertex
    vertex = makePointOnLinesIntersection(line1, line2);

    return vertex;
}

TopoDS_Vertex MyBasicGUI_PointDlg::computeGeomPointSurf()
{
    TopoDS_Vertex vertex;

    if (getConstructorId() != GEOM_POINT_SURF)
        return vertex;

    // * get inputs values

    // - the face
    QVariant v = GroupOnSurface->LineEdit1->property("TopoDS_Shape");
    if ( !v.isValid() )
    	return vertex;
    TopoDS_Shape faceShape = v.value<TopoDS_Shape>();

    bool isParam =  myParamCoord->checkedId() == PARAM_VALUE;
    bool isCoord  = myParamCoord->checkedId() == COORD_VALUE;
    if (isParam)
    {
    	// - params U,V
    	double param_u = GroupOnSurface->SpinBox_DX->value();
    	double param_v = GroupOnSurface->SpinBox_DY->value();

    	// - compute the resulting vertex
    	vertex = makePointOnSurface(faceShape, param_u, param_v);
    }
    else if (isCoord)
    {
    	// - X, Y, Z Coords
    	double x = GroupXYZ->SpinBox_DX->value();
    	double y = GroupXYZ->SpinBox_DY->value();
    	double z = GroupXYZ->SpinBox_DZ->value();

    	// - compute the resulting vertex
    	vertex = makePointOnSurfaceByCoord(faceShape, x, y, z);
    }

    return vertex;
}

//=================================================================================
// function : OnPointSelected
// purpose  :
//=================================================================================
void MyBasicGUI_PointDlg::OnPointSelected(const gp_Pnt& thePnt)
{
    if (getConstructorId() == GEOM_POINT_XYZ) {
        GroupXYZ->SpinBox_DX->setValue(thePnt.X());
        GroupXYZ->SpinBox_DY->setValue(thePnt.Y());
        GroupXYZ->SpinBox_DZ->setValue(thePnt.Z());
    }
}

void MyBasicGUI_PointDlg::updateInputs(const int constructorId)
{
    QString str;
    QVector3D* coords = NULL;

    switch (constructorId) {
    case GEOM_POINT_XYZ :
        coords = coordsInputValue[GEOM_POINT_XYZ];
        break;
    case GEOM_POINT_REF:
        break;
    case GEOM_POINT_EDGE:
        coords = coordsInputValue[GEOM_POINT_EDGE];
        updateParamCoord(true);
        break;
    case GEOM_POINT_INTINT:
        break;
    case GEOM_POINT_SURF:
        coords = coordsInputValue[GEOM_POINT_SURF];
        updateParamCoord(true);
        break;
    default:
        break;
    }

    if (coords != NULL)
    {
        GroupXYZ->SpinBox_DX->setValue(coords->x());
        GroupXYZ->SpinBox_DY->setValue(coords->y());
        GroupXYZ->SpinBox_DZ->setValue(coords->z());
    }

    onUpdateResults(str);
}

//=================================================================================
// function : ClickParamCoord()
// purpose  :
//=================================================================================
void MyBasicGUI_PointDlg::ClickParamCoord(int id)
{
    updateInputs(getConstructorId());
}

//=================================================================================
// function : updateParamCoord
// purpose  :
//=================================================================================
void MyBasicGUI_PointDlg::updateParamCoord(bool theIsUpdate)
{
    bool isParam = myParamCoord->checkedId() == PARAM_VALUE;
    bool isLength = myParamCoord->checkedId() == LENGTH_VALUE;

    const int id = getConstructorId();
    if (id == GEOM_POINT_EDGE) {
        GroupOnCurve->TextLabel2->setVisible(isLength);
        GroupOnCurve->LineEdit2->setVisible(isLength);
        GroupOnCurve->TextLabel3->setVisible(isParam || isLength);
        GroupOnCurve->SpinBox_DX->setVisible(isParam || isLength);
        if (isParam){
            GroupOnCurve->TextLabel3->setText(tr("GEOM_PARAMETER"));
            double value = paramInputValue[PARAM_VALUE];
            GroupOnCurve->SpinBox_DX->setValue(0.5);
            initSpinBox(GroupOnCurve->SpinBox_DX, 0., 1.);
            GroupOnCurve->SpinBox_DX->setValue(value);
        }
        else if (isLength){
            GroupOnCurve->TextLabel3->setText(tr("GEOM_LENGTH"));
            double value = paramInputValue[LENGTH_VALUE];
            GroupOnCurve->SpinBox_DX->setValue(0.5);
            initSpinBox(GroupOnCurve->SpinBox_DX, COORD_MIN, COORD_MAX);
            GroupOnCurve->SpinBox_DX->setValue(value);
        }
    }
    else if (id == GEOM_POINT_SURF) {
        GroupOnSurface->TextLabel2->setVisible(isParam);
        GroupOnSurface->TextLabel3->setVisible(isParam);
        GroupOnSurface->SpinBox_DX->setVisible(isParam);
        GroupOnSurface->SpinBox_DY->setVisible(isParam);
    }

    GroupXYZ->setVisible(!isParam && !isLength);
}

//=================================================================================
// function : onBtnPopup()
// purpose  :
//=================================================================================
void MyBasicGUI_PointDlg::onBtnPopup(QAction* a)
{
    if (a == action_line1_edge)
    {
        GroupLineIntersection->LineEdit1->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_EDGE) );
        GroupLineIntersection->LineEdit1->setFocus();
        GroupLineIntersection->PushButton2->setDown(false);
    }
    else if (a == action_line1_wire)
    {
        GroupLineIntersection->LineEdit1->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_WIRE) );
        GroupLineIntersection->LineEdit1->setFocus();
        GroupLineIntersection->PushButton2->setDown(false);
    }
    else if (a == action_line2_edge)
    {
        GroupLineIntersection->LineEdit2->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_EDGE) );
        GroupLineIntersection->LineEdit2->setFocus();
        GroupLineIntersection->PushButton1->setDown(false);
    }
    else if (a == action_line2_wire)
    {
        GroupLineIntersection->LineEdit2->setProperty( "GeomWidgetType", QVariant::fromValue(TopAbs_WIRE) );
        GroupLineIntersection->LineEdit2->setFocus();
        GroupLineIntersection->PushButton1->setDown(false);
    }
}

//=================================================================================
// function : updateSize
// purpose  : adjust dialog size to minimum
//=================================================================================
void MyBasicGUI_PointDlg::updateSize()
{
    qApp->processEvents();
    updateGeometry();
    resize(minimumSizeHint());
}

void MyBasicGUI_PointDlg::clear()
{
    mainFrame()->_vertex_le->clear();
    modelUnregister(mainFrame()->_vertex_le);

    GroupRefPoint->LineEdit1->clear();
    GroupRefPoint->LineEdit1->setProperty("TopoDS_Shape", QVariant());
    GroupRefPoint->LineEdit1->setProperty("QModelIndex", QVariant());
    GroupRefPoint->LineEdit1->setProperty("HexaData", QVariant());

    GroupOnCurve->LineEdit1->clear();
    GroupOnCurve->LineEdit1->setProperty("TopoDS_Shape", QVariant());
    GroupOnCurve->LineEdit1->setProperty("QModelIndex", QVariant());
    GroupOnCurve->LineEdit1->setProperty("HexaData", QVariant());

    GroupOnCurve->LineEdit2->clear();
    GroupOnCurve->LineEdit2->setProperty("TopoDS_Shape", QVariant());
    GroupOnCurve->LineEdit2->setProperty("QModelIndex", QVariant());
    GroupOnCurve->LineEdit2->setProperty("HexaData", QVariant());

    GroupLineIntersection->LineEdit1->clear();
    GroupLineIntersection->LineEdit1->setProperty("TopoDS_Shape", QVariant());
    GroupLineIntersection->LineEdit1->setProperty("QModelIndex", QVariant());
    GroupLineIntersection->LineEdit1->setProperty("HexaData", QVariant());

    GroupLineIntersection->LineEdit2->clear();
    GroupLineIntersection->LineEdit2->setProperty("TopoDS_Shape", QVariant());
    GroupLineIntersection->LineEdit2->setProperty("QModelIndex", QVariant());
    GroupLineIntersection->LineEdit2->setProperty("HexaData", QVariant());

    GroupOnSurface->LineEdit1->clear();
    GroupOnSurface->LineEdit1->setProperty("TopoDS_Shape", QVariant());
    GroupOnSurface->LineEdit1->setProperty("QModelIndex", QVariant());
    GroupOnSurface->LineEdit1->setProperty("HexaData", QVariant());

    modelUnregister(this);
}


void MyBasicGUI_PointDlg::onWindowActivated(SUIT_ViewManager* vm)
{
    QString vmType = vm->getType();
    if ( ((vmType == SVTK_Viewer::Type()) || (vmType == VTKViewer_Viewer::Type())) &&
            !mainFrame()->RadioButton4->isChecked() &&
            !myParamCoord->button(LENGTH_VALUE)->isChecked() )
        mainFrame()->_vertex_le->setFocus();
    else if ( vmType == OCCViewer_Viewer::Type() ){
        if (mainFrame()->RadioButton1->isChecked())
            // Make the field "Vertex of the model" lose the focus
            mainFrame()->RadioButton1->click();
        else if (mainFrame()->RadioButton2->isChecked())
            GroupRefPoint->LineEdit1->setFocus();
        else if (mainFrame()->RadioButton3->isChecked() &&
                !myParamCoord->button(LENGTH_VALUE)->isChecked())
            GroupOnCurve->LineEdit1->setFocus();
        else if (mainFrame()->RadioButton5->isChecked())
            GroupOnSurface->LineEdit1->setFocus();
    }
}
