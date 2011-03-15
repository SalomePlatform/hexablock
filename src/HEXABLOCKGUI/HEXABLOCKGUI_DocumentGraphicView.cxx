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

/*#include <cmath>
#include <QMenu>
#include <QGraphicView>
#include <QWheelEvent>

#include <cmath>
*/
//#define _DEVDEBUG_

#include <sstream>

#include <iostream>

#include <math.h>
#include <QtGui>

// ajout JPL :
//#include <LightApp_SelectionMgr.h>
//#include <LightApp_Displayer.h>
#include <SalomeApp_Application.h>
#include <SUIT_ViewManager.h>
#include <SUIT_ViewWindow.h>
#include <SVTK_ViewManager.h>
#include <SVTK_ViewModel.h>
#include <SVTK_ViewWindow.h>
#include <SALOME_Actor.h>
#include <VTKViewer_Algorithm.h>
#include <SalomeApp_Study.h>

// VTK includes
#include <vtkRenderer.h>
#include <vtkActorCollection.h>
#include <vtkUnstructuredGrid.h>

// test tutorial (sphere)
#include <vtkPolyDataMapper.h>
#include <vtkSphereSource.h>

// test point (cf. SMESHGUI)
#include <vtkIdList.h>
#include <vtkCellArray.h>
#include <vtkUnsignedCharArray.h>
#include <vtkUnstructuredGrid.h>
#include <vtkDataSetMapper.h>
#include <vtkProperty.h>
#include <vtkLineSource.h>

#include <vtkLine.h>
#include <vtkQuad.h>
#include <vtkHexahedron.h>

#include "vtkLookupTable.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkPointData.h"
#include "vtkProperty.h"


#include <VTKViewer_CellLocationsArray.h>


// end JPL

// ajout JPL :
#include "HEXABLOCKGUI_Displayer.hxx"
#include "HEXABLOCKGUI.hxx"
#include "HEXABLOCKGUI_DocumentItem.hxx"
// end JPL


#ifndef M_PI
#define M_PI 3.1415927
#endif

#include "HEXABLOCKGUI_DocumentModel.hxx"
#include "HEXABLOCKGUI_DocumentGraphicView.hxx"

using namespace std;
using namespace HEXABLOCK::GUI;

DocumentGraphicView::DocumentGraphicView(SalomeApp_Application* app, SUIT_ViewWindow *suitView, QWidget *parent)
    : QAbstractItemView(parent),
      _suitView( suitView )
{
// ajout JPL :
    myApp = app;
    myDisplayer = 0;
// end JPL

}

DocumentGraphicView::~DocumentGraphicView()
{
}



/********************************************************************************
*                   ABSTRACT METHOD ( MUST BE IMPLEMENTED )
********************************************************************************/

/*
    Returns the item that covers the coordinate given in the view.
*/

QModelIndex DocumentGraphicView::indexAt(const QPoint &point) const
{
  std::cout << "CS_BP DocumentGraphicView::indexAt "<<std::endl;
  return QModelIndex();
}

void DocumentGraphicView::scrollTo(const QModelIndex &index, ScrollHint)
{
    std::cout << "CS_BP DocumentGraphicView::scrollTo "<<std::endl;
}

/*
    Returns the position of the item in viewport coordinates.
*/
QRect DocumentGraphicView::visualRect(const QModelIndex &index) const
{
    std::cout << "CS_BP DocumentGraphicView::visualRect "<<std::endl;
    return QRect();
}

// PROTECTED :
int DocumentGraphicView::horizontalOffset() const
{
    std::cout << "CS_BP DocumentGraphicView::horizontalOffset "<<std::endl;
    return horizontalScrollBar()->value();
}

bool DocumentGraphicView::isIndexHidden(const QModelIndex & /*index*/) const
{
    std::cout << "CS_BP DocumentGraphicView::isIndexHidden "<<std::endl;
    return false;
}

QModelIndex DocumentGraphicView::moveCursor(QAbstractItemView::CursorAction cursorAction,
                                            Qt::KeyboardModifiers /*modifiers*/)
{
    std::cout << "CS_BP DocumentGraphicView::moveCursor "<<std::endl;
    QModelIndex current = currentIndex();
    return current;
}

