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
    // ===================================================================
    // Order Book
    // ===================================================================

    static std::expected<Config, std::string> instance();

    LogLevel logLevel() const;
    AssetClass assetClass() const;
    int ordersToGenerate() const;
    int underlyingPoolCount() const;
    int minQnty() const;
    int maxQnty() const;
    double minPrice() const;
    double maxPrice() const;
    bool usePricer() const;

    void logLevel(LogLevel level);
    void assetClass(AssetClass assetClass);
    void ordersToGenerate(int count);
    void underlyingPoolCount(int count);
    void minQnty(int qnty);
    void maxQnty(int qnty);
    void minPrice(double price);
    void maxPrice(double price);
    void usePricer(bool usePricer);

    // ===================================================================
    // Backtesting
    // ===================================================================

    static const strategy::Strategy strategy = strategy::Strategy::SharpMovements;

    int initialBalance();

   private:
    // ===================================================================
    // Order Book
    // ===================================================================
    Config();

    static std::expected<void, std::string> checkConfig(Config& config);

    LogLevel d_logLevel = LogLevel::DEBUG;
    AssetClass d_assetClass = AssetClass::Equity;
    int d_ordersToGenerate = 10000;
    int d_underlyingPoolCount = 1;
    int d_minQnty = 1;
    int d_maxQnty = 20;
    double d_minPrice = 9.0;
    double d_maxPrice = 10.0;
    bool d_usePricer = true;

    // ===================================================================
    // Backtesting
    // ===================================================================

    int d_initialBalance = 10000;
};

}  // namespace solstice

#endif  // CONFIG_H
