#pragma once

#include <QJsonObject>
#include <QMap>

#include "../GraphLib_global.h"

namespace GraphLib {

class GRAPHLIB_EXPORT NodeTypeManager
{
private:
    NodeTypeManager();

public:
    static QVector<QJsonObject> Types;
    static QMap<QString, int> TypeNames;

    static bool loadTypes(const char *file);

    static inline bool loadTypes(const QString &file) { return loadTypes(file.toStdString().c_str()); }
    static inline bool loadTypes(const std::string &file) { return loadTypes(file.c_str()); }
};

}
