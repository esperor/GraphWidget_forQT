
#pragma once


#include <QObject>
#include <QWidget>

namespace GraphLib {

namespace NodeFactory {

class NodeFactoryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NodeFactoryWidget(QWidget *parent = nullptr);

signals:

};

}

}
