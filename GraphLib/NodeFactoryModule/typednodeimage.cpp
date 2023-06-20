#include <QPainter>
#include <QPaintEvent>
#include <QDrag>
#include <QMimeData>
#include <QByteArray>

#include "typednodeimage.h"
#include "constants.h"
#include "TypeManagers/nodetypemanager.h"
#include "utility.h"

namespace GraphLib {

namespace NodeFactoryModule {

TypedNodeImage::TypedNodeImage(QWidget *parent)
    : QWidget{ parent }
    , fontSize{ 11 }
    , _painter{ new QPainter() }
{}

TypedNodeImage::TypedNodeImage(QString type, QWidget *parent)
    : TypedNodeImage(parent)
{
    typeName = type;
}

TypedNodeImage::~TypedNodeImage()
{ delete _painter; }



TypedNodeSpawnData TypedNodeImage::getData() const
{
    return TypedNodeSpawnData(typeName, typeID);
}

QSize TypedNodeImage::getDesiredSize() const
{
    QFont font = standardFont(fontSize);
    QFontMetrics metrics(font);
    return metrics.size(Qt::TextSingleLine, typeName);
}

void TypedNodeImage::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;

        mimeData->setData(c_mimeFormatForNodeFactory, this->getData().toByteArray());
        drag->setMimeData(mimeData);

        QPixmap pixmap(QSize(100, 100));
        pixmap.fill(QColor(255, 255, 255, 0));

        drag->setPixmap(pixmap);
        drag->setHotSpot(QPoint(pixmap.width() / 2, pixmap.height() / 2));

        drag->exec();
    }
}



void TypedNodeImage::paintEvent(QPaintEvent *event)
{
    _painter->begin(this);
    _painter->setRenderHint(QPainter::Antialiasing, true);
    paint(_painter, event);
    _painter->end();
}

void TypedNodeImage::paint(QPainter *painter, QPaintEvent *event)
{
    setUpdatesEnabled(false);

    QPen pen(Qt::SolidLine);
    pen.setColor(c_highlightColor);
    painter->setPen(pen);

    QRect rect = event->rect();

    painter->setFont(standardFont(11));
    painter->drawText(QRect(rect.x(), rect.y(), rect.width(), rect.height()),
                      (Qt::AlignTop | Qt::AlignHCenter), typeName);

    setUpdatesEnabled(true);
}


}

}
