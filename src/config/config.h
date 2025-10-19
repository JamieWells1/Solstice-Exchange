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

    LogLevel logLevel() const;
    AssetClass assetClass() const;
    int ordersToGenerate() const;
    int underlyingPoolCount() const;
    int minQnty() const;
    int maxQnty() const;
    double minPrice() const;
    double maxPrice() const;

    void logLevel(LogLevel level);
    void assetClass(AssetClass assetClass);
    void ordersToGenerate(int count);
    void underlyingPoolCount(int count);
    void minQnty(int qnty);
    void maxQnty(int qnty);
    void minPrice(double price);
    void maxPrice(double price);

   private:
    Config();

    static std::expected<void, std::string> checkConfig(Config& config);

    LogLevel d_logLevel = LogLevel::INFO;
    AssetClass d_assetClass = AssetClass::Future;
    int d_ordersToGenerate = 10000;
    int d_underlyingPoolCount = 50;
    int d_minQnty = 1;
    int d_maxQnty = 20;
    double d_minPrice = 8.0;
    double d_maxPrice = 10.0;
};

const Config& getConfig();

}  // namespace solstice

#endif  // CONFIG_H
