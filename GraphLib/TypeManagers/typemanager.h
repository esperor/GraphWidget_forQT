#pragma once

#include <QJsonObject>
#include <QMap>

namespace GraphLib {

class TypeManager
{
public:
    TypeManager() {}

    const QVector<QJsonObject> &Types() const { return _types; }
    const QMap<QString, int> &TypeNames() const { return _typeNames; }

    inline QString typeNameByID(int id) const { return Types()[id].value("name").toString(); }

    virtual bool loadTypes(const char *file) = 0;

protected:
    QVector<QJsonObject> _types = {};
    QMap<QString, int> _typeNames = {};

};

}
