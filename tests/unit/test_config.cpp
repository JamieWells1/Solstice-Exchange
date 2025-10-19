#include <config.h>
#include <gtest/gtest.h>

namespace solstice
{

TEST(ConfigTests, ValidConfigSucceeds)
{
    auto result = Config::init();
    ASSERT_TRUE(result.has_value());
    result->maxPrice(100);

    EXPECT_EQ(result->maxPrice(), 100);
}

TEST(ConfigTests, DefaultValuesAreCorrect)
{
    auto result = Config::init();
    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(result->logLevel(), LogLevel::INFO);
    EXPECT_EQ(result->assetClass(), AssetClass::Future);
    EXPECT_EQ(result->ordersToGenerate(), 10000);
    EXPECT_EQ(result->underlyingPoolCount(), 50);
    EXPECT_EQ(result->minQnty(), 1);
    EXPECT_EQ(result->maxQnty(), 20);
    EXPECT_EQ(result->minPrice(), 8.0);
    EXPECT_EQ(result->maxPrice(), 10.0);
}

TEST(ConfigTests, CanModifyAllFields)
{
    auto result = Config::init();
    ASSERT_TRUE(result.has_value());

    result->logLevel(LogLevel::DEBUG);
    result->assetClass(AssetClass::Equity);
    result->ordersToGenerate(5000);
    result->underlyingPoolCount(25);
    result->minQnty(5);
    result->maxQnty(50);
    result->minPrice(100.0);
    result->maxPrice(200.0);

    EXPECT_EQ(result->logLevel(), LogLevel::DEBUG);
    EXPECT_EQ(result->assetClass(), AssetClass::Equity);
    EXPECT_EQ(result->ordersToGenerate(), 5000);
    EXPECT_EQ(result->underlyingPoolCount(), 25);
    EXPECT_EQ(result->minQnty(), 5);
    EXPECT_EQ(result->maxQnty(), 50);
    EXPECT_EQ(result->minPrice(), 100.0);
    EXPECT_EQ(result->maxPrice(), 200.0);
}

}  // namespace solstice
