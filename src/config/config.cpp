#include <config.h>

#include <format>

namespace solstice
{

Config::Config() {}

std::expected<void, std::string> Config::checkConfig(Config& config)
{
    auto values = {double(config.d_ordersToGenerate), double(config.d_minQnty),
                   double(config.d_maxQnty),          double(config.d_minPrice),
                   double(config.d_maxPrice),         double(config.d_underlyingPoolCount)};

    for (auto value : values)
    {
        if (value < 0)
        {
            return std::unexpected(std::format("Invalid config value: '{}'\n", value));
        }
    }

    return {};
}

std::expected<Config, std::string> Config::instance()
{
    Config config;

    auto isValid = checkConfig(config);
    if (!isValid)
    {
        return std::unexpected(isValid.error());
    }
    return std::move(config);
}

LogLevel Config::logLevel() const { return d_logLevel; }
AssetClass Config::assetClass() const { return d_assetClass; }
int Config::ordersToGenerate() const { return d_ordersToGenerate; }
int Config::underlyingPoolCount() const { return d_underlyingPoolCount; }
int Config::minQnty() const { return d_minQnty; }
int Config::maxQnty() const { return d_maxQnty; }
double Config::minPrice() const { return d_minPrice; }
double Config::maxPrice() const { return d_maxPrice; }

void Config::logLevel(LogLevel level) { d_logLevel = level; }
void Config::assetClass(AssetClass assetClass) { d_assetClass = assetClass; }
void Config::ordersToGenerate(int count) { d_ordersToGenerate = count; }
void Config::underlyingPoolCount(int count) { d_underlyingPoolCount = count; }
void Config::minQnty(int qnty) { d_minQnty = qnty; }
void Config::maxQnty(int qnty) { d_maxQnty = qnty; }
void Config::minPrice(double price) { d_minPrice = price; }
void Config::maxPrice(double price) { d_maxPrice = price; }

int Config::initialBalance() { return d_initialBalance; }

}  // namespace solstice
