#pragma once

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QBoxLayout>
#include <QVector2D>
#include <QPushButton>
#include <QScrollArea>

#include "GraphLib_global.h"
#include "TypeManagers/nodetypemanager.h"
#include "TypeManagers/pintypemanager.h"
#include "typednodeimage.h"
#include "nfbuttonminimize.h"

namespace GraphLib {

namespace NodeFactoryModule {

class GRAPHLIB_EXPORT NodeFactoryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NodeFactoryWidget(QWidget *parent = nullptr);
    ~NodeFactoryWidget();

    QSize getDesiredSize() const;
    const QPointF &getPosition() const { return _position; }
    int getGap() const { return _gap; }
    bool isMinimized() const { return _bIsMinimized; }

    inline void setDirection(QBoxLayout::Direction dir) { _layout.setDirection(dir); }
    void setGap(int g) { _gap = g; }
    void setPosition(QPointF pos) { _position = pos; }
    inline void setPosition(qreal x, qreal y) { setPosition(QPointF(x, y)); }
    void setX(qreal x) { setPosition(x, getPosition().y()); }
    void setY(qreal y) { setPosition(getPosition().x(), y); }
    void adjustPosition(QVector2D by) { _position += by.toPointF(); }
    inline void adjustPosition(qreal x, qreal y) { adjustPosition(QVector2D(x, y)); }
    void setMinimized(bool b) { _bIsMinimized = b; }
    bool initTypes();
    void clear();

    const NodeTypeManager *_nodeTypeManager;
    const PinTypeManager *_pinTypeManager;

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    void paint(QPainter *painter, QPaintEvent *event);

signals:
    void onMove(QVector2D offset);

private slots:
    void onButtonMinimizeClick();

private:
    QPainter *_painter;
    int _gap;
    bool _bIsMinimized;
    QPointF _position, _lastMouseDownPosition, _mousePressPosition;
    QWidget _layoutHolder;
    QBoxLayout _layout;
    NFButtonMinimize _btnMinimize;
    QVector<QSharedPointer<TypedNodeImage>> _nodeImages;
};

}

}
