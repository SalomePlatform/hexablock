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

//CS_TODO: relever les fonctions qui nécessitent updateData().
//        addGroupElement à tester
#include <algorithm>
#include <string>

#include "utilities.h"

#include "HEXABLOCKGUI_DocumentModel.hxx"
#include "HEXABLOCKGUI_DocumentItem.hxx"

#include "HEXABLOCKGUI.hxx"
#include "HEXABLOCKGUI_Trace.hxx"

#include "HexVertex.hxx"
#include "HexShape.hxx"
#include "HexSubShape.hxx"
#include "HexAssoEdge.hxx"
#include "HexBiCylinder.hxx"

#include <TopoDS.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>

#include "HEXABLOCKGUI_SalomeTools.hxx"

#include <SALOME_ListIO.hxx>
#include <SALOMEconfig.h>
#include <SUIT_ViewWindow.h>

#include <qglobal.h>

#include "HEXABLOCKGUI_VtkDocumentGraphicView.hxx"

#include <Geom_Circle.hxx>
#include <gp_Circ.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS_Edge.hxx>
//#define _DEVDEBUG_


using namespace std;
//using namespace HEXA_NS;
using namespace HEXABLOCK::GUI;

/*****************************************************************
                      DocumentModel
 *****************************************************************/
// DocumentModel::DocumentModel(QObject * parent):
DocumentModel::DocumentModel(HEXA_NS::Document* docIn, const QString& entryIn, QObject * parent):
                              QStandardItemModel(parent),
                              _hexaFile( new QTemporaryFile() ),
                              _hexaDocument( docIn ),
                              _entry( entryIn ),
                              //   _hexaDocument(  new HEXA_NS::Document("/tmp/doc.hex") ), //CS_TODO

                              _vertexDirItem( new QStandardItem(tr("TREE_ITEM_VERTEX")) ),
                              _edgeDirItem( new QStandardItem(tr("TREE_ITEM_EDGE")) ),
                              _quadDirItem( new QStandardItem(tr("TREE_ITEM_QUAD")) ),
                              _hexaDirItem( new QStandardItem(tr("TREE_ITEM_HEXA")) ),

                              _vectorDirItem( new QStandardItem(tr("TREE_ITEM_VECTOR")) ),
                              _elementsDirItem( new QStandardItem(tr("TREE_ITEM_ELEMENT")) ),
                              _crossElementsDirItem( new QStandardItem(tr("TREE_ITEM_CROSSELEMENT")) ),

                              _explicitShapesDirItem( new QStandardItem(tr("TREE_ITEM_EXPSHAPE")) ),
                              _implicitShapesDirItem( new QStandardItem(tr("TREE_ITEM_IMPSHAPE")) ),
                              _cloudOfPointsDirItem( new QStandardItem(tr("TREE_ITEM_CLOUD_OF_POINT")) ),

                              _groupDirItem( new QStandardItem(tr("TREE_ITEM_GROUP")) ),

                              _lawDirItem( new QStandardItem(tr("TREE_ITEM_LAW")) ),
                              _propagationDirItem( new QStandardItem(tr("TREE_ITEM_PROPAGATION")) ),

                              _vertexItemFlags( Qt::NoItemFlags ),
                              _edgeItemFlags( Qt::NoItemFlags ),
                              _quadItemFlags( Qt::NoItemFlags ),
                              _hexaItemFlags( Qt::NoItemFlags ),
                              _vectorItemFlags( Qt::NoItemFlags ),
                              _elementsItemFlags( Qt::NoItemFlags ),
                              _crossElementsItemFlags( Qt::NoItemFlags ),

                              _disallowEdition( false )
{
    //   setColumnCount( 4 ); //CS_TEST
    if ( !_hexaDocument && _hexaFile->open() ){
        _hexaDocument =  new HEXA_NS::Document( _hexaFile->fileName().toLatin1() );
        _hexaDocument->reorderFaces();
    }

    QStandardItem *parentItem = invisibleRootItem();
    //   parentItem->setData( QString::number( reinterpret_cast<intptr_t>(_hexaDocument) ), HEXA_ENTRY_ROLE );
    parentItem->setData( _entry, HEXA_ENTRY_ROLE );

    _vertexDirItem->setData( VERTEX_DIR_TREE,           HEXA_TREE_ROLE );
    _edgeDirItem->setData( EDGE_DIR_TREE,               HEXA_TREE_ROLE );
    _quadDirItem->setData( QUAD_DIR_TREE,               HEXA_TREE_ROLE );
    _hexaDirItem->setData( HEXA_DIR_TREE,               HEXA_TREE_ROLE );

    _vectorDirItem->setData( VECTOR_DIR_TREE,           HEXA_TREE_ROLE );
    _elementsDirItem->setData( ELEMENTS_DIR_TREE,           HEXA_TREE_ROLE );
    _crossElementsDirItem->setData( CROSSELEMENTS_DIR_TREE, HEXA_TREE_ROLE );

    _explicitShapesDirItem->setData( EXPLICIT_SHAPES_DIR_TREE, HEXA_TREE_ROLE );
    _implicitShapesDirItem->setData( IMPLICIT_SHAPES_DIR_TREE, HEXA_TREE_ROLE );
    _cloudOfPointsDirItem->setData( CLOUD_OF_POINTS_DIR_TREE, HEXA_TREE_ROLE );

    _groupDirItem->setData( GROUP_DIR_TREE, HEXA_TREE_ROLE );

    //CS_TODO associations

    _lawDirItem->setData( LAW_DIR_TREE, HEXA_TREE_ROLE );
    _propagationDirItem->setData( PROPAGATION_DIR_TREE, HEXA_TREE_ROLE );

    parentItem->appendRow(_vertexDirItem);
    parentItem->appendRow(_edgeDirItem);
    parentItem->appendRow(_quadDirItem);
    parentItem->appendRow(_hexaDirItem);

    parentItem->appendRow(_vectorDirItem);
    parentItem->appendRow(_elementsDirItem);
    parentItem->appendRow(_crossElementsDirItem);

    parentItem->appendRow(_explicitShapesDirItem);
    parentItem->appendRow(_implicitShapesDirItem);

    parentItem->appendRow(_groupDirItem);
    parentItem->appendRow(_lawDirItem);
    parentItem->appendRow(_propagationDirItem);
}

DocumentModel::~DocumentModel()
{
    if (_hexaDocument != NULL)
        delete _hexaDocument;

    if (_hexaFile != NULL)
        delete _hexaFile;
}

void DocumentModel::setName(const QString& name)
{
    _hexaDocument->setName( name.toLocal8Bit().constData() );
    emit nameChanged(QString(_hexaDocument->getName()));
}

HEXA_NS::EltBase* DocumentModel::getHexaPtr(const QModelIndex& iElt)
{
    HEXA_NS::EltBase *elt = NULL;
    switch ( /*data(iElt, HEXA_TREE_ROLE).toInt()*/iElt.data(HEXA_TREE_ROLE).toInt() ){
    case VERTEX_TREE : elt = getHexaPtr<HEXA_NS::Vertex*>(iElt); break;
    case EDGE_TREE : elt = getHexaPtr<HEXA_NS::Edge*>(iElt); break;
    case QUAD_TREE : elt = getHexaPtr<HEXA_NS::Quad*>(iElt); break;
    case HEXA_TREE : elt = getHexaPtr<HEXA_NS::Hexa*>(iElt); break;
    case VECTOR_TREE : elt = getHexaPtr<HEXA_NS::Vector*>(iElt); break;
    case ELEMENTS_TREE : elt = getHexaPtr<HEXA_NS::Elements*>(iElt); break;
//  case CROSSELEMENTS_TREE : elt = getHexaPtr<HEXA_NS::CrossElements*>(iElt); break;
    case GEOMSHAPE_TREE: elt = getHexaPtr<HEXA_NS::NewShape*>(iElt); break;
    case GEOMPOINT_TREE: elt = getHexaPtr<HEXA_NS::VertexShape*>(iElt); break;
    case GEOMEDGE_TREE:  elt = getHexaPtr<HEXA_NS::EdgeShape*>(iElt); break;
    case GEOMFACE_TREE:  elt = getHexaPtr<HEXA_NS::FaceShape*>(iElt); break;
    }
    return elt;
}

//get the number of elements of type 'eltType' in the document
int DocumentModel::getNbrElt(HEXA_NS::EnumElt eltType)
{
    if (_hexaDocument == NULL) return 0;

   return _hexaDocument->getNbrElt(eltType);
}

//get the number of elements of type 'eltType' used in the document
int DocumentModel::getNbrUsedElt(HEXA_NS::EnumElt eltType)
{
    if (_hexaDocument == NULL) return 0;

    switch(eltType) {
    case HEXA_NS::EL_VERTEX:
        return _hexaDocument->countUsedVertex();
    case HEXA_NS::EL_EDGE:
        return _hexaDocument->countUsedEdge();
    case HEXA_NS::EL_QUAD:
        return _hexaDocument->countUsedQuad();
    case HEXA_NS::EL_HEXA:
        return _hexaDocument->countUsedHexa();
    default: return 0;
    }
}

//associate a shape to the current document
bool DocumentModel::addShape(TopoDS_Shape& aShape, QString& name, bool publish)
{
    if (aShape.IsNull() || name.isEmpty())
        return false;
    bool ok = (_hexaDocument != NULL ? _hexaDocument->addShape(aShape, name.toStdString().c_str()) != NULL : false);

    // * publish the shape in the study
    if (ok && publish)
    {
        HEXABLOCKGUI* module = HEXABLOCKGUI::getInstance();
        if (module != NULL)
        {
            QMap<QString, TopoDS_Shape> topo_shapes;
            topo_shapes[name] = aShape;
            module->addInStudy(topo_shapes, docShapesEntry, docShapesName);
        }
    }

    // * update data
    if (ok)
        updateGeomTree();

    return ok;
}

//get the number of unused elements of type 'eltType' in the document
int DocumentModel::getNbrUnusedElt(HEXA_NS::EnumElt eltType)
{
    if (_hexaDocument == NULL)
        return 0;

    return getNbrElt(eltType) - getNbrUsedElt(eltType);
}

// compute the length of the given edge
double DocumentModel::getLength(const QModelIndex& iEdge)
{
    // * Get the pointer to the edge
    HEXA_NS::Edge*        edge     = getHexaPtr<HEXA_NS::Edge*>(iEdge);
    HEXA_NS::EdgeShape*   geomEdge = getHexaPtr<HEXA_NS::EdgeShape*>(iEdge);

    // * The edge can be from VTK or OCC View
    if (edge != NULL)
        return edge->getLength();
    else if (geomEdge != NULL)
        return geomEdge->getLength();
    else
        return 0.;
}

// Compute the radius of the given edge
double DocumentModel::getRadius(const QModelIndex& iEdge)
{
    // * Get the pointer to the edge
    // * In our case only an OCC edge can have a radius
    HEXA_NS::EdgeShape* edge = getHexaPtr<HEXA_NS::EdgeShape*>(iEdge);
    if (edge == NULL)
        return 0.;

    return edge->getRadius();
}

// Compute the angle of the given edge (internal angle)
double DocumentModel::getAngle(const QModelIndex& iEdge)
{
    // * Get the pointer to the edge
    HEXA_NS::EdgeShape* edge = getHexaPtr<HEXA_NS::EdgeShape*>(iEdge);
    if (edge == NULL)
        return 0.;

    return edge->getAngle();
}

//Load the current Document
void DocumentModel::load()
{
    if (_hexaDocument == NULL)
        return;

    load(_hexaDocument->getName());
}


void DocumentModel::load( const QString& xmlFileName ) // Fill Data
{
    _hexaDocument->loadXml(xmlFileName.toLocal8Bit().constData() );
    clearAll();

    fillData();
//    fillBuilder();
    fillGeometry();
    fillAssociation();
    fillGroups();
    fillMesh();

    emit patternDataChanged();
}

bool DocumentModel::isEmpty() const
{
    if (_hexaDocument == NULL) return true;
    return _hexaDocument->isEmpty();
}

void DocumentModel::save( const QString& xmlFileName )
{
    _hexaDocument->save( xmlFileName.toLocal8Bit().constData() );
}

QString DocumentModel::getName()
{
    return _hexaDocument->getName();
}

void DocumentModel::updateData()
{
    clearData();
    fillData();
    clearMesh();
    fillMesh();
    emit patternDataChanged();
}

void DocumentModel::updateGeomTree()
{
    clearGeometry();
    fillGeometry();
}

QModelIndex DocumentModel::addElementsToTree(HEXA_NS::Elements* helts, QStandardItem* tree)
{
    QModelIndex eltIndex;
    if (helts == NULL || tree == NULL)
        return eltIndex;

    updateData();
    ElementsItem* eltsItem = new ElementsItem(helts);
    tree->appendRow(eltsItem);

    return eltsItem->index();
}

void DocumentModel::refresh()
{
    clearAll();

    fillData();
//    fillBuilder();
    fillGeometry();
    fillAssociation();
    fillGroups();
    fillMesh();

    emit patternDataChanged();
}

void DocumentModel::clearAll()
{
    clearData();
//    clearBuilder();
    clearGeometry();
    clearAssociation();
    clearGroups();
    clearMesh();
}

void DocumentModel::clearData()
{
    _vertexDirItem->removeRows( 0, _vertexDirItem->rowCount() );
    _edgeDirItem->removeRows(0, _edgeDirItem->rowCount() );
    _quadDirItem->removeRows(0, _quadDirItem->rowCount() );
    _hexaDirItem->removeRows(0, _hexaDirItem->rowCount() );

    _vectorDirItem->removeRows(0, _vectorDirItem->rowCount() );
    _elementsDirItem->removeRows(0, _elementsDirItem->rowCount() );
    _crossElementsDirItem->removeRows(0, _crossElementsDirItem->rowCount() );
}


//void DocumentModel::clearBuilder()
//{
//    _vectorDirItem->removeRows(0, _vectorDirItem->rowCount() );
//    _elementsDirItem->removeRows(0, _elementsDirItem->rowCount() );
//    _crossElementsDirItem->removeRows(0, _crossElementsDirItem->rowCount() );
//}

void DocumentModel::clearGeometry()
{
    _explicitShapesDirItem->removeRows(0, _explicitShapesDirItem->rowCount() );
    _implicitShapesDirItem->removeRows(0, _implicitShapesDirItem->rowCount() );
    _cloudOfPointsDirItem->removeRows(0, _cloudOfPointsDirItem->rowCount() );
}

void DocumentModel::clearAssociation()
{
    //CS_TODO
}

void DocumentModel::clearGroups()
{
    _groupDirItem->removeRows(0, _groupDirItem->rowCount() );
}

void DocumentModel::clearMesh()
{
    _lawDirItem->removeRows(0, _lawDirItem->rowCount() );
    _propagationDirItem->removeRows(0, _propagationDirItem->rowCount() );
}


