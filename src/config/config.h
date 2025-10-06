#ifndef CONFIG_H
#define CONFIG_H

#include <log_level.h>

#include <expected>
#include <string>

namespace solstice::matching
{

struct Config
{
   public:
    static std::expected<Config, std::string> initConfig();

    LogLevel d_logLevel = LogLevel::INFO;

    // number of orders to generate
    int d_ordersToGenerate = 100000;

    // number of unique tickers that orders can come in for
    int d_tkrPoolCount = 50;

    // min and max quanties of each order
    int d_minQnty = 1;
    int d_maxQnty = 20;

    // mix and max price of each order
    double d_minPrice = 8.0;
    double d_maxPrice = 10.0;

   private:
    Config();

    static std::expected<void, std::string> checkConfig(Config& config);
};

const Config& getConfig();

}  // namespace solstice::matching

#endif  // CONFIG_H
