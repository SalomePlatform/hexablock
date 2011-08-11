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

#ifndef _HEXABLOCKGUI_DOCUMENTMODEL_HXX_
#define _HEXABLOCKGUI_DOCUMENTMODEL_HXX_

// #include <QAbstractItemModel>
#include <QTemporaryFile>
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
      Q_OBJECT
      public:

        // enum EnumGroup  { HexaCell, QuadCell, EdgeCell, 
        //                   HexaNode, QuadNode, EdgeNode, Vertex_Node};
        typedef HEXA_NS::EnumGroup Group;
        typedef HEXA_NS::KindLaw   KindLaw;

        struct GeomObj
        {
          QString name;
          QString entry;
          QString brep;
          double  start;
          double  end;
        };

        DocumentModel( HEXA_NS::Document* doc, const QString& entry, QObject * parent = 0 );
        DocumentModel( int rows, int columns, QObject * parent = 0 );

        virtual ~DocumentModel();

        void load( const QString& xmlFileName );

        void updateData();

        void clearAll();
        void clearData();
        void clearBuilder();
        void clearAssociation();
        void clearGroups();
        void clearMesh();

        void fillData();
        void fillBuilder();
        void fillAssociation();
        void fillGroups();
        void fillMesh();

        virtual Qt::ItemFlags flags(const QModelIndex &index) const;


        void allowEdition();
        void disallowEdition();

