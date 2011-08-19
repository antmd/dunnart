/*
 * Dunnart - Constraint-based Diagram Editor
 *
 * Copyright (C) 2003-2007  Michael Wybrow
 * Copyright (C) 2006-2008  Monash University
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
 * MA  02110-1301, USA.
 * 
 *
 * Author(s): Michael Wybrow  <http://michael.wybrow.info/>
*/

#ifndef CANVAS_H_
#define CANVAS_H_

#include <QGraphicsScene>
#include <QAction>
#include <QEvent>
#include <QList>
#include <QStack>
#include <QString>
#include <QDomDocument>
#include <QUndoCommand>

class QToolBar;
class QStatusBar;
class QSvgRenderer;
class QUndoStack;
class QUndoCommand;
class QFileInfo;

namespace Avoid {
class Router;
}

namespace dunnart {

namespace gml {
class Graph;
}

class CanvasItem;
class Guideline;
class GraphLayout;
class SelectionResizeHandle;
class UndoMacro;

typedef QList<CanvasItem *> CObjList;

class Actions {
    public:
        unsigned int flags;
        CObjList moveList;
        CObjList resizeList;

        Actions();
        void clear(void);
        bool empty(void) const;
};


static const unsigned int ACTION_NONE           = 0;
static const unsigned int ACTION_ADDITIONS      = 1;
static const unsigned int ACTION_DELETIONS      = 2;
static const unsigned int ACTION_MODIFICATIONS  = 4;


class Canvas : public QGraphicsScene
{
    Q_OBJECT

    Q_PROPERTY (bool automaticGraphLayout READ optAutomaticGraphLayout WRITE setOptAutomaticGraphLayout)
    Q_PROPERTY (bool preventOverlaps READ optPreventOverlaps WRITE setOptPreventOverlaps)
    Q_PROPERTY (bool preserveTopology READ optPreserveTopology WRITE setOptPreserveTopology)
    Q_PROPERTY (bool rubberBandRouting READ optRubberBandRouting WRITE setOptRubberBandRouting)
    Q_PROPERTY (bool fitDiagramWithinPage READ optFitWithinPage WRITE setOptFitWithinPage)
    //Q_PROPERTY (bool colourInterferingConnectors READ optColourInterferingConnectors)
    Q_PROPERTY (double idealConnectorLengthModifier READ optIdealConnectorLengthModifier WRITE set_ideal_connector_length)
    Q_PROPERTY (int connectorRoundingDistance READ optConnectorRoundingDistance WRITE setOptConnRoundingDist)
    Q_PROPERTY (int connectorSegmentPenalty READ optConnPenaltySegment WRITE setOptConnPenaltySegment)
    Q_PROPERTY (bool structuralEditingDisabled READ optStructuralEditingDisabled WRITE setOptStructuralEditingDisabled)

    public:
        Canvas();
        virtual ~Canvas();

        void loadSvgDiagram(const QString& filename);
        void loadGmlDiagram(const QString& filename);
        void loadSVGRootNodeAttributes(const QDomElement& svgRoot);
        void startLayoutUpdateTimer(void);
        void startLayoutFinishTimer(void);
        void set_filename(QString filename);
        QString filename(void);
        QList<CanvasItem *> items(void) const;
        QList<CanvasItem *> selectedItems(void) const;
        void postDiagramLoad(void);
        void setExpandedPage(const QRectF newExpandedPage);
        QRectF pageRect(void) const;
        void setPageRect(const QRectF &rect);
        GraphLayout *layout(void) const;
        Avoid::Router *router(void) const;
        void setStatusBar(QStatusBar *statusBar);
        void pushStatusMessage(const QString& message);
        void popStatusMessage(void);
        void highlightIndicatorsForItemMove(CanvasItem *item);
        void moveSelectionResizeHandle(const int index, const QPointF pos);
        void storeSelectionResizeInfo(void);

        QString saveConstraintInfoToString(void) const;
        void loadConstraintInfoFromString(const QString& constraintInfo);

        // XXX These need to be reworked and given meaningful names:
        void interrupt_graph_layout(void);
        void stop_graph_layout(void);
        void restart_graph_layout(void);
        void fully_restart_graph_layout(void);

        CanvasItem *getItemByID(QString ID) const;
        CanvasItem *getItemByInternalId(uint internalId) const;
        void processSelectionDropEvent(QGraphicsSceneMouseEvent *event);
        