void DocumentModel::fillData()
{
    PatternDataSelectionModel* pdsm = HEXABLOCKGUI::currentDocGView->getPatternDataSelectionModel();
    if (pdsm == NULL)
        return;
    pdsm->clearHighlightedItems();

    // DATA
    HEXA_NS::Vertex *v     = NULL;
    VertexItem      *vItem = NULL;
    int nbVertex = _hexaDocument->countVertex();
    for ( int i=0; i<nbVertex; ++i ){
        v = _hexaDocument->getVertex(i);
        vItem = new VertexItem(v, _entry);
        _vertexDirItem->appendRow(vItem);
    }

    HEXA_NS::Edge *e     = NULL;
    EdgeItem      *eItem = NULL;
    int nbEdge = _hexaDocument->countEdge();
    for ( int i=0; i<nbEdge; ++i ){
        e = _hexaDocument->getEdge(i);
        eItem = new EdgeItem(e, _entry);
        _edgeDirItem->appendRow(eItem);
    }

    HEXA_NS::Quad *q     = NULL;
    QuadItem      *qItem = NULL;
    int nbQuad = _hexaDocument->countQuad();
    for ( int i=0; i<nbQuad; ++i ){
        q = _hexaDocument->getQuad(i);
        qItem = new QuadItem(q, _entry);
        _quadDirItem->appendRow(qItem);
    }

    HEXA_NS::Hexa *h     = NULL;
    HexaItem      *hItem = NULL;
    int nbHexa = _hexaDocument->countHexa();
    for ( int i=0; i<nbHexa; ++i ){
        h = _hexaDocument->getHexa(i);
        hItem = new HexaItem(h, _entry);
        _hexaDirItem->appendRow(hItem);
    }

    // +++++ Builder ++++++++++++++++++++++++++++++

    HEXA_NS::Vector *vec     = NULL;
    VectorItem      *vecItem = NULL;
    int nbVector = _hexaDocument->countVector();
    for ( int i=0; i<nbVector; ++i ){
        vec = _hexaDocument->getVector(i);
        vecItem = new VectorItem(vec);
        _vectorDirItem->appendRow(vecItem);
    }

    //******* A VOIR AVEC ALAIN POUR LE REMPLISSAGE DE L'ARBRE ELEMENTS ET CROSSELEMENTS (ByCylinder) *******/
    //******* OU SONT STOCKES LES ELEMENTS ET LES CROSSELEMENTS DANS LE DOCUMENT?                     ******/

    //   _cylinderDirItem
    //    HEXA_NS::Cylinder *c     = NULL;
    //    HEXA_NS::Elements* c = NULL;
    //    CylinderItem      *cItem = NULL;
    //    ElementsItem* cItem = NULL;
    //    int nbCylinder = _hexaDocument->countCylinder();
    //    for ( int i=0; i<nbCylinder; ++i ){
    //        c = _hexaDocument->getCylinder(i);
    //        cItem = new CylinderItem(c, _entry);
    //        cItem = new ElementsItem(c, _entry);
    //        _cylinderDirItem->appendRow(cItem);
    //        _elementsDirItem->appendRow(cItem);
    //    }

    //   _pipeDirItem
    //    HEXA_NS::Pipe *p     = NULL;
    //    HEXA_NS::Elements* p = NULL;
    //    PipeItem      *pItem = NULL;
    //    ElementsItem* pItem = NULL;
    //    int nbPipe = _hexaDocument->countPipe();
    //    for ( int i=0; i<nbPipe; ++i ){
    //        p = _hexaDocument->getPipe(i);
    //        pItem = new PipeItem(p);
    //        pItem = new ElementsItem(p, _entry);
    //        _pipeDirItem->appendRow(pItem);
    //        _elementsDirItem->appendRow(pItem);
    //    }
    //********************************************************************************************************
}


//void DocumentModel::fillBuilder()
//{
//    HEXA_NS::Vector *v     = NULL;
//    VectorItem      *vItem = NULL;
//    int nbVector = _hexaDocument->countVector();
//    for ( int i=0; i<nbVector; ++i ){
//        v = _hexaDocument->getVector(i);
//        vItem = new VectorItem(v);
//        _vectorDirItem->appendRow(vItem);
//    }
//
//    //******* A VOIR AVEC ALAIN POUR LE REMPLISSAGE DE L'ARBRE ELEMENTS ET CROSSELEMENTS (ByCylinder) *******/
//    //******* OU SONT STOCKES LES ELEMENTS ET LES CROSSELEMENTS DANS LE DOCUMENT?                     ******/
//
//    //   _cylinderDirItem
////    HEXA_NS::Cylinder *c     = NULL;
////    HEXA_NS::Elements* c = NULL;
////    CylinderItem      *cItem = NULL;
////    ElementsItem* cItem = NULL;
////    int nbCylinder = _hexaDocument->countCylinder();
////    for ( int i=0; i<nbCylinder; ++i ){
////        c = _hexaDocument->getCylinder(i);
////        cItem = new CylinderItem(c, _entry);
////        cItem = new ElementsItem(c, _entry);
////        _cylinderDirItem->appendRow(cItem);
////        _elementsDirItem->appendRow(cItem);
////    }
//
//    //   _pipeDirItem
////    HEXA_NS::Pipe *p     = NULL;
////    HEXA_NS::Elements* p = NULL;
////    PipeItem      *pItem = NULL;
////    ElementsItem* pItem = NULL;
////    int nbPipe = _hexaDocument->countPipe();
////    for ( int i=0; i<nbPipe; ++i ){
////        p = _hexaDocument->getPipe(i);
////        pItem = new PipeItem(p);
////        pItem = new ElementsItem(p, _entry);
////        _pipeDirItem->appendRow(pItem);
////        _elementsDirItem->appendRow(pItem);
////    }
//    //********************************************************************************************************
//}

void DocumentModel::fillGeometry()
{
    PatternGeomSelectionModel* pgsm = HEXABLOCKGUI::currentDocGView->getPatternGeomSelectionModel();
    if (_hexaDocument == NULL /*|| isEmpty()*/ || pgsm == NULL)
        return;

    HEXA_NS::NewShape* shape;
    HEXA_NS::VertexShape* node;
    HEXA_NS::EdgeShape* line;
    HEXA_NS::FaceShape* face;
    TopoDS_Compound vertexCompound;
    BRep_Builder compoundBuilder;

    // * fill the shape tree

    shapeById.clear();
    pgsm->clearHighlightedItems();
    QMap<QString, TopoDS_Shape> topo_shapes;
    int nbShapes = _hexaDocument->countShape();
    for (int i=0; i < nbShapes; ++i)
    {
        shape = _hexaDocument->getShape(i);
        QString shapeName = shape->getName();
        if (!docShapesEntry.contains(shapeName))
        {
            if (shape->getOrigin() == HEXA_NS::SH_CLOUD)
            {
                compoundBuilder.MakeCompound(vertexCompound);
                topo_shapes[shapeName] = vertexCompound;
            }
            else
                topo_shapes[shapeName] = shape->getShape();
        }

        GeomShapeItem* shItem = new GeomShapeItem(shape);
        if (shape->getOrigin() == HEXA_NS::SH_IMPORT)
            // * explicit shapes
            _explicitShapesDirItem->appendRow(shItem);
        else
            // * implicit shapes
            _implicitShapesDirItem->appendRow(shItem);

        //add vertices to the tree
        QStandardItem* geomPointDirItem = NULL;
        if (shape->getOrigin() != HEXA_NS::SH_CLOUD)
        {
            geomPointDirItem = new QStandardItem(tr("TREE_ITEM_VERTEX"));
            geomPointDirItem->setData( GEOMPOINT_DIR_TREE, HEXA_TREE_ROLE );
        }
        if (geomPointDirItem != NULL)
            shItem->appendRow(geomPointDirItem);
        int nbVertexShape = shape->countVertex();
        for (int j=0; j < nbVertexShape; ++j)
        {
            node = shape->getVertexShape(j);
            GeomPointItem* gPointItem = new GeomPointItem(node);
            if (geomPointDirItem != NULL)
                geomPointDirItem->appendRow(gPointItem);
            else
                shItem->appendRow(gPointItem);
            shapeById[shapeName+","+QString::number(node->getIdent())] = node;

            // * update the compound of vertices
            if (shape->getOrigin() == HEXA_NS::SH_CLOUD && !vertexCompound.IsNull())
                compoundBuilder.Add(topo_shapes[shapeName], node->getShape());
        }

        if (shape->getOrigin() == HEXA_NS::SH_CLOUD)
            continue; // * it's a cloud of points, there's no edges and no quads to fill

        //add edges to the tree
        QStandardItem* geomEdgeDirItem = new QStandardItem(tr("TREE_ITEM_EDGE"));
        geomEdgeDirItem->setData( GEOMEDGE_DIR_TREE, HEXA_TREE_ROLE );
        shItem->appendRow(geomEdgeDirItem);
        int nbEdgeShape = shape->countEdge();
        for (int j = 0; j < nbEdgeShape; ++j)
        {
            line = shape->getEdgeShape(j);
            GeomEdgeItem* gEdgeItem = new GeomEdgeItem(line);
            geomEdgeDirItem->appendRow(gEdgeItem);
            shapeById[shapeName+","+QString::number(line->getIdent())] = line;
        }

        //add faces to the tree
        QStandardItem* geomFaceDirItem = new QStandardItem(tr("TREE_ITEM_QUAD"));
        geomFaceDirItem->setData( GEOMFACE_DIR_TREE, HEXA_TREE_ROLE );
        shItem->appendRow(geomFaceDirItem);
        int nbFaceShape = shape->countFace();
        for (int j = 0; j < nbFaceShape; ++j)
        {
            face = shape->getFaceShape(j);
            GeomFaceItem* gFaceItem = new GeomFaceItem(face);
            geomFaceDirItem->appendRow(gFaceItem);
            shapeById[shapeName+","+QString::number(face->getIdent())] = face;
        }
    }

    if (topo_shapes.size() == 0)
        return;

    // * register shapes
    HEXABLOCKGUI* module = HEXABLOCKGUI::getInstance();
    if (module != NULL)
        module->addInStudy(topo_shapes, docShapesEntry, docShapesName);
}

void DocumentModel::fillAssociation()
{
}

//------------------------------------------- NEW ITEMS
//void DocumentModel::fillGroups()
//{
//  HEXA_NS::Group *g     = NULL;
//  GroupItem      *gItem = NULL;
//  int nbGroup = _hexaDocument->countGroup();
//  for ( int i=0; i<nbGroup; ++i ){
//    g = _hexaDocument->getGroup(i);
//    gItem = new GroupItem(g, _entry);
//    _groupDirItem->appendRow(gItem);
//  }
//}
//
//void DocumentModel::fillMesh()
//{
//  HEXA_NS::Law *l     = NULL;
//  LawItem      *lItem = NULL;
//  int nbLaw = _hexaDocument->countLaw();
//  for ( int i=0; i<nbLaw; ++i ){
//    l = _hexaDocument->getLaw(i);
//    lItem = new LawItem(l);
//    _lawDirItem->appendRow(lItem);
//  }
//
//  HEXA_NS::Propagation *p     = NULL;
//  PropagationItem      *pItem = NULL;
//  int nbPropagation = _hexaDocument->countPropagation();
//  for ( int i=0; i<nbPropagation; ++i ){
//    p = _hexaDocument->getPropagation(i);
//    pItem = new PropagationItem(p, _entry);
//    pItem->setText(QString("Propagation%1").arg(i) );
//    _propagationDirItem->appendRow(pItem);
//  }
//}
//----------------------------------------------- END NEW ITEMS

void DocumentModel::fillGroups()
{
    HEXA_NS::Group *g     = NULL;
    GroupItem      *gItem = NULL;
    int nbGroup = _hexaDocument->countGroup();
    for ( int i=0; i<nbGroup; ++i ){
        g = _hexaDocument->getGroup(i);
        //std::cout<<"getGroup => "<< i << std::endl;
        gItem = new GroupItem(g);
        gItem->setData( _entry, HEXA_DOC_ENTRY_ROLE );
        _groupDirItem->appendRow(gItem);
    }
}

void DocumentModel::fillMesh()
{
    //   _lawDirItem
    HEXA_NS::Law *l     = NULL;
    LawItem      *lItem = NULL;
    int nbLaw = _hexaDocument->countLaw();
    for ( int i=0; i<nbLaw; ++i ){
        l = _hexaDocument->getLaw(i);
        lItem = new LawItem(l);
        _lawDirItem->appendRow(lItem);
    }

    //   _propagationDirItem
    HEXA_NS::Propagation *p     = NULL;
    PropagationItem      *pItem = NULL;
    int nbPropagation = _hexaDocument->countPropagation();
    for ( int i=0; i<nbPropagation; ++i ){
        p = _hexaDocument->getPropagation(i);
        pItem = new PropagationItem(p);
        pItem->setText(QString("Propagation%1").arg(i) );
        pItem->setData( _entry, HEXA_DOC_ENTRY_ROLE );
        _propagationDirItem->appendRow(pItem);
    }
}

HEXA_NS::Hexa* DocumentModel::getQuadHexa(HEXA_NS::Quad* quad)
{
    HEXA_NS::Hexa* hexa;
    int nbHexa = _hexaDocument->countHexa();
    for ( int i=0; i<nbHexa; ++i ){
        hexa = _hexaDocument->getHexa(i);
        if (hexa->findQuad(quad) > -1) return hexa;
    }
    return NULL;
}


Qt::ItemFlags DocumentModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags;

    if (!index.isValid()) return Qt::ItemIsEnabled;

    if ( _disallowEdition ){
        return QAbstractItemModel::flags(index) | Qt::ItemFlags( ~Qt::ItemIsEditable );
    } else {
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    }
}

void DocumentModel::allowEdition()
{
    _disallowEdition = false;
}

void DocumentModel::disallowEdition()
{
    _disallowEdition = true;
}

void DocumentModel::allowDataSelectionOnly()
{
    _vertexItemFlags = Qt::ItemFlags( ~Qt::ItemIsEditable );
    _edgeItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEditable );
    _quadItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEditable );
    _hexaItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEditable );

    _vectorItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _elementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _crossElementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );

    _groupItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _lawItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _propagationItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
}

void DocumentModel::allowVertexSelectionOnly()
{
    _vertexItemFlags = Qt::ItemFlags( ~Qt::ItemIsEditable);
    _edgeItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEditable );
    _quadItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEditable );
    _hexaItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEditable );

    _vectorItemFlags = Qt::ItemFlags( ~Qt::ItemIsEditable );
    _elementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEditable );
    _crossElementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEditable );

    _groupItemFlags = Qt::ItemFlags( ~Qt::ItemIsEditable );
    _lawItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEditable );
    _propagationItemFlags = Qt::ItemFlags( ~Qt::ItemIsEditable );

    //     emit layoutChanged();
}

void DocumentModel::allowEdgeSelectionOnly()
{
    _vertexItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _edgeItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEditable );
    _quadItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _hexaItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );

    _vectorItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _elementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _crossElementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );

    _groupItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _lawItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _propagationItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
}

void DocumentModel::allowQuadSelectionOnly()
{
    _vertexItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _edgeItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _quadItemFlags = Qt::ItemFlags( ~Qt::ItemIsEditable );
    _hexaItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );

    _vectorItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _elementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _crossElementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );

    _groupItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _lawItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _propagationItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
}

void DocumentModel::allowHexaSelectionOnly()
{
    _vertexItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _edgeItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _quadItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _hexaItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEditable );

    _vectorItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _elementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _crossElementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );

    _groupItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _lawItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _propagationItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
}

