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

#ifndef MYGEOMBASE_SKELETON_H
#define MYGEOMBASE_SKELETON_H

#include "GEOM_GEOMBase.hxx"
#include "MyGEOMBase_Helper.hxx"

#include <QDialog>

class SalomeApp_DoubleSpinBox;
class GeometryGUI;
class MyDlgRef_Skeleton;
class QSpinBox;
class QDoubleSpinBox;
class QLineEdit;
class QButtonGroup;
class QPushButton;

#ifndef COORD_MIN
#  define COORD_MIN -1e+15
#  define COORD_MAX +1e+15
#  define MAX_NUMBER 100000
#  define DBL_DIGITS_DISPLAY 16
#endif // COORD_MIN

class GEOMBASE_EXPORT MyGEOMBase_Skeleton : public QDialog, public MyGEOMBase_Helper
{
  Q_OBJECT

public:
    MyGEOMBase_Skeleton( GeometryGUI*, QWidget* = 0, bool = false, Qt::WindowFlags = 0 );
    ~MyGEOMBase_Skeleton();

private:
    void Init();

protected:
    void                initSpinBox( QSpinBox*, int, int, int = 1 );
    void                initSpinBox( SalomeApp_DoubleSpinBox*, double, double, double = 0.1, const char* = "length_precision" );

    void                updateAttributes( GEOM::GEOM_Object_ptr, const QStringList& );

    void                closeEvent( QCloseEvent* );
    void                keyPressEvent( QKeyEvent* );

    /*! initialize "Name" field with a string "thePrefix_X" (Vertex_3)
     */
    void                initName( const QString& = QString() );

    /*! returns contents of "Name" field
     */
    virtual QString getNewObjectName() const;

    /*! returns id of a selected "constructor" radio button or '-1' in case of error
     */
    int                 getConstructorId() const;
    /*! set selected "constructor" radio button id
     */
    void                setConstructorId( const int );
    /*! unset selection on all "constructor" radio buttons
     */
    void                unsetConstructorId();

    void                showOnlyPreviewControl();

    void                setHelpFileName( const QString& );

    MyDlgRef_Skeleton*    mainFrame();
    QWidget*            centralWidget();
    QPushButton*        buttonCancel() const;
    //QPushButton*        buttonOk() const;
    QPushButton*        buttonApply() const;
    QPushButton*        buttonHelp() const;

protected:
    QLineEdit*          myEditCurrentArgument; //!< Current LineEdit
    GeometryGUI*        myGeomGUI;             //!< reference GEOM GUI
    QString             myHelpFileName;        //!< Associated HTML help file name

    QButtonGroup*       myRBGroup;             //!< radio button group
    MyDlgRef_Skeleton*    myMainFrame;           //!< dialog box's mainframe widgetx

protected slots:
    virtual void        ClickOnCancel();
    virtual void        processPreview();
    void                LineEditReturnPressed();
    void                DeactivateActiveDialog();
    void                ActivateThisDialog();
    void                ClickOnHelp();

signals:
    void                constructorsClicked( int );
};

#endif // MYGEOMBASE_SKELETON_H
