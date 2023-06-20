#include <QFile>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "utility.h"
#include "pintypemanager.h"

namespace GraphLib {

bool PinTypeManager::loadTypes(const char *file)
{
    std::optional<QJsonObject> opt = loadFile(file);
    if (!opt)
        return false;

    QJsonArray array = opt.value().value("types").toArray();

    unsigned int i = 0;
    _types.reserve(array.size());
    for (auto elem : array)
    {
        _types.append(elem.toObject());
        _typeNames.insert(_types.at(i).value("name").toString(), i);
        i++;
    }

    return true;
}

}

