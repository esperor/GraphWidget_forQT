#pragma once

#include <QJsonObject>
#include <QMap>

#include "../GraphLib_global.h"

namespace GraphLib {

class GRAPHLIB_EXPORT NodeTypeManager
{
private:
    NodeTypeManager() = delete;

    static QVector<QJsonObject> _types;
    static QMap<QString, int> _typeNames;

public:
    static const QVector<QJsonObject> &Types() { return _types; }
    static const QMap<QString, int> &TypeNames() { return _typeNames; }

    static bool loadTypes(const char *file);

    static inline bool loadTypes(const QString &file) { return loadTypes(file.toStdString().c_str()); }
    static inline bool loadTypes(const std::string &file) { return loadTypes(file.c_str()); }
};

}
