#pragma once

#include <QObject>
#include <QWidget>

#include "GraphWidgets/Abstracts/basenode.h"

namespace GraphLib {

class TypedNode : public BaseNode
{
    Q_OBJECT

public:
    TypedNode(int typeID, Canvas *canvas);
    TypedNode(int ID, int typeID, Canvas *canvas);

    int getTypeID() const { return _typeID; }

    void setTypeID(int newTypeID) { _typeID = newTypeID; }

private:
    int _typeID;
};
// NodeTypeManager::Types()[data.typeID].value("name").toString()

}