        bool optAutomaticGraphLayout(void) const;
        bool optPreventOverlaps(void) const;
        bool optPreserveTopology(void) const;
        bool optRubberBandRouting(void) const;
        bool optFitWithinPage(void) const;
        bool optColourInterferingConnectors(void) const;
        bool optStructuralEditingDisabled(void) const;
        double optIdealConnectorLengthModifier(void) const;
        int optConnectorRoundingDistance(void) const;
        int optConnPenaltySegment(void) const;

        bool overlayRouterObstacles(void) const;
        bool overlayRouterVisGraph(void) const;
        bool overlayRouterOrthogonalVisGraph(void) const;
        Actions& getActions(void);
        QString assignStringId(QString id);
        uint assignInternalId(void);

        void setLayoutSuspended(bool suspend);
        bool isLayoutSuspended(void) const;

        void setDraggedItem(CanvasItem *item);
        bool processingLayoutUpdates(void) const;

        QSvgRenderer *svgRenderer(void) const;
        QUndoStack *undoStack(void) const;
        UndoMacro *currentUndoMacro(void);
        UndoMacro *beginUndoMacro(const QString& text);
        void endUndoMacro(void);

        // XXX Should be private:
        void recursiveReadSVG(const QDomNode& start, const QString& dunnartNS,
                int pass);
        void saveDiagramAsSVG(QString save_filename);
        const QList<QColor> interferingConnectorColours(void) const;
        double visualPageBuffer(void) const;
        bool useGmlClusters(void) const;
        void setNudgeDistance(const double dist);
        void setIdealConnectorLength(const double length);
        double idealConnectorLength(void) const;
        bool avoidConnectorCrossings(void) const;
        bool avoidClusterCrossings(void) const;
        bool forceOrthogonalConnectors(void) const;

    public slots:
        void bringToFront(void);
        void sendToBack(void);
        void deselectAll(void);
        void cutSelection(void);
        void copySelection(void);
        void pasteSelection(void);
        void deleteSelection(void);
        void lockSelectedShapes(void);
        void selectAll(void);
        void templateFromSelection(int type);
        void alignSelection(int type);
        void distributeSelection(int type);
        void separateSelection(int type);

        void customEvent(QEvent *event);
        void set_ideal_connector_length_from_slider(int int_modifier);
        void set_ideal_connector_length(double modifier);

        void setDebugCOLAOutput(const bool value);
        void setOptAutomaticGraphLayout(const bool value);
        void setOptPreventOverlaps(const bool value);
        void setOptPreserveTopology(const bool value);
        void setOptRubberBandRouting(const bool value);
        void setOptFitWithinPage(const bool value);
        void setOptConnPenaltySegment(const int value);
        void setOptConnRoundingDist(const int value);
        void setOptStructuralEditingDisabled(const bool value);

        void processResponseTasks(void);
        void processUndoResponseTasks(void);


        void setOverlayRouterObstacles(const bool value);
        void setOverlayRouterVisGraph(const bool value);
        void setOverlayRouterOrthogonalVisGraph(const bool value);

        // When rendering for printing, constraint indicators, selection
        // cues and other decorations are not painted.  This mode is used
        // for printing documents as well as exporting SVG, PDF and PS files.
        bool isRenderingForPrinting(void) const;
        void setRenderingForPrinting(const bool printingMode);

    signals:
        void diagramFilenameChanged(const QFileInfo& title);
        void optChangedAutomaticLayout(bool checked);
        void optChangedPreserveTopology(bool checked);
        void optChangedPreventOverlaps(bool checked);
        void optChangedRubberBandRouting(bool checked);
        void optChangedFitWithinPage(bool checked);
        void optChangedStructuralEditingDisabled(bool checked);
        void debugOverlayEnabled(bool enabled);
        void clipboardContentsChanged(void);

    private slots:
        void processLayoutUpdateEvent(void);
        void processLayoutFinishedEvent(void);
        void selectionChangeTriggers(void);
    protected:
        virtual void drawBackground(QPainter *painter, const QRectF& rect);
        virtual void drawForeground(QPainter *painter, const QRectF& rect);
        virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    private:
        bool idIsUnique(QString id) const;
        void recursiveMapIDs(QDomNode start, const QString& ns, int pass);
        bool singlePropUpdateID(QDomElement& node, const QString& prop,
                const QString ns = QString());
        QDomElement writeLayoutOptionsToDomElement(QDomDocument& doc) const;
        void loadLayoutOptionsFromDomElement(const QDomElement& options);
        void setInterferingConnectorColours(const QString colourListString);
        void hideSelectionResizeHandles(void);
        void repositionAndShowSelectionResizeHandles(
                const bool calculatePosition = false);
        void createIndicatorHighlightCache(void);
        //static void highlightIndicators(ShapeObj *shape, const QRectF& shapeRect);
        void clearIndicatorHighlights(const bool clearCache = false);
        void glueObjectsToIndicators(void);
        bool hasVisibleOverlays(void) const;

