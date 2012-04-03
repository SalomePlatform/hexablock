
//  Copyright (C) 2009-2011  CEA/DEN, EDF R&D
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
#include <iostream>
#include <QtGui>


#include "utilities.h"


#include "HEXABLOCKGUI_DocumentDelegate.hxx"
#include "HEXABLOCKGUI_DocumentItem.hxx"
#include "HEXABLOCKGUI_DocumentPanel.hxx"

using namespace std;
using namespace HEXABLOCK::GUI;


DocumentDelegate::DocumentDelegate(QDockWidget *dw, QObject *parent)
    : QItemDelegate(parent),
      _dw(dw),
      _documentModel(0),
      _patternDataSelectionModel(0),
      _patternBuilderSelectionModel(0),
      _groupsSelectionModel(0),
      _meshSelectionModel(0)
{
//   connect( this, SIGNAL( closeEditor(QWidget *, QAbstractItemDelegate::EndEditHint) ),
//            this, SLOT( onCloseEditor(QWidget *, QAbstractItemDelegate::EndEditHint) ) );
//   connect( this, SIGNAL( commitData ( QWidget * ) ),
//            this, SLOT( onCommitData ( QWidget * ) ) );
}


// void DocumentDelegate::commitEditor()
// {
//   MESSAGE("DocumentDelegate::commitEditor(){")
//   MESSAGE("*  sender is: " << sender() );
//   QWidget* editor = qobject_cast<QWidget*>(sender());
//   emit commitData(editor);
// //   emit closeEditor(editor);
//   MESSAGE("}");
// }

QWidget *DocumentDelegate::createEditor( QWidget                    *parent,
                                         const QStyleOptionViewItem &option,
                                         const QModelIndex          &index ) const
{
  MESSAGE("DocumentDelegate::createEditor(){");
  MESSAGE("*  parent is: " << parent);
  MESSAGE("*  _dw    is: " << _dw);
  MESSAGE("*  item   is: " << index.data(Qt::DisplayRole).toString().toStdString());

  HexaBaseDialog *editor = 0;

  switch ( index.data(HEXA_TREE_ROLE).toInt() ){
    case VERTEX_TREE :   editor = new VertexDialog(parent, HexaBaseDialog::UPDATE_MODE);   break;
    case EDGE_TREE :     editor = new EdgeDialog(parent, /*HexaBaseDialog::NEW_MODE*/HexaBaseDialog::INFO_MODE);     break;
    case QUAD_TREE :     editor = new QuadDialog(parent, HexaBaseDialog::INFO_MODE);     break;
    case HEXA_TREE :     editor = new HexaDialog(parent, HexaBaseDialog::INFO_MODE);     break;
    case VECTOR_TREE :   editor = new VectorDialog(parent, HexaBaseDialog::INFO_MODE);   break;
    case CYLINDER_TREE : editor = new CylinderDialog(parent, HexaBaseDialog::INFO_MODE);   break;
    case PIPE_TREE :     editor = new PipeDialog(parent, HexaBaseDialog::INFO_MODE);       break;
  //   case ELEMENTS_TREE :  break;
  //   case CROSSELEMENTS_TREE : break;
    case GROUP_TREE       : editor = new GroupDialog(parent, HexaBaseDialog::UPDATE_MODE); break;
    case LAW_TREE         : editor = new LawDialog(parent, HexaBaseDialog::UPDATE_MODE); break;
    case PROPAGATION_TREE : editor = new PropagationDialog(parent, HexaBaseDialog::INFO_MODE); break;
  }
  if ( editor ){
    if ( _documentModel )                editor->setDocumentModel( _documentModel );
    if ( _patternDataSelectionModel )    editor->setPatternDataSelectionModel( _patternDataSelectionModel );
    if ( _patternBuilderSelectionModel ) editor->setPatternBuilderSelectionModel( _patternBuilderSelectionModel);
    if ( _groupsSelectionModel )         editor->setGroupsSelectionModel( _groupsSelectionModel );
    if ( _meshSelectionModel )           editor->setMeshSelectionModel( _meshSelectionModel);

//     QWidget* w = _dw->widget();
//     if (w) w->close();
    _dw->setWidget( editor );
    _dw->setWindowTitle( editor->windowTitle() );
//         connect ( editor, SIGNAL( editingFinished() ), this, SLOT ( commitEditor() ) );
//         editor->exec();
//     editor->show();
//     editor->raise();
//     editor->setFocus();
  }

  MESSAGE("}");
  return editor;
//   return new QLineEdit(parent);
}


