// Copyright (C) 2009-2024  CEA, EDF
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


#include "HEXABLOCKGUI_Resource.hxx"
#include "Resource.hxx"
#include "HEXABLOCKGUI.hxx"
#include <SUIT_ResourceMgr.h>




//#define _DEVDEBUG_
#include "HEXABLOCKGUI_Trace.hxx"

#define RESOURCE_HEXABLOCK "HEXABLOCK"

using namespace HEXABLOCK::GUI;


HEXABLOCKGUI_Resource::HEXABLOCKGUI_Resource(SUIT_ResourceMgr* r)
{
  DEBTRACE("HEXABLOCKGUI_Resource::HEXABLOCKGUI_Resource");
  _resource = r;
}

// Get resources
// -------------

int HEXABLOCKGUI_Resource::integerValue(const QString& name, const int def) const {
  return _resource->integerValue(RESOURCE_HEXABLOCK, name, def);
}

double HEXABLOCKGUI_Resource::doubleValue(const QString& name, const double def) const {
  return _resource->doubleValue(RESOURCE_HEXABLOCK, name, def);
}

bool HEXABLOCKGUI_Resource::booleanValue(const QString& name, const bool def) const {
  return _resource->booleanValue(RESOURCE_HEXABLOCK, name, def);
}

QFont HEXABLOCKGUI_Resource::fontValue(const QString& name, const QFont& def) const {
  return _resource->fontValue(RESOURCE_HEXABLOCK, name, def);
}

QColor HEXABLOCKGUI_Resource::colorValue(const QString& name, const QColor& def) const {
  return _resource->colorValue(RESOURCE_HEXABLOCK, name, def);
}

QColor HEXABLOCKGUI_Resource::colorValue(const QString& name, const Qt::GlobalColor c) const {
  QColor col(c);
  return colorValue(name, col);
}

QColor HEXABLOCKGUI_Resource::colorValue(const QString& name, const int h, const int s, const int v) const {
  QColor col;
  col.setHsv(h, s, v);
  return colorValue(name, col);
}

QString HEXABLOCKGUI_Resource::stringValue(const QString& name, const QString& def) const {
  return _resource->stringValue(RESOURCE_HEXABLOCK, name, def);
}

// Set resources
// -------------

void HEXABLOCKGUI_Resource::setValue( const QString& name, const int val ) {
    _resource->setValue(RESOURCE_HEXABLOCK, name, val);
}

void HEXABLOCKGUI_Resource::setValue( const QString& name, const double val ) {
    _resource->setValue(RESOURCE_HEXABLOCK, name, val);
}

void HEXABLOCKGUI_Resource::setValue( const QString& name, const bool val ) {
    _resource->setValue(RESOURCE_HEXABLOCK, name, val);
}

void HEXABLOCKGUI_Resource::setValue( const QString& name, const QFont& val ) {
    _resource->setValue(RESOURCE_HEXABLOCK, name, val);
}

void HEXABLOCKGUI_Resource::setValue( const QString& name, const QColor& val ) {
    _resource->setValue(RESOURCE_HEXABLOCK, name, val);
}

void HEXABLOCKGUI_Resource::setValue( const QString& name, const QString& val ) {
    _resource->setValue(RESOURCE_HEXABLOCK, name, val);
}

// Create preferences
// ------------------

