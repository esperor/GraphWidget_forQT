#pragma once

#include <QObject>
#include <QWidget>

#include "GraphWidgets/Abstracts/basenode.h"

namespace GraphLib {

class NodeTypeManager;
class PinTypeManager;

class TypedNode : public BaseNode
{
    Q_OBJECT

public:
    TypedNode(int typeID, Canvas *canvas);
    TypedNode(int ID, int typeID, Canvas *canvas);

    int getTypeID() const { return _typeID; }

    void setTypeID(int newTypeID) { _typeID = newTypeID; }

    const NodeTypeManager *getNodeTypeManager() const { return _nodeTypeManager; }
    const PinTypeManager *getPinTypeManager() const { return _pinTypeManager; }

    void setNodeTypeManager(const NodeTypeManager *manager) { _nodeTypeManager = manager; }
    void setPinTypeManager(const PinTypeManager *manager) { _pinTypeManager = manager; }

private:
    void paintName(QPainter *painter, int desiredWidth, QPoint textOrigin) override;
    int calculateRowsOffset(QPainter *painter) const override;

    const NodeTypeManager *_nodeTypeManager;
    const PinTypeManager *_pinTypeManager;

protected:
    int _typeID;

};

}