//         void setDefaultSelection();
//         void allowSelection();
        void allowDataSelectionOnly();
        void allowVertexSelectionOnly();
        void allowEdgeSelectionOnly();
        void allowQuadSelectionOnly();
        void allowHexaSelectionOnly();

        void allowVectorSelectionOnly();
        void allowCylinderSelectionOnly();
        void allowPipeSelectionOnly();
        void allowElementsSelectionOnly();
        void allowCrossElementsSelectionOnly();

        void allowLawSelectionOnly();


        //  ************  BUILD HEXABLOCK MODEL ************
        QModelIndex addVertex( double x, double y, double z );

        //
        QModelIndex addEdgeVertices (const QModelIndex &i_v0, const QModelIndex &i_v1 );
        QModelIndex addEdgeVector( const QModelIndex &i_v, const QModelIndex &i_vec );

        //
        QModelIndex addQuadVertices( const QModelIndex &i_v0, const QModelIndex &i_v1,
                                     const QModelIndex &i_v2, const QModelIndex &i_v3 );
        QModelIndex addQuadEdges( const QModelIndex &i_e0, const QModelIndex &i_e1,
                                  const QModelIndex &i_e2, const QModelIndex &i_e3 );

        //
        QModelIndex addHexaVertices( const QModelIndex &i_v0, const QModelIndex &i_v1,
                                     const QModelIndex &i_v2, const QModelIndex &i_v3,
                                     const QModelIndex &i_v4, const QModelIndex &i_v5,
                                     const QModelIndex &i_v6, const QModelIndex &i_v7 );
        QModelIndex addHexaQuad( const QModelIndex &i_q0, const QModelIndex &i_q1, const QModelIndex &i_q2, const QModelIndex &i_q3, const QModelIndex &i_q4, const QModelIndex &i_q5 );

        //
        QModelIndex addVector( double dx, double dy, double dz );
        QModelIndex addVectorVertices( const QModelIndex &i_v0, const QModelIndex &i_v1 );

        //
        QModelIndex addCylinder( const QModelIndex &iv, const QModelIndex &ivec, double r,  double h );
        
        //
        QModelIndex addPipe( const QModelIndex &iv, const QModelIndex &ivec, double ri, double re, double h );


        // 
        QModelIndex makeCartesian( const QModelIndex& ivex,
                                   const QModelIndex& ivecx, const QModelIndex& ivecy, const QModelIndex& ivecz,
                                   long nx, long ny, long nz);

        QModelIndex makeCartesian( const QModelIndex& ivex,
                                   const QModelIndex& ivec,
                                   int nx, int ny, int nz );

        QModelIndex makeCylindrical( const QModelIndex& i_pt,
                                     const QModelIndex& i_vx, const QModelIndex& i_vz,
                                     double dr, double da, double dl,
                                     long nr, long na, long nl,
                                     bool fill  = false );

        QModelIndex makeSpherical( const QModelIndex& i_v, const QModelIndex& i_vec, int nb, double k = 1 );


        //
        QModelIndex makeCylinder( const QModelIndex& cyl, const QModelIndex& vec,
                                  int nr, int na, int nl );

        //
        QModelIndex makePipe( const QModelIndex& pipe, const QModelIndex& vecx, 
                              int nr, int na, int nl );

        //
        QModelIndex makeCylinders(const QModelIndex& cyl1, const QModelIndex& cyl2);

        //
        QModelIndex makePipes( const QModelIndex& pipe1, const QModelIndex& pipe2 );


        // ************  EDIT HEXABLOCK MODEL ************

        bool updateVertex( const QModelIndex& vertex, double x, double y, double z );

        //
        bool removeHexa( const QModelIndex& hexa );
        bool removeConnectedHexa( const QModelIndex& hexa );


        //
        QModelIndex prismQuad( const QModelIndex& quad, const QModelIndex& dv, int nb);
        QModelIndex prismQuads( const QModelIndexList& quads, const QModelIndex& dv, int nb);


        //
        QModelIndex joinQuad( const QModelIndex& start_q, const QModelIndex& dest_q,
                              const QModelIndex& v0, const QModelIndex& v1,
                              const QModelIndex& v2, const QModelIndex& v3,
                              int nb );


        QModelIndex joinQuads( const QModelIndexList& start_q, const QModelIndex& dest_q,
                               const QModelIndex& v0, const QModelIndex& v1,
                               const QModelIndex& v2, const QModelIndex& v3,
                               int nb );


        //
        bool mergeVertices( const QModelIndex& va, const QModelIndex& vb );
        bool mergeEdges( const QModelIndex& ea, const QModelIndex& eb,
                         const QModelIndex& v0, const QModelIndex& v1 );
        bool mergeQuads( const QModelIndex& qa, const QModelIndex& qb,
                         const QModelIndex& v0, const QModelIndex& v1,
                         const QModelIndex& v2, const QModelIndex& v3 );

        //
        QModelIndex disconnectVertex( const QModelIndex& h, const QModelIndex& v );
        QModelIndex disconnectEdge( const QModelIndex& h, const QModelIndex& e );
        QModelIndex disconnectQuad( const QModelIndex& h, const QModelIndex& q );

        //
        QModelIndex cutEdge( const QModelIndex &e, int nbcuts );

        //
        QModelIndex makeTranslation( const QModelIndex& elts, const QModelIndex& vec );
        QModelIndex makeScale( const QModelIndex& elts, const QModelIndex& v, double k );

        //
        QModelIndex makeRotation( const QModelIndex& elts, const QModelIndex& v, const QModelIndex& vec, double angle );

        //
        QModelIndex makeSymmetryPoint( const QModelIndex& elts, const QModelIndex& v );
        QModelIndex makeSymmetryLine( const QModelIndex& elts, const QModelIndex& v, const QModelIndex& vec );
        QModelIndex makeSymmetryPlane( const QModelIndex& elts, const QModelIndex& v, const QModelIndex& vec );

        //
        bool performTranslation( const QModelIndex& elts, const QModelIndex& vec );

        //
        bool performScale( const QModelIndex& elts, const QModelIndex& v, double k );

        //
        bool performRotation( const QModelIndex& elts, const QModelIndex& v, const QModelIndex& vec, double angle );

        //
        bool performSymmetryPoint( const QModelIndex& elts, const QModelIndex& v );
        bool performSymmetryLine( const QModelIndex& elts, const QModelIndex& v, const QModelIndex& vec );
        bool performSymmetryPlane( const QModelIndex& elts, const QModelIndex& v, const QModelIndex& vec );


        // ************  ASSOCIATION ************
//         void setVertexAssociation( const QModelIndex& vertex, const GeomObj& assoc );
//         void addEdgeAssociation( const QModelIndex& edge, const GeomObj& assoc );
//         void addQuadAssociation( const QModelIndex& quad, const GeomObj& assoc );

