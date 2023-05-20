
#include "pin.h"

namespace GraphLib {

Pin::Pin(int ID, Node *parentNode, QWidget *parent)
    : AbstractPin{ ID, parentNode, parent }
{
}

}
