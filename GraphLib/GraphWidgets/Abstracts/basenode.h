#pragma once

#include <QObject>
#include <QWidget>
#include <QPoint>
#include <QPointF>
#include <QPainter>
#include <QPaintEvent>
#include <QFont>
#include <QFontMetrics>
#include <QMap>

#include "abstractpin.h"
#include "GraphLib_global.h"

namespace GraphLib {

class Canvas;

class GRAPHLIB_EXPORT BaseNode : public QWidget
{
    Q_OBJECT

public:
    BaseNode(Canvas *canvas);
    BaseNode(int ID, Canvas *canvas);
    ~BaseNode();

    const QPointF &canvasPosition() const { return _canvasPosition; }
    int ID() const { return _ID; }
    const QSize &normalSize() const { return _normalSize; }
    float getParentCanvasZoomMultiplier() const;
    const QString &name() const { return _name; }
    QPoint getOutlineCoordinateForPinID(int pinID) const { return mapToParent(_pinsOutlineCoords[pinID]); }
    bool hasPinConnections() const;
    QSharedPointer< QMap<int, QVector<PinData> > > getPinConnections() const;
    const AbstractPin *getPinByID(int pinID) const { return _pins[pinID]; }
    QRect getMappedRect() const;
    const Canvas *getParentCanvas() const { return _parentCanvas; }
    const QString &getName() const { return _name; }

    void setCanvasPosition(QPointF newCanvasPosition) { _canvasPosition = newCanvasPosition; }
    void setID(int ID) { _ID = ID; }
    void setNormalSize(QSize newSize) { _normalSize = newSize; }
    void setName(QString name) { _name = name; }
    void removePinConnection(int pinID, int connectedPinID);
    void setPinConnection(int pinID, PinData connectedPin);
    void setPinConnected(int pinID, bool isConnected);
    void setSelected(bool b, bool bIsMultiSelectionModifierDown = false) { _bIsSelected = b; if (b) onSelect(bIsMultiSelectionModifierDown, _ID); }

    void moveCanvasPosition(QPointF vector) { _canvasPosition += vector; }

signals:
    void onSelect(bool bIsMultiSelectionModifierDown, int nodeID);
    void onPinDrag(PinDragSignal signal);
    void onPinConnect(PinData outPin, PinData inPin);
    void onPinConnectionBreak(PinData outPin, PinData inPin);

public slots:
    void addPin(AbstractPin *pin);
    void addPin(QString text, PinDirection direction, QColor color = QColor(Qt::GlobalColor::black));

private slots:
    void slot_onPinDrag(PinDragSignal signal);
    void slot_onPinConnect(PinData outPin, PinData inPin);
    void slot_onPinConnectionBreak(PinData outPin, PinData inPin);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:

// -------------------- RENDER HELPERS -----------------------

    void paint(QPainter *painter, QPaintEvent *event);
    virtual void paintSimplifiedName(QPainter *painter, int desiredWidth, QPoint textOrigin);
    virtual void paintName(QPainter *painter, int desiredWidth, QPoint textOrigin);
    virtual int calculateRowsOffset(QPainter *painter) const;

// -----------------------------------------------------------

protected:
    static unsigned int newID() { return IDgenerator++; }
    static unsigned int IDgenerator;

    const Canvas *_parentCanvas;
    int _ID;
    float _zoom;
    QSize _normalSize;
    QPainter *_painter;
    QPointF _canvasPosition;
    // Hidden position is used when the node is being moved for snapping
    QPointF _hiddenPosition;
    bool _bIsSelected;
    QPointF _lastMouseDownPosition;
    QPointF _mousePressPosition;
    QString _name;
    QMap<int, QPoint> _pinsOutlineCoords;

    QMap<int, AbstractPin*> _pins;
};

}
