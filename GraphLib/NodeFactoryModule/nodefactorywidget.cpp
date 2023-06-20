#include <QPaintEvent>
#include <QRect>
#include <QGroupBox>
#include <QApplication>

#include "constants.h"
#include "TypeManagers/nodetypemanager.h"
#include "nodefactorywidget.h"
#include "utility.h"

namespace GraphLib {

namespace NodeFactoryModule {

NodeFactoryWidget::NodeFactoryWidget(QWidget *parent)
    : QWidget{ parent }
    , _painter{ new QPainter() }
    , _gap{ 20 }
    , _bIsMinimized{ false }
    , _position{ QPointF() }
    , _lastMouseDownPosition{ QPointF() }
    , _mousePressPosition{ QPointF() }
    , _layoutHolder{ QWidget(this) }
    , _layout{ QBoxLayout(QBoxLayout::TopToBottom, &_layoutHolder) }
    , _btnMinimize{ NFButtonMinimize(this) }
    , _nodeImages{ QVector<QSharedPointer<TypedNodeImage>>() }
{
    setMouseTracking(true);
    _layoutHolder.setLayout(&_layout);

    _btnMinimize.text = c_nfWidgetArrowUp;
    _btnMinimize.color = c_highlightColor;

    connect(&_btnMinimize, &NFButtonMinimize::onClick, this, &NodeFactoryWidget::onButtonMinimizeClick);
}

NodeFactoryWidget::~NodeFactoryWidget()
{ delete _painter; }

void NodeFactoryWidget::onButtonMinimizeClick()
{
    _bIsMinimized = !_bIsMinimized;
    _btnMinimize.text = _bIsMinimized ? c_nfWidgetArrowDown : c_nfWidgetArrowUp;
    if (_bIsMinimized)
        _layoutHolder.hide();
    else
        _layoutHolder.show();
}

QSize NodeFactoryWidget::getDesiredSize() const
{
    if (_bIsMinimized)
        return QSize(c_nfWidgetMinimalWidth, c_nfWidgetSpacing * 1.5f);

    QSize out;
    int maxMetric = 0;

    int maxImgWidth = 0, maxImgHeight = 0;
    std::ranges::for_each(_nodeImages, [&](QSharedPointer<TypedNodeImage> ptr){
        QSize imgSize = ptr->getDesiredSize();
        maxImgWidth = std::max(maxImgWidth, imgSize.width());
        maxImgHeight = std::max(maxImgHeight, imgSize.height());
    });

    if (_layout.direction() == QBoxLayout::LeftToRight ||
        _layout.direction() == QBoxLayout::RightToLeft)
    {
        maxMetric += c_nfWidgetSpacing;
        maxMetric += _nodeImages.size() * (maxImgWidth + _gap);
        out = QSize(maxMetric, c_nfWidgetSpacing + maxImgHeight);
    }
    else
    {
        maxMetric += _nodeImages.size() * (maxImgHeight + _gap);
        out = QSize( maxImgWidth, maxMetric);
    }
    out.setWidth(std::max(c_nfWidgetMinimalWidth, out.width()));

    return out;
}

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
    if (_nodeTypeManager->Types().isEmpty())
        return false;

    clear();

    try {
        std::ranges::for_each(_nodeTypeManager->TypeNames().keys(), [&](const QString &typeName){
            _nodeImages.append(QSharedPointer<TypedNodeImage>(new TypedNodeImage(typeName)));
            TypedNodeImage *image = _nodeImages.last().get();
            image->TypeManager = _nodeTypeManager;
            image->initType();
            _layout.addWidget(image);
        });
        _layout.addSpacing(c_nfWidgetSpacing / 2);
    } catch (...) { return false; }

    return true;
}


// ------------------ EVENTS --------------------


void NodeFactoryWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::MouseButton::LeftButton)
    {
        _lastMouseDownPosition = mapToParent(event->position());
        _mousePressPosition = _lastMouseDownPosition - _position;
    }
}

void NodeFactoryWidget::mouseReleaseEvent(QMouseEvent *)
{
    this->setCursor(QCursor(Qt::CursorShape::ArrowCursor));
}

void NodeFactoryWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::MouseButton::LeftButton))
        return;

    if ((event->position() - _mousePressPosition).manhattanLength()
        > QApplication::startDragDistance())
        this->setCursor(QCursor(Qt::CursorShape::OpenHandCursor));

    QVector2D offset = QVector2D(mapToParent(event->position()) - _lastMouseDownPosition);
    _position = mapToParent(event->position()) - _mousePressPosition;

    _lastMouseDownPosition = mapToParent(event->position());

    onMove(offset);
}



// ------------------- PAINT ----------------------


void NodeFactoryWidget::paintEvent(QPaintEvent *event)
{
    _painter->begin(this);
    _painter->setRenderHint(QPainter::Antialiasing, true);
    paint(_painter, event);
    _painter->end();
}

void NodeFactoryWidget::paint(QPainter *painter, QPaintEvent *)
{
    // this line avoids flickering between this widget and nodes
    setUpdatesEnabled(false);

    QPen pen(Qt::SolidLine);
    pen.setColor(c_nfWidgetBackgroundColor);
    painter->setBrush(c_nfWidgetBackgroundColor);
    painter->setPen(pen);

    QRect rect = QRect(mapFromParent(_position).toPoint(), getDesiredSize());

    painter->drawRoundedRect(rect, c_nodeRoundingRadius, c_nodeRoundingRadius);

    pen.setColor(c_highlightColor);
    painter->setPen(pen);
    painter->setFont(standardFont(13));
    painter->drawText(QRect(rect.x(), rect.y() + c_nfWidgetSpacing / 3, rect.width(), rect.height()),
                      (Qt::AlignTop | Qt::AlignHCenter), "drag & place");


    _layoutHolder.setFixedSize(getDesiredSize());

    // button minimize
    {
        _btnMinimize.move((this->width() - _btnMinimize.width()) / 2, this->height() - _btnMinimize.height());
    }

    // this line avoids flickering between this widget and nodes
    setUpdatesEnabled(true);
}

}

}
