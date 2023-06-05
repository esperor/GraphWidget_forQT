#include <QJsonArray>

#include "GraphWidgets/typednode.h"
#include "GraphWidgets/canvas.h"
#include "GraphWidgets/pin.h"
#include "TypeManagers/nodetypemanager.h"
#include "TypeManagers/pintypemanager.h"
#include "nodefactory.h"

namespace GraphLib {

namespace NodeFactory {

TypedNode *getNodeOfType(int typeID, Canvas *canvas)
{
    TypedNode *node = new TypedNode(typeID, canvas);

    const QJsonObject &type = NodeTypeManager::Types()[typeID];
    QJsonValue inPins = type.value("in-pins");
    QJsonValue outPins = type.value("out-pins");

    if (inPins != QJsonValue::Undefined)
        addPinsToNodeByJsonValue(inPins, node, PinDirection::In);

    if (outPins != QJsonValue::Undefined)
        addPinsToNodeByJsonValue(outPins, node, PinDirection::Out);

    return node;
}

void addPinsToNodeByJsonValue(const QJsonValue &val, TypedNode *node, PinDirection direction)
{
    const QVector<QJsonObject> &pinTypes = PinTypeManager::Types();
    QJsonArray pins = val.toArray();

    for (auto it = pins.begin(); it < pins.end(); it++)
    {
        QJsonObject pinObject = (*it).toObject();

        QString typeName = pinObject.value("type").toString();
        int id = PinTypeManager::TypeNames()[typeName];
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

QColor parseToColor(const QString &str)
{
    const short rgbNums = 3;

    // Split the color string into 3 numbers in 16 base
    QString list[rgbNums];
    for (int i = 0; i < rgbNums; i++)
        list[i] = str.sliced(i * 2, 2);

    int nums[rgbNums];
    int i = 0;

    for (auto elem : list)
    {
        bool ok;
        int num = elem.toInt(&ok, 16);
        if (!ok || i >= rgbNums)
        {
            qDebug() << num << "should be" << elem;
            return QColor(0,0,0);
        }

        nums[i++] = num;
    }
    return QColor(nums[0], nums[1], nums[2]);
}

NodeFactory::NodeFactory()
{

}

}

}