        double m_visual_page_buffer;
        QString m_filename;
        QTimer *m_layout_update_timer;
        QTimer *m_layout_finish_timer;
        bool m_processing_layout_updates;
        QDomDocument m_clipboard;
        QRectF m_page;
        QRectF m_expanded_page;
        GraphLayout* m_graphlayout;
        Avoid::Router *m_router;
        QSvgRenderer *m_svg_renderer;
        QStatusBar *m_status_bar;
        QStack<QString> m_status_messages;
        uint m_max_string_id;
        uint m_max_internal_id;
        gml::Graph *m_gml_graph;
        bool m_use_gml_clusters;

        double m_connector_nudge_distance;
        double m_ideal_connector_length;
        double m_directed_edge_height_modifier;
        bool m_rectangle_constraint_test;
        bool m_sticky_nodes;
        bool m_downward_edges;
        bool m_avoid_connector_crossings;
        bool m_avoid_cluster_crossings;
        bool m_nudge_orthogonal_routes;
        bool m_simple_paths_during_layout;
        bool m_batch_diagram_layout;
        bool m_force_orthogonal_connectors;

        double m_opt_ideal_connector_length_modifier;
        int  m_opt_connector_rounding_distance;
        bool m_opt_automatic_graph_layout;
        bool m_opt_prevent_overlaps;
        bool m_opt_preserve_topology;
        bool m_opt_rubber_band_routing;
        bool m_opt_fit_within_page;
        bool m_opt_colour_interfering_connectors;
        bool m_opt_stuctural_editing_disabled;
        Actions m_actions;

        std::map<QString, QString> m_paste_id_map;
        std::list<QString> m_paste_bad_constraint_ids;
        // list of nodes in other namespaces
        QList<QDomNode> m_external_node_list;
        QMap<QString, QString> m_extra_namespaces_map;

        // Default list of connector colors.
        QList<QColor> m_default_connector_colours;

        // List of connector colors.
        // This list is used for automatic coloring of connectors that cross or
        // have shared paths, by coloring interference graph.
        // It will be either as sepcified through the interferingConnectorColours
        // dunnart option in XML, set with the setInterferingConnectorColours()
        // fuction, or defaults to m_interfering_connector_colours.
        // Access via interferingConnectorColours().
        QList<QColor> m_interfering_connector_colours;

        std::map<int, Guideline *> m_vguides, m_hguides;
        CanvasItem *m_dragged_item;
        CanvasItem *m_lone_selected_item;
        QUndoStack *m_undo_stack;
        UndoMacro *m_current_undo_macro;

        QRectF selectionShapesBoundingRect;
        QVector<SelectionResizeHandle *> selectionResizeHandles;
        QVector<QRectF> selectionResizeInfo;

        bool m_overlay_router_obstacles;
        bool m_overlay_router_visgraph;
        bool m_overlay_router_orthogonal_visgraph;
        bool m_rendering_for_printing;

#ifdef FPSTIMER
        clock_t startTime;
        clock_t clickUpTime;
        clock_t stopTime;
        clock_t feasibleStartTime = 0;
        clock_t feasibleEndTime = 0;
        clock_t totalTime = 0;
        unsigned int updates = 0;
        bool timerRunning = false;
#endif

        friend class GraphLayout;
        friend class GraphData;
        friend class UndoMacro;
};


class LayoutUpdateEvent : public QEvent
{
    public:
        LayoutUpdateEvent() :
            QEvent((QEvent::Type) (QEvent::User + 1))
        {
        }
};  
        
class LayoutFinishedEvent : public QEvent
{
    public:
        LayoutFinishedEvent() :
            QEvent((QEvent::Type) (QEvent::User + 2))
        {
        }
};  


}
#endif // CANVAS_H_

// vim: filetype=cpp ts=4 sw=4 et tw=0 wm=0 cindent
