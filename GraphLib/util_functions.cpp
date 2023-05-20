#include "util_functions.h"
#include "constants.h"

namespace GraphLib {

QFont standardFont(int size)
{
    return QFont("Jost", size);
}

QPoint snap(const QPointF &position, short interval)
{
    auto nearestSnap = [&](float number){
        int num = static_cast<int>(number);
        int remainder = num % interval;
        if (remainder >= (interval / 2.0f))
            return num + interval - (remainder);
        else
            return num - (remainder);
    };

    return QPoint(nearestSnap(position.x()), nearestSnap(position.y()));
}

QPainterPath standardPath(const QPoint &origin, const QPoint &target, float zoomMult)
{
    const int xDifference = target.x() - origin.x();
    const float xDiffCoeffed = abs(xDifference) * c_diffCoeffForPinConnectionCurves;
    float x1, x2;


    if (xDifference <= c_xDiffFunctionBlendPoint)
    {
        const float yDiffCoeff = 1.0f - static_cast<float>(xDifference) / c_xDiffFunctionBlendPoint;
        const int yDifference = abs(target.y() - origin.y());
        const float yDiffCoeffed = yDifference * c_diffCoeffForPinConnectionCurves * yDiffCoeff;
        const float diffsSum = std::min(c_maxDiffsSum * zoomMult, xDiffCoeffed + yDiffCoeffed);

        x1 = origin.x() + diffsSum;
        x2 = target.x() - diffsSum;
    }
    else
    {
        x1 = origin.x() + xDiffCoeffed;
        x2 = target.x() - xDiffCoeffed;
    }

    QPainterPath path;

    path.moveTo(origin.x(), origin.y());
    path.cubicTo(x1, origin.y(),
                 x2, target.y(),
                 target.x(), target.y());

    return path;
}

}
