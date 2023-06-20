#pragma once

#include <QJsonValue>

#include "TypeManagers/nodetypemanager.h"
#include "TypeManagers/pintypemanager.h"
#include "GraphLib_global.h"

namespace GraphLib {

class TypedNode;
class Canvas;
enum class PinDirection;

namespace NodeFactoryModule {

class GRAPHLIB_EXPORT NodeFactory
{
public:
    NodeFactory();

    TypedNode *getNodeOfType(int typeID, Canvas *canvas);

    const NodeTypeManager *getNodeTypeManager() const { return _nodeTypeManager; }
    const PinTypeManager *getPinTypeManager() const { return _pinTypeManager; }

    void setNodeTypeManager(const NodeTypeManager *manager) { _nodeTypeManager = manager; }
    void setPinTypeManager(const PinTypeManager *manager) { _pinTypeManager = manager; }

private:
    void addPinsToNodeByJsonValue(const QJsonValue &val, TypedNode *node, PinDirection direction);

    const NodeTypeManager *_nodeTypeManager;
    const PinTypeManager *_pinTypeManager;
};

}

}
