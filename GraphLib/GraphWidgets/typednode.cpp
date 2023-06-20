#include "TypeManagers/nodetypemanager.h"
#include "typednode.h"
#include "constants.h"
#include "canvas.h"
#include "utility.h"

namespace GraphLib {

TypedNode::TypedNode(int typeID, Canvas *canvas) : TypedNode(-1, typeID, canvas)
{}

TypedNode::TypedNode(int ID, int typeID, Canvas *canvas)
    : BaseNode(ID, canvas)
    , _typeID{ typeID }
{}

int TypedNode::calculateRowsOffset(QPainter *painter) const
{
    QFont font = standardFont(c_nodeNameSize * _zoom);
    painter->setFont(font);
    QSize nameBounding = painter->fontMetrics().size(Qt::TextSingleLine, _name);
    return nameBounding.height() * 2.25f + c_normalPinD * _zoom;
}

void TypedNode::paintName(QPainter *painter, int desiredWidth, QPoint textOrigin)
{
    QSize nameBounding = painter->fontMetrics().size(Qt::TextSingleLine, _name);

    painter->setFont(standardFont(c_nodeNameSize * _zoom));

    painter->drawText(QRect(textOrigin.x(), textOrigin.y(), desiredWidth, nameBounding.height() * 2),
                     (Qt::AlignVCenter | Qt::AlignHCenter), _name);

    QPen pen = painter->pen();
    QColor temp = pen.color();
    pen.setColor(temp.darker(120));
    painter->setPen(pen);
    painter->setFont(standardFont(c_nodeNameSize * 0.75f * _zoom));

    painter->drawText(QRect(textOrigin.x(), textOrigin.y() + nameBounding.height(), desiredWidth, nameBounding.height() * 2),
                      (Qt::AlignVCenter | Qt::AlignHCenter), _nodeTypeManager->typeNameByID(_typeID));

    pen.setColor(temp);
    painter->setPen(pen);
}

}
