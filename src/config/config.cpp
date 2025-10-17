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

std::expected<Config, std::string> Config::init()
{
    Config config;

    auto isValid = checkConfig(config);
    if (!isValid)
    {
        return std::unexpected(isValid.error());
    }
    return std::move(config);
}

}  // namespace solstice
