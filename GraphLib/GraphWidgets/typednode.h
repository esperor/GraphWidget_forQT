#pragma once

#include <QObject>
#include <QWidget>

#include "GraphWidgets/Abstracts/basenode.h"

namespace GraphLib {

class TypedNode : public BaseNode
{
    Q_OBJECT
public:
    TypedNode(int ID, int typeID, Canvas *canvas, QWidget *parent = nullptr);

private:
    int _typeID;
};
// NodeTypeManager::Types()[data.typeID].value("name").toString()

}
