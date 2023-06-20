#pragma once


#include "typemanager.h"
#include "GraphLib_global.h"

namespace GraphLib {

class GRAPHLIB_EXPORT PinTypeManager : public TypeManager
{
public:
    PinTypeManager() {}

    bool loadTypes(const char *file) override;

    inline bool loadTypes(const QString &file) { return loadTypes(file.toStdString().c_str()); }
    inline bool loadTypes(const std::string &file) { return loadTypes(file.c_str()); }
};

}
