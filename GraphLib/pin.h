#pragma once

#include "Abstracts/abstractpin.h"

namespace GraphLib {

class GRAPHLIB_EXPORT Pin : public AbstractPin
{
    Q_OBJECT

public:
    Pin(int ID, BaseNode *parentNode, QWidget *parent = nullptr);
};

}