void DocumentModel::allowVectorSelectionOnly()
{
    _vertexItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _edgeItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _quadItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _hexaItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );

    _vectorItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEditable );
    _elementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _crossElementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );

    _groupItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _lawItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _propagationItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
}

void DocumentModel::allowCylinderSelectionOnly()
{
    _vertexItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _edgeItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _quadItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _hexaItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );

    _vectorItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _elementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _crossElementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );

    _groupItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _lawItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _propagationItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
}

void DocumentModel::allowPipeSelectionOnly()
{
    _vertexItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _edgeItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _quadItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _hexaItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );

    _vectorItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _elementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _crossElementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );

    _groupItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _lawItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _propagationItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
}

void DocumentModel::allowElementsSelectionOnly()
{
    _vertexItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _edgeItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _quadItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _hexaItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );

    _vectorItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _elementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEditable );
    _crossElementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );

    _groupItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _lawItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _propagationItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
}

/**********************************************************
void DocumentModel::allowCrossElementsSelectionOnly()
{
    _vertexItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _edgeItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _quadItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _hexaItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );

    _vectorItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _elementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _crossElementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEditable );

    _groupItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _lawItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _propagationItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
}
 ********************************************************** */

void DocumentModel::allowLawSelectionOnly()
{
    _vertexItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _edgeItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _quadItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _hexaItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );

    _vectorItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _elementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _crossElementsItemFlags = Qt::ItemFlags( Qt::ItemIsEnabled );

    _groupItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _lawItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEditable );
    _propagationItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
}


void DocumentModel::setName( const QModelIndex& iElt, const QString& name )
{
    HEXA_NS::EltBase *elt = NULL;

    switch ( data(iElt, HEXA_TREE_ROLE).toInt() ){
    case GROUP_TREE :
    {
        HEXA_NS::Group* grp = iElt.data( HEXA_DATA_ROLE ).value< HEXA_NS::Group* >();
        grp->setName( name.toLatin1().data() );
        break;
    }
    case LAW_TREE :
    {
        HEXA_NS::Law* l = iElt.data( HEXA_DATA_ROLE ).value< HEXA_NS::Law* >();
        l->setName( name.toLatin1().data() );
        break;
    }
    //   case PROPAGATION_TREE : elt = iElt.data( HEXA_DATA_ROLE ).value< HEXA_NS::Propagation* >(); break;
    default: elt = getHexaPtr(iElt);
    }

    if ( elt != NULL ) elt->setName( name.toStdString() );
    setData( iElt, name );

}

bool DocumentModel::clearEltAssociations( const QModelIndex& iElt )
{
    bool isOk = false;
    HEXA_NS::EltBase *elt = getHexaPtr(iElt);

    if ( elt != NULL ){
        elt->clearAssociation();
        setData( iElt, QVariant(), HEXA_ASSOC_ENTRY_ROLE );
        isOk = true;
    }

    return isOk;
}

void DocumentModel::clearAssociation(HEXA_NS::EnumElt& eltType)
{
    if (_hexaDocument != NULL)
        _hexaDocument->clearAssociation(eltType);

    QModelIndex iElt;
    HEXA_NS::Vertex* vertex;
    HEXA_NS::Edge* edge;
    HEXA_NS::Quad* quad;
    int nbElts;

    if (HEXABLOCKGUI::currentDocGView == NULL) return;
    PatternDataSelectionModel* pdsm = HEXABLOCKGUI::currentDocGView->getPatternDataSelectionModel();
    PatternDataModel* pdm = HEXABLOCKGUI::currentDocGView->getPatternDataModel();
    if (pdsm == NULL || pdm == NULL) return;

    //update associations in the model
    if (eltType == HEXA_NS::EL_VERTEX)
    {
        nbElts = _hexaDocument->countVertex();
        for (int i = 0; i < nbElts; ++i)
        {
            vertex = _hexaDocument->getVertex(i);
            iElt = pdm->mapToSource(pdsm->indexBy(HEXA_DATA_ROLE, QVariant::fromValue(vertex)));
            if (iElt.isValid())
                setData( iElt, QVariant(), HEXA_ASSOC_ENTRY_ROLE );
        }
    }
    else if (eltType == HEXA_NS::EL_EDGE)
    {
        nbElts = _hexaDocument->countEdge();
        for (int i = 0; i < nbElts; ++i)
        {
            edge = _hexaDocument->getEdge(i);
            iElt = pdm->mapToSource(pdsm->indexBy(HEXA_DATA_ROLE, QVariant::fromValue(edge)));
            if (iElt.isValid())
                setData( iElt, QVariant(), HEXA_ASSOC_ENTRY_ROLE );
        }
    }
    else if (eltType == HEXA_NS::EL_QUAD)
    {
        nbElts = _hexaDocument->countQuad();
        for (int i = 0; i < nbElts; ++i)
        {
            quad = _hexaDocument->getQuad(i);
            iElt = pdm->mapToSource(pdsm->indexBy(HEXA_DATA_ROLE, QVariant::fromValue(quad)));
            if (iElt.isValid())
                setData( iElt, QVariant(), HEXA_ASSOC_ENTRY_ROLE );
        }
    }
}


QModelIndex DocumentModel::addVertex( double x, double y, double z )
{
    QModelIndex vertexIndex;

    HEXA_NS::Vertex* hv = _hexaDocument->addVertex(x, y, z);
    if (BadElement(hv)) return vertexIndex;

    VertexItem* v = new VertexItem(hv, _entry);
    _vertexDirItem->appendRow(v);
    vertexIndex = v->index();
    emit patternDataChanged();

    return vertexIndex;
}

QModelIndex DocumentModel::addEdgeVertices (const QModelIndex &i_v0, const QModelIndex &i_v1 )
{
    QModelIndex edgeIndex;

    HEXA_NS::Vertex* hv0 = getHexaPtr<HEXA_NS::Vertex*>(i_v0);
    HEXA_NS::Vertex* hv1 = getHexaPtr<HEXA_NS::Vertex*>(i_v1);

    HEXA_NS::Edge* he = _hexaDocument->addEdge( hv0, hv1 );
    if ( BadElement(he) ) return edgeIndex;

    EdgeItem* e = new EdgeItem(he, _entry);
    _edgeDirItem->appendRow(e);
    edgeIndex = e->index();
    emit patternDataChanged();

    return edgeIndex;
}

QModelIndex DocumentModel::addEdgeVector( const QModelIndex &i_v, const QModelIndex &i_vec )
{
    QModelIndex edgeIndex;

    HEXA_NS::Vertex* hv   = getHexaPtr<HEXA_NS::Vertex*>(i_v);
    HEXA_NS::Vector* hvec = getHexaPtr<HEXA_NS::Vector*>(i_vec);

    if (hv == NULL || hvec == NULL) return edgeIndex;

    HEXA_NS::Edge* he = _hexaDocument->addEdgeVector( hv, hvec );
    if ( BadElement(he) ) return edgeIndex;

    HEXA_NS::Vertex* hv2 = he->getAval(); //the new vertex resulting from the creation of the edge
    if (hv2 == NULL) return edgeIndex;

    //ADD the edge in the treeview
    EdgeItem* e = new EdgeItem(he, _entry);
    _edgeDirItem->appendRow(e);
    edgeIndex = e->index();

    //The resulting Vertex
    VertexItem* v = new VertexItem(hv2, _entry);
    _vertexDirItem->appendRow(v);

    emit patternDataChanged();

    return edgeIndex;
}

QModelIndex DocumentModel::addQuadVertices( const QModelIndex &i_v0, const QModelIndex &i_v1,
        const QModelIndex &i_v2, const QModelIndex &i_v3 )
{ //CS_TODO : gestion erreur
    QModelIndex quadIndex;

    HEXA_NS::Vertex* hv0 = getHexaPtr<HEXA_NS::Vertex*>(i_v0);
    HEXA_NS::Vertex* hv1 = getHexaPtr<HEXA_NS::Vertex*>(i_v1);
    HEXA_NS::Vertex* hv2 = getHexaPtr<HEXA_NS::Vertex*>(i_v2);
    HEXA_NS::Vertex* hv3 = getHexaPtr<HEXA_NS::Vertex*>(i_v3);

    if ( hv0 && hv1 && hv2 && hv3 ){
        HEXA_NS::Quad* hq = _hexaDocument->addQuadVertices( hv0, hv1, hv2, hv3 );
        if ( BadElement(hq) ) return quadIndex;

        QuadItem* q = new QuadItem(hq, _entry);
        _quadDirItem->appendRow(q);
        quadIndex = q->index();
        emit patternDataChanged();
    }

    return quadIndex;
}

QModelIndex DocumentModel::addQuadEdges( const QModelIndex &e0, const QModelIndex &e1,
        const QModelIndex &e2, const QModelIndex &e3 )
{ //CS_TODO
    QModelIndex quadIndex;

    HEXA_NS::Edge* he0 = getHexaPtr<HEXA_NS::Edge*>(e0);
    HEXA_NS::Edge* he1 = getHexaPtr<HEXA_NS::Edge*>(e1);
    HEXA_NS::Edge* he2 = getHexaPtr<HEXA_NS::Edge*>(e2);
    HEXA_NS::Edge* he3 = getHexaPtr<HEXA_NS::Edge*>(e3);

    if ( he0 && he1 && he2 && he3 ){

        HEXA_NS::Quad* hq = _hexaDocument->addQuad( he0, he1, he2, he3 );
        if ( BadElement(hq) ) return quadIndex;

        QuadItem* q = new QuadItem(hq, _entry);
        _quadDirItem->appendRow(q);
        quadIndex = q->index();
        emit patternDataChanged();
    }

    return quadIndex;
}

QModelIndex DocumentModel::addHexaVertices(
        const QModelIndex &iv0, const QModelIndex &iv1,
        const QModelIndex &iv2, const QModelIndex &iv3,
        const QModelIndex &iv4, const QModelIndex &iv5,
        const QModelIndex &iv6, const QModelIndex &iv7 )
{
    QModelIndex iHexa;

    HEXA_NS::Vertex* hv0 = getHexaPtr<HEXA_NS::Vertex*>(iv0);
    HEXA_NS::Vertex* hv1 = getHexaPtr<HEXA_NS::Vertex*>(iv1);
    HEXA_NS::Vertex* hv2 = getHexaPtr<HEXA_NS::Vertex*>(iv2);
    HEXA_NS::Vertex* hv3 = getHexaPtr<HEXA_NS::Vertex*>(iv3);
    HEXA_NS::Vertex* hv4 = getHexaPtr<HEXA_NS::Vertex*>(iv4);
    HEXA_NS::Vertex* hv5 = getHexaPtr<HEXA_NS::Vertex*>(iv5);
    HEXA_NS::Vertex* hv6 = getHexaPtr<HEXA_NS::Vertex*>(iv6);
    HEXA_NS::Vertex* hv7 = getHexaPtr<HEXA_NS::Vertex*>(iv7);

    HEXA_NS::Hexa* hh = _hexaDocument->addHexaVertices( hv0, hv1, hv2, hv3,
            hv4, hv5, hv6, hv7 );

    if ( BadElement(hh) ) return iHexa;

    HexaItem* h = new HexaItem(hh, _entry);
    _hexaDirItem->appendRow(h);
    iHexa = h->index();
    emit patternDataChanged();

    return iHexa;
}

QModelIndex DocumentModel::addHexaQuad( const QModelIndex &i_q0, const QModelIndex &i_q1,
        const QModelIndex &i_q2,const QModelIndex &i_q3, const QModelIndex &i_q4, const QModelIndex &i_q5 )
{ //CS_TODO : gestion erreur
    QModelIndex hexaIndex;

    HEXA_NS::Quad* hq0 = getHexaPtr<HEXA_NS::Quad*>(i_q0);
    HEXA_NS::Quad* hq1 = getHexaPtr<HEXA_NS::Quad*>(i_q1);
    HEXA_NS::Quad* hq2 = getHexaPtr<HEXA_NS::Quad*>(i_q2);
    HEXA_NS::Quad* hq3 = getHexaPtr<HEXA_NS::Quad*>(i_q3);
    HEXA_NS::Quad* hq4 = getHexaPtr<HEXA_NS::Quad*>(i_q4);
    HEXA_NS::Quad* hq5 = getHexaPtr<HEXA_NS::Quad*>(i_q5);

    HEXA_NS::Hexa* hh = _hexaDocument->addHexa( hq0, hq1, hq2, hq3, hq4, hq5 );
    if ( BadElement(hh) ) return hexaIndex;

    HexaItem* h = new HexaItem(hh, _entry);
    _hexaDirItem->appendRow(h);
    hexaIndex = h->index();
    emit patternDataChanged();

    return hexaIndex;
}

QModelIndex DocumentModel::addHexaQuads( const QModelIndexList &iquads)
{
    QModelIndex hexaIndex;

    HEXA_NS::Hexa* hh = NULL;
    HEXA_NS::Quad* hq0, *hq1, *hq2, *hq3, *hq4, *hq5 = NULL;

    hq0 = getHexaPtr<HEXA_NS::Quad*>(iquads.value(0));
    hq1 = getHexaPtr<HEXA_NS::Quad*>(iquads.value(1));
    hq2 = getHexaPtr<HEXA_NS::Quad*>(iquads.value(2));
    hq3 = getHexaPtr<HEXA_NS::Quad*>(iquads.value(3));
    hq4 = getHexaPtr<HEXA_NS::Quad*>(iquads.value(4));
    hq5 = getHexaPtr<HEXA_NS::Quad*>(iquads.value(5));

    if ( hq0 && hq1 && hq2 && hq3 && hq4 && hq5 )
        hh = _hexaDocument->addHexa( hq0, hq1, hq2, hq3, hq4, hq5 );
    else if ( hq0 && hq1 && hq2 && hq3 && hq4 )
        hh = _hexaDocument->addHexa5Quads( hq0, hq1, hq2, hq3, hq4 );
    else if ( hq0 && hq1 && hq2 && hq3 )
        hh = _hexaDocument->addHexa4Quads( hq0, hq1, hq2, hq3 );
    else if ( hq0 && hq1 && hq2)
        hh = _hexaDocument->addHexa3Quads( hq0, hq1, hq2 );
    else if ( hq0 && hq1 )
        hh = _hexaDocument->addHexa2Quads( hq0, hq1);

    if ( BadElement(hh) ) return hexaIndex;

    updateData();
    unsigned int nbRows = _hexaDirItem->rowCount();
    if (nbRows > 0)
        hexaIndex = _hexaDirItem->child(nbRows-1)->index();

    return hexaIndex;
}

QModelIndex DocumentModel::addVector( double dx, double dy, double dz )
{
    QModelIndex vectorIndex;

    HEXA_NS::Vector* hv = _hexaDocument->addVector(dx, dy, dz);
    if ( BadElement(hv) ) return vectorIndex; //Invalid index

    VectorItem* v = new VectorItem(hv);
    _vectorDirItem->appendRow(v);
    vectorIndex = v->index();

    return vectorIndex;
}