//         GeomObj getVertexAssociation( const QModelIndex& vertex );
//         QList<GeomObj> getEdgeAssociations( const QModelIndex& edge );
//         QList<GeomObj> getQuadAssociations( const QModelIndex& quad );

        void addAssociation( const QModelIndex& elt, const GeomObj& assoc );
        QList<GeomObj> getAssociations( const QModelIndex& elt );


        // ************  GROUPS  ************

        //
        QModelIndex addGroup( const QString& name, Group kind );

        //
        bool removeGroup( const QModelIndex& grp );

        // 7.4 Boite: éditer un groupe
        void setGroupName( const QModelIndex& grp, const QString& name );
        bool addGroupElement( const QModelIndex& grp, const QModelIndex& elt );
        bool removeGroupElement( const QModelIndex& grp, int nro ); //CS_TODO
        bool clearGroupElement( const QModelIndex& grp );


        // ************  LAWS  ************

        //
        QModelIndex addLaw( const QString& name, int nbnodes );

// 8.2 Boite: créer une loi 
// class Law 
// {
// public:
// int     setNodes (int  nbre);
// int     setCoefficient (double coeff);
// void    setKind (KindLaw type);
// }

        bool setLaw( const QModelIndex& ilaw, int nbnodes, double coeff, KindLaw type );

        // 
        bool  removeLaw( const QModelIndex& law );

        // 8.3 Boite: éditer une loi 
        // (idem création)

// 9 Discrétisation
// 9.1 Boite: poser une loi de discrétisation sur une propagation
// int   setLaw (Law* loi);
// void  setWay (bool sens);
        bool setPropagation( const QModelIndex& iPropagation, const QModelIndex& iLaw, bool way );


// 
// 9.1 Boite: éditer 
// (idem création)



      HEXA_NS::Document* documentImpl();
      QString            documentEntry();
      

      signals:
        void patternDataChanged();
        
      private:
        QTemporaryFile    *_hexaFile;
        HEXA_NS::Document *_hexaDocument;
        QString            _entry;

        QMap<QString, QString> _assocName; // clé: id; valeur: nom
        bool              _disallowEdition;

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

      
        //association
        // CS_TODO

        // groups
        QStandardItem     *_groupDirItem;

        // law
        QStandardItem     *_lawDirItem;
        QStandardItem     *_propagationDirItem;


        Qt::ItemFlags     _vertexItemFlags;
        Qt::ItemFlags     _edgeItemFlags;
        Qt::ItemFlags     _quadItemFlags;
        Qt::ItemFlags     _hexaItemFlags;

        Qt::ItemFlags     _vectorItemFlags;
        Qt::ItemFlags     _cylinderItemFlags;
        Qt::ItemFlags     _pipeItemFlags;
        Qt::ItemFlags     _elementsItemFlags;
        Qt::ItemFlags     _crossElementsItemFlags;

        Qt::ItemFlags     _groupItemFlags;
        Qt::ItemFlags     _lawItemFlags;
        Qt::ItemFlags     _propagationItemFlags;

    };



    class  PatternDataModel : public QSortFilterProxyModel
    {
      public:
        PatternDataModel( QObject * parent = 0 );
        virtual ~PatternDataModel();

        virtual Qt::ItemFlags flags(const QModelIndex &index) const;
        virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

        QStandardItem * itemFromIndex ( const QModelIndex & index ) const;


        HEXA_NS::Document* documentImpl();
        QString            documentEntry();


    };


    class  PatternBuilderModel : public QSortFilterProxyModel
    {
      public:
        PatternBuilderModel( QObject * parent = 0 );
        virtual ~PatternBuilderModel();

        virtual Qt::ItemFlags flags(const QModelIndex &index) const;
        virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
        QStandardItem * itemFromIndex ( const QModelIndex & index ) const;
    };



    class  AssociationsModel : public QSortFilterProxyModel
    {
      public:
        AssociationsModel( QObject * parent = 0 );
        virtual ~AssociationsModel();

        virtual Qt::ItemFlags flags(const QModelIndex &index) const;
        virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
        QStandardItem * itemFromIndex ( const QModelIndex & index ) const;
    };


    class  GroupsModel : public QSortFilterProxyModel
    {
      public:
        GroupsModel( QObject * parent = 0 );
        virtual ~GroupsModel();

        virtual Qt::ItemFlags flags(const QModelIndex &index) const;
        virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
        QStandardItem * itemFromIndex ( const QModelIndex & index ) const;
     };


    class  MeshModel : public QSortFilterProxyModel
    {
      public:
        MeshModel( QObject * parent = 0 );
        virtual ~MeshModel();

        virtual Qt::ItemFlags flags(const QModelIndex &index) const;
        virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
        QStandardItem * itemFromIndex ( const QModelIndex & index ) const;
    }; 




  }
}

#endif
