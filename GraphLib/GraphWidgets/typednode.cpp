#include "typednode.h"

namespace GraphLib {

TypedNode::TypedNode(int ID, int typeID, Canvas *canvas, QWidget *parent)
    : BaseNode(ID, canvas, parent)
    , _typeID{ typeID }
{}


}