void HEXABLOCKGUI_Resource::createPreferences(HEXABLOCKGUI* swm)
{
/*
  DEBTRACE("createPreferences");
  // --- General tab ---
  int genTab = swm->addPreference( QObject::tr( "PREF_TAB_GENERAL" ) );

  int componentGroup = swm->addPreference( QObject::tr( "PREF_GROUP_COMPONENT" ), genTab );

  swm->addPreference( QObject::tr( _COMPONENT_INSTANCE_NEW ), componentGroup, LightApp_Preferences::Bool, RESOURCE_HEXABLOCK, _COMPONENT_INSTANCE_NEW );
  swm->addPreference( "Python Script Font", componentGroup, LightApp_Preferences::Font, RESOURCE_HEXABLOCK, "font" );
  swm->addPreference( "User catalog", componentGroup, LightApp_Preferences::File, RESOURCE_HEXABLOCK, "userCatalog" );
  swm->addPreference( "Auto Compute Links", componentGroup, LightApp_Preferences::Bool, RESOURCE_HEXABLOCK, "autoComputeLinks" );
  swm->addPreference( "Simplify Links", componentGroup, LightApp_Preferences::Bool, RESOURCE_HEXABLOCK, "simplifyLink" );
  swm->addPreference( "Better Separation for Links", componentGroup, LightApp_Preferences::Bool, RESOURCE_HEXABLOCK, "addRowCols" );
  swm->addPreference( "Ensure Node Visible When Moved", componentGroup, LightApp_Preferences::Bool, RESOURCE_HEXABLOCK, "ensureVisibleWhenMoved" );
  swm->addPreference( "Tabified Panels Up", componentGroup, LightApp_Preferences::Bool, RESOURCE_HEXABLOCK, "tabPanelsUp" );

  // Link colors
  int linkTab = swm->addPreference( QObject::tr( "Link colors" ) );

  int idGroup = swm->addPreference( QObject::tr( "PREF_GROUP_GENERAL" ), linkTab );
  swm->setPreferenceProperty( idGroup, "columns", 1 );

  swm->addPreference( QObject::tr( "Link draw color" ),           idGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, "link_draw_color" );
  swm->addPreference( QObject::tr( "Link select color" ),         idGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, "link_select_color" );
  swm->addPreference( QObject::tr( "Stream link draw color" ),    idGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, "stream_link_draw_color" );
  swm->addPreference( QObject::tr( "Stream link select color" ),  idGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, "stream_link_select_color" );
  swm->addPreference( QObject::tr( "Control link draw color" ),   idGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, "control_link_draw_color" );
  swm->addPreference( QObject::tr( "Control link select color" ), idGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, "control_link_select_color" );
  swm->addPreference( QObject::tr( "Emphasis link color" ),       idGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, "emphasizeBrushColor" );

  swm->addPreference( QObject::tr( "link pen darkness" ),         idGroup, LightApp_Preferences::Integer, RESOURCE_HEXABLOCK, "link_pen_darkness" );

  // --- nodes without color states ---
  int nodeTab = swm->addPreference( QObject::tr( "PREF_TAB_NODE" ) );

  int nodeSubtab = swm->addPreference( QObject::tr( "PREF_GROUP_SCENE" ), nodeTab );
  swm->setPreferenceProperty(nodeSubtab , "columns", 2);

  swm->addPreference( QObject::tr( "Pen"        ), nodeSubtab, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _Scene_pen     );
  swm->addPreference( QObject::tr( "Brush"      ), nodeSubtab, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _Scene_brush   );
  swm->addPreference( QObject::tr( "High pen"   ), nodeSubtab, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _Scene_hiPen   );
  swm->addPreference( QObject::tr( "High brush" ), nodeSubtab, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _Scene_hiBrush );

  nodeSubtab = swm->addPreference( QObject::tr( "PREF_GROUP_BLOC" ), nodeTab );
  swm->setPreferenceProperty(nodeSubtab , "columns", 2);

  swm->addPreference( QObject::tr( "Pen"        ), nodeSubtab, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _ComposedNode_pen     );
  swm->addPreference( QObject::tr( "Brush"      ), nodeSubtab, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _ComposedNode_brush   );
  swm->addPreference( QObject::tr( "High pen"   ), nodeSubtab, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _ComposedNode_hiPen   );
  swm->addPreference( QObject::tr( "High brush" ), nodeSubtab, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _ComposedNode_hiBrush );

  nodeSubtab = swm->addPreference( QObject::tr( "PREF_GROUP_NODE" ), nodeTab );
  swm->setPreferenceProperty(nodeSubtab , "columns", 2);

  swm->addPreference( QObject::tr( "Pen"        ), nodeSubtab, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _ElementaryNode_pen     );
  swm->addPreference( QObject::tr( "Brush"      ), nodeSubtab, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _ElementaryNode_brush   );
  swm->addPreference( QObject::tr( "High pen"   ), nodeSubtab, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _ElementaryNode_hiPen   );
  swm->addPreference( QObject::tr( "High brush" ), nodeSubtab, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _ElementaryNode_hiBrush );

  nodeSubtab = swm->addPreference( QObject::tr( "PREF_GROUP_HEADER" ), nodeTab );
  swm->setPreferenceProperty(nodeSubtab , "columns", 2);

  swm->addPreference( QObject::tr( "Pen"        ), nodeSubtab, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _Header_pen     );
  swm->addPreference( QObject::tr( "Brush"      ), nodeSubtab, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _Header_brush   );
  swm->addPreference( QObject::tr( "High pen"   ), nodeSubtab, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _Header_hiPen   );
  swm->addPreference( QObject::tr( "High brush" ), nodeSubtab, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _Header_hiBrush );

  nodeSubtab = swm->addPreference( QObject::tr( "PREF_GROUP_CONTROL" ), nodeTab );
  swm->setPreferenceProperty(nodeSubtab , "columns", 2);

  swm->addPreference( QObject::tr( "Pen"        ), nodeSubtab, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _CtrlPort_pen     );
  swm->addPreference( QObject::tr( "Brush"      ), nodeSubtab, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _CtrlPort_brush   );
  swm->addPreference( QObject::tr( "High pen"   ), nodeSubtab, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _CtrlPort_hiPen   );
  swm->addPreference( QObject::tr( "High brush" ), nodeSubtab, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _CtrlPort_hiBrush );

  nodeSubtab = swm->addPreference( QObject::tr( "PREF_GROUP_PORT" ), nodeTab );
  swm->setPreferenceProperty(nodeSubtab , "columns", 2);

  swm->addPreference( QObject::tr( "Pen"        ), nodeSubtab, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _DataPort_pen     );
  swm->addPreference( QObject::tr( "Brush"      ), nodeSubtab, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _DataPort_brush   );
  swm->addPreference( QObject::tr( "High pen"   ), nodeSubtab, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _DataPort_hiPen   );
  swm->addPreference( QObject::tr( "High brush" ), nodeSubtab, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _DataPort_hiBrush );

  nodeSubtab = swm->addPreference( QObject::tr( "PREF_GROUP_DRAG" ), nodeTab );
  swm->setPreferenceProperty(nodeSubtab, "columns", 1);

  swm->addPreference( QObject::tr( "On dragging"), nodeSubtab, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, __dragOver );

  // --- Color of state of nodes ---
  int stateTab = swm->addPreference( QObject::tr( "PREF_TAB_STATE" ) );

  int editGroup = swm->addPreference( QObject::tr( "PREF_GROUP_EDIT" ), stateTab );
  swm->setPreferenceProperty( editGroup, "columns", 1 );

  swm->addPreference( QObject::tr( _editedNodeBrushColor ), editGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _editedNodeBrushColor );
  swm->addPreference( QObject::tr( _normalNodeBrushColor ), editGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _normalNodeBrushColor );
  swm->addPreference( QObject::tr( _runNodeBrushColor    ), editGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _runNodeBrushColor    );
  swm->addPreference( QObject::tr( _validNodeColor       ), editGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _validNodeColor       );
  swm->addPreference( QObject::tr( _invalidNodeColor     ), editGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _invalidNodeColor     );

  int runGroup = swm->addPreference( QObject::tr( "PREF_GROUP_RUN" ), stateTab );
  swm->setPreferenceProperty( runGroup, "columns", 2 );

  swm->addPreference( QObject::tr( _NOTYETINITIALIZED ), runGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _NOTYETINITIALIZED );
  swm->addPreference( QObject::tr( _INITIALISED       ), runGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _INITIALISED       );
  swm->addPreference( QObject::tr( _RUNNING           ), runGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _RUNNING           );
  swm->addPreference( QObject::tr( _WAITINGTASKS      ), runGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _WAITINGTASKS      );
  swm->addPreference( QObject::tr( _PAUSED            ), runGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _PAUSED            );
  swm->addPreference( QObject::tr( _FINISHED          ), runGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _FINISHED          );
  swm->addPreference( QObject::tr( _STOPPED           ), runGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _STOPPED           );
  swm->addPreference( QObject::tr( _UNKNOWN           ), runGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _UNKNOWN           );

  swm->addPreference( QObject::tr( _UNDEFINED         ), runGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _UNDEFINED         );
  swm->addPreference( QObject::tr( _INVALID           ), runGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _INVALID           );
  swm->addPreference( QObject::tr( _READY             ), runGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _READY             );
  swm->addPreference( QObject::tr( _TOLOAD            ), runGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _TOLOAD            );
  swm->addPreference( QObject::tr( _LOADED            ), runGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _LOADED            );
  swm->addPreference( QObject::tr( _TOACTIVATE        ), runGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _TOACTIVATE        );
  swm->addPreference( QObject::tr( _ACTIVATED         ), runGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _ACTIVATED         );
  swm->addPreference( QObject::tr( _DESACTIVATED      ), runGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _DESACTIVATED      );
  swm->addPreference( QObject::tr( _DONE              ), runGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _DONE              );
  swm->addPreference( QObject::tr( _SUSPENDED         ), runGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _SUSPENDED         );
  swm->addPreference( QObject::tr( _LOADFAILED        ), runGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _LOADFAILED        );
  swm->addPreference( QObject::tr( _EXECFAILED        ), runGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _EXECFAILED        );
  swm->addPreference( QObject::tr( _PAUSE             ), runGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _PAUSE             );
  swm->addPreference( QObject::tr( _INTERNALERR       ), runGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _INTERNALERR       );
  swm->addPreference( QObject::tr( _DISABLED          ), runGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _DISABLED          );
  swm->addPreference( QObject::tr( _FAILED            ), runGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _FAILED            );
  swm->addPreference( QObject::tr( _ERROR             ), runGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _ERROR             );
  swm->addPreference( QObject::tr( _DEFAULT           ), runGroup, LightApp_Preferences::Color, RESOURCE_HEXABLOCK, _DEFAULT           );
*/
}

