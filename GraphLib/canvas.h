#pragma once

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPointF>
#include <QVector>
#include <QWheelEvent>
#include <optional>
#include <variant>

#include "node.h"

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

    void setSnappingInterval(int num) { _snappingInterval = num; }

public slots:
    void addNode(QPoint canvasPosition, QString name);

signals:

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;

private slots:
    void onPinDrag(PinDragSignal signal);
    void onPinConnect(PinData outPin, PinData inPin);

private:
    void paint(QPainter *painter, QPaintEvent *event);
//    QVector<QPainterPath

    QPainter *_painter;
    int _dotPaintGap;
    bool _bIsMouseDown;
    std::optional<PinData> _draggedPin;

    // If there is no target, _draggedPinTarget will be null
    // If the target is cursor, _draggedPinTarget's value will be null
    // Else there will be an object of PinData struct
    std::optional< std::optional<PinData> > _draggedPinTargetInfo;
    QPoint _draggedPinTarget;
    QPointF _position, _lastMouseDownPosition, _mousePosition;
    short _zoom;
    int _accumulativeZoomDelta;

    int _snappingInterval;
    bool _bIsSnappingEnabled;

    QVector<Node*> _nodes;

    // Key for _connectedPins is an out-pin and the value is an in-pin
    QMultiMap<PinData, PinData> _connectedPins;
    QMap<int, float> _zoomMultipliers =
        {
            {   0, 2.0f  },
            {  -1, 1.5f  },
            {  -2, 1.0f  },
            {  -3, 0.90f },
            {  -4, 0.75f },
            {  -5, 0.6f  },
            {  -6, 0.5f  },
            {  -7, 0.4f  },
            {  -8, 0.3f  },
            {  -9, 0.2f  },
            { -10, 0.1f  },
        };
};

}