QModelIndex DocumentModel::addVectorVertices( const QModelIndex &iv0, const QModelIndex &iv1 )
{
    QModelIndex iVec;

    HEXA_NS::Vertex* hv0 = getHexaPtr<HEXA_NS::Vertex*>(iv0);
    HEXA_NS::Vertex* hv1 = getHexaPtr<HEXA_NS::Vertex*>(iv1);

    HEXA_NS::Vector* hvec = _hexaDocument->addVectorVertices( hv0, hv1 );
    if ( BadElement(hvec) ) return iVec;

    VectorItem* vec = new VectorItem(hvec);
    _vectorDirItem->appendRow(vec);
    iVec = vec->index();

    return iVec;
}


// ************  EDIT HEXABLOCK MODEL ************

bool DocumentModel::updateVertex( const QModelIndex& ivertex, double x, double y, double z )
{
    bool ret = false;

    HEXA_NS::Vertex* hVertex = getHexaPtr<HEXA_NS::Vertex*>(ivertex);

    if ( hVertex ){
//        hVertex->setName( name.toStdString() );
        hVertex->setX ( x );
        hVertex->setY ( y );
        hVertex->setZ ( z );
        emit patternDataChanged();
        ret = true;
    }

    return ret;
}

bool DocumentModel::removeHexa( const QModelIndex& ihexa )
{
    HEXA_NS::Hexa* hHexa = getHexaPtr<HEXA_NS::Hexa*>(ihexa);

    int r = _hexaDocument->removeHexa( hHexa );
    if ( r == HOK ){
        updateData();
        return true;
    }

    return false;
}

bool DocumentModel::removeConnectedHexa( const QModelIndex& ihexa )
{
    HEXA_NS::Hexa* hHexa = getHexaPtr<HEXA_NS::Hexa*>(ihexa);

    int r = _hexaDocument->removeConnectedHexa( hHexa );
    if ( r == HOK ){
        updateData();
        return true;
    }

    return false;
}

bool DocumentModel::mergeVertices( const QModelIndex &iv0, const QModelIndex &iv1 ) //CS_TODO : impact sur le model?
{
    HEXA_NS::Vertex* hv0 = getHexaPtr<HEXA_NS::Vertex*>(iv0);
    HEXA_NS::Vertex* hv1 = getHexaPtr<HEXA_NS::Vertex*>(iv1);

    int r = _hexaDocument->mergeVertices( hv0, hv1 );
    if ( r == HOK ){
        updateData(); //CS_TODO more or less?
        return true;
    }

    return false;
}

bool DocumentModel::mergeEdges( const QModelIndex &ie0, const QModelIndex &ie1,
        const QModelIndex &iv0, const QModelIndex &iv1 )
//CS_TODO : impact sur le model?
{
    HEXA_NS::Edge* he0 = getHexaPtr<HEXA_NS::Edge*>(ie0);
    HEXA_NS::Edge* he1 = getHexaPtr<HEXA_NS::Edge*>(ie1);

    HEXA_NS::Vertex* hv0 = getHexaPtr<HEXA_NS::Vertex*>(iv0);
    HEXA_NS::Vertex* hv1 = getHexaPtr<HEXA_NS::Vertex*>(iv1);

    //Merge
    int r = _hexaDocument->mergeEdges( he0, he1, hv0, hv1 );
    if ( r == HOK ){
        updateData();
        return true;
    }

    return false;
}

bool DocumentModel::mergeQuads( const QModelIndex& iquad0, const QModelIndex& iquad1,
        const QModelIndex& iv0, const QModelIndex& iv1,
        const QModelIndex& iv2, const QModelIndex& iv3 )
{
    HEXA_NS::Quad* hquad0 = data(iquad0, HEXA_DATA_ROLE).value<HEXA_NS::Quad *>();
    HEXA_NS::Quad* hquad1 = data(iquad1, HEXA_DATA_ROLE).value<HEXA_NS::Quad *>();

    HEXA_NS::Vertex* hv0 = getHexaPtr<HEXA_NS::Vertex*>(iv0);
    HEXA_NS::Vertex* hv1 = getHexaPtr<HEXA_NS::Vertex*>(iv1);
    HEXA_NS::Vertex* hv2 = getHexaPtr<HEXA_NS::Vertex*>(iv2);
    HEXA_NS::Vertex* hv3 = getHexaPtr<HEXA_NS::Vertex*>(iv3);

    int r = _hexaDocument->mergeQuads( hquad0, hquad1, hv0, hv1, hv2, hv3 );
    if ( r == HOK ){
        updateData();
        return true;
    }

    return false;
}

//
QModelIndex DocumentModel::disconnectVertex( const QModelIndex& ihexa, const QModelIndex& ivertex )
{
    QModelIndex iElts;

    HEXA_NS::Hexa*   hHexa   = getHexaPtr<HEXA_NS::Hexa*>(ihexa);
    HEXA_NS::Vertex* hVertex = getHexaPtr<HEXA_NS::Vertex*>(ivertex);

    HEXA_NS::Elements* hElts = _hexaDocument->disconnectVertex( hHexa, hVertex );
    if ( BadElement(hElts) ) return iElts;

    updateData(); //CS_TO_CHECK
    ElementsItem* elts = new ElementsItem(hElts);
    _elementsDirItem->appendRow(elts);
    iElts = elts->index();

    return iElts;
}

QModelIndex DocumentModel::disconnectEdge( const QModelIndex& ihexa, const QModelIndex& iedge )
{
    QModelIndex iElts;

    HEXA_NS::Hexa* hHexa = getHexaPtr<HEXA_NS::Hexa*>(ihexa);
    HEXA_NS::Edge* hEdge = getHexaPtr<HEXA_NS::Edge*>(iedge);

    HEXA_NS::Elements* hElts = _hexaDocument->disconnectEdge( hHexa, hEdge );
    if ( BadElement(hElts) ) return iElts;

    updateData(); //CS_TO_CHECK
    ElementsItem* elts = new ElementsItem(hElts);
    _elementsDirItem->appendRow(elts);
    iElts = elts->index();

    return iElts;
}


QModelIndex DocumentModel::disconnectEdges( const QModelIndexList& ihexas, const QModelIndexList& iedges )
{
    QModelIndex iElts;

    HEXA_NS::Hexas hHexas;
    HEXA_NS::Edges hEdges;

    //edges list
    HEXA_NS::Edge* hedge = NULL;
    foreach( const QModelIndex& iedge, iedges ){
        hedge = getHexaPtr<HEXA_NS::Edge*>(iedge);
        hEdges.push_back( hedge );
    }

    //hexas list
    HEXA_NS::Hexa* hhexa = NULL;
    foreach( const QModelIndex& ihexa, ihexas ){
        hhexa = getHexaPtr<HEXA_NS::Hexa*>(ihexa);
        hHexas.push_back( hhexa );
    }


    HEXA_NS::Elements* hElts = _hexaDocument->disconnectEdges( hHexas, hEdges );
    if ( BadElement(hElts) ) return iElts;

    updateData(); //CS_TO_CHECK
    ElementsItem* elts = new ElementsItem(hElts);
    _elementsDirItem->appendRow(elts);
    iElts = elts->index();

    return iElts;
}


QModelIndex DocumentModel::disconnectQuad( const QModelIndex& ihexa, const QModelIndex& iquad )
{
    QModelIndex iElts;

    HEXA_NS::Hexa* hHexa = getHexaPtr<HEXA_NS::Hexa*>(ihexa);
    HEXA_NS::Quad* hQuad = getHexaPtr<HEXA_NS::Quad*>(iquad);

    HEXA_NS::Elements* hElts = _hexaDocument->disconnectQuad( hHexa, hQuad );
    if ( BadElement(hElts) ) return iElts;

    updateData(); //CS_TO_CHECK
    ElementsItem* elts = new ElementsItem(hElts);
    _elementsDirItem->appendRow(elts);
    iElts = elts->index();

    return iElts;
}


QModelIndex DocumentModel::makeTranslation( const QModelIndex& ielts, const QModelIndex& ivec )
{
    QModelIndex iElts;

    HEXA_NS::Elements* hElts = getHexaPtr<HEXA_NS::Elements*>(ielts);
    HEXA_NS::Vector*   hVec  = getHexaPtr<HEXA_NS::Vector*>(ivec);

    HEXA_NS::Elements* hNewElts = _hexaDocument->makeTranslation( hElts, hVec );
    if ( BadElement(hNewElts) ) return iElts;

    updateData(); //CS_TODO  more or less?
    ElementsItem* eltsItem = new ElementsItem(hNewElts);
    _elementsDirItem->appendRow(eltsItem);
    iElts = eltsItem->index();

    return iElts;
}



QModelIndex DocumentModel::makeScale( const QModelIndex& ielts, const QModelIndex& ivex, double k )
{
    QModelIndex iElts;

    HEXA_NS::Elements* hElts = getHexaPtr<HEXA_NS::Elements*>(ielts);
    HEXA_NS::Vertex*   hVex  = getHexaPtr<HEXA_NS::Vertex*>(ivex);

    HEXA_NS::Elements* hNewElts = _hexaDocument->makeScale( hElts, hVex, k );
    if ( BadElement(hNewElts) ) return iElts;

    updateData(); //CS_TODO  more or less?
    ElementsItem* eltsItem = new ElementsItem(hNewElts);
    _elementsDirItem->appendRow(eltsItem);
    iElts = eltsItem->index();

    return iElts;
}

QModelIndex DocumentModel::makeRotation( const QModelIndex& ielts,
        const QModelIndex& iv,
        const QModelIndex& ivec, double angle )
{
    QModelIndex iElts;

    HEXA_NS::Elements* hElts = getHexaPtr<HEXA_NS::Elements*>(ielts);
    HEXA_NS::Vertex*   hVex  = getHexaPtr<HEXA_NS::Vertex*>(iv);
    HEXA_NS::Vector*   hVec  = getHexaPtr<HEXA_NS::Vector*>(ivec);

    HEXA_NS::Elements* hNewElts = _hexaDocument->makeRotation( hElts, hVex, hVec, angle );
    if ( BadElement(hNewElts) ) return iElts;

    updateData(); //CS_TODO  more or less?
    ElementsItem* eltsItem = new ElementsItem(hNewElts);
    _elementsDirItem->appendRow(eltsItem);
    iElts = eltsItem->index();

    return iElts;
}


QModelIndex DocumentModel::makeSymmetryPoint( const QModelIndex& ielts, const QModelIndex& iv )
{
    QModelIndex iElts;

    HEXA_NS::Elements* hElts = getHexaPtr<HEXA_NS::Elements*>(ielts);
    HEXA_NS::Vertex*   hVex  = getHexaPtr<HEXA_NS::Vertex*>(iv);

    HEXA_NS::Elements* hNewElts = _hexaDocument->makeSymmetryPoint (hElts, hVex);
    if ( BadElement(hNewElts) ) return iElts;

    updateData(); //CS_TODO  more or less?
    ElementsItem* eltsItem = new ElementsItem(hNewElts);
    _elementsDirItem->appendRow(eltsItem);
    iElts = eltsItem->index();

    return iElts;
}


QModelIndex DocumentModel::makeSymmetryLine( const QModelIndex& ielts,
        const QModelIndex& iv,
        const QModelIndex& ivec )
{
    QModelIndex iElts;

    HEXA_NS::Elements* hElts = getHexaPtr<HEXA_NS::Elements*>(ielts);
    HEXA_NS::Vertex*   hVex  = getHexaPtr<HEXA_NS::Vertex*>(iv);
    HEXA_NS::Vector*   hVec  = getHexaPtr<HEXA_NS::Vector*>(ivec);


    HEXA_NS::Elements* hNewElts = _hexaDocument->makeSymmetryLine( hElts, hVex, hVec );
    if ( BadElement(hNewElts) ) return iElts;

    updateData(); //CS_TODO  more or less?
    ElementsItem* eltsItem = new ElementsItem(hNewElts);
    _elementsDirItem->appendRow(eltsItem);
    iElts = eltsItem->index();

    return iElts;
}


QModelIndex DocumentModel::makeSymmetryPlane( const QModelIndex& ielts, const QModelIndex& iv, const QModelIndex& ivec )
{
    QModelIndex iElts;

    HEXA_NS::Elements* hElts = getHexaPtr<HEXA_NS::Elements*>(ielts);
    HEXA_NS::Vertex*   hVex  = getHexaPtr<HEXA_NS::Vertex*>(iv);
    HEXA_NS::Vector*   hVec  = getHexaPtr<HEXA_NS::Vector*>(ivec);

    HEXA_NS::Elements* hNewElts = _hexaDocument->makeSymmetryPlane( hElts, hVex, hVec );
    if ( BadElement(hNewElts) ) return iElts;

    updateData(); //CS_TODO  more or less?
    ElementsItem* eltsItem = new ElementsItem(hNewElts);
    _elementsDirItem->appendRow(eltsItem);
    iElts = eltsItem->index();

    return iElts;
}


bool DocumentModel::performTranslation( const QModelIndex& ielts, const QModelIndex& ivec )
{
    HEXA_NS::Elements* hElts = getHexaPtr<HEXA_NS::Elements*>(ielts);
    HEXA_NS::Vector*   hVec  = getHexaPtr<HEXA_NS::Vector*>(ivec);

    int r = _hexaDocument->performTranslation (hElts, hVec);
    if ( r == HOK ){ //Job well done?
        updateData();
        return true;
    }

    return false;
}


//
bool DocumentModel::performScale( const QModelIndex& ielts, const QModelIndex& ivex, double k )
{
    HEXA_NS::Elements* hElts = getHexaPtr<HEXA_NS::Elements*>(ielts);
    HEXA_NS::Vertex*   hVex  = getHexaPtr<HEXA_NS::Vertex*>(ivex);

    int r = _hexaDocument->performScale (hElts, hVex, k);
    if ( r == HOK ){
        updateData();
        return true;
    }

    return false;
}

//
bool DocumentModel::performRotation( const QModelIndex& ielts, const QModelIndex& ivex, const QModelIndex& ivec, double angle )
{
    HEXA_NS::Elements* hElts = getHexaPtr<HEXA_NS::Elements*>(ielts);
    HEXA_NS::Vertex*   hVex  = getHexaPtr<HEXA_NS::Vertex*>(ivex);
    HEXA_NS::Vector*   hVec  = getHexaPtr<HEXA_NS::Vector*>(ivec);

    int r = _hexaDocument-> performRotation( hElts, hVex, hVec, angle );
    if ( r == HOK ){
        updateData();
        return true;
    }

    return false;
}


//
bool DocumentModel::performSymmetryPoint( const QModelIndex& ielts, const QModelIndex& ivex )
{
    HEXA_NS::Elements* hElts = getHexaPtr<HEXA_NS::Elements*>(ielts);
    HEXA_NS::Vertex*   hVex  = getHexaPtr<HEXA_NS::Vertex*>(ivex);

    int r = _hexaDocument->performSymmetryPoint( hElts, hVex );
    if ( r == HOK ){
        updateData();
        return true;
    }

    return false;
}


bool DocumentModel::performSymmetryLine( const QModelIndex& ielts, const QModelIndex& ivex, const QModelIndex& ivec )
{
    HEXA_NS::Elements* hElts = getHexaPtr<HEXA_NS::Elements*>(ielts);
    HEXA_NS::Vertex*   hVex  = getHexaPtr<HEXA_NS::Vertex*>(ivex);
    HEXA_NS::Vector*   hVec  = getHexaPtr<HEXA_NS::Vector*>(ivec);

    int r = _hexaDocument->performSymmetryLine( hElts, hVex, hVec );
    if ( r == HOK ){
        updateData();
        return true;
    }

    return false;
}


