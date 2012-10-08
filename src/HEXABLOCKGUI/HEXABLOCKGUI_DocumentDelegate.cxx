
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
#include <iostream>
#include <QtGui>
#include <qpixmap.h>
#include <qrect.h>
#include <qstyle.h>


#include "utilities.h"
#include "HEXABLOCKGUI.hxx"


#include "HEXABLOCKGUI_DocumentDelegate.hxx"

using namespace std;
using namespace HEXABLOCK::GUI;


//QWidget* currentEditor = NULL;

DocumentDelegate::DocumentDelegate(QDockWidget *dw, QObject *parent)
    : QItemDelegate(parent),
      _dw(dw),
      _documentModel(0),
      _patternDataSelectionModel(0),
      _patternBuilderSelectionModel(0),
      _groupsSelectionModel(0),
      _meshSelectionModel(0),
      _currentEditor(NULL)
{
//   connect( this, SIGNAL( closeEditor(QWidget *, QAbstractItemDelegate::EndEditHint) ),
//            this, SLOT( onCloseEditor(QWidget *, QAbstractItemDelegate::EndEditHint) ) );
//   connect( this, SIGNAL( commitData ( QWidget * ) ),
//            this, SLOT( onCommitData ( QWidget * ) ) );
}


QWidget *DocumentDelegate::createEditor( QWidget                    *parent,
                                         const QStyleOptionViewItem &option,
                                         const QModelIndex          &index ) const
{
  MESSAGE("DocumentDelegate::createEditor(){");
  MESSAGE("*  parent is: " << parent);
  MESSAGE("*  _dw    is: " << _dw);
  MESSAGE("*  item   is: " << index.data(Qt::DisplayRole).toString().toStdString());

  //close current editor if there's one before creating a new one
  if (_currentEditor != NULL)
  {
	  delete _currentEditor;
	  _currentEditor = NULL;
  }

  HexaBaseDialog *editor = NULL;

  if (_dw->widget()) _dw->widget()->close();
  if ( !_dw->isVisible() ) _dw->setVisible(true);

  switch ( index.data(HEXA_TREE_ROLE).toInt() ){
    case VERTEX_TREE :   editor = new VertexDialog(_dw, HexaBaseDialog::UPDATE_MODE);   break;
    case EDGE_TREE :     editor = new EdgeDialog(_dw, /*HexaBaseDialog::NEW_MODE*/HexaBaseDialog::INFO_MODE);     break;
    case QUAD_TREE :     editor = new QuadDialog(_dw, HexaBaseDialog::INFO_MODE);     break;
    case HEXA_TREE :     editor = new HexaDialog(_dw, HexaBaseDialog::INFO_MODE);     break;
    case VECTOR_TREE :   editor = new VectorDialog(_dw, HexaBaseDialog::INFO_MODE);   break;
    case CYLINDER_TREE : editor = new CylinderDialog(_dw, HexaBaseDialog::INFO_MODE);   break;
    case PIPE_TREE :     editor = new PipeDialog(_dw, HexaBaseDialog::INFO_MODE);       break;
  //   case ELEMENTS_TREE :  break;
  //   case CROSSELEMENTS_TREE : break;
    case GROUP_TREE       : editor = new GroupDialog(_dw, HexaBaseDialog::INFO_MODE/*UPDATE_MODE*/); break;
    case LAW_TREE         : editor = new LawDialog(_dw, HexaBaseDialog::UPDATE_MODE); break;
    case PROPAGATION_TREE : editor = new PropagationDialog(_dw, HexaBaseDialog::INFO_MODE); break;
  }
  if ( editor != NULL ){
    if ( _documentModel )                editor->setDocumentModel( _documentModel );
    if ( _patternDataSelectionModel )
    {
    	editor->setPatternDataSelectionModel( _patternDataSelectionModel );
//    	_patternDataSelectionModel->currentDialog = editor;
    }
    if ( _patternBuilderSelectionModel ) editor->setPatternBuilderSelectionModel( _patternBuilderSelectionModel);
    if ( _groupsSelectionModel )         editor->setGroupsSelectionModel( _groupsSelectionModel );
    if ( _meshSelectionModel )           editor->setMeshSelectionModel( _meshSelectionModel);

    HEXABLOCKGUI::assocInProgress = false;
    //show the editor in the dockwidget
    editor->resetSizeAndShow(_dw);
  }
  else
	  _dw->close();

  MESSAGE("}");

  _currentEditor = editor;
  return editor;
}

//Close the current edition dialog
void DocumentDelegate::closeDialog()
{
	if (_currentEditor!=NULL)
	{
		_currentEditor->close();
		emit closeEditor(_currentEditor, NoHint);
		delete _currentEditor;
		_currentEditor = NULL;
	}
}

