#ifndef PRICER_H
#define PRICER_H

#include <asset_class.h>
#include <order_book.h>

#include <memory>
#include <unordered_map>

namespace solstice::pricing
{

struct EquityPriceData
{
   public:
    int lastPrice();
    int highestBid();
    int lowestAsk();
    int demandFactor();

   private:
    int d_lastPrice;
    int d_highestBid;
    int d_lowestAsk;
    int d_demandFactor;
};

struct FuturePriceData
{
   public:
    int lastPrice();
    int highestBid();
    int lowestAsk();
    int demandFactor();

   private:
    int d_lastPrice;
    int d_highestBid;
    int d_lowestAsk;
    int d_demandFactor;
};

class Pricer
{
   public:
    Pricer(std::shared_ptr<matching::OrderBook> orderBook);

    template <typename T>
    void initialisePricerEquities()
    {
        for (const auto& underlying : underlyingsPool<Equity>())
        {
            d_equityDataMap[underlying];
        }
    }

    template <typename T>
    void initialisePricerFutures()
    {
        for (const auto& underlying : underlyingsPool<Future>())
        {
            d_futureDataMap[underlying];
        }
    }

   private:
    double generateSeedPrice();

    std::unordered_map<Equity, EquityPriceData> d_equityDataMap;
    std::unordered_map<Future, FuturePriceData> d_futureDataMap;

    double d_seedPrice;

    std::shared_ptr<matching::OrderBook> d_orderBook;
};
}  // namespace solstice::pricing

#endif  // PRICER_H