void DocumentDelegate::setEditorData( QWidget *editor,
                                      const QModelIndex &index) const
{
  MESSAGE("DocumentDelegate::setEditorData(){");
  MESSAGE("*  item   is: " << index.data(Qt::DisplayRole).toString().toStdString());

//   HexaBaseDialog* editor = dynamic_cast<HexaBaseDialog*>( editor );

  switch ( index.data(HEXA_TREE_ROLE).toInt() ){
    case VERTEX_TREE : {
      HEXA_NS::Vertex *value = index.data( HEXA_DATA_ROLE ).value< HEXA_NS::Vertex* >();
      VertexDialog *vertexEditor = static_cast<VertexDialog*>(editor);
      vertexEditor->setValue(value);
    }
    break;
    case EDGE_TREE : {
      HEXA_NS::Edge *value = index.data( HEXA_DATA_ROLE ).value< HEXA_NS::Edge* >();
      EdgeDialog *edgeEditor = static_cast<EdgeDialog*>(editor);
      edgeEditor->setValue(value);
    }
    break;
    case QUAD_TREE : {
      HEXA_NS::Quad *value = index.data( HEXA_DATA_ROLE ).value< HEXA_NS::Quad* >();
      QuadDialog *quadEditor = static_cast<QuadDialog*>(editor);
      quadEditor->setValue(value);
    }
    break;
    case HEXA_TREE : {
      HEXA_NS::Hexa *value = index.data( HEXA_DATA_ROLE ).value< HEXA_NS::Hexa* >();
      HexaDialog *hexaEditor = static_cast<HexaDialog*>(editor);
      hexaEditor->setValue(value);
    }
    break;
    case VECTOR_TREE : {
      HEXA_NS::Vector *value = index.data( HEXA_DATA_ROLE ).value< HEXA_NS::Vector* >();
      VectorDialog *vectorEditor = static_cast<VectorDialog*>(editor);
      vectorEditor->setValue(value);
    }
    break;
    case CYLINDER_TREE : {
      HEXA_NS::Cylinder *value = index.data( HEXA_DATA_ROLE ).value< HEXA_NS::Cylinder* >();
      CylinderDialog *cylinderEditor = static_cast<CylinderDialog*>(editor);
      cylinderEditor->setValue(value);
    }
    break;
    case PIPE_TREE : {
      HEXA_NS::Pipe *value = index.data( HEXA_DATA_ROLE ).value< HEXA_NS::Pipe* >();
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

//   editor->setFocus();
//  editor->exec();
  MESSAGE("}");
}

// void DocumentDelegate::setModelData( QWidget *editor, QAbstractItemModel *model,
//                                      const QModelIndex &index) const
// {
//     cout << "//////////////////// CS_BP DocumentDelegate::setModelData BEGIN "<< editor << endl;
// // //     QStandardItem  *item = 0;
// // //     QItemDelegate::setModelData( editor, model,index);
// // 
// // //     // CS_TODO : simplification?
// //     QSortFilterProxyModel *pmodel = dynamic_cast<QSortFilterProxyModel *>( model );
// //     if ( pmodel ){
// //       QStandardItemModel *smodel = dynamic_cast<QStandardItemModel *>( pmodel->sourceModel() );
// //       if ( smodel ){
// // //         item = smodel->itemFromIndex(index);
// //         QItemDelegate::setModelData( editor, smodel, pmodel->mapToSource(index));
// //       }
// //     } else {
// //       QStandardItemModel *smodel = dynamic_cast<QStandardItemModel *>( model );
// //       if ( smodel ){
// // //         item = smodel->itemFromIndex(index);
// //         QItemDelegate::setModelData( editor, model,index);
// //       }
// //     }
// //   QItemDelegate::setModelData( editor, model, index);
//   cout << "//////////////////// CS_BP DocumentDelegate::setModelData END"<< editor << endl;    
// 
// }


void DocumentDelegate::setModelData( QWidget *editor, 
                                     QAbstractItemModel *model,
                                     const QModelIndex &index) const
{
//   cout <<" XXXXXXXXXXXXXX setModelData editor => " << editor << endl;
  MESSAGE("DocumentDelegate::setModelData(){");
  MESSAGE("*  item   is: " << index.data(Qt::DisplayRole).toString().toStdString());
//   HexaDialog *hexaEditor = qobject_cast<HexaDialog*>(editor);
// //   PipeDialog *qtcast = qobject_cast<PipeDialog*>(editor);
// //   PipeDialog *cppcast = dynamic_cast<PipeDialog*>(editor);
// 
//   if  ( hexaEditor ){
//     HEXA_NS::Hexa *value  = hexaEditor->getValue();
//     cout << "XXXXXXXXXXX DocumentDelegate::setModelData value  "<< value  << endl;
//     model->setData( index, QVariant::fromValue( value ), HEXA_DATA_ROLE );
//     model->setData( index, value->getName(), Qt::DisplayRole/*Qt::EditRole*/ );
//     model->setData( index, QString::number( reinterpret_cast<intptr_t>(value) ), HEXA_ENTRY_ROLE ); 
//   }
//   editor->show();
//   editor->raise();
  
//   cout << "AAAAAAA _dw"<< _dw<< endl;
//   cout << "AAAAAAA _dw->widget()"<< _dw->widget() << endl;
//   cout << "AAAAAAA isVisible"<< _dw->isVisible() << endl;
//   cout << "AAAAAAA editor isVisible"<< editor->isVisible() << endl;
// //   editor = new HexaDialog();
//   _dw->setWindowTitle( "HOOOOOOOOOO" );
//   _dw->setWidget(editor);
//   _dw->setVisible(false);
// //   editor->raise();
//   _dw->raise();
//   _dw->setVisible(true);
//   cout << "BBBBBBB _dw"<< _dw<< endl;
//   cout << "BBBBBBB _dw->widget()"<< _dw->widget() << endl;
//   cout << "BBBBBBB isVisible"<< _dw->isVisible() << endl;
//   cout << "BBBBBBB editor isVisible"<< editor->isVisible() << endl;
  
//               HEXA_NS::Quad *value   = quadEditor->getValue();
//               model->setData(index, QVariant::fromValue( value ), HEXA_DATA_ROLE);
  MESSAGE("}");
}





void DocumentDelegate::updateEditorGeometry(  QWidget *editor,
                                              const QStyleOptionViewItem &option,
                                              const QModelIndex &index ) const
{

  MESSAGE("DocumentDelegate::updateEditorGeometry(){");
  MESSAGE("*  item   is: " << index.data(Qt::DisplayRole).toString().toStdString());
//   cout << "CS_BP option.rect.size()"<<option.rect.size()<<endl;
  MESSAGE("*  option.rect.width() is: " << option.rect.width() << option.rect.width() );
  MESSAGE("*  option.rect.x()     is: " << option.rect.x() << option.rect.x() );
  MESSAGE("*  option.rect.y()     is: " << option.rect.y() << option.rect.y() );
//   editor->setGeometry(option.rect);
  editor->show();
//   editor->raise();
  MESSAGE("}");
}





void DocumentDelegate::setDocumentModel( DocumentModel* m )
{
  _documentModel = m;
}

void DocumentDelegate::setPatternDataSelectionModel( PatternDataSelectionModel* s )
{
  _patternDataSelectionModel = s;
}

void DocumentDelegate::setPatternBuilderSelectionModel( PatternBuilderSelectionModel* s )
{
  _patternBuilderSelectionModel = s;
}

void DocumentDelegate::setGroupsSelectionModel( GroupsSelectionModel* s )
{
  _groupsSelectionModel = s ;
}

void DocumentDelegate::setMeshSelectionModel( MeshSelectionModel* s )
{
  _meshSelectionModel = s;
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


  
//   bool r;
//   if ( event->type() == 3 ){
//     r = true;
//   } else {
//     r = QItemDelegate::editorEvent ( event, model, option, index );
//   }
//   MESSAGE("*  result  is: " << r );
//   MESSAGE("}");
//   return r;

  return QItemDelegate::editorEvent ( event, model, option, index );
}

// void DocumentDelegate::onCloseEditor( QWidget *w, QAbstractItemDelegate::EndEditHint h)
// {
//   MESSAGE("DocumentDelegate::onCloseEditor(){");
//   MESSAGE("}");
// }
// 
// 
// void DocumentDelegate::onCommitData ( QWidget * editor )
// {
//   MESSAGE("DocumentDelegate::onCommitData(){");
//   MESSAGE("}");
// }


// connect( this, SIGNAL( commitData ( QWidget * ) ),
//          this, SLOT( onCommitData ( QWidget * ) ) );


// // virtual bool 	eventFilter ( QObject * editor, QEvent * event )
// // {
// // //   std::cout << "eventFilter  eventFilter " << event->type() << std::endl;
// // //   std::cout << std::endl;
// // 
// //  if (event->type() == QEvent::FocusOut) {
// // //              QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
// // //              qDebug() << "Ate key press" << keyEvent->key();
// // //              return true;
// // return false;
// //          } else {
// //               return QItemDelegate::eventFilter ( editor, event );
// // //              return false;
// //          }



// void DocumentDelegate::setModelData( QWidget *editor, QAbstractItemModel *model,
//                                      const QModelIndex &index) const
// {
//     cout << "*********************** CS_BP DocumentDelegate::setModelData BEGIN "<< editor << endl;
//     QStandardItem  *item = 0;
// 
//     // CS_TODO : simplification?
//     QSortFilterProxyModel *pmodel = dynamic_cast<QSortFilterProxyModel *>( model );
//     if ( pmodel ){
//       QStandardItemModel *smodel = dynamic_cast<QStandardItemModel *>( pmodel->sourceModel() );
//       if ( smodel ){
//         item = smodel->itemFromIndex(pmodel->mapToSource(index));
//       }
//     } else {
//       QStandardItemModel *smodel = dynamic_cast<QStandardItemModel *>( model );
//       if ( smodel ){
//         item = smodel->itemFromIndex(index);
//       }
//     }
// 
// 
//     if ( item ){
// //       cout << "item " << item->type() << endl;
//       if ( item->data().isValid() )
//         cout << "***************   item " << item->data(Qt::DisplayRole).toString().toStdString() << endl;
//       switch ( item->type() ){
//         case VERTEXITEM : {
//               VertexDialog *vertexEditor = static_cast<VertexDialog*>(editor);
//               HEXA_NS::Vertex *value     = vertexEditor->getValue();
//               model->setData(index, QVariant::fromValue( value ), HEXA_DATA_ROLE);
//             }
//             break;
// 
//         case EDGEITEM : {
//               EdgeDialog *edgeEditor = static_cast<EdgeDialog*>(editor);
//               HEXA_NS::Edge *value   = edgeEditor->getValue();
//               model->setData(index, QVariant::fromValue( value ), HEXA_DATA_ROLE);
//             }
//             break;
// 
//         case QUADITEM : {
//               QuadDialog *quadEditor = static_cast<QuadDialog*>(editor);
//               HEXA_NS::Quad *value   = quadEditor->getValue();
//               model->setData(index, QVariant::fromValue( value ), HEXA_DATA_ROLE);
//             }
//             break;
// 
//         case HEXAITEM : {
//               HexaDialog *hexaEditor = static_cast<HexaDialog*>(editor);
//               HEXA_NS::Hexa *value   = hexaEditor->getValue();
//               model->setData(index, QVariant::fromValue( value ), HEXA_DATA_ROLE);
//             }
//             break;
// 
//         case VECTORITEM : {
//               VectorDialog *vectorEditor = static_cast<VectorDialog*>(editor);
//               HEXA_NS::Vector *value   = vectorEditor->getValue();
//               model->setData(index, QVariant::fromValue( value ), HEXA_DATA_ROLE);
//             }
//             break;
// 
//         case CYLINDERITEM : {
//               CylinderDialog *cylinderEditor = static_cast<CylinderDialog*>(editor);
//               HEXA_NS::Cylinder *value   = cylinderEditor->getValue();
//               model->setData(index, QVariant::fromValue( value ), HEXA_DATA_ROLE);
//             }
//         break;
//         case PIPEITEM : {
//               PipeDialog *pipeEditor = static_cast<PipeDialog*>(editor);
//               HEXA_NS::Pipe *value   = pipeEditor->getValue();
//               model->setData(index, QVariant::fromValue( value ), HEXA_DATA_ROLE);
//             }
//         break;
// //         case ELEMENTSITEM : editor = new ElementsDialog(parent);   break;
// //         case CROSSELEMENTSITEM : editor = new CrossElementsDialog(parent);   break;
//         case GROUPITEM :{
//               GroupDialog *groupEditor = static_cast<GroupDialog*>(editor);
//               HEXA_NS::Group *value = groupEditor->getValue();
//               model->setData(index, QVariant::fromValue( value ), HEXA_DATA_ROLE);
//         }
//         break;
//         case LAWITEM : {
//               LawDialog *lawEditor = static_cast<LawDialog*>(editor);
//               HEXA_NS::Law *value = lawEditor->getValue();
//               model->setData(index, QVariant::fromValue( value ), HEXA_DATA_ROLE);
//         }
//         break;
//         case PROPAGATIONITEM : {
//               PropagationDialog *propagationEditor = static_cast<PropagationDialog*>(editor);
//               HEXA_NS::Propagation *value = propagationEditor->getValue();
//               model->setData(index, QVariant::fromValue( value ), HEXA_DATA_ROLE);
//         }
//         break;
// 
//       }
//     }
//   editor->show();
//   cout << "*********************** CS_BP DocumentDelegate::setModelData END "<< editor << endl;
// }