#pragma once

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QWeakPointer>
#include <QSharedPointer>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPointF>
#include <QVector>
#include <QTimer>
#include <QWheelEvent>
#include <optional>
#include <variant>

#include "NodeFactory/nodefactorywidget.h"
#include "GraphWidgets/Abstracts/basenode.h"
#include "GraphLib_global.h"


namespace GraphLib {

class GRAPHLIB_EXPORT Canvas : public QWidget
{
    Q_OBJECT

public:
    Canvas(QWidget *parent = nullptr);
    ~Canvas();

    float getZoomMultiplier() const     { return _zoomMultipliers[_zoom]; }
    bool getSnappingEnabled() const     { return _bIsSnappingEnabled; }
    int getSnappingInterval() const     { return _snappingInterval; }
    const QPointF &getOffset() const    { return _offset; }

    void setSnappingInterval(int num) { _snappingInterval = num; }
    void moveView(QVector2D vector);
    void moveViewUp(float by);
    void moveViewDown(float by);
    void moveViewLeft(float by);
    void moveViewRight(float by);

    QPointF mapToCanvas(QPointF point) const;
    QPoint mapToCanvas(QPoint point) const;

    // If one or more of params of QPointF is negative, current mouse position will be used
    void zoomIn(int times = 1, QPointF where = QPointF(-1, -1));

    // If one of params of QPointF is negative, current mouse position will be used
    void zoomOut(int times = 1, QPointF where = QPointF(-1, -1));

    QWeakPointer<BaseNode> addBaseNode(QPoint canvasPosition, QString name);
    QWeakPointer<BaseNode> addNode(BaseNode *node);
    QWeakPointer<BaseNode> addTypedNode(QPoint canvasPosition, int typeID);

public slots:
    void moveCanvas(QPointF offset);

signals:
    void onNodesRemoved();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onNodeSelect(bool bIsMultiSelectionModifierDown, int nodeID);
    void onPinDrag(PinDragSignal signal);
    void onPinConnect(PinData outPin, PinData inPin);
    void tick();

private:
    void paint(QPainter *painter, QPaintEvent *event);
    void moveCanvasOnPinDragNearEdge(QPointF mousePosition);
    void zoom(int times, QPointF where);
    static unsigned int newID() { return IDgenerator++; }
    static unsigned int IDgenerator;

    QPainter *_painter;
    int _dotPaintGap;
    std::optional<PinData> _draggedPin;

    // If there is no target, _draggedPinTarget will be null
    // If the target is cursor, _draggedPinTarget's value will be null
    // Else there will be an object of PinData struct
    std::optional< std::optional<PinData> > _draggedPinTargetInfo;
    QPoint _draggedPinTarget;
    QPointF _offset, _lastMouseDownPosition, _mousePosition;
    short _zoom;
    int _accumulativeZoomDelta;

    int _snappingInterval;
    bool _bIsSnappingEnabled;
    std::optional<QRect> _selectionRect;

    QMap<int, QSharedPointer<BaseNode>> _nodes;

    // Key for _connectedPins is an out-pin and the value is an in-pin
    QMultiMap<PinData, PinData> _connectedPins;
    QTimer *_timer;
    NodeFactory::NodeFactoryWidget *_nfWidget;
    QMap<int, QSharedPointer<BaseNode>> _selectedNodes;

    const static QMap<short, float> _zoomMultipliers;
};

}
