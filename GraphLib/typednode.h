#pragma once

#include <QObject>
#include <QWidget>

#include "Abstracts/basenode.h"

namespace GraphLib {

class TypedNode : public BaseNode
{
    Q_OBJECT
public:
    TypedNode(int ID, int typeID, Canvas *canvas, QWidget *parent = nullptr);

private:
    int _typeID;
};

}
