#include <config.h>

#include <algorithm>
#include <iostream>

namespace solstice
{

Config::Config() {}

std::expected<void, std::string> Config::checkConfig(Config& config)
{
    auto values = {double(config.d_ordersToGenerate),
                   double(config.d_minQnty), double(config.d_maxQnty),
                   double(config.d_minPrice), double(config.d_maxPrice)};

    if (std::any_of(std::begin(values), std::end(values),
                    [](auto x) { return x < 0; }))
    {
        return std::unexpected("Config values cannot be less than 0");
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
}  // namespace solstice
