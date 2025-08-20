#include <config.h>

#include <algorithm>

using namespace solstice;

Config::Config() {}

std::expected<void, std::string> Config::checkConfig(Config& config)
{
    if (std::any_of(std::begin({double(config.d_ordersToGenerate),
                                double(config.d_minQuantity),
                                double(config.d_maxQuantity),
                                double(config.d_minPrice),
                                double(config.d_maxPrice)}),
                    std::end({double(config.d_ordersToGenerate),
                              double(config.d_minQuantity),
                              double(config.d_maxQuantity),
                              double(config.d_minPrice),
                              double(config.d_maxPrice)}),
                    [](auto x) { return x < 0; }))
    {
        return std::unexpected("Config values cannot be less than 0");
    }

    if (d_priceVolatility < 0 || d_priceVolatility > 1)
    {
        return std::unexpected("Price volatility must be between 0 and 1");
    }

    return {};
}

std::expected<Config, std::string> Config::initConfig()
{
    Config config;

    auto isValid = checkConfig(config);

    if (!isValid)
    {
        return std::unexpected(isValid.error());
    }

    return config;
}
