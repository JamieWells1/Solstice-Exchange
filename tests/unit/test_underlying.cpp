#include <asset_class.h>
#include <gtest/gtest.h>

namespace solstice
{

template <typename T>
void resetGlobalStateBeforeTest()
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
void resetGlobalStateAllBeforeTest()
{
    (resetGlobalStateBeforeTest<Ts>(), ...);
}

template <typename... Ts>
void resetGlobalStateAll()
{
    (resetGlobalState<Ts>(), ...);
}

class UnderlyingTests : public ::testing::Test
{
   protected:
    void SetUp() override { resetGlobalStateAllBeforeTest<Equity, Future>(); }

    void TearDown() override { resetGlobalStateAll<Equity, Future>(); }
};

TEST_F(UnderlyingTests, UnderlyingToString)
{
    ASSERT_TRUE(to_string(Equity::AAPL) == std::string("AAPL"));
}

TEST_F(UnderlyingTests, UnderlyingPoolPopulates)
{
    setUnderlyingsPool(10, ALL_EQUITIES);
    ASSERT_TRUE(!getUnderlyingsPool<Equity>().empty());
}

TEST_F(UnderlyingTests, ValidRandomUnderlying)
{
    setUnderlyingsPool(10, ALL_EQUITIES);

    Equity result = *getRandomUnderlying<Equity>();

    ASSERT_TRUE(std::find(ALL_EQUITIES.begin(), ALL_EQUITIES.end(), result) != ALL_EQUITIES.end());

    ASSERT_TRUE((std::is_same_v<std::remove_cv_t<decltype(result)>, Equity>));
}

TEST_F(UnderlyingTests, GetRandomUnderlyingFailsWhenPoolEmpty)
{
    auto result = getRandomUnderlying<Equity>();
    ASSERT_FALSE(result.has_value());
    EXPECT_TRUE(result.error().find("Underlying pool is empty") != std::string::npos);
}

TEST_F(UnderlyingTests, SetUnderlyingsPoolWithFullSet)
{
    setUnderlyingsPool(-1, ALL_EQUITIES);
    auto pool = getUnderlyingsPool<Equity>();
    EXPECT_EQ(pool.size(), ALL_EQUITIES.size());
}

TEST_F(UnderlyingTests, SetUnderlyingsPoolWithLimitedSize)
{
    setUnderlyingsPool(5, ALL_EQUITIES);
    auto pool = getUnderlyingsPool<Equity>();
    EXPECT_EQ(pool.size(), 5);
}

TEST_F(UnderlyingTests, SetUnderlyingsPoolOnlyInitializesOnce)
{
    setUnderlyingsPool(5, ALL_EQUITIES);
    auto pool1 = getUnderlyingsPool<Equity>();

    setUnderlyingsPool(10, ALL_EQUITIES);
    auto pool2 = getUnderlyingsPool<Equity>();

    EXPECT_EQ(pool1.size(), pool2.size());
    EXPECT_EQ(pool2.size(), 5);
}

TEST_F(UnderlyingTests, GetUnderlyingsPoolInitialisedReturnsTrueAfterInit)
{
    ASSERT_FALSE(getUnderlyingsPoolInitialised<Equity>());
    setUnderlyingsPool(10, ALL_EQUITIES);
    ASSERT_TRUE(getUnderlyingsPoolInitialised<Equity>());
}

TEST_F(UnderlyingTests, FutureUnderlyingPoolWorks)
{
    setUnderlyingsPool(5, ALL_FUTURES);
    auto pool = getUnderlyingsPool<Future>();
    EXPECT_EQ(pool.size(), 5);

    auto result = getRandomUnderlying<Future>();
    ASSERT_TRUE(result.has_value());
}

}  // namespace solstice
