#include "pindata.h"
#include "constants.h"
#include "GraphWidgets/Abstracts/abstractpin.h"

namespace GraphLib {

PinData::PinData(const AbstractPin *pin)
    : pinDirection{ pin->getDirection() }
    , nodeID{ pin->getNodeID() }
    , pinID{ pin->ID() }
{}

PinData::PinData(const PinData &other)
{ *this = other; }

PinData::PinData(PinDirection _direction, int _nodeID, int _pinID, int _typeID)
    : pinDirection{ _direction }, nodeID{ _nodeID }, pinID{ _pinID }, typeID{ _typeID }
{}

PinData::~PinData() {}

void PinData::operator=(const PinData &other)
{
    pinDirection = other.pinDirection;
    nodeID = other.nodeID;
    pinID = other.pinID;
    typeID = other.typeID;
}

QByteArray PinData::toByteArray() const
{
    QByteArray output;
    output.append(QByteArray::number(static_cast<int>(pinDirection == PinDirection::In)));
    output.append(c_dataSeparator);

    output.append(QByteArray::number(nodeID))           .append(c_dataSeparator);
    output.append(QByteArray::number(pinID))            .append(c_dataSeparator);
    output.append(QByteArray::number(typeID));
    return output;
}

PinData PinData::fromByteArray(const QByteArray &byteArray)
{
    unsigned short i = 0;

    PinData data;
    QList<QByteArray> arrays = byteArray.split(c_dataSeparator);
    data.pinDirection = static_cast<bool>(arrays[i++].toInt()) ?
                            PinDirection::In : PinDirection::Out;

    data.nodeID = arrays[i++].toInt();
    data.pinID = arrays[i++].toInt();
    data.typeID = arrays[i++].toInt();

    return data;
}

QDebug &operator<<(QDebug &debug, const PinData &obj)
{
    bool temp = debug.autoInsertSpaces();
    debug.setAutoInsertSpaces(false);

    debug << (obj.pinDirection == PinDirection::In ? "In-" : "Out-")
          << "Pin(NodeID: " << obj.nodeID <<  ", pinID: " << obj.pinID << ")";

    debug.setAutoInsertSpaces(temp);
    return debug;
}

QDataStream &operator<<(QDataStream &out, const PinData &obj)
{
    out << (obj.pinDirection == PinDirection::In ? "In-" : "Out-")
        << "Pin(NodeID: " << obj.nodeID <<  ", pinID: " << obj.pinID << ")";

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
           && first.pinDirection == second.pinDirection;
}

}
