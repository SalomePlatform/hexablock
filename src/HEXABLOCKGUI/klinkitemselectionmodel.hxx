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

#ifndef KLINKITEMSELECTIONMODEL_H
#define KLINKITEMSELECTIONMODEL_H

#include "HEXABLOCKGUI_Export.hxx"

#include <QObject>
#include <QItemSelectionModel>
#include <QAbstractProxyModel>

#include <iostream>

// #include "kdeui_export.h"
// #include "klinkitemselectionmodel_p.hxx"

#include "hexa_base.hxx"
#include "kmodelindexproxymapper.hxx"

class KLinkItemSelectionModelPrivate;

class HEXABLOCK_EXPORT KLinkItemSelectionModel : public QItemSelectionModel
{
    Q_OBJECT
public:
    KLinkItemSelectionModel(QAbstractItemModel *targetModel, QItemSelectionModel *linkedItemSelectionModel, QObject *parent = 0);
    ~KLinkItemSelectionModel();
    /* reimp */ void select(const QModelIndex &index, QItemSelectionModel::SelectionFlags command);
    /* reimp */ void select(const QItemSelection &selection, QItemSelectionModel::SelectionFlags command);

protected:
    KLinkItemSelectionModelPrivate * const d_ptr;

private:
    Q_DECLARE_PRIVATE(KLinkItemSelectionModel)
    Q_PRIVATE_SLOT( d_func(), void sourceSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected))

};


// QAbstractProxyModel::mapSelectionFromSource creates invalid ranges to we filter
// those out manually in a loop. Hopefully fixed in Qt 4.7.2, so we ifdef it out.
// http://qt.gitorious.org/qt/qt/merge_requests/2474
// http://qt.gitorious.org/qt/qt/merge_requests/831
#if QT_VERSION < 0x040702
#define RANGE_FIX_HACK
#endif

#ifdef RANGE_FIX_HACK
static QItemSelection klink_removeInvalidRanges(const QItemSelection &selection)
{
//   std::cout<< " klink_removeInvalidRanges " << std::endl;
  QItemSelection result;
  Q_FOREACH(const QItemSelectionRange &range, selection)
  {

//    Q_FOREACH(const QModelIndex &i, range.indexes ())
//    {
////       std::cout<< " =====> " << i.data().toString().toStdString() << std::endl;
//    }
    if (!range.isValid())
      continue;
//     std::cout<< " is VALID !!"<< std::endl;
    result << range;
  }
  return result;
}
#endif


class KLinkItemSelectionModelPrivate
{
public:
    KLinkItemSelectionModelPrivate(KLinkItemSelectionModel *proxySelectionModel, QAbstractItemModel *model,
                                    QItemSelectionModel *linkedItemSelectionModel)
      : q_ptr(proxySelectionModel),
        m_model(model),
        m_linkedItemSelectionModel(linkedItemSelectionModel),
        m_ignoreCurrentChanged(false),
        m_indexMapper(new KModelIndexProxyMapper(model, linkedItemSelectionModel->model(), proxySelectionModel))
    {
    }

    Q_DECLARE_PUBLIC(KLinkItemSelectionModel)
    KLinkItemSelectionModel * const q_ptr;


    bool assertSelectionValid(const QItemSelection &selection) const {
      foreach(const QItemSelectionRange &range, selection) {
//         if (!range.isValid()) {
//           kDebug() << selection;
//         }
        
        Q_ASSERT(range.isValid());
      }
      return true;
    }

void sourceSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{

    
    Q_Q(KLinkItemSelectionModel);
#ifdef RANGE_FIX_HACK
//     std::cout<<"XXXXXXXXXXXXX  sourceSelectionChanged!!!!!!!! RANGE_FIX_HACK"<<std::endl;
    QItemSelection _selected = klink_removeInvalidRanges(selected);
    QItemSelection _deselected = klink_removeInvalidRanges(deselected);
#else
//     std::cout<<"XXXXXXXXXXXXX  sourceSelectionChanged!!!!!!!! "<<std::endl;
    QItemSelection _selected = selected;
    QItemSelection _deselected = deselected;
#endif
    Q_ASSERT(assertSelectionValid(_selected));
    Q_ASSERT(assertSelectionValid(_deselected));

//     std::cout << "XXXXXXXXXXXXX  _selected.count() "   << _selected.count() << std::endl;
//     std::cout << "XXXXXXXXXXXXX  _deselected.count() " << _deselected.count() << std::endl;

    const QItemSelection mappedDeselection = m_indexMapper->mapSelectionRightToLeft(_deselected);
    const QItemSelection mappedSelection   = m_indexMapper->mapSelectionRightToLeft(_selected);

//     const QItemSelection mappedDeselection = _deselected;
//     const QItemSelection mappedSelection   = _selected;

//     std::cout << "XXXXXXXXXXXXX  mappedSelection.count() "   << mappedSelection.count() << std::endl;
//     std::cout << "XXXXXXXXXXXXX  mappedDeselection.count() " << mappedDeselection.count() << std::endl;

    q->QItemSelectionModel::select(mappedDeselection, QItemSelectionModel::Deselect);
    q->QItemSelectionModel::select(mappedSelection, QItemSelectionModel::Select);

//     q->select(mappedDeselection, QItemSelectionModel::Deselect);
//     q->select(mappedSelection, QItemSelectionModel::Select);


}

    QAbstractItemModel * const m_model;
    QItemSelectionModel * const m_linkedItemSelectionModel;
    bool m_ignoreCurrentChanged;
    KModelIndexProxyMapper * const m_indexMapper;
};



#endif
