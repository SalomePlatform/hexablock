// Copyright (C) 2009-2016  CEA/DEN, EDF R&D
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

#ifndef _HEXABLOCKGUI_DOCUMENTMODEL_HXX_
#define _HEXABLOCKGUI_DOCUMENTMODEL_HXX_

#include "HEXABLOCKGUI_Export.hxx"

#include <QTemporaryFile>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

#include "HEXABLOCKGUI_DocumentItem.hxx"
#include "HexDocument.hxx"
#include "HexNewShape.hxx"
#include "Hex.hxx"


namespace HEXABLOCK
{
  namespace GUI
  {
    class HEXABLOCK_EXPORT DocumentModel : public QStandardItemModel
    {
      Q_OBJECT
    public:

      struct GeomObj
      {
          QString shapeName;  //main shape name
          QString shapeEntry; //main shape entry
          QString name;
          QString subId;      // sub-shape id (if it's a sub shape)
          QString brep;
          double  start;
          double  end;
      };

      typedef HEXA_NS::EnumGroup Group;
      typedef HEXA_NS::KindLaw   KindLaw;

      typedef QList<GeomObj> GeomObjList;

      DocumentModel( HEXA_NS::Document* doc, const QString& entry, QObject * parent = 0 );
      DocumentModel( int rows, int columns, QObject * parent = 0 );

      virtual ~DocumentModel();


      void setName(const QString& name);
      QString getName();
      HEXA_NS::Document* getHexaDocument() const { return _hexaDocument; }
      void load( const QString& xmlFileName );
      void load(); //Loads the current document
      void save( const QString& xmlFileName );
      void updateData();
      void updateGeomTree();
      void refresh(); //refresh data
      bool isEmpty() const;

      double getLength(const QModelIndex& iEdge);
      double getRadius(const QModelIndex& iEdge);
      double getAngle(const QModelIndex& iEdge);

      void clearAll();
      void clearData();
//      void clearBuilder();
      void clearGeometry();
      void clearAssociation();
      void clearGroups();
      void clearMesh();

      void fillData();
//      void fillBuilder();
      void fillGeometry();
      void fillAssociation();
      void fillGroups();
      void fillMesh();

      virtual Qt::ItemFlags flags(const QModelIndex &index) const;


      void allowEdition();
      void disallowEdition();

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

      Hex::Hex* getHexaRoot() { return Hex::Hex::getInstance(); }

      HEXA_NS::EltBase* getHexaPtr(const QModelIndex& iElt);
      template<typename T>
      T getHexaPtr(QModelIndex iElt)
      {
        if (iElt.isValid())
          return iElt.data( HEXA_DATA_ROLE ).value< T >();

        return NULL;
      }

      int getNbrElt(HEXA_NS::EnumElt eltType);
      int getNbrUsedElt(HEXA_NS::EnumElt eltType);
      int getNbrUnusedElt(HEXA_NS::EnumElt eltType);

      QModelIndex addElementsToTree(HEXA_NS::Elements* helts, QStandardItem* tree);

      //associate a shape to the current document
      bool addShape(TopoDS_Shape& forme, QString& shapeName, bool publish=true);

      //returns the geom obj id in the document using its entry
      QString getGeomObjName(QString& studyEntry) const {
          return docShapesName.contains(studyEntry) ? docShapesName[studyEntry] : QString();
      }

      //returns the geom obj entry in the document using its id
      QString getGeomObjEntry(QString& shapeName) const {
          return docShapesEntry.contains(shapeName) ? docShapesEntry[shapeName] : QString();
      }

      //returns the document's shapes entries
      QList<QString> getShapesEntries() const { return docShapesName.uniqueKeys(); }

      //returns the associated geom object to the data index
      HEXA_NS::SubShape* getGeomPtr(QString& id) const
      {
          if (!shapeById.contains(id))
              return NULL;
          return shapeById[id];
      }

      QModelIndex getGeomModelIndex(QString& id) const;

      void setGeomObjName(QString& studyEntry, QString& shapeName) {docShapesName[studyEntry] = shapeName;}
      void setGeomObjEntry(QString& shapeName, QString& studyEntry) {docShapesEntry[shapeName] = studyEntry;}

