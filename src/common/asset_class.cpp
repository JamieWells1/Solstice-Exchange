#include <asset_class.h>

#include <ostream>
#include <random>

namespace solstice
{

// ===== AssetClass ====

const char* to_string(AssetClass cls) { return ASSET_CLASS_STR[static_cast<size_t>(cls)]; }

std::ostream& operator<<(std::ostream& os, AssetClass assetClass)
{
    return os << to_string(assetClass);
}

AssetClass randomAssetClass()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, static_cast<int>(AssetClass::COUNT) - 1);

    return static_cast<AssetClass>(dist(gen));
}

std::expected<Underlying, std::string> getUnderlying(AssetClass assetClass)
{
    switch (assetClass)
    {
        case AssetClass::Equity:
        {
            auto underlying = randomUnderlying<Equity>();
            if (!underlying)
            {
                return std::unexpected(underlying.error());
            }
            return *underlying;
        }
        case AssetClass::Future:
        {
            auto underlying = randomUnderlying<Future>();
            if (!underlying)
            {
                return std::unexpected(underlying.error());
            }
            return *underlying;
        }
        default:
            return std::unexpected("Invalid asset class\n");
    }
}

// template definitions moved to asset_class.h

}  // namespace solstice