/*
    Find the indices corresponding to the extent of the selection.
*/
void DocumentGraphicView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command)
{
    std::cout << "CS_BP DocumentGraphicView::setSelection "<<std::endl;
}


int DocumentGraphicView::verticalOffset() const
{
    std::cout << "CS_BP DocumentGraphicView::verticalOffset "<<std::endl;
    return verticalScrollBar()->value();
}

/*
    Returns a region corresponding to the selection in viewport coordinates.
*/
QRegion DocumentGraphicView::visualRegionForSelection(const QItemSelection &selection) const
{
    std::cout << "CS_BP DocumentGraphicView::visualRegionForSelection "<<std::endl;
    return QRegion();
}




/********************************************************************************
*                   PROTECTED SLOTS 
********************************************************************************/
void DocumentGraphicView::closeEditor ( QWidget * editor, QAbstractItemDelegate::EndEditHint hint )
{ 
  std::cout << "DocumentGraphicView::closeEditor" << std::endl; 
}

void DocumentGraphicView::commitData ( QWidget * editor )
{ 
  std::cout << "DocumentGraphicView::commitData" << std::endl; 
}

void DocumentGraphicView::currentChanged ( const QModelIndex & current, const QModelIndex & previous )
{ 
  std::cout << "DocumentGraphicView::currentChanged" << std::endl; 
}

void DocumentGraphicView::dataChanged ( const QModelIndex & topLeft, const QModelIndex & bottomRight )
{ 
  std::cout << "DocumentGraphicView::dataChanged" << std::endl;
//   std::cout << "topLeft -> " << topLeft.data().toString().toStdString()<<std::endl;
//   std::cout << "bottomRight ->" << bottomRight.data().toString().toStdString()<<std::endl;

  updateObject(topLeft);
}

void DocumentGraphicView::editorDestroyed ( QObject * editor )
{ 
  std::cout << "DocumentGraphicView::editorDestroyed" << std::endl; 
}

void DocumentGraphicView::rowsAboutToBeRemoved ( const QModelIndex & parent, int start, int end )
{ 
  std::cout << "DocumentGraphicView::rowsAboutToBeRemoved" << std::endl; 
}

void DocumentGraphicView::rowsInserted ( const QModelIndex & parent, int start, int end )
{ 
  QModelIndex newRow;
  for ( int i = start; i<= end; ++i ){
    newRow = parent.child(i,0);
    addObject(newRow);
  }
}

void DocumentGraphicView::selectionChanged ( const QItemSelection & selected, const QItemSelection & deselected )
{ 
  std::cout << "DocumentGraphicView::selectionChanged" << std::endl; 
}

void DocumentGraphicView::updateGeometries ()
{ 
  std::cout << "DocumentGraphicView::updateGeometries " << std::endl; 
}












SUIT_ViewWindow* DocumentGraphicView::get_SUIT_ViewWindow()
{
  return _suitView;
}



// ajout JPL
LightApp_Displayer* DocumentGraphicView::displayer()
{
    if( !myDisplayer )
        myDisplayer = new HEXABLOCKGUI_Displayer(myApp, this);
    return myDisplayer;
}
// end JPL

// ajout JPL
bool DocumentGraphicView::updateObject(const QModelIndex& index) //CS_UPDATE
{
    std::cout << "updateObject()" << std::endl;
    
    const PatternDataModel* smodel = dynamic_cast<const PatternDataModel*>(model());
    QStandardItem* sitem = NULL;

    if ( smodel != NULL )
    {
        sitem = smodel->itemFromIndex(index);
        if ( sitem != NULL )
        {
            QVariant variant = index.model()->data(index, HEXA_ENTRY_ROLE);
            if ( !variant.isValid() ) return false;
            QString entry = variant.toString();
            SALOME_Actor* anActor = FindActorByEntry( _suitView, entry.toStdString().c_str() );
            RemoveActor(_suitView, anActor);
            addObject(index);
            return true;
        
        }
    }
}

