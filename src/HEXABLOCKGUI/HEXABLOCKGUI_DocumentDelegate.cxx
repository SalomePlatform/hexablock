#include <iostream>
#include <QtGui>
#include "HEXABLOCKGUI_DocumentDelegate.hxx"
#include "HEXABLOCKGUI_DocumentItem.hxx"
#include "HEXABLOCKGUI_DocumentPanel.hxx"

using namespace std;
using namespace HEXABLOCK::GUI;


DocumentDelegate::DocumentDelegate(QDockWidget *dw, QObject *parent)
    : QItemDelegate(parent),
      _dw(dw)
{
}

QWidget *DocumentDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    cout << "CS_BP createEditor createEditor createEditor createEditor"<<endl;
    QWidget *editor = 0;

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
      switch ( item->type() ){
        case VERTEXITEM : editor = new VertexDialog(parent);   break;
        case EDGEITEM :   editor = new EdgeDialog(parent);     break;
        case QUADITEM :   editor = new QuadDialog(parent);     break;
        case HEXAITEM :   editor = new HexaDialog(parent);     break;
        case VECTORITEM : editor = new VectorDialog(parent);   break;
      }
      if ( editor ){
        _dw->setWidget(editor);
      }
    }

//     editor->show();
    return editor;
}



void DocumentDelegate::setEditorData( QWidget *editor,
                                      const QModelIndex &index) const
{
    cout << "CS_BP setEditorData setEditorData setEditorData setEditorData "<<endl;
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
      switch ( item->type() ){
        case VERTEXITEM : {
//               HEXA_NS::Vertex *value = index.model()->data(index, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
              HEXA_NS::Vertex *value = item->data( HEXA_DATA_ROLE ).value< HEXA_NS::Vertex* >();
              VertexDialog *vertexEditor = static_cast<VertexDialog*>(editor);
              vertexEditor->setValue(value);
        }
        break;


        case EDGEITEM : {
//               HEXA_NS::Quad *value = index.model()->data(index, HEXA_DATA_ROLE).value<HEXA_NS::Quad *>();
              HEXA_NS::Edge *value = item->data( HEXA_DATA_ROLE ).value< HEXA_NS::Edge* >();
              EdgeDialog *edgeEditor = static_cast<EdgeDialog*>(editor);
              edgeEditor->setValue(value);
        }
        break;

        case QUADITEM : {
//               HEXA_NS::Quad *value = index.model()->data(index, HEXA_DATA_ROLE).value<HEXA_NS::Quad *>();
              HEXA_NS::Quad *value = item->data( HEXA_DATA_ROLE ).value< HEXA_NS::Quad* >();
              QuadDialog *quadEditor = static_cast<QuadDialog*>(editor);
              quadEditor->setValue(value);
        }
        break;

        case HEXAITEM : {
//               HEXA_NS::Quad *value = index.model()->data(index, HEXA_DATA_ROLE).value<HEXA_NS::Quad *>();
              HEXA_NS::Hexa *value = item->data( HEXA_DATA_ROLE ).value< HEXA_NS::Hexa* >();
              HexaDialog *hexaEditor = static_cast<HexaDialog*>(editor);
              hexaEditor->setValue(value);
        }
        break;

        case VECTORITEM : {
//               HEXA_NS::Quad *value = index.model()->data(index, HEXA_DATA_ROLE).value<HEXA_NS::Quad *>();
              HEXA_NS::Vector *value = item->data( HEXA_DATA_ROLE ).value< HEXA_NS::Vector* >();
              VectorDialog *vectorEditor = static_cast<VectorDialog*>(editor);
              vectorEditor->setValue(value);
        }
        break;


      }
    }

}


void DocumentDelegate::setModelData( QWidget *editor, QAbstractItemModel *model,
                                     const QModelIndex &index) const
{
    cout << "CS_BP setModelData setModelData setModelData setModelData "<<endl;
    QStandardItem  *item = 0;

    // CS_TODO : simplification?
    QSortFilterProxyModel *pmodel = dynamic_cast<QSortFilterProxyModel *>( model );
    if ( pmodel ){
      QStandardItemModel *smodel = dynamic_cast<QStandardItemModel *>( pmodel->sourceModel() );
      if ( smodel ){
        item = smodel->itemFromIndex(pmodel->mapToSource(index));
      }
    } else {
      QStandardItemModel *smodel = dynamic_cast<QStandardItemModel *>( model );
      if ( smodel ){
        item = smodel->itemFromIndex(index);
      }
    }


    if ( item ){
      switch ( item->type() ){
        case VERTEXITEM : {
              VertexDialog *vertexEditor = static_cast<VertexDialog*>(editor);
              HEXA_NS::Vertex *value     = vertexEditor->getValue();
              model->setData(index, QVariant::fromValue( value ), HEXA_DATA_ROLE);
            }
            break;

        case EDGEITEM : {
              EdgeDialog *edgeEditor = static_cast<EdgeDialog*>(editor);
              HEXA_NS::Edge *value   = edgeEditor->getValue();
              model->setData(index, QVariant::fromValue( value ), HEXA_DATA_ROLE);
            }
            break;

        case QUADITEM : {
              QuadDialog *quadEditor = static_cast<QuadDialog*>(editor);
              HEXA_NS::Quad *value   = quadEditor->getValue();
              model->setData(index, QVariant::fromValue( value ), HEXA_DATA_ROLE);
            }
            break;

        case HEXAITEM : {
              HexaDialog *hexaEditor = static_cast<HexaDialog*>(editor);
              HEXA_NS::Hexa *value   = hexaEditor->getValue();
              model->setData(index, QVariant::fromValue( value ), HEXA_DATA_ROLE);
            }
            break;

        case VECTORITEM : {
              VectorDialog *vectorEditor = static_cast<VectorDialog*>(editor);
              HEXA_NS::Vector *value   = vectorEditor->getValue();
              model->setData(index, QVariant::fromValue( value ), HEXA_DATA_ROLE);
            }
            break;

      }
    }
}


void DocumentDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    cout << "CS_BP updateEditorGeometry"<<endl;
    editor->setGeometry(option.rect);
}

