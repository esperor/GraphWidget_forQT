#include <QByteArray>

#include "nodespawndata.h"
#include "constants.h"

namespace GraphLib {

NodeSpawnData::NodeSpawnData(const NodeSpawnData &other) : name{ other.name }
{}

NodeSpawnData::NodeSpawnData(const QString &_name)
    : name{ _name } {}

QByteArray NodeSpawnData::toByteArray()
{
    QByteArray output;
    output.append(name.toStdString());
    return output;
}

NodeSpawnData NodeSpawnData::fromByteArray(const QByteArray &byteArray)
{
    unsigned short i = 0;

    NodeSpawnData data;
    QList<QByteArray> arrays = byteArray.split(c_dataSeparator);

    data.name = QString::fromStdString(arrays[i++].toStdString());

    return data;
}



TypedNodeSpawnData::TypedNodeSpawnData(const TypedNodeSpawnData &other)
    : NodeSpawnData{ other.name }
    , typeID{ other.typeID }
{}

TypedNodeSpawnData::TypedNodeSpawnData(const QString &_name, int _typeID)
    : NodeSpawnData{ _name }
    , typeID{ _typeID }
{}

QByteArray TypedNodeSpawnData::toByteArray()
{
    QByteArray output;
    output.append(name.toStdString());
    output.append(c_dataSeparator);
    output.append(QByteArray::number(typeID));
    return output;
}

TypedNodeSpawnData TypedNodeSpawnData::fromByteArray(const QByteArray &byteArray)
{
    unsigned short i = 0;

    TypedNodeSpawnData data;
    QList<QByteArray> arrays = byteArray.split(c_dataSeparator);

    data.name = QString::fromStdString(arrays[i++].toStdString());
    data.typeID = arrays[i++].toInt();

    return data;
}


QDebug &operator<<(QDebug &debug, const NodeSpawnData &obj)
{
    bool temp = debug.autoInsertSpaces();
    debug.setAutoInsertSpaces(false);

    debug << "NodeSpawnData(" << obj.name << ")";

    debug.setAutoInsertSpaces(temp);
    return debug;
}

QDataStream &operator<<(QDataStream &out, const NodeSpawnData &obj)
{
    out << "NodeSpawnData(" << obj.name << ")";
    return out;
}

bool operator==(const NodeSpawnData &first, const NodeSpawnData &second) { return first.name == second.name; }

QDebug &operator<<(QDebug &debug, const TypedNodeSpawnData &obj)
{
    bool temp = debug.autoInsertSpaces();
    debug.setAutoInsertSpaces(false);

    debug << "TypedNodeSpawnData(" << obj.name << ", " << obj.typeID << ")";

    debug.setAutoInsertSpaces(temp);
    return debug;
}

QDataStream &operator<<(QDataStream &out, const TypedNodeSpawnData &obj)
{
    out << "TypedNodeSpawnData(" << obj.name << ", " << obj.typeID << ")";
    return out;
}

bool operator==(const TypedNodeSpawnData &first, const TypedNodeSpawnData &second)
{
    return first.name == second.name &&
           first.typeID == second.typeID;
}

}
