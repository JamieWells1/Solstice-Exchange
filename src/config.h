#ifndef CONFIG_H
#define CONFIG_H

#include <ticker.h>

#include <expected>

namespace solstice
{

struct Config
{
   public:
    static std::expected<Config, std::string> initConfig();

    // number of orders to generate
    static constexpr int d_ordersToGenerate = 5;

    // min and max quantity of equities in each order
    static constexpr int d_minQuantity = 1;
    static constexpr int d_maxQuantity = 100;

    // mix and max price of each order
    static constexpr double d_minPrice = 10.0;
    static constexpr double d_maxPrice = 1000.0;

    // price volatility of the market -- values can be in the range (0, 1)
    static constexpr double d_priceVolatility = 0.5;

   private:
    Config();

    static std::expected<void, std::string> checkConfig(Config& config);
};
}  // namespace solstice

#endif  // CONFIG_H