bool DocumentModel::performSymmetryPlane( const QModelIndex& ielts,
        const QModelIndex& ivex,
        const QModelIndex& ivec )
{
    HEXA_NS::Elements* hElts = getHexaPtr<HEXA_NS::Elements*>(ielts);
    HEXA_NS::Vertex*   hVex  = getHexaPtr<HEXA_NS::Vertex*>(ivex);
    HEXA_NS::Vector*   hVec  = getHexaPtr<HEXA_NS::Vector*>(ivec);

    int r = _hexaDocument->performSymmetryPlane( hElts, hVex, hVec );
    if ( r == HOK ){
        updateData();
        return true;
    }

    return false;
}

QModelIndex DocumentModel::replace( const QModelIndexList& iquads_source,
                                    const QModelIndexList& iquads_dest,
                                    const QModelIndex& ip1_source, const QModelIndex& ic1_dest,
                                    const QModelIndex& ip2_source, const QModelIndex& ic2_dest)
{
    QModelIndex ielts;

    HEXA_NS::Vertex* hp1 = getHexaPtr<HEXA_NS::Vertex*>(ip1_source);
    HEXA_NS::Vertex* hp2 = getHexaPtr<HEXA_NS::Vertex*>(ip2_source);
    HEXA_NS::Vertex* hc1 = getHexaPtr<HEXA_NS::Vertex*>(ic1_dest);
    HEXA_NS::Vertex* hc2 = getHexaPtr<HEXA_NS::Vertex*>(ic2_dest);

    HEXA_NS::Quads   hquads_source, hquads_dest;
    HEXA_NS::Quad*   hquad = NULL;
    foreach( const QModelIndex& iquad, iquads_source ){
        hquad = getHexaPtr<HEXA_NS::Quad*>(iquad);
        hquads_source.push_back( hquad );
    }

    foreach( const QModelIndex& iquad, iquads_dest) {
        hquad = getHexaPtr<HEXA_NS::Quad*>(iquad);
        hquads_dest.push_back(hquad);
    }

    HEXA_NS::Elements* helts = _hexaDocument->replace( hquads_source,
                                                            hquads_dest,
                                                            hp1, hc1, hp2, hc2);
    if ( BadElement(helts) )
        return ielts;

    updateData();
    ElementsItem* eltsItem = new ElementsItem(helts);
    _elementsDirItem->appendRow(eltsItem);
    ielts = eltsItem->index();

    return ielts;
}

QModelIndex DocumentModel::getGeomModelIndex(QString& id) const
{
    QModelIndex result;
    PatternGeomSelectionModel* pgsm = HEXABLOCKGUI::currentDocGView->getPatternGeomSelectionModel();

    // ** get the model index of the geom element having this ID
    HEXA_NS::SubShape* eltPtr = getGeomPtr(id);
    HEXA_NS::VertexShape* geomVertex = dynamic_cast<HEXA_NS::VertexShape*>(eltPtr);
    HEXA_NS::EdgeShape* geomEdge = dynamic_cast<HEXA_NS::EdgeShape*>(eltPtr);
    HEXA_NS::FaceShape* geomFace = dynamic_cast<HEXA_NS::FaceShape*>(eltPtr);
    if (geomVertex != NULL)
        result = pgsm->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(geomVertex));
    if (geomEdge != NULL)
        result = pgsm->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(geomEdge));
    if (geomFace != NULL)
        result = pgsm->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(geomFace));

    return result;
}

QMultiMap< QString, int >     DocumentModel::getAssocShapesIds(const QModelIndex& dataIndex)
{
   QMultiMap< QString, int > result;
   HEXA_NS::NewShape* mainShape;
   HEXA_NS::VertexShape* node;
   HEXA_NS::EdgeShape* line;
   HEXA_NS::FaceShape* face;
   QString shapeName;
   int subid;

   PatternDataSelectionModel* pdsm = HEXABLOCKGUI::currentDocGView->getPatternDataSelectionModel();
   QModelIndexList assocsInd = pdsm->getGeomAssociations(dataIndex);
   foreach( const QModelIndex& anAssoc, assocsInd )
   {
      node = getHexaPtr<HEXA_NS::VertexShape*>(anAssoc);
      line = getHexaPtr<HEXA_NS::EdgeShape*>(anAssoc);
      face = getHexaPtr<HEXA_NS::FaceShape*>(anAssoc);

      if (node != NULL)
         {
            mainShape = node->getParentShape();
            if (mainShape != NULL)
               {
                  shapeName = mainShape->getName();
                  subid = node->getIdent();
               }
         }
      else if (line != NULL)
         {
            mainShape = line->getParentShape();
            if (mainShape != NULL)
               {
                  shapeName = mainShape->getName();
                  subid = line->getIdent();
               }
         }
      else if (face != NULL)
         {
            mainShape = face->getParentShape();
            if (mainShape != NULL)
               {
                  shapeName = mainShape->getName();
                  subid = face->getIdent();
               }
         }

      if (!shapeName.isEmpty())
         result.insert( shapeName, subid );

      shapeName = QString();
   }
   return result;
}

bool DocumentModel::setVertexAssociation( const QModelIndex& iVertex, double x, double y, double z)
{
    HEXA_NS::Vertex* vertex = getHexaPtr<HEXA_NS::Vertex *>(iVertex);

    if (vertex == NULL || _hexaDocument == NULL || vertex->setAssociation(x, y, z) != HOK)
        return false;

    HEXA_NS::VertexShape* assoc = vertex->getAssociation();
    if (assoc != NULL)
    {
        HEXA_NS::NewShape* mainSh = assoc->getParentShape();
        setData( iVertex, QVariant::fromValue(QString(mainSh->getName())+","+QString::number(assoc->getIdent())+";"), HEXA_ASSOC_ENTRY_ROLE );
    }

    return true;
}

bool DocumentModel::setVertexAssociation( const QModelIndex& iVertex, const QModelIndex& iGeomVertex)
{
    //parameters control
    QString newAssoc;
    HEXA_NS::Vertex* vertex = getHexaPtr<HEXA_NS::Vertex *>(iVertex);
    HEXA_NS::VertexShape* geomVertex = getHexaPtr<HEXA_NS::VertexShape*>(iGeomVertex);
    if (_hexaDocument == NULL || vertex == NULL || geomVertex == NULL) return false;
    HEXA_NS::NewShape* mainShape = geomVertex->getParentShape();
    if (mainShape == NULL) return false;

    //add association
    if (vertex->setAssociation(geomVertex) != HOK)
        return false;

    //update association state in the model
    newAssoc =  QString(mainShape->getName()) + "," + QString::number(geomVertex->getIdent()) + ";";
    setData( iVertex, QVariant::fromValue(newAssoc), HEXA_ASSOC_ENTRY_ROLE );

    return true;
}

bool DocumentModel::addEdgeAssociation( const QModelIndex& iEdge, const QModelIndex& iGeomEdge, double start, double end)
{
    //parameters control
    QString currentAssoc, newAssoc;
    HEXA_NS::Edge* edge = getHexaPtr<HEXA_NS::Edge*>(iEdge);
    HEXA_NS::EdgeShape* geomEdge = getHexaPtr<HEXA_NS::EdgeShape*>(iGeomEdge);
    if (_hexaDocument == NULL || edge == NULL ||
            geomEdge == NULL || start > end) return false;
    HEXA_NS::NewShape* mainShape = geomEdge->getParentShape();
    if (mainShape == NULL) return false;

    //add association in the engine side
    if (edge->addAssociation(geomEdge, start, end) != HOK)
        return false;

    //add/update association in the model side (UI)
    currentAssoc = data(iEdge, HEXA_ASSOC_ENTRY_ROLE).toString();
    newAssoc = QString(mainShape->getName()) + "," + QString::number(geomEdge->getIdent()) + ";";
    if (!currentAssoc.isEmpty()) newAssoc = currentAssoc + newAssoc;

    setData( iEdge, QVariant::fromValue(newAssoc), HEXA_ASSOC_ENTRY_ROLE );

    return true;
}


bool DocumentModel::addQuadAssociation (const QModelIndex& iQuad, const QModelIndex& iGeomFace)
{
    //parameters control
    QString currentAssoc, newAssoc;
    HEXA_NS::Quad* quad = getHexaPtr<HEXA_NS::Quad*>(iQuad);
    HEXA_NS::FaceShape* geomFace = getHexaPtr<HEXA_NS::FaceShape*>(iGeomFace);
    if (_hexaDocument == NULL || quad == NULL || geomFace == NULL)
        return false;
    HEXA_NS::NewShape* mainShape = geomFace->getParentShape();
    if (mainShape == NULL) return false;

    //add association
    if (quad->addAssociation(geomFace) != HOK)
        return false;

    //update association
    currentAssoc = data( iQuad, HEXA_ASSOC_ENTRY_ROLE).toString();
    newAssoc = QString(mainShape->getName()) + "," + QString::number(geomFace->getIdent()) + ";";
    if (!currentAssoc.isEmpty()) newAssoc = currentAssoc + newAssoc;
    setData( iQuad, QVariant::fromValue(newAssoc), HEXA_ASSOC_ENTRY_ROLE );

    return true;
}


QModelIndex DocumentModel::getVertexAssociation(const QModelIndex& iVertex)
{
    HEXA_NS::Vertex* vertex = getHexaPtr<HEXA_NS::Vertex*>(iVertex);
    if (vertex == NULL)
        return QModelIndex();

    //get the associated geom vertex
    HEXA_NS::VertexShape* geomVertex = vertex->getAssociation();
    if (geomVertex == NULL)
        return QModelIndex();

    //convert geom vertex to qmodelindex
    PatternGeomSelectionModel* pgsm = HEXABLOCKGUI::currentDocGView->getPatternGeomSelectionModel();
    if (pgsm == NULL)
        return QModelIndex();

    return pgsm->indexBy(HEXA_DATA_ROLE, QVariant::fromValue(geomVertex));
}


QModelIndexList DocumentModel::getEdgeAssociations(const QModelIndex& iEdge)
{
    QModelIndexList result;
    HEXA_NS::Edge* edge = getHexaPtr<HEXA_NS::Edge*>(iEdge);
    if (edge == NULL) return result;

    HEXA_NS::EdgeShape* geomEdge;
    HEXA_NS::AssoEdge* anEdgeAssoc;

    PatternGeomSelectionModel* pGSModel = HEXABLOCKGUI::currentDocGView->getPatternGeomSelectionModel();
    int nbAssocs = edge->countAssociation();
    for (int i = 0; i < nbAssocs; ++i)
    {
        anEdgeAssoc = edge->getAssociation(i);
        if (anEdgeAssoc == NULL) continue;
        geomEdge = anEdgeAssoc->getEdgeShape();
        if (geomEdge == NULL) continue;
        result << pGSModel->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(geomEdge));
    }

    return result;
}


QModelIndexList DocumentModel::getQuadAssociations(const QModelIndex& iQuad)
{
    QModelIndexList result;
    QModelIndex geomQuadIndex;
    HEXA_NS::Quad* quad = getHexaPtr<HEXA_NS::Quad*>(iQuad);
    if (quad == NULL) return result;

    if (HEXABLOCKGUI::currentDocGView == NULL) return result;
    PatternGeomSelectionModel* pgsm = HEXABLOCKGUI::currentDocGView->getPatternGeomSelectionModel();
    PatternGeomModel* pgm = HEXABLOCKGUI::currentDocGView->getPatternGeomModel();
    if (pgsm == NULL || pgm == NULL) return result;

    HEXA_NS::FaceShape* geomFace;

    int nbAssocs = quad->countAssociation();
    for (int i = 0; i < nbAssocs; ++i)
    {
        geomFace = quad->getAssociation(i);
        if (geomFace == NULL) continue;

        geomQuadIndex = pgsm->indexBy( HEXA_DATA_ROLE, QVariant::fromValue(geomFace) );
        result << geomQuadIndex;
    }

    return result;
}


bool DocumentModel::associateOpenedLine( const QModelIndexList& iedges,
        HEXA_NS::NewShapes shapes,
        HEXA_NS::IntVector subIds,
        double pstart,
        double pend )
{

    HEXA_NS::Edges  mline;

    HEXA_NS::Edge* hedge = NULL;
    foreach( const QModelIndex& iedge, iedges ){
        hedge = getHexaPtr<HEXA_NS::Edge*>(iedge);
        if (hedge != NULL)
            mline.push_back( hedge );
    }

    int err = _hexaDocument->associateOpenedLine( mline, shapes, subIds, pstart, pend );
    if ( err == HOK ){
        updateData();
        return true;
    }
    return false;
}

bool DocumentModel::associateClosedLine( const  QModelIndex& ivertex,
        const  QModelIndexList& iedges,
        HEXA_NS::NewShapes shapes,
        HEXA_NS::IntVector subIds,
        double pstart,
        bool   inv )
{
    HEXA_NS::Vertex* mfirst = getHexaPtr<HEXA_NS::Vertex*>(ivertex);
    HEXA_NS::Edges   mline;

    HEXA_NS::Edge* hedge = NULL;
    foreach( const QModelIndex& iedge, iedges ){
        hedge = getHexaPtr<HEXA_NS::Edge*>(iedge);
        if (hedge != NULL)
            mline.push_back( hedge );
    }

    int err = _hexaDocument->associateClosedLine( mfirst, mline, shapes, subIds, pstart, inv);
    if ( err == HOK ){
        updateData();
        return true;
    }
    return false;
}

// ************  GROUPS  ************
//
QModelIndex DocumentModel::addGroup( const QString& name, Group kind )
{
    QModelIndex iGroup;

    HEXA_NS::Group* hGroup = _hexaDocument->addGroup( name.toLocal8Bit().constData(), kind );
    if ( hGroup == NULL ) return iGroup;

    GroupItem* groupItem = new GroupItem(hGroup);
    groupItem->setData( _entry, HEXA_DOC_ENTRY_ROLE );
    _groupDirItem->appendRow(groupItem);
    iGroup = groupItem->index();

    return iGroup;
}


bool DocumentModel::removeGroup( const QModelIndex& igrp )
{
    HEXA_NS::Group* hGroup = data(igrp, HEXA_DATA_ROLE).value<HEXA_NS::Group *>();
    int r = _hexaDocument->removeGroup ( hGroup );

    if ( r == HOK ){
        removeRow( igrp.row(), igrp.parent());
        return true;
    }

    return false;
}


