

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <QString>
#include <QByteArray>
#include <QPointF>
#include <algorithm>
#include <string>

#include "GraphLib_global.h"
#include "abstractpin.h"
#include "util_functions.h"

using namespace testing;
using namespace GraphLib;

TEST(TestPinData, ByteArrayConversions)
{
    QString str("Text");
    QColor clr(223, 12, 23, 255);
    PinData first(PinDirection::In, 0, 0, str, clr);
    QByteArray arr = first.toByteArray();
    PinData second = PinData::fromByteArray(arr);
    EXPECT_EQ(first, second);
}

TEST(TestUtilityFunctions, TestSnapping)
{
    auto pointToString = [](QPoint point) {
        return std::to_string(point.x()) + ", " + std::to_string(point.y());
    };

    QPointF point1(167.4f, 129.0f);
    QPointF point2(10.2f, 14.1f);

    QVector< std::pair<QPoint, QPoint> > assertions =
        {
            { QPoint(170, 130), snap(point1, 10) },
            { QPoint(165, 130), snap(point1, 5) },
            { QPoint(10, 15), snap(point2, 5) },
            { QPoint(10, 14), snap(point2, 1) },
        };

    auto lambda = [&](const QPoint &correct, const QPoint &result) {
        EXPECT_EQ(correct, result) << "Expected " << pointToString(correct) << " and got " << pointToString(result);
    };

    std::ranges::for_each(assertions, [&](const std::pair<QPoint, QPoint> &pair){
        lambda(pair.first, pair.second);
    });
}

