#pragma once

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QBoxLayout>
#include <QScrollArea>

#include "GraphLib_global.h"
#include "typednodeimage.h"

namespace GraphLib {

namespace NodeFactory {

class GRAPHLIB_EXPORT NodeFactoryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NodeFactoryWidget(QWidget *parent = nullptr);
    ~NodeFactoryWidget();

    inline void setDirection(QBoxLayout::Direction dir) { _layout.setDirection(dir); }
    bool initTypes();
    void clear();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void paint(QPainter *painter, QPaintEvent *event);

signals:

private:
    QPainter *_painter;
    QBoxLayout _layout;
    QVector<QSharedPointer<TypedNodeImage>> _nodeImages;
};

}

}
