//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include "HEXABLOCKGUI.h"

#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Desktop.h>
#include <SalomeApp_Application.h>

#include <SALOME_LifeCycleCORBA.hxx>

// QT Includes
#include <QInputDialog>
#include <QIcon>

using namespace std;

// Constructor
HEXABLOCKGUI::HEXABLOCKGUI() :
  SalomeApp_Module( "HEXABLOCK" ), // default name
  LightApp_Module( "HEXABLOCK" )
{
}

// Gets an reference to the module's engine
HEXABLOCK_ORB::HEXABLOCK_Gen_ptr HEXABLOCKGUI::InitHEXABLOCKGen( SalomeApp_Application* app )
{
  Engines::Component_var comp = app->lcc()->FindOrLoad_Component( "FactoryServer","HEXABLOCK" );
  HEXABLOCK_ORB::HEXABLOCK_Gen_ptr clr = HEXABLOCK_ORB::HEXABLOCK_Gen::_narrow(comp);
  ASSERT(!CORBA::is_nil(clr));
  return clr;
}

// Module's initialization
void HEXABLOCKGUI::initialize( CAM_Application* app )
{

  SalomeApp_Module::initialize( app );

  InitHEXABLOCKGen( dynamic_cast<SalomeApp_Application*>( app ) );

  QWidget* aParent = application()->desktop();
  SUIT_ResourceMgr* aResourceMgr = app->resourceMgr();

  // create actions
  createAction( 190, tr( "TLT_MY_NEW_ITEM" ), QIcon(), tr( "MEN_MY_NEW_ITEM" ), tr( "STS_MY_NEW_ITEM" ), 0, aParent, false,
		this, SLOT( OnMyNewItem() ) );
  QPixmap aPixmap = aResourceMgr->loadPixmap( "HEXABLOCK",tr( "ICON_GET_BANNER" ) );
  createAction( 901, tr( "TLT_GET_BANNER" ), QIcon( aPixmap ), tr( "MEN_GET_BANNER" ), tr( "STS_GET_BANNER" ), 0, aParent, false,
		this, SLOT( OnGetBanner() ) );

  // create menus
  int aMenuId;
  aMenuId = createMenu( tr( "MEN_FILE" ), -1, -1 );
  createMenu( separator(), aMenuId, -1, 10 );
  aMenuId = createMenu( tr( "MEN_FILE_HEXABLOCK" ), aMenuId, -1, 10 );
  createMenu( 190, aMenuId );

  aMenuId = createMenu( tr( "MEN_HEXABLOCK" ), -1, -1, 30 );
  createMenu( 901, aMenuId, 10 );

  // create toolbars
  int aToolId = createTool ( tr( "TOOL_HEXABLOCK" ) );
  createTool( 901, aToolId );
}

// Module's engine IOR
QString HEXABLOCKGUI::engineIOR() const
{
  CORBA::String_var anIOR = getApp()->orb()->object_to_string( InitHEXABLOCKGen( getApp() ) );
  return QString( anIOR.in() );
}

// Module's activation
bool HEXABLOCKGUI::activateModule( SUIT_Study* theStudy )
{
  bool bOk = SalomeApp_Module::activateModule( theStudy );

  setMenuShown( true );
  setToolShown( true );

  return bOk;
}

// Module's deactivation
bool HEXABLOCKGUI::deactivateModule( SUIT_Study* theStudy )
{
  setMenuShown( false );
  setToolShown( false );

  return SalomeApp_Module::deactivateModule( theStudy );
}

// Default windows
void HEXABLOCKGUI::windows( QMap<int, int>& theMap ) const
{
  theMap.clear();
  theMap.insert( SalomeApp_Application::WT_ObjectBrowser, Qt::LeftDockWidgetArea );
  theMap.insert( SalomeApp_Application::WT_PyConsole,     Qt::BottomDockWidgetArea );
}

// Action slot
void HEXABLOCKGUI::OnMyNewItem()
{
  SUIT_MessageBox::warning( getApp()->desktop(),tr( "INF_HEXABLOCK_BANNER" ), tr( "INF_HEXABLOCK_MENU" ) );
}

// Action slot
void HEXABLOCKGUI::OnGetBanner()
{
  // Dialog to get the Name
  bool ok = FALSE;
  QString myName = QInputDialog::getText( getApp()->desktop(), tr( "QUE_HEXABLOCK_LABEL" ), tr( "QUE_HEXABLOCK_NAME" ),
					  QLineEdit::Normal, QString::null, &ok );

  if ( ok && !myName.isEmpty()) // if we got a name, get a HEXABLOCK component and ask for makeBanner
  {
//     HEXABLOCK_ORB::HEXABLOCK_Gen_ptr hexablockgen = HEXABLOCKGUI::InitHEXABLOCKGen( getApp() );
//     QString banner = hexablockgen->makeBanner( (const char*)myName.toLatin1() );
//     SUIT_MessageBox::information( getApp()->desktop(), tr( "INF_HEXABLOCK_BANNER" ), banner, tr( "BUT_OK" ) );
  }
}

// Export the module
extern "C" {
  CAM_Module* createModule()
  {
    return new HEXABLOCKGUI();
  }
}
