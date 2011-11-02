
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
// void QAbstractItemDelegate::closeEditor ( QWidget * editor, QAbstractItemDelegate::EndEditHint hint = NoHint )

connect( this, SIGNAL( closeEditor ( QWidget *, QAbstractItemDelegate::EndEditHint ) ),
         this, SLOT( onCloseEditor( QWidget *, QAbstractItemDelegate::EndEditHint ) ) );
// 
// connect( this, SIGNAL( commitData ( QWidget * ) ),
//          this, SLOT( onCommitData ( QWidget * ) ) );

//     emit commitData ( dw );

}




void DocumentDelegate::commitEditor()
{
  QWidget* editor = qobject_cast<QWidget*>(sender());
  cout << "*********************** CS_BP DocumentDelegate::commitEditor BEGIN"<< editor << endl;
  cout << "COMMITEDITOR COMMITEDITOR COMMITEDITOR COMMITEDITOR COMMITEDITOR  COMMITEDITOR  COMMITEDITOR  "<< editor << endl;
  emit commitData(editor);
//   emit closeEditor(editor);
}

QWidget *DocumentDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    cout << "*********************** CS_BP DocumentDelegate::createEditor BEGIN"<<endl;
//     cout << "*********************** parent"<< parent << endl;
//     cout << "*********************** _dw "  << _dw    << endl;
//     QWidget *editor = 0;
    HexaBaseDialog *editor = 0;
    QStandardItem  *item = 0;
    // CS_TODO : simplification?
    const QSortFilterProxyModel *pmodel = dynamic_cast<const QSortFilterProxyModel *>( index.model() );
    if ( pmodel ){
      const QStandardItemModel *model = dynamic_cast<const QStandardItemModel *>( pmodel->sourceModel() );
      if ( model ){
        item = model->itemFromIndex(pmodel->mapToSource(index));
      }
    } else {
      const QStandardItemModel *model = dynamic_cast<const QStandardItemModel *>( index.model() );
      if ( model ){
        item = model->itemFromIndex(index);
      }
    }

    if ( item ){
//       cout << "*********************** item "<< item->type() << endl;
      if ( item->data().isValid() )
        cout << "***************   item " << item->data(Qt::DisplayRole).toString().toStdString() << endl;
      switch ( item->type() ){
        case VERTEXITEM : editor = new VertexDialog(parent);   break;
        case EDGEITEM :   editor = new EdgeDialog(parent);     break;
        case QUADITEM :   editor = new QuadDialog(parent);     break;
        case HEXAITEM :   editor = new HexaDialog(parent);     break;
        case VECTORITEM : editor = new VectorDialog(parent);   break;
        case CYLINDERITEM : editor = new CylinderDialog(parent);   break;
        case PIPEITEM :     editor = new PipeDialog(parent);       break;
//         case ELEMENTSITEM : editor = new ElementsDialog(parent);   break;
//         case CROSSELEMENTSITEM : editor = new CrossElementsDialog(parent);   break;
        case GROUPITEM :          editor = new GroupDialog(parent);   break;
        case LAWITEM :            editor = new LawDialog(parent);     break;
        case PROPAGATIONITEM :    editor = new PropagationDialog(parent); break;
      }
      if ( editor ){
//         cout << "*********************** editor "<<endl;
        if ( _documentModel ) editor->setDocumentModel( _documentModel );
        if ( _patternDataSelectionModel ) editor->setPatternDataSelectionModel( _patternDataSelectionModel );
        if ( _patternBuilderSelectionModel ) editor->setPatternBuilderSelectionModel( _patternBuilderSelectionModel);
        if ( _meshSelectionModel ) editor->setMeshSelectionModel( _meshSelectionModel);
//         QItemSelectionModel*          _groupsSelectionModel;
        _dw->setWidget(editor);

        cout << "############## TITLE => " << editor->windowTitle().toStdString() << endl;
        _dw->setWindowTitle( tr("INPUT PANEL : %1").arg(editor->windowTitle()) );
//         connect ( editor, SIGNAL( editingFinished() ), this, SLOT ( commitEditor() ) );
//         editor->exec();
      }
    }

//     editor->exec();
//     editor->show();
//     cout << "*********************** CS_BP DocumentDelegate::createEditor END "<<editor<< endl;
    return editor;
//     return new QLineEdit( parent );
}