bool DocumentGraphicView::addObject(const QModelIndex& index)
{
    QString entry = index.data(HEXA_ENTRY_ROLE).toString();
    SVTK_ViewWindow* myViewWindow = dynamic_cast<SVTK_ViewWindow*>(_suitView);
    if (myViewWindow != NULL)
    {
        // affichage :
        SUIT_ViewManager* vman = myViewWindow->getViewManager();
        SUIT_ViewModel* vmodel = vman->getViewModel();
        displayer()->Display(entry, true, dynamic_cast<SALOME_View*>(vmodel));
        displayer()->UpdateViewer();
        // end JPL

        return true;
    }
    else
    {
        return false;
    }
}


void DocumentGraphicView::RemoveActor(SUIT_ViewWindow *theWnd, SALOME_Actor* theActor)
{
    std::cout << "RemoveActor() : 1" << std::endl;
    SVTK_ViewWindow* myViewWindow = dynamic_cast<SVTK_ViewWindow*>(theWnd);
//    SVTK_ViewWindow* myViewWindow = dynamic_cast<SVTK_ViewWindow*>(_suitView);    
    if (myViewWindow != NULL)
    {
        myViewWindow->RemoveActor(theActor);
        if(theActor->hasIO())
        {
            std::cout << "RemoveActor() : 2" << std::endl;            
            Handle(SALOME_InteractiveObject) anIO = theActor->getIO();
            if(anIO->hasEntry())
            {
                std::cout << "RemoveActor() : 3" << std::endl;                
                std::string anEntry = anIO->getEntry();
                SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( myViewWindow->getViewManager()->study() );
                int aStudyId = aStudy->id();
//                 TVisualObjCont::key_type aKey(aStudyId,anEntry);
//                 VISUAL_OBJ_CONT.erase(aKey);
            }
        }
        theActor->Delete();
        myViewWindow->Repaint();
        std::cout << "RemoveActor() : 4" << std::endl;        
    }
}

SALOME_Actor* DocumentGraphicView::CreateActor(const QString& entry)
{
    SALOME_Actor* actor = NULL;

    const PatternDataModel* smodel = dynamic_cast<const PatternDataModel*>(model());
    QStandardItem*  sitem = NULL;

    if ( smodel != NULL ){
        QModelIndexList indexes = smodel->match( smodel->index(0, 0),
                                            HEXA_ENTRY_ROLE,
                                            entry,
                                            1,
                                            Qt::MatchRecursive );
        if ( indexes.count() > 0 ) {
            sitem = smodel->itemFromIndex(indexes[0]);
            if ( sitem != NULL ) {
                actor = SALOME_Actor::New();
                actor->setIO( new SALOME_InteractiveObject( entry.toLatin1(), "HEXABLOCK" ) );
                switch ( sitem->type() )
                {
                    case VERTEXITEM: // Créer vertex
                    {
                        HEXA_NS::Vertex* value = sitem->data(HEXA_DATA_ROLE).value<HEXA_NS::Vertex*>();
                        if (value != NULL)
                          _buildActor(actor, value);
//                         else
//                         {
//                             // @todo message d'erreur : problème dans la
//                             // récupération du modèle
//                         }
                        break;
                    }
                    case EDGEITEM: // créer edge
                    {
                        HEXA_NS::Edge* value = sitem->data(HEXA_DATA_ROLE).value<HEXA_NS::Edge*>();
                        if (value != NULL)
                          _buildActor(actor, value);
//                         else
//                         {
//                             // @todo message d'erreur : problème dans la
//                             // récupération du modèle
//                         }
                        break;
                    }
                    case QUADITEM: // Créer quad
                    {
                        HEXA_NS::Quad* value = sitem->data(HEXA_DATA_ROLE).value<HEXA_NS::Quad*>();
                        if (value != NULL)
                          _buildActor(actor, value);
//                         else
//                         {
//                             // @todo message d'erreur : problème dans la
//                             // récupération du modèle
//                         }
                        break;
                    }
                    case HEXAITEM: // creer Hexa
                    {
                        HEXA_NS::Hexa* value = sitem->data(HEXA_DATA_ROLE).value<HEXA_NS::Hexa*>();
                        if (value != NULL)
                          _buildActor(actor, value);
//                         else
//                         {
//                             // @todo message d'erreur : problème dans la
//                             // récupération du modèle
//                         }
                        break;
                    }
                    default:
                    {
                        // @todo message d'erreur : 
                        break;
                    }
                } // end switch
            }//if ( sitem != NULL ) {
        } //if ( indexes.count() > 0 ) {
    } //if ( smodel != NULL ){

    return actor;
}

