#include <QFile>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "nodetypemanager.h"
#include "utility.h"

namespace GraphLib {

NodeTypeManager::NodeTypeManager()
{}

QVector<QJsonObject> NodeTypeManager::Types = {};
QMap<QString, int> NodeTypeManager::TypeNames = {};


bool NodeTypeManager::loadTypes(const char *file)
{
    std::optional<QJsonObject> opt = loadFile(file);
    if (!opt)
        return false;

    QJsonArray array = opt.value().value("types").toArray();

    unsigned int i = 0;
    Types.reserve(array.size());
    for (auto elem : array)
    {
        Types.append(elem.toObject());
        TypeNames.insert(Types.at(i).value("name").toString(), i);
        i++;
    }

    return true;
}

}
