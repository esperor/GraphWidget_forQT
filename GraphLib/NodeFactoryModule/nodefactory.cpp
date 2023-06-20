#include <QJsonArray>

#include "GraphWidgets/typednode.h"
#include "GraphWidgets/canvas.h"
#include "GraphWidgets/pin.h"
#include "TypeManagers/nodetypemanager.h"
#include "TypeManagers/pintypemanager.h"
#include "nodefactory.h"
#include "utility.h"

namespace GraphLib {

namespace NodeFactoryModule {


NodeFactory::NodeFactory()
{}

TypedNode *NodeFactory::getNodeOfType(int typeID, Canvas *canvas)
{
    TypedNode *node = new TypedNode(typeID, canvas);

    node->setName(_nodeTypeManager->Types()[typeID].value("name").toString());
    node->setNodeTypeManager(_nodeTypeManager);
    node->setPinTypeManager(_pinTypeManager);

    const QJsonObject &type = _nodeTypeManager->Types()[typeID];
    QJsonValue inPins = type.value("in-pins");
    QJsonValue outPins = type.value("out-pins");

    if (inPins != QJsonValue::Undefined)
        addPinsToNodeByJsonValue(inPins, node, PinDirection::In);

    if (outPins != QJsonValue::Undefined)
        addPinsToNodeByJsonValue(outPins, node, PinDirection::Out);

    return node;
}

void NodeFactory::addPinsToNodeByJsonValue(const QJsonValue &val, TypedNode *node, PinDirection direction)
{
    const QVector<QJsonObject> &pinTypes = _pinTypeManager->Types();
    QJsonArray pins = val.toArray();

    for (auto it = pins.begin(); it < pins.end(); it++)
    {
        QJsonObject pinObject = (*it).toObject();

        QString typeName = pinObject.value("type").toString();
        int id = _pinTypeManager->TypeNames()[typeName];
        const QJsonObject &type = pinTypes[id];
        QString colorString = type.value("color").toString();
        QColor color = parseToColor(colorString);

        Pin *pin = new Pin(node);
        pin->setColor(color);
        pin->setText(typeName);
        pin->setDirection(direction);

        node->addPin(pin);
    }
}


}

}
