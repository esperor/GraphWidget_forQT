#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QString>
#include <QByteArray>
#include <QMap>
#include <QJsonObject>
#include <QPointF>
#include <algorithm>
#include <string>

#include "NodeFactory/nodefactory.h"
#include "TypeManagers/nodetypemanager.h"
#include "TypeManagers/pintypemanager.h"
#include "GraphLib_global.h"
#include "GraphWidgets/Abstracts/abstractpin.h"
#include "DataClasses/nodespawndata.h"
#include "utility.h"

using namespace testing;
using namespace GraphLib;

TEST(TestPinData, ByteArrayConversions)
{
    QString str("Text");
    QColor clr(223, 12, 23, 255);
    PinData first(PinDirection::In, 0, 0, 0, str, clr);
    QByteArray arr = first.toByteArray();
    PinData second = PinData::fromByteArray(arr);
    EXPECT_EQ(first, second);
}

TEST(TestNodeSpawnData, ByteArrayConversions)
{
    QString str("Text");
    NodeSpawnData first(str);
    QByteArray arr1 = first.toByteArray();
    NodeSpawnData second = NodeSpawnData::fromByteArray(arr1);
    EXPECT_EQ(first, second);

    TypedNodeSpawnData third(str, 1);
    QByteArray arr2 = third.toByteArray();
    TypedNodeSpawnData fourth = TypedNodeSpawnData::fromByteArray(arr2);
    EXPECT_EQ(third, fourth);
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

TEST(TestFileLoading, Loading)
{
    QString path = "./../../GraphSubdirs/GraphTests/test_files/";
    QString pins = "pins.json", nodes = "nodes.json";

    ASSERT_TRUE(NodeTypeManager::loadTypes(path + nodes));
    ASSERT_TRUE(PinTypeManager::loadTypes(path + pins));
}

TEST(TestFileLoading, JsonParsing)
{
    EXPECT_EQ(4, NodeTypeManager::Types().size()) << "Expected " << 4 << " and got " << NodeTypeManager::Types().size() << " node types.";
    EXPECT_EQ(3, PinTypeManager::Types().size()) << "Expected " << 3 << " and got " << PinTypeManager::Types().size() << " pin types.";
}

TEST(TestFileLoading, Properties)
{
    EXPECT_EQ("power", PinTypeManager::Types().at(0).value("name").toString());
    EXPECT_EQ(0, PinTypeManager::TypeNames()["power"]);

    EXPECT_EQ("Monitor", NodeTypeManager::Types().at(0).value("name").toString());
    EXPECT_EQ(0, NodeTypeManager::TypeNames()["Monitor"]);
}

TEST(TestNodeFactory, ParseToColor)
{
    auto check = [](QString str, int r, int g, int b) {
        QColor clr = NodeFactory::parseToColor(str);

        EXPECT_EQ(clr.isValid(), true);

        EXPECT_EQ(clr.red(), r);
        EXPECT_EQ(clr.green(), g);
        EXPECT_EQ(clr.blue(), b);
    };

    QString str1("00AF00");

//    QStringList list = str1.split(QRegularExpression(".."));
//    EXPECT_EQ(list.size(), 3);
//    EXPECT_EQ(list[0], "00");
//    EXPECT_EQ(list[1], "AF");

//    bool ok;
//    int parsed = list[1].toInt(&ok, 16);

//    EXPECT_TRUE(ok);
//    EXPECT_EQ(parsed, 0xAF);

    check(str1, 0, 0xAF, 0);
    QString str2("2F03DD");
    check(str2, 0x2F, 0x03, 0xDD);
    QString str3("FFFFFF");
    check(str3, 0xFF, 0xFF, 0xFF);
}

