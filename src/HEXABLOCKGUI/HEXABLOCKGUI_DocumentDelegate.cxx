
// Copyright (C) 2009-2013  CEA/DEN, EDF R&D
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
      _currentEditor(NULL)
{
}


QWidget *DocumentDelegate::createEditor( QWidget                    *parent,
                                         const QStyleOptionViewItem &option,
                                         const QModelIndex          &index ) const
{
  //close current editor if there's one before creating a new one
  if (_currentEditor != NULL)
  {
	  delete _currentEditor;
	  _currentEditor = NULL;
  }

  HexaBaseDialog *editor = NULL;

  if (_dw->widget()) _dw->widget()->close();
  if ( !_dw->isVisible() ) _dw->setVisible(true);

  switch ( index.data(HEXA_TREE_ROLE).toInt() )
  {
    case VERTEX_TREE :   editor = new VertexDialog(_dw, HexaBaseDialog::INFO_MODE);   break;
    case EDGE_TREE :     editor = new EdgeDialog(_dw, HexaBaseDialog::INFO_MODE);     break;
    case QUAD_TREE :     editor = new QuadDialog(_dw, HexaBaseDialog::INFO_MODE);     break;
    case HEXA_TREE :     editor = new HexaDialog(_dw, HexaBaseDialog::INFO_MODE);     break;
    case VECTOR_TREE :   editor = new VectorDialog(_dw, HexaBaseDialog::INFO_MODE);   break;

//    case ELEMENTS_DIR_TREE : trouver le type exact (cylinder ou pipe) et creer la boite d'info en fonction.

    // * OBSOLETE ******
//    case CYLINDER_TREE : editor = new CylinderDialog(_dw, HexaBaseDialog::INFO_MODE);   break;
//    case PIPE_TREE :     editor = new PipeDialog(_dw, HexaBaseDialog::INFO_MODE);       break;
    // ******************

    case GROUP_TREE       : editor = new GroupDialog(_dw, HexaBaseDialog::INFO_MODE/*UPDATE_MODE*/); break;
    case LAW_TREE         : editor = new LawDialog(_dw, HexaBaseDialog::INFO_MODE); break;
    case PROPAGATION_TREE : editor = new PropagationDialog(_dw, HexaBaseDialog::INFO_MODE); break;
  }

  if ( editor != NULL )
  {
    HEXABLOCKGUI::assocInProgress = false;

    //show the editor in the dockwidget
    editor->resetSizeAndShow(_dw);
  }
  else
	  _dw->close();

  _currentEditor = editor;

  return editor;
}

//Close the current edition dialog
void DocumentDelegate::closeDialog()
{
	if (_currentEditor!=NULL)
	{
		_currentEditor->close();
		emit closeEditor(_currentEditor, NoHint); //Problem
		delete _currentEditor;
		_currentEditor = NULL;
	}
}

void DocumentDelegate::setEditorData( QWidget *editor,
                                      const QModelIndex &index) const
{
  HexaBaseDialog* hexaEditor = dynamic_cast<HexaBaseDialog*>( editor );
  if (hexaEditor == NULL) return;
  DocumentModel* documentModel = hexaEditor->getDocumentModel();
  if (documentModel == NULL) return;

  switch ( index.data(HEXA_TREE_ROLE).toInt() ){
    case VERTEX_TREE : {
      HEXA_NS::Vertex *value = documentModel->getHexaPtr<HEXA_NS::Vertex *>(index);
      VertexDialog *vertexEditor = static_cast<VertexDialog*>(editor);
      vertexEditor->setValue(value);
    }
    break;
    case EDGE_TREE : {
      HEXA_NS::Edge *value = documentModel->getHexaPtr<HEXA_NS::Edge*>(index);
      EdgeDialog *edgeEditor = static_cast<EdgeDialog*>(editor);
      edgeEditor->setValue(value);
    }
    break;
    case QUAD_TREE : {
      HEXA_NS::Quad *value = documentModel->getHexaPtr<HEXA_NS::Quad*>(index);
      QuadDialog *quadEditor = static_cast<QuadDialog*>(editor);
      quadEditor->setValue(value);
    }
    break;
    case HEXA_TREE : {
      HEXA_NS::Hexa *value = documentModel->getHexaPtr<HEXA_NS::Hexa*>(index);
      HexaDialog *hexaEditor = static_cast<HexaDialog*>(editor);
      hexaEditor->setValue(value);
    }
    break;
    case VECTOR_TREE : {
      HEXA_NS::Vector *value = documentModel->getHexaPtr<HEXA_NS::Vector*>(index);
      VectorDialog *vectorEditor = static_cast<VectorDialog*>(editor);
      vectorEditor->setValue(value);
    }
    break;

    /*
    case ELEMENTS_DIR_TREE : {
        HEXA_NS::Elements* value = documentModel->getHexaPtr<HEXA_NS::Elements*>(index);
        // trouver le type exact (cylinder ou pipe) pour choisir le bon editor
        // editor->setValue(value)
    }
    break;
    */

    // ************ OBSOLETE ********************
//    case CYLINDER_TREE : {
//      HEXA_NS::Cylinder *value = documentModel->getHexaPtr<HEXA_NS::Cylinder*>(index);
//      CylinderDialog *cylinderEditor = static_cast<CylinderDialog*>(editor);
//      cylinderEditor->setValue(value);
//    }
//    break;
//    case PIPE_TREE : {
//      HEXA_NS::Pipe *value = documentModel->getHexaPtr<HEXA_NS::Pipe*>(index);
//      PipeDialog *pipeEditor= static_cast<PipeDialog*>(editor);
//      pipeEditor->setValue(value);
//    }
//    break;
    // ************ FIN OBSOLETE *****************

    case GROUP_TREE : {
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
}


bool DocumentDelegate::editorEvent ( QEvent                     *event,
                                     QAbstractItemModel         *model,
                                     const QStyleOptionViewItem &option,
                                     const QModelIndex          &index )
{
	return QItemDelegate::editorEvent ( event, model, option, index );
}

bool DocumentDelegate::eventFilter ( QObject * editor, QEvent * event )
{
	if ( event->type() == QEvent::FocusOut ){
		return true; //do nothing for this signal
	}
	else if (event->type() == QEvent::HideToParent && editor != NULL)
	{
	    //close the current editor when the tree is reduced
	    ((QWidget*) editor->parent())->close();
	}

	return false;
}
