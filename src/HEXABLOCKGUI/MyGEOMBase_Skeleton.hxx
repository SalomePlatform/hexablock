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

#ifndef MYGEOMBASE_SKELETON_H
#define MYGEOMBASE_SKELETON_H

#include "HEXABLOCKGUI_Export.hxx"

#include "HEXABLOCKGUI_OccGraphicView.hxx"
#include "HEXABLOCKGUI.hxx"
#include "HEXABLOCKGUI_DocumentPanel.hxx"

#include <QDialog>

class SalomeApp_DoubleSpinBox;
class MyDlgRef_Skeleton;
class QSpinBox;
class QDoubleSpinBox;
class QLineEdit;
class QButtonGroup;
class QPushButton;

#ifndef COORD_MIN
#  define COORD_MIN -1e+12
#  define COORD_MAX +1e+12
#  define MAX_NUMBER 100000
#  define DBL_DIGITS_DISPLAY 13
#endif // COORD_MIN

namespace HEXABLOCK
{
namespace GUI
{

class HEXABLOCK_EXPORT MyGEOMBase_Skeleton : public HexaBaseDialog
{
	Q_OBJECT

public:
	MyGEOMBase_Skeleton( QWidget* = 0, Qt::WindowFlags = 0 );
	~MyGEOMBase_Skeleton();

private:
	void Init();

protected:
	virtual bool        apply(QModelIndex& result) = 0;
	virtual void       _initInputWidget( Mode editmode ) = 0;
	void                initSpinBox( QSpinBox*, int, int, int = 1 );
	void                initSpinBox( SalomeApp_DoubleSpinBox*, double, double, double = 0.01,
                                    const char* = "length_precision", unsigned int decimals = 6 );

	void                closeEvent( QCloseEvent* );

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

	MyDlgRef_Skeleton*  mainFrame();
	QWidget*            centralWidget();

protected:
	QLineEdit*          myEditCurrentArgument; //!< Current LineEdit

	QButtonGroup*       myRBGroup;             //!< radio button group
	MyDlgRef_Skeleton*  myMainFrame;           //!< dialog box's mainframe widgetx

signals:
void                constructorsClicked( int );
};

}
}

#endif // MYGEOMBASE_SKELETON_H