QModelIndexList DocumentModel::getGroupElements( const QModelIndex& iGroup, DocumentModel::Group& kind ) const
{
    QModelIndexList iElements;

    HEXA_NS::Group* g = iGroup.data(HEXA_DATA_ROLE).value<HEXA_NS::Group *>();
    if ( g == NULL ) return iElements;

    QModelIndexList iFound;
    QVariant q;
    HEXA_NS::EltBase* eltBase = NULL;
    int nbElement = g->countElement();
    for ( int nr = 0; nr < nbElement; ++nr ){
        eltBase = g->getElement( nr );
        kind = g->getKind();
        switch ( kind ){
        case HEXA_NS::HexaCell: case HEXA_NS::HexaNode: q = QVariant::fromValue( (HEXA_NS::Hexa *)eltBase ); break;
        case HEXA_NS::QuadCell: case HEXA_NS::QuadNode: q = QVariant::fromValue( (HEXA_NS::Quad *)eltBase ); break;
        case HEXA_NS::EdgeCell: case HEXA_NS::EdgeNode: q = QVariant::fromValue( (HEXA_NS::Edge *)eltBase ); break;
        case HEXA_NS::VertexNode: q = QVariant::fromValue( (HEXA_NS::Vertex *)eltBase ); break;
        }
        iFound = match( index(0, 0),
                HEXA_DATA_ROLE,
                q,
                1,
                Qt::MatchRecursive );
        if ( !iFound.isEmpty() )
            iElements << iFound[0];
    }
    return iElements;
}

// 7.4 Boite: éditer un groupe
void DocumentModel::setGroupName( const QModelIndex& igrp, const QString& name )
{
    HEXA_NS::Group* hGroup = data(igrp, HEXA_DATA_ROLE).value<HEXA_NS::Group *>();

    if ( hGroup ){
        hGroup->setName( name.toLocal8Bit().constData() );
        setData(igrp, QVariant::fromValue( name ) );
    }
}

bool DocumentModel::addGroupElement( const QModelIndex& igrp, const QModelIndex& ielt )
{
    HEXA_NS::Group*   hGroup = data(igrp, HEXA_DATA_ROLE).value<HEXA_NS::Group *>();
    if (hGroup == NULL) return false;

    HEXA_NS::EltBase* hElt   = NULL;
    switch ( hGroup->getKind() ){
    case HEXA_NS::HexaCell: case HEXA_NS::HexaNode: hElt = getHexaPtr<HEXA_NS::Hexa*>(ielt); break;
    case HEXA_NS::QuadCell: case HEXA_NS::QuadNode: hElt = getHexaPtr<HEXA_NS::Quad*>(ielt); break;
    case HEXA_NS::EdgeCell: case HEXA_NS::EdgeNode: hElt = getHexaPtr<HEXA_NS::Edge*>(ielt); break;
    case HEXA_NS::VertexNode: hElt = getHexaPtr<HEXA_NS::Vertex*>(ielt); break;
    }

    int res = HERR;
    if ( hElt != NULL )
        res = hGroup->addElement( hElt );

    if ( res == HOK ) return true;

    return false;
}

bool DocumentModel::removeGroupElement( const QModelIndex& igrp, int nro )
{ //CS_TODO : remove child?
    HEXA_NS::Group* hGroup = data(igrp, HEXA_DATA_ROLE).value<HEXA_NS::Group *>();
    if (hGroup == NULL) return false;

    if (hGroup->removeElement( nro ) == HOK) return true;

    return false;
}

bool DocumentModel::clearGroupElement( const QModelIndex& igrp )
{
    HEXA_NS::Group* hGroup = data(igrp, HEXA_DATA_ROLE).value<HEXA_NS::Group *>();

    if ( hGroup != NULL)
    {
        hGroup->clearElement();
        return true;
    }
    return false;
}

// ************  LAWS  ************

//
QModelIndex DocumentModel::addLaw( const QString& name, int nbnodes )
{
    QModelIndex iLaw;

    HEXA_NS::Law* hLaw = _hexaDocument->addLaw( name.toLocal8Bit().constData(), nbnodes );
    if ( BadElement(hLaw) ) return iLaw;

    LawItem* lawItem = new LawItem(hLaw);
    _lawDirItem->appendRow(lawItem);
    iLaw = lawItem->index();

    return iLaw;
}

bool DocumentModel::setLaw( const QModelIndex& ilaw, int nbnodes, double coeff, KindLaw type )
{
    bool ret = false;

    HEXA_NS::Law* hLaw = data(ilaw, HEXA_DATA_ROLE).value<HEXA_NS::Law *>();

    if ( hLaw ){
        int ok;
        ok = hLaw->setNodes( nbnodes );
        ( ok == HOK) ? ret = true : ret = false;
        ok = hLaw->setCoefficient( coeff );
        hLaw->setKind(type);
    }

    return ret;
}

//
bool  DocumentModel::removeLaw( const QModelIndex& ilaw )
{
    HEXA_NS::Law* hLaw = data(ilaw, HEXA_DATA_ROLE).value<HEXA_NS::Law *>();
    int r = _hexaDocument->removeLaw( hLaw );

    if ( r == HOK ){
        removeRow( ilaw.row(),  ilaw.parent());
        return true;
    }

    return false;
}

bool DocumentModel::setPropagation( const QModelIndex& iPropagation, const QModelIndex& iLaw, bool way )
{
    HEXA_NS::Propagation* hPropagation = data(iPropagation, HEXA_DATA_ROLE).value<HEXA_NS::Propagation *>();
    HEXA_NS::Law* hLaw = data(iLaw, HEXA_DATA_ROLE).value<HEXA_NS::Law *>();

    int r = hPropagation->setLaw( hLaw );
    hPropagation->setWay( way );

    if ( r == HOK ) return true;

    return false;
}


QModelIndexList DocumentModel::getPropagation( const QModelIndex& iPropagation ) const
{
    QModelIndexList iEdges;

    QModelIndexList iFound;
    HEXA_NS::Propagation* propa = iPropagation.data(HEXA_DATA_ROLE).value<HEXA_NS::Propagation *>();
    if ( propa == NULL ) return iEdges;

    const HEXA_NS::Edges& edges = propa->getEdges();
    for ( HEXA_NS::Edges::const_iterator anEdge = edges.begin(), endIt = edges.end();
            anEdge != endIt;
            ++anEdge ){
        iFound = match( index(0, 0),
                HEXA_DATA_ROLE,
                QVariant::fromValue( *anEdge ),
                1,
                Qt::MatchRecursive);
        if ( !iFound.isEmpty() )
            iEdges << iFound[0];
    }

    return iEdges;
}

HEXA_NS::Document* DocumentModel::documentImpl()
{
    return _hexaDocument;
}

QString DocumentModel::documentEntry()
{
    return _entry;
}

// 8.3 Boite: éditer une loi  CS_TODO
// (idem création)

/*****************************************************************
                      PatternDataModel
 *****************************************************************/

PatternDataModel::PatternDataModel( QObject * parent ) :
                              QSortFilterProxyModel( parent )
{
    QString dataRegExp = QString("(%1|%2|%3|%4|%5|%6|%7|%8|%9|%10|%11|%12)").
            arg(VERTEX_TREE).arg(EDGE_TREE).arg(QUAD_TREE).arg(HEXA_TREE).arg(VECTOR_TREE).arg(ELEMENTS_TREE).
            arg(VERTEX_DIR_TREE).arg(EDGE_DIR_TREE).arg(QUAD_DIR_TREE).arg(HEXA_DIR_TREE).arg(VECTOR_DIR_TREE).
            arg(ELEMENTS_DIR_TREE);

    setFilterRole(HEXA_TREE_ROLE);
    setFilterRegExp ( QRegExp(dataRegExp) );
}

// *** Pour suppression des panneaux "Builder" et "Geometry" ****/

//PatternDataModel::PatternDataModel( QObject * parent ) :
//                              QSortFilterProxyModel( parent )
//{
//    QString dataRegExp = QString("(%1|%2|%3|%4|%5|%6|%7|%8|%9|%10|%11|%12|%13|%14|%15|%16|%17|%18|%19|%20|%21|%22|%23|%24|%25|%26)").
//            arg(VERTEX_TREE).arg(EDGE_TREE).arg(QUAD_TREE).arg(HEXA_TREE).
//            arg(VERTEX_DIR_TREE).arg(EDGE_DIR_TREE).arg(QUAD_DIR_TREE).
//            arg(HEXA_DIR_TREE).arg(VECTOR_TREE).arg(ELEMENTS_TREE).
//            arg(VECTOR_DIR_TREE).arg(ELEMENTS_DIR_TREE).
//            arg(IMPLICIT_SHAPES_TREE).arg(EXPLICIT_SHAPES_TREE).arg(CLOUD_OF_POINTS_TREE).
//            arg(GEOMSHAPE_TREE).arg(GEOMPOINT_TREE).arg(GEOMEDGE_TREE).arg(GEOMFACE_TREE).
//            arg(IMPLICIT_SHAPES_DIR_TREE).arg(EXPLICIT_SHAPES_DIR_TREE).arg(CLOUD_OF_POINTS_DIR_TREE).
//            arg(GEOMSHAPE_DIR_TREE).arg(GEOMPOINT_DIR_TREE).arg(GEOMEDGE_DIR_TREE).arg(GEOMFACE_DIR_TREE);
//
//    setFilterRole(HEXA_TREE_ROLE);
//    setFilterRegExp ( QRegExp(dataRegExp) );
//}

// ****************

PatternDataModel::~PatternDataModel()
{
}

HEXA_NS::Document* PatternDataModel::documentImpl()
{
    HEXA_NS::Document* doc = NULL;
    DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
    if (m) doc = m->documentImpl();
    return doc;
}

QString PatternDataModel::documentEntry()
{
    QString entry;
    DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
    if (m) entry = m->documentEntry();
    return entry;
}

Qt::ItemFlags PatternDataModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags;

    DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
    if ( m != NULL ){
        flags = m->flags( mapToSource(index) );
    }
    return flags;
}


QVariant PatternDataModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
    if ( section == 0 && orientation == Qt::Horizontal && role == Qt::DisplayRole ){
        return QVariant( "Data" );
    } else {
        return QSortFilterProxyModel::headerData ( section, orientation, role );
    }
}


QStandardItem* PatternDataModel::itemFromIndex ( const QModelIndex & index ) const
{
    QStandardItem *item = NULL;
    DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
    if ( m != NULL ){
        item = m->itemFromIndex( mapToSource(index) );
    }
    return item;
}

//PatternBuilderModel::PatternBuilderModel( QObject * parent ) :
//                              QSortFilterProxyModel( parent )
//{
//    QString builderRegExp =QString("(%1|%2|%3|%4|%5|%6)").
//            arg(VECTOR_TREE).arg(ELEMENTS_TREE).
//            arg(CROSSELEMENTS_TREE).arg(VECTOR_DIR_TREE).arg(ELEMENTS_DIR_TREE).arg(CROSSELEMENTS_DIR_TREE);
//
//    setFilterRole( HEXA_TREE_ROLE );
//    setFilterRegExp ( QRegExp( builderRegExp ) );
//}

//PatternBuilderModel::~PatternBuilderModel()
//{
//}

//Qt::ItemFlags PatternBuilderModel::flags(const QModelIndex &index) const
//{
//    Qt::ItemFlags flags;
//
//    DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
//    if ( m != NULL ){
//        flags = m->flags( mapToSource(index) );
//    }
//    return flags;
//}

//QVariant PatternBuilderModel::headerData ( int section, Qt::Orientation orientation, int role ) const
//{
//    if ( section == 0 && orientation == Qt::Horizontal && role == Qt::DisplayRole ){
//        return QVariant( "Builder" );
//    } else {
//        return QSortFilterProxyModel::headerData ( section, orientation, role );
//    }
//}

//QStandardItem* PatternBuilderModel::itemFromIndex ( const QModelIndex & index ) const
//{
//    QStandardItem *item = NULL;
//    DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
//    if ( m != NULL ){
//        item = m->itemFromIndex( mapToSource(index) );
//    }
//    return item;
//}

//====================================================================
//              PatternGeomModel
//====================================================================
PatternGeomModel::PatternGeomModel( QObject * parent ) :
                              QSortFilterProxyModel( parent )
{
    QString builderRegExp =QString("(%1|%2|%3|%4|%5|%6|%7|%8|%9|%10|%11|%12|%13|%14)").
            arg(EXPLICIT_SHAPES_TREE).arg(IMPLICIT_SHAPES_TREE).arg(CLOUD_OF_POINTS_TREE).
            arg(GEOMSHAPE_TREE).arg(GEOMPOINT_TREE).arg(GEOMEDGE_TREE).arg(GEOMFACE_TREE).
            arg(EXPLICIT_SHAPES_DIR_TREE).arg(IMPLICIT_SHAPES_DIR_TREE).arg(CLOUD_OF_POINTS_DIR_TREE).
            arg(GEOMSHAPE_DIR_TREE).arg(GEOMPOINT_DIR_TREE).arg(GEOMEDGE_DIR_TREE).arg(GEOMFACE_DIR_TREE);

    setFilterRole( HEXA_TREE_ROLE );
    setFilterRegExp ( QRegExp(builderRegExp ) );
}

PatternGeomModel::~PatternGeomModel()
{
}

Qt::ItemFlags PatternGeomModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags;

    DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
    if ( m != NULL ){
        flags = m->flags( mapToSource(index) );
    }
    return flags;
}

QVariant PatternGeomModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
    if ( section == 0 && orientation == Qt::Horizontal && role == Qt::DisplayRole ){
        return QVariant( "Geometry" );
    } else {
        return QSortFilterProxyModel::headerData ( section, orientation, role );
    }
}

QStandardItem* PatternGeomModel::itemFromIndex ( const QModelIndex & index ) const
{
    QStandardItem *item = NULL;
    DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
    if ( m != NULL ){
        item = m->itemFromIndex( mapToSource(index) );
    }
    return item;
}
//==============================================================


AssociationsModel::AssociationsModel( QObject * parent ) :
                              QSortFilterProxyModel( parent )
{
    QString assocRegExp;

    setFilterRole( HEXA_TREE_ROLE );
    setFilterRegExp ( QRegExp(assocRegExp) );
}

AssociationsModel::~AssociationsModel()
{
}

Qt::ItemFlags AssociationsModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags;

    DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
    if ( m != NULL ){
        flags = m->flags( mapToSource(index) );
    }
    return flags;
}

QVariant AssociationsModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
    if ( section == 0 && orientation == Qt::Horizontal && role == Qt::DisplayRole ){
        return QVariant( "Associations" );
    } else {
        return QSortFilterProxyModel::headerData ( section, orientation, role );
    }
}

QStandardItem* AssociationsModel::itemFromIndex ( const QModelIndex & index ) const
{
    QStandardItem *item = NULL;
    DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
    if ( m != NULL ){
        item = m->itemFromIndex( mapToSource(index) );
    }
    return item;
}

GroupsModel::GroupsModel( QObject * parent ) :
                              QSortFilterProxyModel( parent )
{
    QString groupsRegExp =QString("(%1|%2)").arg(GROUP_TREE).arg(GROUP_DIR_TREE);

    setFilterRole( HEXA_TREE_ROLE );
    setFilterRegExp ( QRegExp(groupsRegExp ) );
}

GroupsModel::~GroupsModel()
{
}

Qt::ItemFlags GroupsModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags;

    DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
    if ( m != NULL ){
        flags = m->flags( mapToSource(index) );
    }
    return flags;
}

QVariant GroupsModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
    if ( section == 0 && orientation == Qt::Horizontal && role == Qt::DisplayRole ){
        return QVariant( "Groups" );
    } else {
        return QSortFilterProxyModel::headerData ( section, orientation, role );
    }
}

