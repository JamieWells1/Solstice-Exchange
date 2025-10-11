#include <asset_class.h>

#include <ostream>

namespace solstice {

AssetClass randomAssetClass() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, static_cast<int>(AssetClass::COUNT) - 1);
    return static_cast<AssetClass>(dist(gen));
}

std::ostream& operator<<(std::ostream& os, AssetClass assetClass) {
    return os << to_string(assetClass);
}

std::ostream& operator<<(std::ostream& os, Equity equity) {
    return os << to_string(equity);
}

std::ostream& operator<<(std::ostream& os, Future ftr) {
    return os << to_string(ftr);
}

} // namespace solstice
