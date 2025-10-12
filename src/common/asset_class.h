#ifndef ASSET_CLASS_H
#define ASSET_CLASS_H

#include <algorithm>
#include <array>
#include <cstdint>
#include <expected>
#include <random>
#include <variant>
#include <vector>

namespace
{

template <typename T>
std::vector<T> d_underlyingsPool;

template <typename T>
bool d_underlyingsPoolInitialised{false};

}  // namespace

namespace solstice
{

enum class AssetClass : uint8_t
{
    Equity,
    Future,
    COUNT
};

static constexpr std::array<const char*, static_cast<size_t>(AssetClass::COUNT)> ASSET_CLASS_STR = {
    "Equity", "Future"};

constexpr std::array<AssetClass, static_cast<size_t>(AssetClass::COUNT)> ALL_ASSET_CLASSES = {
    AssetClass::Equity, AssetClass::Future};

const char* to_string(AssetClass cls);

// ===================================================================
// Enum: Equity
// ===================================================================

enum class Equity : uint8_t
{
    AAPL,
    MSFT,
    GOOGL,
    AMZN,
    META,
    BLK,
    NVDA,
    AMD,
    INTC,
    QCOM,
    JPM,
    BAC,
    CRM,
    GS,
    MS,
    ORCL,
    IBM,
    TSM,
    UBER,
    LYFT,
    COUNT
};

static constexpr std::array<const char*, static_cast<size_t>(Equity::COUNT)> EQTY_STR = {
    "AAPL", "MSFT", "GOOGL", "AMZN", "META", "BLK",  "NVDA", "AMD", "INTC", "QCOM",
    "JPM",  "BAC",  "CRM",   "GS",   "MS",   "ORCL", "IBM",  "TSM", "UBER", "LYFT"};

constexpr std::array<Equity, static_cast<size_t>(Equity::COUNT)> ALL_EQUITIES = {
    Equity::AAPL, Equity::MSFT, Equity::GOOGL, Equity::AMZN, Equity::META,
    Equity::BLK,  Equity::NVDA, Equity::AMD,   Equity::INTC, Equity::QCOM,
    Equity::JPM,  Equity::BAC,  Equity::CRM,   Equity::GS,   Equity::MS,
    Equity::ORCL, Equity::IBM,  Equity::TSM,   Equity::UBER, Equity::LYFT};

// ===================================================================
// Enum: Future
// ===================================================================

enum class Future : uint8_t
{
    AAPL_MAR25,
    AAPL_JUN25,
    AAPL_SEP25,
    AAPL_DEC25,

    MSFT_MAR25,
    MSFT_JUN25,
    MSFT_SEP25,
    MSFT_DEC25,

    TSLA_MAR25,
    TSLA_JUN25,
    TSLA_SEP25,
    TSLA_DEC25,

    COUNT
};

static constexpr std::array<const char*, static_cast<size_t>(Future::COUNT)> FTR_STR = {
    "AAPL_MAR25", "AAPL_JUN25", "AAPL_SEP25", "AAPL_DEC25", "MSFT_MAR25", "MSFT_JUN25",
    "MSFT_SEP25", "MSFT_DEC25", "TSLA_MAR25", "TSLA_JUN25", "TSLA_SEP25", "TSLA_DEC25",
};

constexpr std::array<Future, static_cast<size_t>(Future::COUNT)> ALL_FUTURES = {
    Future::AAPL_MAR25, Future::AAPL_JUN25, Future::AAPL_SEP25, Future::AAPL_DEC25,
    Future::MSFT_MAR25, Future::MSFT_JUN25, Future::MSFT_SEP25, Future::MSFT_DEC25,
    Future::TSLA_MAR25, Future::TSLA_JUN25, Future::TSLA_SEP25, Future::TSLA_DEC25};

// ===================================================================
// Type Declaration
// ===================================================================

using Underlying = std::variant<Equity, Future>;

std::expected<Underlying, std::string> assetClassToUnderlying(AssetClass assetClass);

// ===================================================================
// Template Functions
// ===================================================================

template <typename T>
const char* to_string(T type)
{
    int underlying = static_cast<size_t>(type);

    if (std::is_same_v<T, Future>) return FTR_STR[underlying];

    if (std::is_same_v<T, Equity>) return EQTY_STR[underlying];
}

template <typename T>
const std::expected<T, std::string> getRandomUnderlying()
{
    const auto& pool = d_underlyingsPool<T>;
    if (pool.empty())
    {
        return std::unexpected("Underlying pool is empty");
    }

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, pool.size() - 1);

    return pool[dist(gen)];
}

// Overload for std::variant
template <typename... Types>
const char* to_string(const std::variant<Types...>& variant)
{
    return std::visit([](const auto& underlying) { return to_string(underlying); }, variant);
}

// Equality operator for comparing variant with one of its alternatives
template <typename T, typename... Types>
inline bool operator==(const std::variant<Types...>& variant, const T& value)
{
    if (const T* ptr = std::get_if<T>(&variant))
    {
        return *ptr == value;
    }
    return false;
}

template <typename T, typename... Types>
inline bool operator==(const T& value, const std::variant<Types...>& variant)
{
    return variant == value;
}

// ===================================================================
// Inline Getters/Setters
// ===================================================================

template <typename T>
inline const bool getUnderlyingsPoolInitialised()
{
    return d_underlyingsPoolInitialised<T>;
}

template <typename T>
inline bool setUnderlyingsPoolInitialised(bool isInitialised)
{
    d_underlyingsPoolInitialised<T> = isInitialised;
    return d_underlyingsPoolInitialised<T>;
}

template <typename T>
inline const std::vector<T>& getUnderlyingsPool()
{
    return d_underlyingsPool<T>;
}

template <typename T, std::size_t N>
inline void setUnderlyingsPool(int poolSize, const std::array<T, N>& fullSet)
{
    if (getUnderlyingsPoolInitialised<T>()) return;

    auto& pool = d_underlyingsPool<T>;
    pool.assign(fullSet.begin(), fullSet.end());

    if (poolSize > 0 && poolSize < static_cast<int>(pool.size()))
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::shuffle(pool.begin(), pool.end(), gen);
        pool.resize(poolSize);
    }

    setUnderlyingsPoolInitialised<T>(true);
}

}  // namespace solstice

#endif  // ASSET_CLASS_H
