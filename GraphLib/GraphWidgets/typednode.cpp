#include "TypeManagers/nodetypemanager.h"
#include "typednode.h"


namespace GraphLib {

TypedNode::TypedNode(int typeID, Canvas *canvas) : TypedNode(-1, typeID, canvas)
{}

TypedNode::TypedNode(int ID, int typeID, Canvas *canvas)
    : BaseNode(ID, canvas)
    , _typeID{ typeID }
{
    setName(NodeTypeManager::Types()[typeID].value("name").toString());
}

}
