#include <config.h>
#include <get_random.h>
#include <pricer.h>

namespace solstice::pricing
{

int EquityPriceData::lastPrice() { return d_lastPrice; }

int EquityPriceData::highestBid() { return d_highestBid; }

int EquityPriceData::lowestAsk() { return d_lowestAsk; }

Pricer::Pricer(std::shared_ptr<matching::OrderBook> orderBook) : d_orderBook(orderBook)
{
    d_seedPrice = generateSeedPrice();
}

double Pricer::generateSeedPrice()
{
    Config cfg = Config::instance().value();
    return Random::getRandomDouble(cfg.minPrice(), cfg.maxPrice());
}

}  // namespace solstice::pricing
