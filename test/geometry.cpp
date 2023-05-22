#include "probe/graphics.h"

#include <gtest/gtest.h>

using namespace probe;

TEST(GeometryTest, Equal)
{
    EXPECT_TRUE(geometry_t{} == geometry_t{});
    EXPECT_TRUE(geometry_t(12, 34, 21, 43) == geometry_t(12, 34, 21, 43));

    EXPECT_FALSE(geometry_t(12, 34) == geometry_t(12, 34, 21, 43));
    EXPECT_TRUE(geometry_t(12, 34) == geometry_t(12, 34));
}

TEST(GeometryTest, ContainsPoint)
{
    geometry_t g{ 15, 15, 300, 600 };

    // outside
    EXPECT_FALSE(g.contains(14, 14));
    EXPECT_FALSE(g.contains(point_t{ 14, 14 }));

    EXPECT_FALSE(g.contains(314, 615));
    EXPECT_FALSE(g.contains(point_t{ 314, 615 }));

    // border
    EXPECT_TRUE(g.contains(15, 15));
    EXPECT_TRUE(g.contains(point_t{ 15, 15 }));

    EXPECT_FALSE(g.contains(15, 15, true));
    EXPECT_FALSE(g.contains(point_t{ 15, 15 }, true));

    EXPECT_TRUE(g.contains(314, 614));
    EXPECT_TRUE(g.contains(point_t{ 314, 614 }));

    EXPECT_FALSE(g.contains(314, 614, true));
    EXPECT_FALSE(g.contains(point_t{ 314, 614 }, true));

    // inside
    EXPECT_TRUE(g.contains(16, 16));
    EXPECT_TRUE(g.contains(point_t{ 16, 16 }));

    EXPECT_TRUE(g.contains(16, 16, true));
    EXPECT_TRUE(g.contains(point_t{ 16, 16 }, true));

    EXPECT_TRUE(g.contains(313, 613));
    EXPECT_TRUE(g.contains(point_t{ 313, 613 }));
}

TEST(GeometryTest, ContainsGeometry)
{
    geometry_t g{ 15, 15, 300, 600 };

    // outside
    EXPECT_FALSE(g.contains(geometry_t{ 0, 0, 12, 12 }));
    EXPECT_FALSE(g.contains(geometry_t{ 0, 0, 100, 34 }));

    // border
    EXPECT_TRUE(g.contains(g));
    EXPECT_FALSE(g.contains(g, true));

    // inside
    EXPECT_TRUE(g.contains(geometry_t{ 16, 16, 299, 599 }));
    EXPECT_TRUE(g.contains(geometry_t{ 34, 78, 15, 433 }));
}

TEST(GeometryTest, Translated)
{
    geometry_t g{ 15, 15, 300, 600 };

    EXPECT_EQ(g.translated(3, 5), geometry_t(18, 20, 300, 600));
}

TEST(GeometryTest, Intersected)
{
    geometry_t g{ 15, 15, 300, 600 };

    EXPECT_EQ(g.intersected(geometry_t(0, 0, 34, 56)), geometry_t(15, 15, 19, 41));
    EXPECT_EQ(g.intersected(geometry_t(0, 0, 14, 14)), geometry_t());
}

TEST(GeometryTest, United)
{
    geometry_t g{ 15, 15, 300, 600 };

    EXPECT_EQ(g.united(geometry_t(0, 0, 34, 56)), geometry_t(0, 0, 315, 615));
    EXPECT_EQ(g.united(geometry_t(0, 0, 14, 14)), geometry_t(0, 0, 315, 615));
}