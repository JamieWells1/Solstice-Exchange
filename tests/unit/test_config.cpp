#include <config.h>
#include <gtest/gtest.h>

namespace solstice
{

TEST(ConfigTests, ValidConfigSucceeds)
{
    auto result = Config::init();
    ASSERT_TRUE(result.has_value());
    result->d_maxPrice = 100;

    EXPECT_EQ(result->d_maxPrice, 100);
}

TEST(ConfigTests, DefaultValuesAreCorrect)
{
    auto result = Config::init();
    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(result->d_logLevel, LogLevel::INFO);
    EXPECT_EQ(result->d_assetClass, AssetClass::Future);
    EXPECT_EQ(result->d_ordersToGenerate, 10000);
    EXPECT_EQ(result->d_underlyingPoolCount, 50);
    EXPECT_EQ(result->d_minQnty, 1);
    EXPECT_EQ(result->d_maxQnty, 20);
    EXPECT_EQ(result->d_minPrice, 8.0);
    EXPECT_EQ(result->d_maxPrice, 10.0);
}

TEST(ConfigTests, CanModifyAllFields)
{
    auto result = Config::init();
    ASSERT_TRUE(result.has_value());

    result->d_logLevel = LogLevel::DEBUG;
    result->d_assetClass = AssetClass::Equity;
    result->d_ordersToGenerate = 5000;
    result->d_underlyingPoolCount = 25;
    result->d_minQnty = 5;
    result->d_maxQnty = 50;
    result->d_minPrice = 100.0;
    result->d_maxPrice = 200.0;

    EXPECT_EQ(result->d_logLevel, LogLevel::DEBUG);
    EXPECT_EQ(result->d_assetClass, AssetClass::Equity);
    EXPECT_EQ(result->d_ordersToGenerate, 5000);
    EXPECT_EQ(result->d_underlyingPoolCount, 25);
    EXPECT_EQ(result->d_minQnty, 5);
    EXPECT_EQ(result->d_maxQnty, 50);
    EXPECT_EQ(result->d_minPrice, 100.0);
    EXPECT_EQ(result->d_maxPrice, 200.0);
}

}  // namespace solstice