      void setName( const QModelIndex& iElt, const QString& name );
      bool clearEltAssociations( const QModelIndex& iElt );
      HEXA_NS::Hexa* getQuadHexa(HEXA_NS::Quad* quad);

      //  ************  BUILD HEXABLOCK MODEL (OBSOLETE)************
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

      QModelIndex addHexaQuads( const QModelIndexList &i_quads ); //NEW HEXA3


      //
      QModelIndex addVector( double dx, double dy, double dz );
      QModelIndex addVectorVertices( const QModelIndex &i_v0, const QModelIndex &i_v1 );

      //
//      QModelIndex addCylinder( const QModelIndex &iv, const QModelIndex &ivec, double r,  double h );
//
//      //
//      QModelIndex addPipe( const QModelIndex &iv, const QModelIndex &ivec, double ri, double re, double h );



      // ===================== NEW =================================

      // ===== CARTESIAN GRID

      QModelIndex makeCartesianTop(int nx, int ny, int nz);
      QModelIndex makeCartesianUni(const QModelIndex& icenter, const QModelIndex& ibase, const QModelIndex& ivec,
                                   const QModelIndex& iaxis, double lx, double ly, double lz, int nx, int ny, int nz);
      QModelIndex makeCartesian(const QModelIndex& icenter, const QModelIndex& ibase, const QModelIndex& ivec,
                                const QModelIndex& iaxis, vector<double>& radius, vector<double>& angles, vector<double>& heights);

      // ====== SPHERE

      QModelIndex makeSphereTop (int nr, int na, int nh);

      QModelIndex makeSphereUni (QModelIndex& icenter,
                                 QModelIndex& ivec_x, QModelIndex& ivec_z,
                                 double rtrou, double rext, double ang,
                                 QModelIndex& ivplan,
                                 int nr, int na, int nh);

      QModelIndex makeSphere    (QModelIndex& icenter,
                                 QModelIndex& ivec_x, QModelIndex& ivec_z,
                                 vector<double>& tray, vector<double>& tang, vector<double>& thaut);

      // ====== SPHERICAL

      QModelIndex makeSphericalTop (int nbre, int crit);

      QModelIndex makeSphericalUni (QModelIndex& icenter,
                                    QModelIndex& ivec_x, QModelIndex& ivec_z,
                                    double rayon,
                                    int nbre, int crit);

      QModelIndex makeSpherical    (QModelIndex& icenter,
                                    QModelIndex& ivec_x, QModelIndex& ivec_z,
                                    vector<double>& rayon,
                                    int crit);

      // =========== RIND
      QModelIndex makeRindTop (int nr, int na, int nh);

      QModelIndex makeRindUni (QModelIndex& icenter,
                               QModelIndex& ivec_x, QModelIndex& ivec_z,
                               double raytrou, double rint, double rext, double ang,
                               QModelIndex& ivplan,
                               int nr, int na, int nh);

      QModelIndex makeRind   (QModelIndex& icenter,
                              QModelIndex& ivec_x, QModelIndex& ivec_z,
                              vector<double>& tray, vector<double>& tang, vector<double>& thaut);

      // ======== Cylinder
      QModelIndex makeCylinderTop(int nr, int na, int nh);

      QModelIndex makeCylinderUni(QModelIndex& iorig, QModelIndex& ivecx, QModelIndex& ivecz,
                                   double rint, double rext, double angle, double haut,
                                   int nr, int na, int nh);

      QModelIndex makeCylinder   (QModelIndex& iorig, QModelIndex& ivecx, QModelIndex& ivecz,
                                  vector<double>& tray, vector<double>& tang, vector<double>& thaut);

      // ======== Cylinders
      QModelIndex makeCylinders  (QModelIndex& iorig1, QModelIndex& ivecz1,  double r1, double h1,
                                  QModelIndex& iorig2, QModelIndex& ivecz2, double r2, double h2);

      // =========== PIPE
      QModelIndex makePipeTop (int nr, int na, int nh);

      QModelIndex makePipeUni (QModelIndex& iorig, QModelIndex& ivecx, QModelIndex& ivecz,
                               double rint, double rext, double angle, double haut,
                               int nr, int na, int nh);

      QModelIndex makePipe    (QModelIndex& iorig, QModelIndex& ivecx, QModelIndex& ivecz,
                               vector<double>& tray, vector<double>& tang, vector<double>& thaut);