QStandardItem* GroupsModel::itemFromIndex ( const QModelIndex & index ) const
{
    QStandardItem *item = NULL;
    DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
    if ( m != NULL ){
        item = m->itemFromIndex( mapToSource(index) );
    }
    return item;
}

QModelIndexList GroupsModel::getGroupElements( const QModelIndex& iGroup, DocumentModel::Group& kind ) const
{
    QModelIndexList elements;
    DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
    if ( m != NULL ){
        elements = m->getGroupElements( mapToSource(iGroup), kind );
    }
    return elements;
}

MeshModel::MeshModel( QObject * parent ) :
                              QSortFilterProxyModel( parent )
{
    QString meshRegExp =QString("(%1|%2|%3|%4)").arg(LAW_TREE).arg(LAW_DIR_TREE)                            .arg(PROPAGATION_TREE).arg(PROPAGATION_DIR_TREE);

    setFilterRole( HEXA_TREE_ROLE );
    setFilterRegExp ( QRegExp(meshRegExp) );
}

MeshModel::~MeshModel()
{
}

Qt::ItemFlags MeshModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags;

    DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
    if ( m != NULL ){
        flags = m->flags( mapToSource(index) );
    }
    return flags;
}

QVariant MeshModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
    if ( section == 0 && orientation == Qt::Horizontal && role == Qt::DisplayRole ){
        return QVariant( "Mesh" );
    } else {
        return QSortFilterProxyModel::headerData ( section, orientation, role );
    }
}

QStandardItem* MeshModel::itemFromIndex ( const QModelIndex & index ) const
{
    QStandardItem *item = NULL;
    DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
    if ( m != NULL ){
        item = m->itemFromIndex( mapToSource(index) );
    }
    return item;
}

QModelIndexList MeshModel::getPropagation( const QModelIndex& iPropagation ) const
{
    QModelIndexList edges;
    DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
    if ( m != NULL ){
        edges = m->getPropagation( mapToSource(iPropagation) );
    }
    return edges;
}


// ================================== NEW ======================================

// ===== CARTESIAN GRID

QModelIndex DocumentModel::makeCartesianTop(int nx, int ny, int nz)
{
    QModelIndex result;

    HEXA_NS::Elements* helts = _hexaDocument->makeCartesianTop( nx, ny, nz );
    if ( BadElement(helts) )
        return result;

    result = addElementsToTree(helts, _elementsDirItem);

    return result;
}

QModelIndex DocumentModel::makeCartesianUni(const QModelIndex& icenter,
                            const QModelIndex& ibase, const QModelIndex& ivec, const QModelIndex& iaxis,
                            double lx, double ly, double lz, int nx, int ny, int nz)
{
    QModelIndex result;

    HEXA_NS::Vertex* hcenter = getHexaPtr<HEXA_NS::Vertex*>(icenter);
    HEXA_NS::Vector* hbase   = getHexaPtr<HEXA_NS::Vector*>(ibase);
    HEXA_NS::Vector* hvec    = getHexaPtr<HEXA_NS::Vector*>(ivec);
    HEXA_NS::Vector* haxis   = getHexaPtr<HEXA_NS::Vector*>(iaxis);

    HEXA_NS::Elements* helts = _hexaDocument->makeCartesianUni( hcenter, hbase, hvec, haxis,
                                                                lx, ly, lz, nx, ny, nz);
    if ( BadElement(helts) )
        return result;

    result = addElementsToTree(helts, _elementsDirItem);

    return result;
}

QModelIndex DocumentModel::makeCartesian(const QModelIndex& icenter,
                                         const QModelIndex& ibase, const QModelIndex& ivec, const QModelIndex& iaxis,
                                         vector<double>& radius, vector<double>& angles, vector<double>& heights)
{
    QModelIndex result;

    HEXA_NS::Vertex* hcenter = getHexaPtr<HEXA_NS::Vertex*>(icenter);
    HEXA_NS::Vector* hbase   = getHexaPtr<HEXA_NS::Vector*>(ibase);
    HEXA_NS::Vector* hvec    = getHexaPtr<HEXA_NS::Vector*>(ivec);
    HEXA_NS::Vector* haxis   = getHexaPtr<HEXA_NS::Vector*>(iaxis);

    HEXA_NS::Elements* helts = _hexaDocument->makeCartesian( hcenter, hbase, hvec, haxis,
                                                             radius, angles, heights);
    if ( BadElement(helts) )
        return result;

    result = addElementsToTree(helts, _elementsDirItem);

    return result;
}


// ===== SPHERE

QModelIndex DocumentModel::makeSphereTop (int nr, int na, int nh)
{
    QModelIndex result;

    HEXA_NS::Elements* helts = _hexaDocument->makeSphereTop( nr, na, nh );
    if ( BadElement(helts) )
        return result;

    result = addElementsToTree(helts, _elementsDirItem);

    return result;
}

QModelIndex DocumentModel::makeSphereUni (QModelIndex& icenter,
                                          QModelIndex& ivec_x, QModelIndex& ivec_z,
                                          double rtrou, double rext, double ang,
                                          QModelIndex& ivplan,
                                          int nr, int na, int nh)
{
    QModelIndex result;

    HEXA_NS::Vertex* hcenter = getHexaPtr<HEXA_NS::Vertex*>(icenter);
    HEXA_NS::Vector* hvec_x  = getHexaPtr<HEXA_NS::Vector*>(ivec_x);
    HEXA_NS::Vector* hvec_z  = getHexaPtr<HEXA_NS::Vector*>(ivec_z);
    HEXA_NS::Vertex* hvplan  = getHexaPtr<HEXA_NS::Vertex*>(ivplan);

    HEXA_NS::Elements* helts = _hexaDocument->makeSphereUni( hcenter, hvec_x, hvec_z,
                                                             rtrou, rext, ang, hvplan,
                                                             nr, na, nh);
    result = addElementsToTree(helts, _elementsDirItem);

    return result;
}

QModelIndex DocumentModel::makeSphere    (QModelIndex& icenter,
                                          QModelIndex& ivec_x, QModelIndex& ivec_z,
                                          vector<double>& tray, vector<double>& tang, vector<double>& thaut)
{
    QModelIndex result;

    HEXA_NS::Vertex* hcenter = getHexaPtr<HEXA_NS::Vertex*>(icenter);
    HEXA_NS::Vector* hvec_x  = getHexaPtr<HEXA_NS::Vector*>(ivec_x);
    HEXA_NS::Vector* hvec_z  = getHexaPtr<HEXA_NS::Vector*>(ivec_z);

    HEXA_NS::Elements* helts = _hexaDocument->makeSphere( hcenter, hvec_x, hvec_z,
                                                          tray, tang, thaut);

    result = addElementsToTree(helts, _elementsDirItem);

    return result;
}

// ====== SPHERICAL

QModelIndex DocumentModel::makeSphericalTop (int nbre, int crit)
{
    QModelIndex result;

    HEXA_NS::Elements* helts = _hexaDocument->makeSphericalTop(nbre, crit);
    if (BadElement(helts))
        return result;

    result = addElementsToTree(helts, _elementsDirItem);

    return result;
}

QModelIndex DocumentModel::makeSphericalUni (QModelIndex& icenter,
                                             QModelIndex& ivec_x, QModelIndex& ivec_z,
                                             double rayon,
                                             int nbre, int crit)
{
    QModelIndex result;

    HEXA_NS::Vertex* hcenter = getHexaPtr<HEXA_NS::Vertex*>(icenter);
    HEXA_NS::Vector* hvec_x  = getHexaPtr<HEXA_NS::Vector*>(ivec_x);
    HEXA_NS::Vector* hvec_z  = getHexaPtr<HEXA_NS::Vector*>(ivec_z);

    HEXA_NS::Elements* helts = _hexaDocument->makeSphericalUni(hcenter, hvec_x, hvec_z, rayon, nbre, crit);
    if (BadElement(helts))
        return result;

    result = addElementsToTree(helts, _elementsDirItem);

    return result;
}

QModelIndex DocumentModel::makeSpherical    (QModelIndex& icenter,
                                             QModelIndex& ivec_x, QModelIndex& ivec_z,
                                             vector<double>& rayon,
                                             int crit)
{
    QModelIndex result;

    HEXA_NS::Vertex* hcenter = getHexaPtr<HEXA_NS::Vertex*>(icenter);
    HEXA_NS::Vector* hvec_x = getHexaPtr<HEXA_NS::Vector*>(ivec_x);
    HEXA_NS::Vector* hvec_z = getHexaPtr<HEXA_NS::Vector*>(ivec_z);

    HEXA_NS::Elements* helts = _hexaDocument->makeSpherical(hcenter, hvec_x, hvec_z, rayon, crit);
    if (BadElement(helts))
        return result;

    result = addElementsToTree(helts, _elementsDirItem);

    return result;
}

// ===== RIND
QModelIndex DocumentModel::makeRindTop (int nr, int na, int nh)
{
    QModelIndex result;

    HEXA_NS::Elements* helts = _hexaDocument->makeRindTop(nr, na, nh);
    if (BadElement(helts))
        return result;

    result = addElementsToTree(helts, _elementsDirItem);

    return result;
}

QModelIndex DocumentModel::makeRindUni (QModelIndex& icenter,
                                        QModelIndex& ivec_x, QModelIndex& ivec_z,
                                        double raytrou, double rint, double rext, double ang,
                                        QModelIndex& ivplan,
                                        int nr, int na, int nh)
{
    QModelIndex result;

    HEXA_NS::Vertex* hcenter = getHexaPtr<HEXA_NS::Vertex*>(icenter);
    HEXA_NS::Vector* hvec_x = getHexaPtr<HEXA_NS::Vector*>(ivec_x);
    HEXA_NS::Vector* hvec_z = getHexaPtr<HEXA_NS::Vector*>(ivec_z);
    HEXA_NS::Vertex* hvplan = getHexaPtr<HEXA_NS::Vertex*>(ivplan);

    HEXA_NS::Elements* helts = _hexaDocument->makeRindUni(hcenter, hvec_x, hvec_z, raytrou, rint, rext, ang,
                                                          hvplan, nr, na, nh);
    if (BadElement(helts))
        return result;

    result = addElementsToTree(helts, _elementsDirItem);

    return result;
}

QModelIndex DocumentModel::makeRind    (QModelIndex& icenter,
                                        QModelIndex& ivec_x, QModelIndex& ivec_z,
                                        vector<double>& tray, vector<double>& tang, vector<double>& thaut)
{
    QModelIndex result;

    HEXA_NS::Vertex* hcenter = getHexaPtr<HEXA_NS::Vertex*>(icenter);
    HEXA_NS::Vector* hvec_x = getHexaPtr<HEXA_NS::Vector*>(ivec_x);
    HEXA_NS::Vector* hvec_z = getHexaPtr<HEXA_NS::Vector*>(ivec_z);

    HEXA_NS::Elements* helts = _hexaDocument->makeRind(hcenter, hvec_x, hvec_z, tray, tang, thaut);
    if (BadElement(helts))
        return result;

    result = addElementsToTree(helts, _elementsDirItem);

    return result;
}

// ======== Cylinder
QModelIndex DocumentModel::makeCylinderTop(int nr, int na, int nh)
{
    QModelIndex result;

    HEXA_NS::Elements* helts = _hexaDocument->makeCylinderTop(nr, na, nh);
    if (BadElement(helts))
        return result;

    result = addElementsToTree(helts, _elementsDirItem);

    return result;
}

QModelIndex DocumentModel::makeCylinderUni(QModelIndex& iorig, QModelIndex& ivecx, QModelIndex& ivecz,
                                           double rint, double rext, double angle, double haut,
                                           int nr, int na, int nh)
{
    QModelIndex result;

    HEXA_NS::Vertex* horig = getHexaPtr<HEXA_NS::Vertex*>(iorig);
    HEXA_NS::Vector* hvecx = getHexaPtr<HEXA_NS::Vector*>(ivecx);
    HEXA_NS::Vector* hvecz = getHexaPtr<HEXA_NS::Vector*>(ivecz);

    HEXA_NS::Elements* helts = _hexaDocument->makeCylinderUni(horig, hvecx, hvecz,
                                                              rint, rext, angle, haut,
                                                              nr, na, nh);
    if (BadElement(helts))
        return result;

    result = addElementsToTree(helts, _elementsDirItem);

    return result;
}

QModelIndex DocumentModel::makeCylinder(QModelIndex& iorig, QModelIndex& ivecx, QModelIndex& ivecz,
                                        vector<double>& tray, vector<double>& tang, vector<double>& thaut)
{
    QModelIndex result;

    HEXA_NS::Vertex* horig = getHexaPtr<HEXA_NS::Vertex*>(iorig);
    HEXA_NS::Vector* hvecx = getHexaPtr<HEXA_NS::Vector*>(ivecx);
    HEXA_NS::Vector* hvecz = getHexaPtr<HEXA_NS::Vector*>(ivecz);

    HEXA_NS::Elements* helts = _hexaDocument->makeCylinder(horig, hvecx, hvecz, tray, tang, thaut);
    if (BadElement(helts))
        return result;

    result = addElementsToTree(helts, _elementsDirItem);

    return result;
}


// ======== Cylinders
QModelIndex DocumentModel::makeCylinders(QModelIndex& iorig1, QModelIndex& ivecz1,  double r1, double h1,
                                         QModelIndex& iorig2, QModelIndex& ivecz2, double r2, double h2)
{
    QModelIndex result;

    HEXA_NS::Vertex* horig1 = getHexaPtr<HEXA_NS::Vertex*>(iorig1);
    HEXA_NS::Vertex* horig2 = getHexaPtr<HEXA_NS::Vertex*>(iorig2);
    HEXA_NS::Vector* hvecz1 = getHexaPtr<HEXA_NS::Vector*>(ivecz1);
    HEXA_NS::Vector* hvecz2 = getHexaPtr<HEXA_NS::Vector*>(ivecz2);

    HEXA_NS::BiCylinder* helts = _hexaDocument->makeCylinders(horig1, hvecz1, r1, h1,
                                                              horig2, hvecz2, r2, h2);
    if (BadElement(helts))
        return result;

    result = addElementsToTree(helts, _crossElementsDirItem);

    return result;
}

// =========== PIPE
QModelIndex DocumentModel::makePipeTop(int nr, int na, int nh)
{
    QModelIndex result;

    HEXA_NS::Elements* helts = _hexaDocument->makePipeTop(nr, na, nh);
    if (BadElement(helts))
        return result;

    result = addElementsToTree(helts, _elementsDirItem);

    return result;
}

QModelIndex DocumentModel::makePipeUni(QModelIndex& iorig, QModelIndex& ivecx, QModelIndex& ivecz,
                                       double rint, double rext, double angle, double haut,
                                       int nr, int na, int nh)
{
    QModelIndex result;

    HEXA_NS::Vertex* horig = getHexaPtr<HEXA_NS::Vertex*>(iorig);
    HEXA_NS::Vector* hvecx = getHexaPtr<HEXA_NS::Vector*>(ivecx);
    HEXA_NS::Vector* hvecz = getHexaPtr<HEXA_NS::Vector*>(ivecz);

    HEXA_NS::Elements* helts = _hexaDocument->makePipeUni(horig, hvecx, hvecz, rint, rext, angle,
                                                          haut, nr, na, nh);
    if (BadElement(helts))
        return result;

    result = addElementsToTree(helts, _elementsDirItem);

    return result;
}

