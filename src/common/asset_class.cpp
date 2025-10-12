#include <asset_class.h>

#include <expected>
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

// ===== Templates ====
// Template definitions moved to asset_class.h

}  // namespace solstice
