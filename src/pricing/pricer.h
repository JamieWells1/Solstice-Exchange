#ifndef PRICER_H
#define PRICER_H

#include <asset_class.h>
#include <order_book.h>

#include <unordered_map>

namespace solstice::pricing
{

struct EquityPriceData
{
   public:
    int d_lastPrice;
    int d_highestBid;
    int d_lowestAsk;

   private:
    int lastPrice();
    int highestBid();
    int lowestAsk();
};

class Pricer
{
   public:
    Pricer();

    double generateSeedPrice();

   private:
    std::unordered_map<Equity, EquityPriceData> EquityDataMap;

    double d_seedPrice;
};
}  // namespace solstice::pricing

#endif  // PRICER_H
