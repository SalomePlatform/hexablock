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


#include "MyGEOMBase_Skeleton.hxx"
#include "HEXABLOCKGUI_VtkDocumentGraphicView.hxx"

#include "MyDlgRef.hxx"

#include <SalomeApp_Application.h>
#include <SalomeApp_DoubleSpinBox.h>
#include <SalomeApp_Study.h>
#include <LightApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <SUIT_Desktop.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SUIT_MessageBox.h>

#include <QKeyEvent>
#include <QSpinBox>
#include <QDoubleSpinBox>

using namespace HEXABLOCK::GUI;

//=================================================================================
// class    : MyGEOMBase_Skeleton()
// purpose  : Constructs a MyGEOMBase_Skeleton which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            true to construct a modal dialog.
//=================================================================================
MyGEOMBase_Skeleton::MyGEOMBase_Skeleton( QWidget* parent, Qt::WindowFlags fl )
  : HexaBaseDialog(parent, NEW_MODE, fl),
    myRBGroup( 0 )
{

  myMainFrame = new MyDlgRef_Skeleton( this );
  QVBoxLayout* topLayout = new QVBoxLayout( this );
  topLayout->setMargin( 0 ); topLayout->setSpacing( 0 );
  topLayout->addWidget( myMainFrame );

  myMainFrame->GroupBoxPublish->setTitle( tr( "GEOM_PUBLISH_RESULT_GRP" ) );
  myMainFrame->CheckBoxRestoreSS->setText( tr( "GEOM_RESTORE_SUB_SHAPES" ) );
  myMainFrame->CheckBoxAddPrefix->setText( tr( "GEOM_RSS_ADD_FREFIX" ) );
  myMainFrame->CheckBoxPreview->setText( tr("GEOM_PREVIEW") );

  myRBGroup = new QButtonGroup( this );
  myRBGroup->addButton( myMainFrame->RadioButton1, 0 );
  myRBGroup->addButton( myMainFrame->RadioButton2, 1 );
  myRBGroup->addButton( myMainFrame->RadioButton3, 2 );
  myRBGroup->addButton( myMainFrame->RadioButton4, 3 );
  myRBGroup->addButton( myMainFrame->RadioButton5, 4 );

  connect( myRBGroup, SIGNAL( buttonClicked( int ) ), this, SIGNAL( constructorsClicked( int ) ) );
  Init();
}

//=================================================================================
// function : ~MyGEOMBase_Skeleton()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
MyGEOMBase_Skeleton::~MyGEOMBase_Skeleton()
{
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void MyGEOMBase_Skeleton::Init()
{
  /* displays Dialog */
  myMainFrame->RadioButton1->setChecked( true );
  myMainFrame->RadioButton4->hide();
  myMainFrame->RadioButton5->hide();
  myMainFrame->GroupBoxPublish->hide();
}

//=================================================================================
// function : initSpinBox()
// purpose  :
//=================================================================================
void MyGEOMBase_Skeleton::initSpinBox( QSpinBox* spinBox,
                                     int min,  int max, int step )
{
  spinBox->setRange( min, max );
  spinBox->setSingleStep( step );
}

//=================================================================================
// function : initSpinBox()
// purpose  :
//=================================================================================
void MyGEOMBase_Skeleton::initSpinBox( SalomeApp_DoubleSpinBox* spinBox,
                                     double min,  double max,
                                     double step, const char* quantity, unsigned int decimals)
{
  spinBox->setPrecision( decimals );
  spinBox->setDecimals( decimals );
  spinBox->setRange( min, max );
  spinBox->setSingleStep( step );
}

//=================================================================================
// function : closeEvent()
// purpose  : same than click on cancel button
//=================================================================================
void MyGEOMBase_Skeleton::closeEvent( QCloseEvent* e )
{
    TopoDS_Shape shape;
    HEXABLOCKGUI::currentOccGView->displayPreview(shape);

    QDialog::closeEvent( e );
}

//=================================================================================
// function : getConstructorId()
// purpose  :
//=================================================================================
int MyGEOMBase_Skeleton::getConstructorId() const
{
  if ( myRBGroup )
    return myRBGroup->checkedId();
  return -1;
}

//=================================================================================
// function : setConstructorId( id )
// purpose  :
//=================================================================================
void MyGEOMBase_Skeleton::setConstructorId( const int id )
{
  if ( myRBGroup && myRBGroup->button( id ) )
    myRBGroup->button( id )->setChecked( true );
}

//=================================================================================
// function : unsetConstructorId
// purpose  :
//=================================================================================
void MyGEOMBase_Skeleton::unsetConstructorId()
{
  // 0020428: EDF 906 GEOM : Performance for Group creation in GEOM
  // uncheck all buttons
  // workaround, because setChecked( false ) does not result in Qt4
  bool isExclusive = myRBGroup->exclusive();
  myRBGroup->setExclusive( false );
  QList<QAbstractButton*> btnList = myRBGroup->buttons();
  for ( int j = 0; j < 2; j++ )
  {
    QList<QAbstractButton*>::const_iterator it = btnList.constBegin(), itEnd = btnList.constEnd();
    for ( ; it != itEnd; ++it )
      (*it)->setCheckable( j == 1 );
  }
  myRBGroup->setExclusive( isExclusive );
}

MyDlgRef_Skeleton* MyGEOMBase_Skeleton::mainFrame()
{
  return myMainFrame;
}

QWidget* MyGEOMBase_Skeleton::centralWidget()
{
  return myMainFrame->GroupMedium;
}

//=================================================================================
// function : showOnlyPreviewControl()
// purpose  : display only CheckBoxPreview check box,
//            hide CheckBoxRestoreSS and CheckBoxAddPrefix
//=================================================================================
void MyGEOMBase_Skeleton::showOnlyPreviewControl(){
  mainFrame()->GroupBoxPublish->show();
  mainFrame()->CheckBoxRestoreSS->hide();
  mainFrame()->CheckBoxAddPrefix->hide();
}

