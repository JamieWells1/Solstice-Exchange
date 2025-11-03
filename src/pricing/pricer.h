#ifndef PRICER_H
#define PRICER_H

#include <asset_class.h>
#include <get_random.h>
#include <market_side.h>
#include <order_book.h>

#include <memory>
#include <unordered_map>
#include <variant>

namespace solstice::pricing
{

template <typename PriceData>
void setInitialDemandFactor(PriceData& underlying)  // type
{
    underlying.demandFactor(Random::getRandomDouble(-1, 1));
}

template <typename PriceData>
void setInitialPrice(PriceData& underlying)
{
    underlying.lastPrice(Random::getRandomDouble(10, 200));
}

template <typename PriceData>
void setInitialMovingAverage(PriceData& underlying)
{
    underlying.movingAverage(underlying.lastPrice());
}

struct EquityPriceData
{
   public:
    EquityPriceData(Equity underlying) : d_equity(underlying)
    {
        setInitialDemandFactor(*this);
        setInitialPrice(*this);
        setInitialMovingAverage(*this);
    }

    Equity underlying();

    double lastPrice();
    double highestBid();
    double lowestAsk();
    double demandFactor();
    double movingAverage();

    void underlying(Equity eq);

    void lastPrice(int newLastPrice);
    void highestBid(int newHighestBid);
    void lowestAsk(int newLowestAsk);
    void demandFactor(int newDemandFactor);
    void movingAverage(double newMovingAverage);

   private:
    static constexpr int movingAverageRange = 10;

    Equity d_equity;

    double d_lastPrice;
    double d_highestBid;
    double d_lowestAsk;
    double d_demandFactor;
    double d_movingAverage;
};

struct FuturePriceData
{
   public:
    FuturePriceData(Future underlying) : d_future(underlying)
    {
        setInitialDemandFactor(*this);
        setInitialPrice(*this);
        setInitialMovingAverage(*this);
    }

    Future underlying();

    double lastPrice();
    double highestBid();
    double lowestAsk();
    double demandFactor();
    double movingAverage();

    void underlying(Future fut);

    void lastPrice(int newLastPrice);
    void highestBid(int newHighestBid);
    void lowestAsk(int newLowestAsk);
    void demandFactor(int newDemandFactor);
    void movingAverage(double newMovingAverage);

   private:
    static constexpr int movingAverageRange = 10;

    Future d_future;

    double d_lastPrice;
    double d_highestBid;
    double d_lowestAsk;
    double d_demandFactor;
    double d_movingAverage;
};

struct PricerDepOrderData
{
   public:
    PricerDepOrderData(MarketSide d_marketSide, double d_price, double d_qnty);

    MarketSide marketSide();
    double price();
    double qnty();

   private:
    MarketSide d_marketSide;
    double d_price;
    double d_qnty;
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
            setInitialDemandFactor(d_equityDataMap[underlying]);
        }
    }

    template <typename T>
    void initialisePricerFutures()
    {
        for (const auto& underlying : underlyingsPool<Future>())
        {
            d_futureDataMap[underlying];
            setInitialDemandFactor(d_futureDataMap[underlying]);
        }
    }

    void update(matching::OrderPtr order, bool orderMatched);

    template <typename T>
    PricerDepOrderData compute(T underlying)
    {
        return std::visit(
            [this](auto&& underlying)
            {
                auto marketSide = calculateMarketSide(underlying);
                auto price = calculatePrice(underlying, marketSide);
                auto qnty = calculateQnty(underlying, marketSide, price);

                return PricerDepOrderData(marketSide, price, qnty);
            },
            underlying);
    }

   private:
    double generateSeedPrice();

    EquityPriceData& getPriceData(Equity eq);
    FuturePriceData& getPriceData(Future fut);

    MarketSide calculateMarketSide(Equity eq);
    MarketSide calculateMarketSide(Future fut);

    MarketSide calculateMarketSideImpl(double probability);

    // propogate results from market side calc
    double calculatePrice(Equity eq, MarketSide mktSide);
    double calculatePrice(Future fut, MarketSide mktSide);

    // propogate results from market side calc and price calc
    double calculateQnty(Equity eq, MarketSide mktSide, double price);
    double calculateQnty(Future fut, MarketSide mktSide, double price);

    std::unordered_map<Equity, EquityPriceData> d_equityDataMap;
    std::unordered_map<Future, FuturePriceData> d_futureDataMap;

    double d_seedPrice;

    std::shared_ptr<matching::OrderBook> d_orderBook;
};
}  // namespace solstice::pricing

#endif  // PRICER_H
