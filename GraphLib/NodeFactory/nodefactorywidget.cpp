#include <QPaintEvent>
#include <QRect>
#include <QGroupBox>

#include "../constants.h"
#include "../TypeManagers/nodetypemanager.h"
#include "nodefactorywidget.h"
#include "utility.h"

namespace GraphLib {

namespace NodeFactory {

NodeFactoryWidget::NodeFactoryWidget(QWidget *parent)
    : QWidget{ parent }
    , _painter{ new QPainter() }
    , _layout{ QBoxLayout(QBoxLayout::TopToBottom, this) }
    , _nodeImages{ QVector<QSharedPointer<TypedNodeImage>>() }
{
    setMouseTracking(true);
    setLayout(&_layout);
}

NodeFactoryWidget::~NodeFactoryWidget()
{ delete _painter; }

void NodeFactoryWidget::clear()
{
    QLayoutItem* child;
    while(_layout.count() != 0)
    {
        child = _layout.takeAt(0);
        delete child;
    }

    _layout.addSpacing(c_nfWidgetSpacing);
}

bool NodeFactoryWidget::initTypes()
{
    if (NodeTypeManager::Types().isEmpty())
        return false;

    clear();

    std::ranges::for_each(NodeTypeManager::TypeNames().keys(), [&](const QString &typeName){
        _nodeImages.append(QSharedPointer<TypedNodeImage>(new TypedNodeImage(typeName)));
        TypedNodeImage *image = _nodeImages.last().get();
        _layout.addWidget(image);
    });

    return true;
}



// ------------------- PAINT ----------------------


void NodeFactoryWidget::paintEvent(QPaintEvent *event)
{
    _painter->begin(this);
    _painter->setRenderHint(QPainter::Antialiasing, true);
    paint(_painter, event);
    _painter->end();
}

void NodeFactoryWidget::paint(QPainter *painter, QPaintEvent *event)
{
    // this line avoids flickering between this widget and nodes
    setUpdatesEnabled(false);

    QPen pen(Qt::SolidLine);
    pen.setColor(c_nfWidgetBackgroundColor);
    painter->setBrush(c_nfWidgetBackgroundColor);
    painter->setPen(pen);

    QRect rect = event->rect();
    int maxMetric = std::max(rect.height(), rect.width());

    painter->drawRoundedRect(rect, c_nodeRoundingRadius, c_nodeRoundingRadius);

    pen.setColor(c_nodesOutlineColor);
    painter->setPen(pen);
    painter->setFont(standardFont(13));
    painter->drawText(QRect(rect.x(), rect.y() + c_nfWidgetSpacing / 3, rect.width(), rect.height()),
                      (Qt::AlignTop | Qt::AlignHCenter), "drag & place");

    // this line avoids flickering between this widget and nodes
    setUpdatesEnabled(true);
}

}

}
