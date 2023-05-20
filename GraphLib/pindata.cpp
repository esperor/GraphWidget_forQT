
#include "pindata.h"
#include "constants.h"
#include "abstractpin.h"

namespace GraphLib {

PinData::PinData(const AbstractPin *pin)
    : pinDirection{ pin->getDirection() }
    , nodeID{ pin->getNodeID() }
    , pinID{ pin->getID() }
    , pinText{ pin->getText() }
    , color{ pin->getColor() }
{}

PinData::PinData(const PinData &other)
    : pinDirection{ other.pinDirection }, nodeID{ other.nodeID }, pinID{ other.pinID }, pinText{ other.pinText }, color{ other.color }
{}

PinData::PinData(PinDirection _direction, int _nodeID, int _pinID, const QString &_text, const QColor &_color)
    : pinDirection{ _direction }, nodeID{ _nodeID }, pinID{ _pinID }, pinText{ _text }, color{ _color }
{}

PinData::~PinData() {}

QByteArray PinData::toByteArray()
{
    QByteArray output;
    output.append(QByteArray::number(static_cast<int>(pinDirection == PinDirection::In)));
    output.append(c_pinDataSeparator);

    output.append(QByteArray::number(nodeID))           .append(c_pinDataSeparator);
    output.append(QByteArray::number(pinID))            .append(c_pinDataSeparator);
    output.append(pinText.toStdString())                .append(c_pinDataSeparator);
    output.append(QByteArray::number(color.red()))      .append(c_pinDataSeparator);
    output.append(QByteArray::number(color.green()))    .append(c_pinDataSeparator);
    output.append(QByteArray::number(color.blue()))     .append(c_pinDataSeparator);
    output.append(QByteArray::number(color.alpha()))    .append(c_pinDataSeparator);
    return output;
}

PinData PinData::fromByteArray(const QByteArray &byteArray)
{
    PinData data;
    QList<QByteArray> arrays = byteArray.split(c_pinDataSeparator);
    data.pinDirection = static_cast<bool>(arrays[0].toInt()) ?
                            PinDirection::In : PinDirection::Out;

    data.nodeID = arrays[1].toInt();
    data.pinID = arrays[2].toInt();
    data.pinText = QString::fromStdString(arrays[3].toStdString());

    int r, g, b, alpha;
    r = arrays[4].toInt();
    g = arrays[5].toInt();
    b = arrays[6].toInt();
    alpha = arrays[7].toInt();
    data.color = QColor(r, g, b, alpha);

    return data;
}

QDebug &operator<<(QDebug &debug, const PinData &obj)
{
    bool temp = debug.autoInsertSpaces();
    debug.setAutoInsertSpaces(false);

    debug << (obj.pinDirection == PinDirection::In ? "In-" : "Out-")
          << "Pin(NodeID: " << obj.nodeID <<  ", pinID: " << obj.pinID << ", text: " << obj.pinText << ")";

    debug.setAutoInsertSpaces(temp);
    return debug;
}

QDataStream &operator<<(QDataStream &out, const PinData &obj)
{
    out << (obj.pinDirection == PinDirection::In ? "In-" : "Out-")
        << "Pin(NodeID: " << obj.nodeID <<  ", pinID: " << obj.pinID << ", text: " << obj.pinText << ")";

    return out;
}

bool operator<(const PinData &first, const PinData &second)
{
    return first.pinID < second.pinID;
}

bool operator>(const PinData &first, const PinData &second)
{
    return first.pinID > second.pinID;
}

bool operator==(const PinData &first, const PinData &second)
{
    return first.nodeID == second.nodeID
           && first.pinID == second.pinID
           && first.pinDirection == second.pinDirection
           && first.pinText == second.pinText
           && first.color == second.color;
}

}