void DocumentGraphicView::_buildActor(SALOME_Actor* actor, HEXA_NS::Vertex *value)
{
    if (dynamic_cast<HEXA_NS::Vertex*>(value) != NULL)
    {
        vtkUnstructuredGrid* aGrid = vtkUnstructuredGrid::New();
                      
        vtkPoints* aPoints = vtkPoints::New();
        aPoints->SetNumberOfPoints(1);
        aPoints->SetPoint(0, value->getX(), value->getY(), value->getZ());
                      
        // Create cells
                      
        vtkIdList *anIdList = vtkIdList::New();
        anIdList->SetNumberOfIds(1);
                      
        vtkCellArray *aCells = vtkCellArray::New();
        aCells->Allocate(2, 0);
                      
        vtkUnsignedCharArray* aCellTypesArray = vtkUnsignedCharArray::New();
        aCellTypesArray->SetNumberOfComponents(1);
        aCellTypesArray->Allocate(1);
                      
        anIdList->SetId(0, 0);
        aCells->InsertNextCell(anIdList);
        aCellTypesArray->InsertNextValue(VTK_VERTEX);
        anIdList->Delete();
                      
        VTKViewer_CellLocationsArray* aCellLocationsArray = VTKViewer_CellLocationsArray::New();
        aCellLocationsArray->SetNumberOfComponents(1);
        aCellLocationsArray->SetNumberOfTuples(1);
                      
        aCells->InitTraversal();
        vtkIdType npts;
        aCellLocationsArray->SetValue(0, aCells->GetTraversalLocation(npts));
                      
        aGrid->SetPoints(aPoints);
        aPoints->Delete();
                      
        aGrid->SetCells(aCellTypesArray,aCellLocationsArray,aCells);
        aCellLocationsArray->Delete();
        aCellTypesArray->Delete();
        aCells->Delete();
                      
        // Create and display actor
        vtkDataSetMapper* aMapper = vtkDataSetMapper::New();
        aMapper->SetInput(aGrid);
        aGrid->Delete();
                      
                      
        //actor->PickableOff();
        actor->SetMapper(aMapper);
        aMapper->Delete();
                      
        vtkProperty* aProp = vtkProperty::New();
        aProp->SetRepresentationToWireframe();
        aProp->SetColor(10, 10, 250); // @todo a revoir
        aProp->SetPointSize(5);
        actor->SetProperty(aProp);
        aProp->Delete();
    }
}

void DocumentGraphicView::_buildActor(SALOME_Actor* actor, HEXA_NS::Edge *value)
{
    if (dynamic_cast<HEXA_NS::Edge*>(value) != NULL)
    {
        double point1[3];
        point1[0] = value->getVertex(0)->getX();
        point1[1] = value->getVertex(0)->getY();                      
        point1[2] = value->getVertex(0)->getZ();

        double point2[3];
        point2[0] = value->getVertex(1)->getX();
        point2[1] = value->getVertex(1)->getY();                      
        point2[2] = value->getVertex(1)->getZ();
                      
        MyVTKLinePlotter linePlotter;
        float scalar = value->getVertex(0)->getScalar();
        linePlotter.PlotLine(point1, point2, scalar);
        linePlotter.CreateActor(actor);
    }
}

