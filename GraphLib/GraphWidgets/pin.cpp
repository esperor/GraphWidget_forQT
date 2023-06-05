#include "pin.h"

namespace GraphLib {

Pin::Pin(BaseNode *parent)
    : Pin(-1, parent)
{}

Pin::Pin(int ID, BaseNode *parent)
    : AbstractPin{ ID, parent }
{}

}
