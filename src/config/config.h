#ifndef CONFIG_H
#define CONFIG_H

#include <asset_class.h>
#include <log_level.h>
#include <strategy.h>

#include <expected>
#include <string>

namespace solstice
{

struct Config
{
   public:
    static std::expected<Config, std::string> init();

    static const strategy::Strategy strategy = strategy::Strategy::SharpMovements;

    LogLevel d_logLevel = LogLevel::INFO;
    AssetClass d_assetClass = AssetClass::Future;
    int d_ordersToGenerate = 10000;
    int d_underlyingPoolCount = 50;
    int d_minQnty = 1;
    int d_maxQnty = 20;
    double d_minPrice = 8.0;
    double d_maxPrice = 10.0;

   private:
    Config();

    static std::expected<void, std::string> checkConfig(Config& config);
};

const Config& getConfig();

}  // namespace solstice

#endif  // CONFIG_H
