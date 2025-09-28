#ifndef CONFIG_H
#define CONFIG_H

#include <expected>
#include <string>

namespace solstice
{

struct Config
{
   public:
    static std::expected<Config, std::string> initConfig();

    // number of orders to generate
    int d_ordersToGenerate = 2000;

    // number of unique tickers that orders can come in for
    int d_tkrPoolCount = 3;

    // min and max quanties of each order
    int d_minQnty = 1;
    int d_maxQnty = 10;

    // mix and max price of each order
    double d_minPrice = 10.0;
    double d_maxPrice = 20.0;

    // price volatility of the market -- values can be in the range (0, 1)
    double d_priceVolatility = 0.5;

   private:
    Config();

    static std::expected<void, std::string> checkConfig(Config& config);
};

const Config& getConfig();

}  // namespace solstice

#endif  // CONFIG_H