void DocumentDelegate::setEditorData( QWidget *editor,
                                      const QModelIndex &index) const
{
  MESSAGE("DocumentDelegate::setEditorData(){");
  MESSAGE("*  item   is: " << index.data(Qt::DisplayRole).toString().toStdString());

//   HexaBaseDialog* editor = dynamic_cast<HexaBaseDialog*>( editor );

  switch ( index.data(HEXA_TREE_ROLE).toInt() ){
    case VERTEX_TREE : {
//      HEXA_NS::Vertex *value = index.data( HEXA_DATA_ROLE ).value< HEXA_NS::Vertex* >();
      HEXA_NS::Vertex *value = _documentModel->getHexaPtr<HEXA_NS::Vertex *>(index);
      VertexDialog *vertexEditor = static_cast<VertexDialog*>(editor);
      vertexEditor->setValue(value);
    }
    break;
    case EDGE_TREE : {
      HEXA_NS::Edge *value = _documentModel->getHexaPtr<HEXA_NS::Edge*>(index);
      EdgeDialog *edgeEditor = static_cast<EdgeDialog*>(editor);
      edgeEditor->setValue(value);
    }
    break;
    case QUAD_TREE : {
      HEXA_NS::Quad *value = _documentModel->getHexaPtr<HEXA_NS::Quad*>(index);
      QuadDialog *quadEditor = static_cast<QuadDialog*>(editor);
      quadEditor->setValue(value);
    }
    break;
    case HEXA_TREE : {
      HEXA_NS::Hexa *value = _documentModel->getHexaPtr<HEXA_NS::Hexa*>(index);
      HexaDialog *hexaEditor = static_cast<HexaDialog*>(editor);
      hexaEditor->setValue(value);
    }
    break;
    case VECTOR_TREE : {
      HEXA_NS::Vector *value = _documentModel->getHexaPtr<HEXA_NS::Vector*>(index);
      VectorDialog *vectorEditor = static_cast<VectorDialog*>(editor);
      vectorEditor->setValue(value);
    }
    break;
    case CYLINDER_TREE : {
      HEXA_NS::Cylinder *value = _documentModel->getHexaPtr<HEXA_NS::Cylinder*>(index);
      CylinderDialog *cylinderEditor = static_cast<CylinderDialog*>(editor);
      cylinderEditor->setValue(value);
    }
    break;
    case PIPE_TREE : {
      HEXA_NS::Pipe *value = _documentModel->getHexaPtr<HEXA_NS::Pipe*>(index);
      PipeDialog *pipeEditor= static_cast<PipeDialog*>(editor);
      pipeEditor->setValue(value);
    }
    break;
//         case ELEMENTSITEM : editor = new ElementsDialog(parent);   break;
//         case CROSSELEMENTSITEM : editor = new CrossElementsDialog(parent);   break;
    case GROUP_TREE :{
      HEXA_NS::Group *value = index.data( HEXA_DATA_ROLE ).value< HEXA_NS::Group* >();
      GroupDialog *groupEditor = static_cast<GroupDialog*>(editor);
      groupEditor->setValue(value);
    }
    break;
    case LAW_TREE : {
      HEXA_NS::Law *value = index.data( HEXA_DATA_ROLE ).value< HEXA_NS::Law* >();
      LawDialog *lawEditor = static_cast<LawDialog*>(editor);
      lawEditor->setValue(value);
    }
    break;
    case PROPAGATION_TREE : {
      HEXA_NS::Propagation *value = index.data( HEXA_DATA_ROLE ).value< HEXA_NS::Propagation* >();
      PropagationDialog *propagationEditor = static_cast<PropagationDialog*>(editor);
      propagationEditor->setValue(value);
    }
    break;
  }

  MESSAGE("}");
}


bool DocumentDelegate::editorEvent ( QEvent                     *event,
                                     QAbstractItemModel         *model,
                                     const QStyleOptionViewItem &option,
                                     const QModelIndex          &index )
{
	MESSAGE("DocumentDelegate::editorEvent(){");
	MESSAGE("*  item   is: " << index.data().toString().toStdString());
	MESSAGE("*  event  is: " << event->type() );

	Qt::ItemFlags flags = model->flags(index);
	if ( flags == Qt::ItemFlags( ~Qt::ItemIsEditable ) ){
		MESSAGE("*  you can select it ");
	} else {
		MESSAGE("*  you cannot select it ");
	}

	//return QItemDelegate::editorEvent ( event, model, option, index );
	return false;
}

bool DocumentDelegate::eventFilter ( QObject * editor, QEvent * event )
{
	if ( event->type() == QEvent::FocusOut ){
		//((QWidget*) editor->parent())->close();
		return true;
	}
	return false;
}
