#include "probe/types.h"

#include <gtest/gtest.h>

using namespace probe;

TEST(VersionEqualTest, Empty)
{
    EXPECT_TRUE(version_t{} == version_t{});
    EXPECT_FALSE(version_t{} != version_t{});
    EXPECT_TRUE(strict_equal(version_t{}, version_t{}));
}

TEST(VersionEqualTest, Major)
{
    version_t ver{ 12 };
    EXPECT_TRUE(ver == ver);
    EXPECT_FALSE(ver != ver);
    EXPECT_TRUE(strict_equal(ver, ver));
}

TEST(VersionEqualTest, MajorMinor)
{

    version_t ver{ 12, 34 };
    EXPECT_TRUE(ver == ver);
    EXPECT_FALSE(ver != ver);
    EXPECT_TRUE(strict_equal(ver, ver));
}

TEST(VersionEqualTest, MajorMinorPatch)
{

    version_t ver{ 12, 34, 56 };
    EXPECT_TRUE(ver == ver);
    EXPECT_FALSE(ver != ver);
    EXPECT_TRUE(strict_equal(ver, ver));
}

TEST(VersionEqualTest, MajorMinorPatchBuild)
{

    version_t ver{ 12, 34, 56, 78 };
    EXPECT_TRUE(ver == ver);
    EXPECT_FALSE(ver != ver);
    EXPECT_TRUE(strict_equal(ver, ver));
}

TEST(VersionEqualTest, Codename)
{
    version_t ver_codename{ 12, 34, 56, 78, "codename" };
    EXPECT_TRUE(ver_codename == ver_codename);
    EXPECT_FALSE(ver_codename != ver_codename);
    EXPECT_TRUE(strict_equal(ver_codename, ver_codename));

    version_t ver_wo_codname{ 12, 34, 56, 78 };
    EXPECT_TRUE(ver_codename == ver_wo_codname);
    EXPECT_FALSE(strict_equal(ver_codename, ver_wo_codname));

    version_t ver_namecode{ 12, 34, 56, 78, "namecode" };
    EXPECT_TRUE(ver_codename == ver_namecode);
    EXPECT_FALSE(strict_equal(ver_codename, ver_namecode));
}

TEST(VersionRegexTest, Empty)
{
    EXPECT_EQ(to_version(""), version_t{});
    EXPECT_EQ(to_version(""), version_t{});
}

TEST(VersionRegexTest, Major)
{
    EXPECT_TRUE(strict_equal(to_version("32"), version_t{ 32 }));
    EXPECT_TRUE(strict_equal(to_version("x32"), version_t{}));
}

TEST(VersionRegexTest, MajorMinor)
{
    EXPECT_FALSE(strict_equal(to_version("32.14"), version_t{}));
    EXPECT_TRUE(strict_equal(to_version("32.14"), version_t{ 32, 14 }));

    EXPECT_TRUE(strict_equal(to_version("xx32.14xx"), version_t{ 32, 14 }));
    EXPECT_TRUE(strict_equal(to_version("xx 32.14 xx"), version_t{ 32, 14 }));
    EXPECT_TRUE(strict_equal(to_version("32x2x32.14x3x4"), version_t{ 32, 14 }));
    EXPECT_TRUE(strict_equal(to_version("32x/x2x 32.14 x3s"), version_t{ 32, 14 }));
}

TEST(VersionRegexTest, MajorMinorPatch)
{
    version_t expected{ 32, 14, 15 };

    EXPECT_FALSE(strict_equal(to_version("32.14.15"), version_t{}));
    EXPECT_TRUE(strict_equal(to_version("32.14.15"), expected));

    EXPECT_TRUE(strict_equal(to_version("xx32.14.15xx"), expected));
    EXPECT_TRUE(strict_equal(to_version("xx 32.14.15 xx"), expected));
    EXPECT_TRUE(strict_equal(to_version("32x2x32.14.15x3x4"), expected));
    EXPECT_TRUE(strict_equal(to_version("3x/x2x 32.14.15 x3s"), expected));
}

TEST(VersionRegexTest, MajorMinorPatchBuild)
{
    version_t expected{ 32, 14, 15, 26 };

    EXPECT_FALSE(strict_equal(to_version("32.14.15.26"), version_t{}));
    EXPECT_TRUE(strict_equal(to_version("32.14.15.26"), expected));

    EXPECT_TRUE(strict_equal(to_version("xx32.14.15.26xx"), expected));
    EXPECT_TRUE(strict_equal(to_version("xx 32.14.15.26 xx"), expected));
    EXPECT_TRUE(strict_equal(to_version("32x2x32.14.15.26x3x4"), expected));
    EXPECT_TRUE(strict_equal(to_version("3x/x2x 32.14.15.26 x3s"), expected));

    EXPECT_FALSE(strict_equal(to_version("32.14.15-26"), version_t{}));
    EXPECT_TRUE(strict_equal(to_version("32.14.15-26"), expected));

    EXPECT_TRUE(strict_equal(to_version("xx32.14.15-26xx"), expected));
    EXPECT_TRUE(strict_equal(to_version("xx 32.14.15-26 xx"), expected));
    EXPECT_TRUE(strict_equal(to_version("32x2x32.14.15-26x3x4"), expected));
    EXPECT_TRUE(strict_equal(to_version("3x/x2x 32.14.15-26 x3s"), expected));
}

TEST(VersionRegexTest, Codename)
{
    version_t expected{ 32, 14, 15, 26, "code" };

    EXPECT_FALSE(strict_equal(to_version("32.14.15.26-code"), version_t{}));
    EXPECT_TRUE(strict_equal(to_version("32.14.15.26-code"), expected));
    EXPECT_TRUE(to_version("32.14.15.26 code") == expected);

    EXPECT_FALSE(strict_equal(to_version("xx32.14.15.26-codexx"), expected));
    EXPECT_TRUE(strict_equal(to_version("xx 32.14.15.26-code xx"), expected));
    EXPECT_FALSE(strict_equal(to_version("32x2x32.14.15.26-codex3x4"), expected));
    EXPECT_TRUE(strict_equal(to_version("3x/x2x 32.14.15.26-code x3s"), expected));

    EXPECT_FALSE(strict_equal(to_version("32.14.15-26-code"), version_t{}));
    EXPECT_TRUE(strict_equal(to_version("32.14.15-26-code"), expected));

    EXPECT_FALSE(strict_equal(to_version("xx32.14.15-26-codexx"), expected));
    EXPECT_TRUE(strict_equal(to_version("xx 32.14.15-26-code xx"), expected));
    EXPECT_FALSE(strict_equal(to_version("32x2x32.14.15-26-codex3x4"), expected));
    EXPECT_TRUE(strict_equal(to_version("3x/x2x 32.14.15-26-code x3s"), expected));

    version_t expected_wo_b{ 32, 14, 15, 0, "code" };

    EXPECT_FALSE(strict_equal(to_version("32.14.15-code"), version_t{}));
    EXPECT_TRUE(strict_equal(to_version("32.14.15-code"), expected_wo_b));
    EXPECT_TRUE(to_version("32.14.15 code") == expected_wo_b);

    EXPECT_FALSE(strict_equal(to_version("xx32.14.15-codexx"), expected_wo_b));
    EXPECT_TRUE(strict_equal(to_version("xx 32.14.15-code xx"), expected_wo_b));
    EXPECT_FALSE(strict_equal(to_version("32x2x32.14.15-codex3x4"), expected_wo_b));
    EXPECT_TRUE(strict_equal(to_version("3x/x2x 32.14.15-code x3s"), expected_wo_b));
}