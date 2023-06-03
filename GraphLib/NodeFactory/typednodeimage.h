#pragma once

#include <QObject>
#include <QWidget>

#include "DataClasses/nodespawndata.h"

namespace GraphLib {

namespace NodeFactory {

class TypedNodeImage : public QWidget
{
    Q_OBJECT

public:
    explicit TypedNodeImage(QWidget *parent = nullptr);
    TypedNodeImage(QString type, QWidget *parent = nullptr);
    ~TypedNodeImage();

    TypedNodeSpawnData getData() const;

    QString typeName;
    int typeID;

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    void paint(QPainter *painter, QPaintEvent *event);

signals:

private:
    QPainter *_painter;

};

}

}