void HEXABLOCKGUI_Resource::preferencesChanged()
{
  DEBTRACE("preferencesChanged");

  // Notice: it is here for updating the view, etc.

  // General resource
  Resource::COMPONENT_INSTANCE_NEW = booleanValue(_COMPONENT_INSTANCE_NEW, COMPONENTINSTANCENEW);
  Resource::pythonfont           = fontValue( "font" ,PYTHONFONT);
  Resource::userCatalog           = stringValue( "userCatalog" ,USERCATALOG);
  Resource::autoComputeLinks = booleanValue("autoComputeLinks", AUTOCOMPUTELINKS);
  Resource::simplifyLink = booleanValue("simplifyLink", SIMPLIFYLINK);
  Resource::addRowCols = booleanValue("addRowCols", ADDROWCOLS);
  Resource::ensureVisibleWhenMoved = booleanValue("ensureVisibleWhenMoved", ENSUREVISIBLEWHENMOVED);
  Resource::tabPanelsUp = booleanValue("tabPanelsUp", TABPANELSUP);

  // Color of state of nodes
  Resource::editedNodeBrushColor = colorValue(_editedNodeBrushColor, EDITEDNODEBRUSHCOLOR);
  Resource::normalNodeBrushColor = colorValue(_normalNodeBrushColor, NORMALNODEBRUSHCOLOR);
  Resource::runNodeBrushColor    = colorValue(_runNodeBrushColor   , RUNNODEBRUSHCOLOR);
  Resource::validNodeColor       = colorValue(_validNodeColor      , VALIDNODECOLOR);
  Resource::invalidNodeColor     = colorValue(_invalidNodeColor    , INVALIDNODECOLOR);

  Resource::NOTYETINITIALIZED    = colorValue(_NOTYETINITIALIZED   ,  NOTYETINITIALIZED_);
  Resource::INITIALISED          = colorValue(_INITIALISED         ,  INITIALISED_);
  Resource::RUNNING              = colorValue(_RUNNING             , RUNNING_     );
  Resource::WAITINGTASKS         = colorValue(_WAITINGTASKS        , WAITINGTASKS_);
  Resource::PAUSED               = colorValue(_PAUSED              , PAUSED_      );
  Resource::FINISHED             = colorValue(_FINISHED            , FINISHED_    );
  Resource::STOPPED              = colorValue(_STOPPED             , STOPPED_     );
  Resource::UNKNOWN              = colorValue(_UNKNOWN             , UNKNOWN_     );

  Resource::UNDEFINED            = colorValue(_UNDEFINED           , UNDEFINED_   );
  Resource::INVALID              = colorValue(_INVALID             , INVALID_     );
  Resource::READY                = colorValue(_READY               , READY_       );
  Resource::TOLOAD               = colorValue(_TOLOAD              , TOLOAD_      );
  Resource::LOADED               = colorValue(_LOADED              , LOADED_      );
  Resource::TOACTIVATE           = colorValue(_TOACTIVATE          , TOACTIVATE_  );
  Resource::ACTIVATED            = colorValue(_ACTIVATED           , ACTIVATED_   );
  Resource::DESACTIVATED         = colorValue(_DESACTIVATED        , DESACTIVATED_);
  Resource::DONE                 = colorValue(_DONE                , DONE_        );
  Resource::SUSPENDED            = colorValue(_SUSPENDED           , SUSPENDED_   );
  Resource::LOADFAILED           = colorValue(_LOADFAILED          , LOADFAILED_  );
  Resource::EXECFAILED           = colorValue(_EXECFAILED          , EXECFAILED_  );
  Resource::PAUSE                = colorValue(_PAUSE               , PAUSE_       );
  Resource::INTERNALERR          = colorValue(_INTERNALERR         , INTERNALERR_ );
  Resource::DISABLED             = colorValue(_DISABLED            , DISABLED_    );
  Resource::FAILED               = colorValue(_FAILED              , FAILED_      );
  Resource::ERROR                = colorValue(_ERROR               , ERROR_       );
  Resource::DEFAULT              = colorValue(_DEFAULT             , DEFAULT_     );

  // Color of links
  Resource::link_draw_color           = colorValue("link_draw_color"          , LINKDRAW_COLOR         );
  Resource::stream_link_draw_color    = colorValue("stream_link_draw_color"   , STREAMLINKDRAW_COLOR   );
  Resource::link_select_color         = colorValue("link_select_color"        , LINK_SELECT_COLOR      );
  Resource::stream_link_select_color  = colorValue("stream_link_select_color" , STREAMLINK_SELECT_COLOR);
  Resource::control_link_draw_color   = colorValue("control_link_draw_color"  , CTRLLINKDRAW_COLOR     );
  Resource::control_link_select_color = colorValue("control_link_select_color", CTRLLINK_SELECT_COLOR  );
  Resource::emphasizeBrushColor       = colorValue("emphasizeBrushColor"      , EMPHASIZEBRUSHCOLOR    );

  Resource::link_pen_darkness = integerValue("link_pen_darkness", LINK_PEN_DARKNESS  );

  // Color of nodes
  Resource::Scene_pen              = colorValue(_Scene_pen,               Scene_pen_             );
  Resource::Scene_hiPen            = colorValue(_Scene_hiPen,             Scene_hiPen_           );
  Resource::Scene_brush            = colorValue(_Scene_brush,             Scene_brush_           );
  Resource::Scene_hiBrush          = colorValue(_Scene_hiBrush,           Scene_hiBrush_         );
  Resource::ComposedNode_brush     = colorValue(_ComposedNode_brush,      ComposedNode_brush_    );
  Resource::ComposedNode_hiBrush   = colorValue(_ComposedNode_hiBrush,    ComposedNode_hiBrush_  );
  Resource::ComposedNode_pen       = colorValue(_ComposedNode_pen,        ComposedNode_pen_      );
  Resource::ComposedNode_hiPen     = colorValue(_ComposedNode_hiPen,      ComposedNode_hiPen_    );
  Resource::ElementaryNode_brush   = colorValue(_ElementaryNode_brush,    ElementaryNode_brush_  );
  Resource::ElementaryNode_hiBrush = colorValue(_ElementaryNode_hiBrush,  ElementaryNode_hiBrush_);
  Resource::ElementaryNode_pen     = colorValue(_ElementaryNode_pen,      ElementaryNode_pen_    );
  Resource::ElementaryNode_hiPen   = colorValue(_ElementaryNode_hiPen,    ElementaryNode_hiPen_  );
  Resource::Header_brush           = colorValue(_Header_brush,            Header_brush_          );
  Resource::Header_hiBrush         = colorValue(_Header_hiBrush,          Header_hiBrush_        );
  Resource::Header_pen             = colorValue(_Header_pen,              Header_pen_            );
  Resource::Header_hiPen           = colorValue(_Header_hiPen,            Header_hiPen_          );
  Resource::CtrlPort_brush         = colorValue(_CtrlPort_brush,          CtrlPort_brush_        );
  Resource::CtrlPort_hiBrush       = colorValue(_CtrlPort_hiBrush,        CtrlPort_hiBrush_      );
  Resource::CtrlPort_pen           = colorValue(_CtrlPort_pen,            CtrlPort_pen_          );
  Resource::CtrlPort_hiPen         = colorValue(_CtrlPort_hiPen,          CtrlPort_hiPen_        );
  Resource::DataPort_brush         = colorValue(_DataPort_brush,          DataPort_brush_        );
  Resource::DataPort_hiBrush       = colorValue(_DataPort_hiBrush,        DataPort_hiBrush_      );
  Resource::DataPort_pen           = colorValue(_DataPort_pen,            DataPort_pen_          );
  Resource::DataPort_hiPen         = colorValue(_DataPort_hiPen,          DataPort_hiPen_        );

  Resource::dragOver               = colorValue(__dragOver,               dragOver_              );
}

void HEXABLOCKGUI_Resource::preferencesChanged( const QString& sect, const QString& name ) 
{
  if( sect==RESOURCE_HEXABLOCK )
    {
      preferencesChanged();
    }
}

