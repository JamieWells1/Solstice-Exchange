#include <asset_class.h>
#include <gtest/gtest.h>

namespace solstice
{

template <typename T>
void initGlobalState()
{
    d_underlyingsPool<T> = {};
    d_underlyingsPoolInitialised<T> = false;
}

template <typename T>
void resetGlobalState()
{
    d_underlyingsPool<T> = {};
    d_underlyingsPoolInitialised<T> = false;
}

template <typename... Ts>
void initialiseGlobalStateAll()
{
    (initGlobalState<Ts>(), ...);
}

template <typename... Ts>
void resetGlobalStateAll()
{
    (resetGlobalState<Ts>(), ...);
}

class MyTest : public ::testing::Test
{
   protected:
    void SetUp() override { initialiseGlobalStateAll<Equity, Future>(); }

    void TearDown() override { resetGlobalStateAll<Equity, Future>(); }
};

TEST(UnderlyingTests, UnderlyingToString)
{
    ASSERT_TRUE(to_string(Equity::AAPL) == std::string("AAPL"));
}

TEST(UnderlyingTests, UnderlyingPoolPopulates)
{
    SetUp();

    setUnderlyingsPool(10, ALL_EQUITIES);
    ASSERT_TRUE(!getUnderlyingsPool<Equity>().empty());

    TearDown();
}

TEST(UnderlyingTests, ValidRandomUnderlying)
{
    SetUp();
    setUnderlyingsPool(10, ALL_EQUITIES);

    Equity result = *getRandomUnderlying<Equity>();

    ASSERT_TRUE(std::find(ALL_EQUITIES.begin(), ALL_EQUITIES.end(), result) != ALL_EQUITIES.end());

    ASSERT_TRUE((std::is_same_v<std::remove_cv_t<decltype(result)>, Equity>));
    TearDown();
}

}  // namespace solstice
