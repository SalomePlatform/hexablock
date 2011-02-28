//  Copyright (C) 2006-2010  CEA/DEN, EDF R&D
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

#ifndef _HEXABLOCKGUI_DOCUMENTMODEL_HXX_
#define _HEXABLOCKGUI_DOCUMENTMODEL_HXX_

// #include <QAbstractItemModel>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

// #include <QItemSelection>
// #include <QBrush>
// #include <list>

#include "HEXABLOCKGUI_DocumentItem.hxx"

#include "HexDocument.hxx"

namespace HEXABLOCK
{
  namespace GUI
  {

    class DocumentModel : public QStandardItemModel 
    {
      public:
        DocumentModel( QObject * parent = 0 );
        DocumentModel( int rows, int columns, QObject * parent = 0 );

        void clear();
        void load( const QString& xmlFileName );

        //  ************  NEW ************
        QModelIndex addVertex( double x, double y, double z );
        QModelIndex addQuad( const QModelIndex &i_v0, const QModelIndex &i_v1,
                             const QModelIndex &i_v2, const QModelIndex &i_v3 );
        QModelIndex addHexaFromQuad( const QModelIndex &i_q0, const QModelIndex &i_q1, const QModelIndex &i_q2,
                                     const QModelIndex &i_q3, const QModelIndex &i_q4, const QModelIndex &i_q5 );

        // ************  EDIT ************
        bool mergeVertices( const QModelIndex &i_v0, const QModelIndex &i_v1 );
        bool mergeEdges( const QModelIndex &i_e0, const QModelIndex &i_e1,
                         const QModelIndex &i_v0, const QModelIndex &i_v1 );
        QModelIndex cutEdge( const QModelIndex &i_e0, int nbcuts );//Edge* edge, int nbcuts ); Elements* 

        // ajout JPL
        // mise a jour graphique d'un vertex et de tous les objets
        // parents associes :
        void updateView(VertexItem* vItem);
        // end JPL
        
      private:
        HEXA_NS::Document *_hexaDocument;

        //data
        QStandardItem     *_vertexDirItem;
        QStandardItem     *_edgeDirItem;
        QStandardItem     *_quadDirItem;
        QStandardItem     *_hexaDirItem;

        //builder
        QStandardItem     *_vectorDirItem;
        QStandardItem     *_cylinderDirItem;
        QStandardItem     *_pipeDirItem;
        QStandardItem     *_elementsDirItem;
        QStandardItem     *_crossElementsDirItem;
    };



    class  PatternDataModel : public QSortFilterProxyModel 
    {
      public:
        PatternDataModel( QObject * parent = 0 );
        virtual ~PatternDataModel();

        Qt::ItemFlags vertexItemFlags;
        Qt::ItemFlags edgeItemFlags;
        Qt::ItemFlags quadItemFlags;
        Qt::ItemFlags hexaItemFlags;

        virtual Qt::ItemFlags flags(const QModelIndex &index) const;
        virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

        QModelIndex addVertex( double x, double y, double z );
        QModelIndex addQuad( const QModelIndex &i_v0, const QModelIndex &i_v1,
                             const QModelIndex &i_v2, const QModelIndex &i_v3 );
        QModelIndex addHexaFromQuad( const QModelIndex &i_q0, const QModelIndex &i_q1, const QModelIndex &i_q2,
                                     const QModelIndex &i_q3, const QModelIndex &i_q4, const QModelIndex &i_q5 );

        bool mergeVertices( const QModelIndex &i_v0, const QModelIndex &i_v1 ); //Vertex* v1, Vertex* v2 );
        bool mergeEdges( const QModelIndex &i_e0, const QModelIndex &i_e1,
                         const QModelIndex &i_v0, const QModelIndex &i_v1 );
        QModelIndex cutEdge( const QModelIndex &i_e0, int nbcuts );


        QStandardItem * itemFromIndex ( const QModelIndex & index ) const;

        // ajout JPL
        void updateVertex(const QModelIndex& i_v);
        // end JPL

    };


    class  PatternBuilderModel : public QSortFilterProxyModel 
    {
      public:
        PatternBuilderModel( QObject * parent = 0 );
        virtual ~PatternBuilderModel();

        virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
     };



  }
}

#endif
