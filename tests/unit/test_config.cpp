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

}  // namespace solstice