void DocumentGraphicView::_buildActor(SALOME_Actor* actor, HEXA_NS::Quad *value)
{
    if (dynamic_cast<HEXA_NS::Quad*>(value) != NULL)
    {

        double point1[3];
        point1[0] = value->getVertex(0)->getX();
        point1[1] = value->getVertex(0)->getY();                      
        point1[2] = value->getVertex(0)->getZ();

        double point2[3];
        point2[0] = value->getVertex(1)->getX();
        point2[1] = value->getVertex(1)->getY();                      
        point2[2] = value->getVertex(1)->getZ();

        double point3[3];
        point3[0] = value->getVertex(2)->getX();
        point3[1] = value->getVertex(2)->getY();                      
        point3[2] = value->getVertex(2)->getZ();

        double point4[3];
        point4[0] = value->getVertex(3)->getX();
        point4[1] = value->getVertex(3)->getY();                      
        point4[2] = value->getVertex(3)->getZ();

// methode qui "marche"        
//         MyVTKLinePlotter linePlotter;
//         float scalar = value->getVertex(0)->getScalar();
//         linePlotter.PlotLine(point1, point2, scalar);
//         linePlotter.PlotLine(point2, point3, scalar);
//         linePlotter.PlotLine(point3, point4, scalar);
//         linePlotter.PlotLine(point4, point1, scalar);        
//         linePlotter.CreateActor(actor);
// end

//         // test :
//         vtkPoints* quadPoints = vtkPoints::New();
//         quadPoints->SetNumberOfPoints(4);
//         quadPoints->InsertPoint(0, point1[0], point1[1], point1[2]);
//         quadPoints->InsertPoint(1, point2[0], point2[1], point2[2]);
//         quadPoints->InsertPoint(2, point3[0], point3[1], point3[2]);
//         quadPoints->InsertPoint(3, point4[0], point4[1], point4[2]);        
//         vtkQuad* aQuad = vtkQuad::New();
//         aQuad->GetPointIds()->SetId(0, 0);
//         aQuad->GetPointIds()->SetId(1, 1);
//         aQuad->GetPointIds()->SetId(2, 2);
//         aQuad->GetPointIds()->SetId(3, 3);        
//         vtkUnstructuredGrid* aQuadGrid = vtkUnstructuredGrid::New();
//         aQuadGrid->Allocate(1, 1);
//         aQuadGrid->InsertNextCell(aQuad->GetCellType(), aQuad->GetPointIds());
//         aQuadGrid->SetPoints(quadPoints);
//         vtkDataSetMapper* aQuadMapper = vtkDataSetMapper::New();
//         aQuadMapper->SetInput(aQuadGrid);
//         actor->SetMapper(aQuadMapper);
//         actor->AddPosition(2, 0, 2);
//         actor->GetProperty()->SetDiffuseColor(.2, 1, 1);     
        
    }
}

void DocumentGraphicView::_buildActor(SALOME_Actor* actor, HEXA_NS::Hexa *value)
{
    if (dynamic_cast<HEXA_NS::Hexa*>(value) != NULL)
    {
        double point1[3];
        point1[0] = value->getVertex(0)->getX();
        point1[1] = value->getVertex(0)->getY();                      
        point1[2] = value->getVertex(0)->getZ();

        double point2[3];
        point2[0] = value->getVertex(1)->getX();
        point2[1] = value->getVertex(1)->getY();                      
        point2[2] = value->getVertex(1)->getZ();

        double point3[3];
        point3[0] = value->getVertex(2)->getX();
        point3[1] = value->getVertex(2)->getY();                      
        point3[2] = value->getVertex(2)->getZ();

        double point4[3];
        point4[0] = value->getVertex(3)->getX();
        point4[1] = value->getVertex(3)->getY();                      
        point4[2] = value->getVertex(3)->getZ();

        double point5[3];
        point5[0] = value->getVertex(4)->getX();
        point5[1] = value->getVertex(4)->getY();                      
        point5[2] = value->getVertex(4)->getZ();

        double point6[3];
        point6[0] = value->getVertex(5)->getX();
        point6[1] = value->getVertex(5)->getY();                      
        point6[2] = value->getVertex(5)->getZ();

        double point7[3];
        point7[0] = value->getVertex(6)->getX();
        point7[1] = value->getVertex(6)->getY();                      
        point7[2] = value->getVertex(6)->getZ();

        double point8[3];
        point8[0] = value->getVertex(7)->getX();
        point8[1] = value->getVertex(7)->getY();                      
        point8[2] = value->getVertex(7)->getZ();
                      

        MyVTKLinePlotter linePlotter;
        float scalar = value->getVertex(0)->getScalar();        
        // face du bas (A)
        linePlotter.PlotLine(point1, point2, scalar);
        linePlotter.PlotLine(point2, point3, scalar);
        linePlotter.PlotLine(point3, point4, scalar);
        linePlotter.PlotLine(point4, point1, scalar);        

        // face du haut (B)
        linePlotter.PlotLine(point5, point6, scalar);        
        linePlotter.PlotLine(point6, point7, scalar);        
        linePlotter.PlotLine(point7, point8, scalar);        
        linePlotter.PlotLine(point8, point5, scalar);        

        // face gauche (C)
        linePlotter.PlotLine(point1, point5, scalar);
        linePlotter.PlotLine(point2, point6, scalar);        

        // face droite (D)
        linePlotter.PlotLine(point4, point8, scalar);
        linePlotter.PlotLine(point3, point7, scalar);        
        linePlotter.CreateActor(actor);

    }
}

