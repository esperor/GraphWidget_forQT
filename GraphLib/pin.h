#pragma once

#include "abstractpin.h"

namespace GraphLib {

class GRAPHLIB_EXPORT Pin : public AbstractPin
{
    Q_OBJECT

public:
    Pin(int ID, Node *parentNode, QWidget *parent = nullptr);
};

}