      // ======== Pipes
      QModelIndex makePipes  (QModelIndex& iorig1, QModelIndex& ivecz1, double rint1, double rex1, double h1,
                              QModelIndex& iorig2, QModelIndex& ivecz2, double rint2, double rex2, double h2);

      // ======== Join Quads
      QModelIndex joinQuadUni(QModelIndex& istart, QModelIndex& idest, QModelIndex& iv1, QModelIndex& iv2,
                              QModelIndex& iv3, QModelIndex& iv4, int nb);

      QModelIndex joinQuad   (QModelIndex&  istart, QModelIndex& idest, QModelIndex& iva1, QModelIndex& ivb1,
                              QModelIndex& iva2, QModelIndex& ivb2, vector<double>& tlen);

      QModelIndex joinQuadsUni (QModelIndexList& istarts, QModelIndex& idest, QModelIndex& iv1, QModelIndex& iv2,
                                QModelIndex& iv3, QModelIndex& iv4, int nb);

      QModelIndex joinQuads    (QModelIndexList& istarts, QModelIndex& idest, QModelIndex& iva1, QModelIndex& ivb1,
                                QModelIndex& iva2, QModelIndex& ivb2, vector<double>& tlen);

      // ======== Quad Revolution
      QModelIndex revolutionQuadUni(QModelIndex& istart, QModelIndex& icenter, QModelIndex& iaxis,
                                    double angle, int nbre);

      QModelIndex revolutionQuad(QModelIndex& istart, QModelIndex& icenter, QModelIndex& iaxis,
                                 vector<double>& angles);

      QModelIndex revolutionQuadsUni(QModelIndexList& istarts, QModelIndex& icenter, QModelIndex& iaxis,
                                     double angle, int nbre);

      QModelIndex revolutionQuads(QModelIndexList& istarts, QModelIndex& icenter, QModelIndex& iaxis,
                                  vector<double>& angles);

      // ==== PrismQuad or ExtrudeQuad
      QModelIndex extrudeQuadTop (QModelIndex& istart, int nbre);
      QModelIndex extrudeQuadUni (QModelIndex& istart, QModelIndex& dv, double len, int nbre);
      QModelIndex extrudeQuad    (QModelIndex& istart, QModelIndex& dv, vector<double>& tlen);

      QModelIndex extrudeQuadsTop (QModelIndexList& istarts, int nbre);
      QModelIndex extrudeQuadsUni (QModelIndexList& istarts, QModelIndex& axis, double len, int nbre);
      QModelIndex extrudeQuads    (QModelIndexList& istarts, QModelIndex& iaxis, vector<double>& tlen);

      // ==== Cut Edge
      QModelIndex cutUni     (QModelIndex& iEdge, int nbre);
      QModelIndex cut        (QModelIndex& iEdge, vector<double>& tlen);

      // ============================== END NEW ================================

      // ************  EDIT HEXABLOCK MODEL ************

      bool updateVertex( const QModelIndex& vertex, double x, double y, double z );

      //
      bool removeHexa( const QModelIndex& hexa );
      bool removeConnectedHexa( const QModelIndex& hexa );

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
      QModelIndex disconnectEdges( const QModelIndexList& h, const QModelIndexList& e );

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

      QModelIndex replace( const QModelIndexList& iquads_source, const QModelIndexList& iquads_dest,
                           const QModelIndex& ip1_source, const QModelIndex& ic1_dest,
                           const QModelIndex& ip2_source, const QModelIndex& ic2_dest);


      // ************  ASSOCIATION ************
      bool setVertexAssociation( const QModelIndex& iVertex, double x, double y, double z);
      bool setVertexAssociation(const QModelIndex& iVertex, const QModelIndex& iGeomVertex);
      bool addEdgeAssociation(const QModelIndex& iEdge, const QModelIndex& iGeomEdge, double start, double end);
      bool addQuadAssociation (const QModelIndex& iQuad, const QModelIndex& iGeomFace);

      QMultiMap< QString, int >     getAssocShapesIds(const QModelIndex& dataIndex);
      QModelIndex     getVertexAssociation(const QModelIndex& iVertex);
      QModelIndexList getEdgeAssociations(const QModelIndex& iEdge);
      QModelIndexList getQuadAssociations(const QModelIndex& iQuad);

