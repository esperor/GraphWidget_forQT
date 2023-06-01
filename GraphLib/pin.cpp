#include "pin.h"

namespace GraphLib {

Pin::Pin(int ID, BaseNode *parentNode, QWidget *parent)
    : AbstractPin{ ID, parentNode, parent }
{
}

}
