#include <asset_class.h>
#include <gtest/gtest.h>
#include <order.h>

#include <algorithm>
#include <type_traits>

// only stateless helper functions live in this file, hence the .cpp import
#include <asset_class.cpp>

namespace solstice
{

TEST(AssetClassTests, AssetClassConvertsToString)
{
    ASSERT_TRUE(to_string(AssetClass::Equity) == std::string("Equity"));
}

TEST(AssetClassTests, ValidRandomAssetClass)
{
    auto result = randomAssetClass();
    ASSERT_TRUE(std::find(ALL_ASSET_CLASSES.begin(), ALL_ASSET_CLASSES.end(), result) !=
                ALL_ASSET_CLASSES.end());

    ASSERT_TRUE((std::is_same_v<decltype(result), AssetClass>));
}

}  // namespace solstice