     //---------- perimees -------------
      bool associateOpenedLine( const QModelIndexList& edges,
          const GeomObjList&     assocs,
          double pstart,
          double pend );

      bool associateClosedLine( const QModelIndex& vertex,
          const QModelIndexList& edges,
          const GeomObjList&     assocs,
          double pstart,
          bool   inv = false );
      //-------------

      bool associateOpenedLine( const QModelIndexList& edges,
              HEXA_NS::NewShapes shapes,
              HEXA_NS::IntVector subIds,
              double pstart,
              double pend );

      bool associateClosedLine( const QModelIndex& vertex,
              const QModelIndexList& edges,
              HEXA_NS::NewShapes shapes,
              HEXA_NS::IntVector subIds,
              double pstart,
              bool   inv = false );

      void clearAssociation(HEXA_NS::EnumElt& eltType);


      // ************  GROUPS  ************
      //
      QModelIndex addGroup( const QString& name, Group kind );

      //
      bool removeGroup( const QModelIndex& grp );

      //
      QModelIndexList getGroupElements( const QModelIndex& iGroup, Group& kind) const;

      // 7.4 Boite: éditer un groupe
      void setGroupName( const QModelIndex& grp, const QString& name );
      bool addGroupElement( const QModelIndex& grp, const QModelIndex& elt );
      bool removeGroupElement( const QModelIndex& grp, int nro ); //CS_TODO
      bool clearGroupElement( const QModelIndex& grp );


      // ************  LAWS  ************
      //
      QModelIndex addLaw( const QString& name, int nbnodes );
      bool setLaw( const QModelIndex& ilaw, int nbnodes, double coeff, KindLaw type );
      bool  removeLaw( const QModelIndex& law );
      bool setPropagation( const QModelIndex& iPropagation, const QModelIndex& iLaw, bool way );
      QModelIndexList getPropagation( const QModelIndex& iPropagation ) const;

      // tools
      HEXA_NS::Document* documentImpl();
      QString            documentEntry();

      signals:
      void patternDataChanged();
      void nameChanged(const QString& name);

    private:
      QTemporaryFile    *_hexaFile;
      HEXA_NS::Document *_hexaDocument;
      QString            _entry;

      //geom
      QMap<QString, QString> docShapesEntry;
      QMap<QString, QString> docShapesName;
      QMap<QString, HEXA_NS::SubShape*> shapeById;

      QMap<QString, QString> _assocName; // clé: id; valeur: nom
      bool              _disallowEdition;

      //data
      QStandardItem     *_vertexDirItem;
      QStandardItem     *_edgeDirItem;
      QStandardItem     *_quadDirItem;
      QStandardItem     *_hexaDirItem;

      //builder
      QStandardItem     *_vectorDirItem;
      QStandardItem     *_elementsDirItem;
      QStandardItem     *_crossElementsDirItem;

      //geometry
      QStandardItem     *_explicitShapesDirItem;
      QStandardItem     *_implicitShapesDirItem;
      QStandardItem     *_cloudOfPointsDirItem;

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


//    class  PatternBuilderModel : public QSortFilterProxyModel
//    {
//    public:
//      PatternBuilderModel( QObject * parent = 0 );
//      virtual ~PatternBuilderModel();
//
//      virtual Qt::ItemFlags flags(const QModelIndex &index) const;
//      virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
//      QStandardItem * itemFromIndex ( const QModelIndex & index ) const;
//    };

    class PatternGeomModel : public QSortFilterProxyModel
    {
    public:
        PatternGeomModel( QObject* parent = 0);
        virtual ~PatternGeomModel();

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

      QModelIndexList getGroupElements( const QModelIndex& iGroup, DocumentModel::Group& kind ) const;
    };


    class  MeshModel : public QSortFilterProxyModel
    {
    public:
      MeshModel( QObject * parent = 0 );
      virtual ~MeshModel();

      virtual Qt::ItemFlags flags(const QModelIndex &index) const;
      virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
      QStandardItem * itemFromIndex ( const QModelIndex & index ) const;


      QModelIndexList getPropagation( const QModelIndex& iPropagation ) const;
    }; 

  }
}

#endif