// code repris de SMESHGUI_VTKUtils :
SALOME_Actor* DocumentGraphicView::FindActorByEntry(SUIT_ViewWindow *theWindow,
                                                    const char* theEntry)
{
    SVTK_ViewWindow* aViewWindow = dynamic_cast<SVTK_ViewWindow*>(theWindow);
    vtkRenderer *aRenderer = aViewWindow->getRenderer();
    VTK::ActorCollectionCopy aCopy(aRenderer->GetActors());
    vtkActorCollection *aCollection = aCopy.GetActors();
    aCollection->InitTraversal();
    while(vtkActor *anAct = aCollection->GetNextActor())
    {
        if(SALOME_Actor *anActor = dynamic_cast<SALOME_Actor*>(anAct))
        {
            if(anActor->hasIO())
            {
                Handle(SALOME_InteractiveObject) anIO = anActor->getIO();
                if(anIO->hasEntry() && strcmp(anIO->getEntry(), theEntry) == 0)
                {
                    return anActor;
                }
            }
        }
    }

    return NULL;
}

MyVTKLinePlotter::MyVTKLinePlotter()
    :m_scalarMin(0.0), m_scalarMax(1.0), 
     m_lookupTable(NULL), m_curPointID(0), m_allLineWidth(1)
{
    m_points = vtkPoints::New();
    m_lines = vtkCellArray::New();
    m_lineScalars = vtkFloatArray::New();
}

void MyVTKLinePlotter::SetScalarRange(double minval, double maxval)
{
    m_scalarMin = minval ;
    m_scalarMax = maxval ;
}

void MyVTKLinePlotter::SetLookupTable(vtkLookupTable* table)
{
    m_lookupTable = table ;
}

void MyVTKLinePlotter::PlotLine(double m[3], double n[3], double scalar)
{

    m_lineScalars->SetNumberOfComponents(1);
    m_points->InsertNextPoint(m);
    m_lineScalars->InsertNextTuple1(scalar);
    m_points->InsertNextPoint(n);
    m_lineScalars->InsertNextTuple1(scalar);

    m_lines->InsertNextCell(2);
    m_lines->InsertCellPoint(m_curPointID);
    m_lines->InsertCellPoint(m_curPointID+1);

    m_curPointID+=2;
}

void MyVTKLinePlotter::PlotLine(double x, double y, double z,
                                double x2, double y2, double z2, double scalar)
{
    double m[3],n[3] ;
    m[0]=x; m[1]=y; m[2]=z;
    n[0]=x2; n[1]=y2; n[2]=z2;
    PlotLine(m,n,scalar);
	
}


void MyVTKLinePlotter::SetAllLineWidth(int width)
{
    m_allLineWidth = width ;
}

vtkPolyData* MyVTKLinePlotter::CreatePolyData()
{
    // Create poly data 
    vtkPolyData* polyData = vtkPolyData::New();
    polyData->SetPoints(m_points);
    polyData->SetLines(m_lines);
    polyData->GetPointData()->SetScalars(m_lineScalars);

    return polyData;
}

//vtkActor* MyVTKLinePlotter::CreateActor()
bool MyVTKLinePlotter::CreateActor(SALOME_Actor* actor)    
{
	
    // Create poly data
    vtkPolyData* polyData = CreatePolyData();

    // create a color lookup table
    if (m_lookupTable==NULL)	
    {
        m_lookupTable = vtkLookupTable::New();
    }
		
    // create mapper
    vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
    mapper->SetInput(polyData);
    mapper->SetLookupTable(m_lookupTable);

    mapper->SetColorModeToMapScalars();
    mapper->SetScalarRange(m_scalarMin, m_scalarMax);
    mapper->SetScalarModeToUsePointData();

    // create actor
//	vtkActor actor = vtkActor::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetLineWidth(m_allLineWidth);


//	return actor ;	
}





// end JPL