QModelIndex DocumentModel::makePipe(QModelIndex& iorig, QModelIndex& ivecx, QModelIndex& ivecz,
                                    vector<double>& tray, vector<double>& tang, vector<double>& thaut)
{
    QModelIndex result;

    HEXA_NS::Vertex* horig = getHexaPtr<HEXA_NS::Vertex*>(iorig);
    HEXA_NS::Vector* hvecx = getHexaPtr<HEXA_NS::Vector*>(ivecx);
    HEXA_NS::Vector* hvecz = getHexaPtr<HEXA_NS::Vector*>(ivecz);

    HEXA_NS::Elements* helts = _hexaDocument->makePipe(horig, hvecx, hvecz, tray, tang, thaut);
    if (BadElement(helts))
        return result;

    result = addElementsToTree(helts, _elementsDirItem);

    return result;
}

// ======== Pipes
QModelIndex DocumentModel::makePipes(QModelIndex& iorig1, QModelIndex& ivecz1, double rint1, double rex1, double h1,
                                     QModelIndex& iorig2, QModelIndex& ivecz2, double rint2, double rex2, double h2)
{
    QModelIndex result;

    HEXA_NS::Vertex* horig1 = getHexaPtr<HEXA_NS::Vertex*>(iorig1);
    HEXA_NS::Vertex* horig2 = getHexaPtr<HEXA_NS::Vertex*>(iorig2);
    HEXA_NS::Vector* hvecz1 = getHexaPtr<HEXA_NS::Vector*>(ivecz1);
    HEXA_NS::Vector* hvecz2 = getHexaPtr<HEXA_NS::Vector*>(ivecz2);

    HEXA_NS::BiCylinder* helts = _hexaDocument->makePipes(horig1, hvecz1, rint1, rex1, h1,
                                                        horig2, hvecz2, rint2, rex2, h2);
    if (BadElement(helts))
        return result;

    result = addElementsToTree(helts, _crossElementsDirItem);

    return result;
}

// ======== Join Quads
QModelIndex DocumentModel::joinQuadUni(QModelIndex&  istart, QModelIndex& idest, QModelIndex& iv1, QModelIndex& iv2,
                                       QModelIndex& iv3, QModelIndex& iv4, int nb)
{
    QModelIndex result;

    HEXA_NS::Quad* hstart = getHexaPtr<HEXA_NS::Quad*>(istart);
    HEXA_NS::Quad* hdest = getHexaPtr<HEXA_NS::Quad*>(idest);
    HEXA_NS::Vertex* hv1 = getHexaPtr<HEXA_NS::Vertex*>(iv1);
    HEXA_NS::Vertex* hv2 = getHexaPtr<HEXA_NS::Vertex*>(iv2);
    HEXA_NS::Vertex* hv3 = getHexaPtr<HEXA_NS::Vertex*>(iv3);
    HEXA_NS::Vertex* hv4 = getHexaPtr<HEXA_NS::Vertex*>(iv4);

    HEXA_NS::Elements* helts = _hexaDocument->joinQuadUni(hstart, hdest, hv1, hv2, hv3, hv4, nb);
    if (BadElement(helts))
        return result;

    result = addElementsToTree(helts, _elementsDirItem);

    return result;
}

QModelIndex DocumentModel::joinQuad(QModelIndex&  istart, QModelIndex& idest, QModelIndex& iva1, QModelIndex& ivb1,
                     QModelIndex& iva2, QModelIndex& ivb2, vector<double>& tlen)
{
    QModelIndex result;

    HEXA_NS::Quad* hstart = getHexaPtr<HEXA_NS::Quad*>(istart);
    HEXA_NS::Quad* hdest = getHexaPtr<HEXA_NS::Quad*>(idest);
    HEXA_NS::Vertex* hva1 = getHexaPtr<HEXA_NS::Vertex*>(iva1);
    HEXA_NS::Vertex* hvb1 = getHexaPtr<HEXA_NS::Vertex*>(ivb1);
    HEXA_NS::Vertex* hva2 = getHexaPtr<HEXA_NS::Vertex*>(iva2);
    HEXA_NS::Vertex* hvb2 = getHexaPtr<HEXA_NS::Vertex*>(ivb2);

    HEXA_NS::Elements* helts = _hexaDocument->joinQuad(hstart, hdest, hva1, hvb1, hva2, hvb2, tlen);
    if (BadElement(helts))
        return result;

    result = addElementsToTree(helts, _elementsDirItem);

    return result;
}

QModelIndex DocumentModel::joinQuadsUni(QModelIndexList& istarts, QModelIndex& idest, QModelIndex& iv1, QModelIndex& iv2,
                                        QModelIndex& iv3, QModelIndex& iv4, int nb)
{
    QModelIndex result;

    HEXA_NS::Quads hstarts;
    int nbQuads = istarts.size();
    for (int i = 0; i < nbQuads; ++i)
        hstarts.push_back(getHexaPtr<HEXA_NS::Quad*>(istarts[i]));

    HEXA_NS::Quad* hdest = getHexaPtr<HEXA_NS::Quad*>(idest);
    HEXA_NS::Vertex* hv1 = getHexaPtr<HEXA_NS::Vertex*>(iv1);
    HEXA_NS::Vertex* hv2 = getHexaPtr<HEXA_NS::Vertex*>(iv2);
    HEXA_NS::Vertex* hv3 = getHexaPtr<HEXA_NS::Vertex*>(iv3);
    HEXA_NS::Vertex* hv4 = getHexaPtr<HEXA_NS::Vertex*>(iv4);

    HEXA_NS::Elements* helts = _hexaDocument->joinQuadsUni(hstarts, hdest, hv1, hv2, hv3, hv4, nb);
    if (BadElement(helts))
        return result;

    result = addElementsToTree(helts, _elementsDirItem);

    return result;
}

QModelIndex DocumentModel::joinQuads(QModelIndexList& istarts, QModelIndex& idest, QModelIndex& iva1, QModelIndex& ivb1,
                                     QModelIndex& iva2, QModelIndex& ivb2, vector<double>& tlen)
{
    QModelIndex result;

    HEXA_NS::Quads hstarts;
    int nbQuads = istarts.size();
    for (int i = 0; i < nbQuads; ++i)
        hstarts.push_back(getHexaPtr<HEXA_NS::Quad*>(istarts[i]));

    HEXA_NS::Quad* hdest = getHexaPtr<HEXA_NS::Quad*>(idest);
    HEXA_NS::Vertex* hva1 = getHexaPtr<HEXA_NS::Vertex*>(iva1);
    HEXA_NS::Vertex* hvb1 = getHexaPtr<HEXA_NS::Vertex*>(ivb1);
    HEXA_NS::Vertex* hva2 = getHexaPtr<HEXA_NS::Vertex*>(iva2);
    HEXA_NS::Vertex* hvb2 = getHexaPtr<HEXA_NS::Vertex*>(ivb2);

    HEXA_NS::Elements* helts = _hexaDocument->joinQuads(hstarts, hdest, hva1, hvb1, hva2, hvb2, tlen);
    if (BadElement(helts))
        return result;

    result = addElementsToTree(helts, _elementsDirItem);

    return result;
}

// ======== Quad Revolution
QModelIndex DocumentModel::revolutionQuadUni(QModelIndex& istart, QModelIndex& icenter, QModelIndex& iaxis,
                                             double angle, int nbre)
{
    QModelIndex result;

    HEXA_NS::Quad* hstart = getHexaPtr<HEXA_NS::Quad*>(istart);
    HEXA_NS::Vertex* hcenter = getHexaPtr<HEXA_NS::Vertex*>(icenter);
    HEXA_NS::Vector* haxis = getHexaPtr<HEXA_NS::Vector*>(iaxis);

    HEXA_NS::Elements* helts = _hexaDocument->revolutionQuadUni(hstart, hcenter, haxis, angle, nbre);
    if (BadElement(helts))
        return result;

    result = addElementsToTree(helts, _elementsDirItem);

    return result;
}

QModelIndex DocumentModel::revolutionQuad(QModelIndex& istart, QModelIndex& icenter, QModelIndex& iaxis,
                                          vector<double>& angles)
{
    QModelIndex result;

    HEXA_NS::Quad* hstart = getHexaPtr<HEXA_NS::Quad*>(istart);
    HEXA_NS::Vertex* hcenter = getHexaPtr<HEXA_NS::Vertex*>(icenter);
    HEXA_NS::Vector* haxis = getHexaPtr<HEXA_NS::Vector*>(iaxis);

    HEXA_NS::Elements* helts = _hexaDocument->revolutionQuad(hstart, hcenter, haxis, angles);
    if (BadElement(helts))
        return result;

    result = addElementsToTree(helts, _elementsDirItem);

    return result;
}

QModelIndex DocumentModel::revolutionQuadsUni(QModelIndexList& istarts, QModelIndex& icenter, QModelIndex& iaxis,
                                              double angle, int nbre)
{
    QModelIndex result;

    HEXA_NS::Quads hstarts;
    int nbQuads = istarts.count();
    for (int i = 0; i < nbQuads; ++i)
        hstarts.push_back(getHexaPtr<HEXA_NS::Quad*>(istarts[i]));

    HEXA_NS::Vertex* hcenter = getHexaPtr<HEXA_NS::Vertex*>(icenter);
    HEXA_NS::Vector* haxis = getHexaPtr<HEXA_NS::Vector*>(iaxis);

    HEXA_NS::Elements* helts = _hexaDocument->revolutionQuadsUni(hstarts, hcenter, haxis, angle, nbre);
    if (BadElement(helts))
        return result;

    result = addElementsToTree(helts, _elementsDirItem);

    return result;
}

QModelIndex DocumentModel::revolutionQuads(QModelIndexList& istarts, QModelIndex& icenter, QModelIndex& iaxis,
                                           vector<double>& angles)
{
    QModelIndex result;

    HEXA_NS::Quads hstarts;
    int nbQuads = istarts.count();
    for (int i = 0; i < nbQuads; ++i)
        hstarts.push_back(getHexaPtr<HEXA_NS::Quad*>(istarts[i]));

    HEXA_NS::Vertex* hcenter = getHexaPtr<HEXA_NS::Vertex*>(icenter);
    HEXA_NS::Vector* haxis = getHexaPtr<HEXA_NS::Vector*>(iaxis);

    HEXA_NS::Elements* helts = _hexaDocument->revolutionQuads(hstarts, hcenter, haxis, angles);
    if (BadElement(helts))
        return result;

    result = addElementsToTree(helts, _elementsDirItem);

    return result;

}

// ==== PrismQuad or ExtrudeQuad
QModelIndex DocumentModel::extrudeQuadTop(QModelIndex& istart, int nbre)
{
    QModelIndex result;

    HEXA_NS::Quad* hstart = getHexaPtr<HEXA_NS::Quad*>(istart);
    HEXA_NS::Elements* helts = _hexaDocument->extrudeQuadTop(hstart, nbre);
    if (BadElement(helts))
        return result;

    result = addElementsToTree(helts, _elementsDirItem);

    return result;
}

QModelIndex DocumentModel::extrudeQuadUni(QModelIndex& istart, QModelIndex& dv, double len, int nbre)
{
    QModelIndex result;

    HEXA_NS::Quad* hstart = getHexaPtr<HEXA_NS::Quad*>(istart);
    HEXA_NS::Vector* hvec = getHexaPtr<HEXA_NS::Vector*>(dv);

    HEXA_NS::Elements* helts = _hexaDocument->extrudeQuadUni(hstart, hvec, len, nbre);
    if (BadElement(helts))
        return result;

    result = addElementsToTree(helts, _elementsDirItem);

    return result;
}

QModelIndex DocumentModel::extrudeQuad(QModelIndex& istart, QModelIndex& dv, vector<double>& tlen)
{
    QModelIndex result;

    HEXA_NS::Quad* hstart = getHexaPtr<HEXA_NS::Quad*>(istart);
    HEXA_NS::Vector* hvec = getHexaPtr<HEXA_NS::Vector*>(dv);

    HEXA_NS::Elements* helts = _hexaDocument->extrudeQuad(hstart, hvec, tlen);
    if (BadElement(helts))
        return result;
    result = addElementsToTree(helts, _elementsDirItem);

    return result;
}

QModelIndex DocumentModel::extrudeQuadsTop (QModelIndexList& istarts, int nbre)
{
    QModelIndex result;

    HEXA_NS::Quads hquads;
    int nbQuads = istarts.count();
    for (int i = 0; i < nbQuads; ++i)
        hquads.push_back(getHexaPtr<HEXA_NS::Quad*>(istarts[i]));

    HEXA_NS::Elements* helts = _hexaDocument->extrudeQuadsTop(hquads, nbre);
    if (BadElement(helts))
        return result;
    result = addElementsToTree(helts, _elementsDirItem);

    return result;
}

QModelIndex DocumentModel::extrudeQuadsUni (QModelIndexList& istarts, QModelIndex& axis, double len, int nbre)
{
    QModelIndex result;

    HEXA_NS::Quads hquads;
    int nbQuads = istarts.count();
    for (int i = 0; i < nbQuads; ++i)
        hquads.push_back(getHexaPtr<HEXA_NS::Quad*>(istarts[i]));

    HEXA_NS::Vector* haxis = getHexaPtr<HEXA_NS::Vector*>(axis);

    HEXA_NS::Elements* helts = _hexaDocument->extrudeQuadsUni(hquads, haxis, len, nbre);
    if (BadElement(helts))
        return result;
    result = addElementsToTree(helts, _elementsDirItem);

    return result;
}

QModelIndex DocumentModel::extrudeQuads(QModelIndexList& istarts, QModelIndex& iaxis, vector<double>& tlen)
{
    QModelIndex result;

    HEXA_NS::Quads hquads;
    int nbQuads = istarts.count();
    for (int i=0; i < nbQuads; ++i)
        hquads.push_back(getHexaPtr<HEXA_NS::Quad*>(istarts[i]));

    HEXA_NS::Vector* haxis = getHexaPtr<HEXA_NS::Vector*>(iaxis);

    HEXA_NS::Elements* helts = _hexaDocument->extrudeQuads(hquads, haxis, tlen);
    if (BadElement(helts))
        return result;
    result = addElementsToTree(helts, _elementsDirItem);

    return result;
}

// ==== Cut Edge
QModelIndex DocumentModel::cutUni(QModelIndex& iEdge, int nbre)
{
    QModelIndex result;

    HEXA_NS::Edge* hedge = getHexaPtr<HEXA_NS::Edge*>(iEdge);

    HEXA_NS::Elements* helts = _hexaDocument->cutUni(hedge, nbre);
    if (BadElement(helts))
        return result;
    result = addElementsToTree(helts, _elementsDirItem);

    return result;
}

QModelIndex DocumentModel::cut(QModelIndex& iEdge, vector<double>& tlen)
{
    QModelIndex result;

    HEXA_NS::Edge* hedge = getHexaPtr<HEXA_NS::Edge*>(iEdge);

    HEXA_NS::Elements* helts = _hexaDocument->cut(hedge, tlen);
    if (BadElement(helts))
        return result;
    result = addElementsToTree(helts, _elementsDirItem);

    return result;
}

// ================================== END NEW ==================================
