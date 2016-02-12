/*
    Copyright (C) 2010 Klarälvdalens Datakonsult AB,
        a KDAB Group company, info@kdab.net,
        author Stephen Kelly <stephen@kdab.com>

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

#include <QObject>
#include "klinkitemselectionmodel.hxx"
// #include "klinkitemselectionmodel_p.hxx"

#include "kmodelindexproxymapper.hxx"

// #include "kdebug.h"

#include <QItemSelection>

#include <iostream>


KLinkItemSelectionModel::KLinkItemSelectionModel(QAbstractItemModel *model, QItemSelectionModel *proxySelector, QObject *parent)
        : QItemSelectionModel(model, parent),
        d_ptr(new KLinkItemSelectionModelPrivate(this, model, proxySelector))
{
    connect(proxySelector, SIGNAL(selectionChanged(QItemSelection, QItemSelection)), SLOT(sourceSelectionChanged(QItemSelection, QItemSelection)));
}

KLinkItemSelectionModel::~KLinkItemSelectionModel()
{
    delete d_ptr;
}

void KLinkItemSelectionModel::select(const QModelIndex &index, QItemSelectionModel::SelectionFlags command)
{
    //std::cout<<"KLinkItemSelectionModel::select(const QModelIndex &index"<<std::endl;
    Q_D(KLinkItemSelectionModel);
    // When an item is removed, the current index is set to the top index in the model.
    // That causes a selectionChanged signal with a selection which we do not want.
    if (d->m_ignoreCurrentChanged) {
        return;
    }
    QItemSelectionModel::select(index, command);
    if (index.isValid())
        d->m_linkedItemSelectionModel->select(d->m_indexMapper->mapSelectionLeftToRight(QItemSelection(index, index)), command);
    else {
        d->m_linkedItemSelectionModel->clearSelection();
    }
}

// // QAbstractProxyModel::mapSelectionFromSource creates invalid ranges to we filter
// // those out manually in a loop. Hopefully fixed in Qt 4.7.2, so we ifdef it out.
// // http://qt.gitorious.org/qt/qt/merge_requests/2474
// // http://qt.gitorious.org/qt/qt/merge_requests/831
// #if QT_VERSION < 0x040702
// #define RANGE_FIX_HACK
// #endif
// 
// #ifdef RANGE_FIX_HACK
// static QItemSelection klink_removeInvalidRanges(const QItemSelection &selection)
// {
//   QItemSelection result;
//   Q_FOREACH(const QItemSelectionRange &range, selection)
//   {
//     if (!range.isValid())
//       continue;
//     result << range;
//   }
//   return result;
// }
// #endif

void KLinkItemSelectionModel::select(const QItemSelection &selection, QItemSelectionModel::SelectionFlags command)
{
//  std::cout<<"KLinkItemSelectionModel::select(const QItemSelection &selection"<<std::endl;

  Q_FOREACH(const QItemSelectionRange &range, selection){
//     if (range.isEmpty()){
//       std::cout<<"range    EMPTY"<<std::endl;
//     } else {
//       std::cout<<"range    NOT EMPTY"<<std::endl;
//     }
//    if (range.isValid()){
//      std::cout<<"range    VALID"<<std::endl;
//      std::cout<<"count           =>"<<range.indexes().count()<<std::endl;
//      Q_FOREACH(const QModelIndex &i, range.indexes())
//        std::cout<< " KLinkItemSelectionModel::select =====> " << i.data().toString().toStdString() << std::endl;
//    } else {
//      std::cout<<"range    NOT VALID"<<std::endl;
//    }
  }


    Q_D(KLinkItemSelectionModel);
    d->m_ignoreCurrentChanged = true;
#ifdef RANGE_FIX_HACK
    QItemSelection _selection = klink_removeInvalidRanges(selection);
#else
    QItemSelection _selection = selection;
#endif
    QItemSelectionModel::select(_selection, command);
    Q_ASSERT(d->assertSelectionValid(_selection));
    QItemSelection mappedSelection = d->m_indexMapper->mapSelectionLeftToRight(_selection);
    Q_ASSERT(d->assertSelectionValid(mappedSelection));
    d->m_linkedItemSelectionModel->select(mappedSelection, command);
    d->m_ignoreCurrentChanged = false;
}



// #include "klinkitemselectionmodel_moc.cxx"
// #include "klinkitemselectionmodel.moc"

