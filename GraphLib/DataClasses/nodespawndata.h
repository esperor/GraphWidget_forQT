#pragma once

#include <QString>
#include <QtDebug>

#include "GraphLib_global.h"

namespace GraphLib {

class GRAPHLIB_EXPORT NodeSpawnData
{
public:
    NodeSpawnData() {}
    NodeSpawnData(const NodeSpawnData &other);
    NodeSpawnData(const QString &_name);

    QByteArray toByteArray();
    static NodeSpawnData fromByteArray(const QByteArray &byteArray);

    QString name;

};

class GRAPHLIB_EXPORT TypedNodeSpawnData : public NodeSpawnData
{
public:
    TypedNodeSpawnData() {}
    TypedNodeSpawnData(const TypedNodeSpawnData &other);
    TypedNodeSpawnData(const QString &_name, int _typeID);

    QByteArray toByteArray();
    static TypedNodeSpawnData fromByteArray(const QByteArray &byteArray);

    int typeID;

};

QDebug GRAPHLIB_EXPORT &operator<<(QDebug &debug, const NodeSpawnData &obj);

QDataStream GRAPHLIB_EXPORT &operator<<(QDataStream &out, const NodeSpawnData &obj);

inline bool operator<(const NodeSpawnData &first, const NodeSpawnData &second) { return first.name < second.name; }
inline bool operator>(const NodeSpawnData &first, const NodeSpawnData &second) { return first.name > second.name; }
bool GRAPHLIB_EXPORT operator==(const NodeSpawnData &first, const NodeSpawnData &second);


QDebug GRAPHLIB_EXPORT &operator<<(QDebug &debug, const TypedNodeSpawnData &obj);

QDataStream GRAPHLIB_EXPORT &operator<<(QDataStream &out, const TypedNodeSpawnData &obj);

inline bool operator<(const TypedNodeSpawnData &first, const TypedNodeSpawnData &second) { return first.typeID < second.typeID; }
inline bool operator>(const TypedNodeSpawnData &first, const TypedNodeSpawnData &second) { return first.typeID > second.typeID; }
bool GRAPHLIB_EXPORT operator==(const TypedNodeSpawnData &first, const TypedNodeSpawnData &second);

}

Q_DECLARE_METATYPE(GraphLib::NodeSpawnData)
Q_DECLARE_METATYPE(GraphLib::TypedNodeSpawnData)
