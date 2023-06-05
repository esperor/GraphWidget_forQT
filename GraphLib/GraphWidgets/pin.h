#pragma once

#include "GraphWidgets/Abstracts/abstractpin.h"

namespace GraphLib {

class GRAPHLIB_EXPORT Pin : public AbstractPin
{
    Q_OBJECT

public:
    Pin(BaseNode *parent);
    Pin(int ID, BaseNode *parent);
};

}