void DocumentDelegate::setEditorData( QWidget *editor,
                                      const QModelIndex &index) const
{
  cout << "*********************** CS_BP DocumentDelegate::setEditorData BEGIN "<< editor << endl;
    QStandardItem  *item = 0;
    // CS_TODO : simplification?
    const QSortFilterProxyModel *pmodel = dynamic_cast<const QSortFilterProxyModel *>( index.model() );
    if ( pmodel ){
      const QStandardItemModel *model = dynamic_cast<const QStandardItemModel *>( pmodel->sourceModel() );
      if ( model ){
        cout << "JJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJ"<< endl;
        item = model->itemFromIndex(pmodel->mapToSource(index));
      }
    } else {
      const QStandardItemModel *model = dynamic_cast<const QStandardItemModel *>( index.model() );
      if ( model ){
        item = model->itemFromIndex(index);
      }
    }

    if ( item ){
//       cout << "item " << item->type() << endl;
      if ( item->data().isValid() )
        cout << "***************   item " << item->data(Qt::DisplayRole).toString().toStdString() << endl;
      switch ( item->type() ){
        case VERTEXITEM : {
//               HEXA_NS::Vertex *value = index.model()->data(index, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
              HEXA_NS::Vertex *value = item->data( HEXA_DATA_ROLE ).value< HEXA_NS::Vertex* >();
              VertexDialog *vertexEditor = static_cast<VertexDialog*>(editor);
              vertexEditor->setValue(value);
//               vertexEditor->setIndex(index);
//               vertexEditor->exec();
        }
        break;
        case EDGEITEM : {
              HEXA_NS::Edge *value = item->data( HEXA_DATA_ROLE ).value< HEXA_NS::Edge* >();
              EdgeDialog *edgeEditor = static_cast<EdgeDialog*>(editor);
              edgeEditor->setValue(value);
        }
        break;
        case QUADITEM : {
              HEXA_NS::Quad *value = item->data( HEXA_DATA_ROLE ).value< HEXA_NS::Quad* >();
              QuadDialog *quadEditor = static_cast<QuadDialog*>(editor);
              quadEditor->setValue(value);
        }
        break;
        case HEXAITEM : {
              HEXA_NS::Hexa *value = item->data( HEXA_DATA_ROLE ).value< HEXA_NS::Hexa* >();
              HexaDialog *hexaEditor = static_cast<HexaDialog*>(editor);
              hexaEditor->setValue(value);
        }
        break;
        case VECTORITEM : {
              HEXA_NS::Vector *value = item->data( HEXA_DATA_ROLE ).value< HEXA_NS::Vector* >();
              VectorDialog *vectorEditor = static_cast<VectorDialog*>(editor);
              vectorEditor->setValue(value);
        }
        break;
        case CYLINDERITEM : {
              HEXA_NS::Cylinder *value = item->data( HEXA_DATA_ROLE ).value< HEXA_NS::Cylinder* >();
              CylinderDialog *cylinderEditor = static_cast<CylinderDialog*>(editor);
              cylinderEditor->setValue(value);
        }
        break;
        case PIPEITEM : {
              HEXA_NS::Pipe *value = item->data( HEXA_DATA_ROLE ).value< HEXA_NS::Pipe* >();
              PipeDialog *pipeEditor= static_cast<PipeDialog*>(editor);
              pipeEditor->setValue(value);
        }
        break;
//         case ELEMENTSITEM : editor = new ElementsDialog(parent);   break;
//         case CROSSELEMENTSITEM : editor = new CrossElementsDialog(parent);   break;
        case GROUPITEM :{
              HEXA_NS::Group *value = item->data( HEXA_DATA_ROLE ).value< HEXA_NS::Group* >();
              GroupDialog *groupEditor = static_cast<GroupDialog*>(editor);
              groupEditor->setValue(value);
        }
        break;
        case LAWITEM : {
              HEXA_NS::Law *value = item->data( HEXA_DATA_ROLE ).value< HEXA_NS::Law* >();
              LawDialog *lawEditor = static_cast<LawDialog*>(editor);
              lawEditor->setValue(value);
        }
        break;
        case PROPAGATIONITEM : {
              HEXA_NS::Propagation *value = item->data( HEXA_DATA_ROLE ).value< HEXA_NS::Propagation* >();
              PropagationDialog *propagationEditor = static_cast<PropagationDialog*>(editor);
              propagationEditor->setValue(value);
        }
        break;
      }
    }
// editor->show();
//  editor->exec();
cout << "*********************** CS_BP DocumentDelegate::setEditorData END"<< editor << endl;
}








void DocumentDelegate::setModelData( QWidget *editor, QAbstractItemModel *model,
                                     const QModelIndex &index) const
{
    cout << "//////////////////// CS_BP DocumentDelegate::setModelData BEGIN "<< editor << endl;
//     QStandardItem  *item = 0;
//     QItemDelegate::setModelData( editor, model,index);

//     // CS_TODO : simplification?
//     QSortFilterProxyModel *pmodel = dynamic_cast<QSortFilterProxyModel *>( model );
//     if ( pmodel ){
//       QStandardItemModel *smodel = dynamic_cast<QStandardItemModel *>( pmodel->sourceModel() );
//       if ( smodel ){
// //         item = smodel->itemFromIndex(index);
//         QItemDelegate::setModelData( editor, smodel, pmodel->mapToSource(index));
//       }
//     } else {
//       QStandardItemModel *smodel = dynamic_cast<QStandardItemModel *>( model );
//       if ( smodel ){
// //         item = smodel->itemFromIndex(index);
//         QItemDelegate::setModelData( editor, model,index);
//       }
//     }
//   QItemDelegate::setModelData( editor, model, index);
  cout << "//////////////////// CS_BP DocumentDelegate::setModelData END"<< editor << endl;    

}




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


void DocumentDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    cout << "CS_BP updateEditorGeometry"<<editor<<endl;
    editor->show();
//     editor->setGeometry(option.rect);
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

void DocumentDelegate::setGroupSelectionModel( QItemSelectionModel* s )
{
  _groupsSelectionModel = s ;
}

void DocumentDelegate::setMeshSelectionModel( QItemSelectionModel* s )
{
  _meshSelectionModel = s;
}

