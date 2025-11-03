#include <asset_class.h>
#include <config.h>
#include <get_random.h>
#include <market_side.h>
#include <pricer.h>

namespace solstice::pricing
{

// EquityPriceData getters

double EquityPriceData::lastPrice() { return d_lastPrice; }

double EquityPriceData::highestBid() { return d_highestBid; }

double EquityPriceData::lowestAsk() { return d_lowestAsk; }

double EquityPriceData::demandFactor() { return d_demandFactor; }

double EquityPriceData::movingAverage() { return d_movingAverage; }

// EquityPriceData setters

void EquityPriceData::lastPrice(int newLastPrice) { d_lastPrice = newLastPrice; }

void EquityPriceData::highestBid(int newHighestBid) { d_highestBid = newHighestBid; }

void EquityPriceData::lowestAsk(int newLowestAsk) { d_lowestAsk = newLowestAsk; }

void EquityPriceData::demandFactor(int newDemandFactor) { d_demandFactor = newDemandFactor; }

void EquityPriceData::movingAverage(double newMovingAverage) { d_movingAverage = newMovingAverage; }

// FuturePriceData getters

double FuturePriceData::lastPrice() { return d_lastPrice; }

double FuturePriceData::highestBid() { return d_highestBid; }

double FuturePriceData::lowestAsk() { return d_lowestAsk; }

double FuturePriceData::demandFactor() { return d_demandFactor; }

double FuturePriceData::movingAverage() { return d_movingAverage; }

// FuturePriceData setters

void FuturePriceData::lastPrice(int newLastPrice) { d_lastPrice = newLastPrice; }

void FuturePriceData::highestBid(int newHighestBid) { d_highestBid = newHighestBid; }

void FuturePriceData::lowestAsk(int newLowestAsk) { d_lowestAsk = newLowestAsk; }

void FuturePriceData::demandFactor(int newDemandFactor) { d_demandFactor = newDemandFactor; }

void FuturePriceData::movingAverage(double newMovingAverage) { d_movingAverage = newMovingAverage; }

// PricerDepOrderData

PricerDepOrderData::PricerDepOrderData(MarketSide marketSide, double price, double qnty)
    : d_marketSide(marketSide), d_price(price), d_qnty(qnty)
{
}

MarketSide PricerDepOrderData::marketSide() { return d_marketSide; }

double PricerDepOrderData::price() { return d_price; }

double PricerDepOrderData::qnty() { return d_qnty; }

// Pricer

Pricer::Pricer(std::shared_ptr<matching::OrderBook> orderBook) : d_orderBook(orderBook)
{
    d_seedPrice = generateSeedPrice();
}

// ===================================================================
// POST-PROCESSING
// ===================================================================

void Pricer::update(matching::OrderPtr order, bool orderMatched)
{
    // TODO: Implement (called after order is processed by Orchestrator::processOrder)
}

// ===================================================================
// PRE-PROCESSING
// ===================================================================

template <typename T>
PricerDepOrderData Pricer::compute(T underlying)
{
    auto marketSide = calculateMarketSide(underlying);
    auto price = calculatePrice(underlying);
    auto qnty = calculateQnty(underlying);

    return PricerDepOrderData(marketSide, price, qnty);
}

double Pricer::generateSeedPrice()
{
    Config cfg = Config::instance().value();
    return Random::getRandomDouble(cfg.minPrice(), cfg.maxPrice());
}

EquityPriceData& Pricer::getPriceData(Equity eq) { return d_equityDataMap[eq]; }

FuturePriceData& Pricer::getPriceData(Future fut) { return d_futureDataMap[fut]; }

MarketSide Pricer::calculateMarketSide(Equity eq)
{
    EquityPriceData data = getPriceData(eq);
    double p = data.demandFactor() * data.demandFactor();

    return calculateMarketSideImpl(p);
}

MarketSide Pricer::calculateMarketSide(Future fut)
{
    FuturePriceData data = getPriceData(fut);
    double p = data.demandFactor() * data.demandFactor();

    return calculateMarketSideImpl(p);
}

MarketSide Pricer::calculateMarketSideImpl(double probability)
{
    double random = Random::getRandomDouble(-1, 1);

    MarketSide mktSide;

    // higher probability of bid/ask if higher demand factor
    bool isBid = random < probability && random > 0;
    bool isAsk = random > probability && random < 0;

    if (isBid)
    {
        return MarketSide::Bid;
    }

    if (isAsk)
    {
        return MarketSide::Ask;
    }

    // get random if random number doesn't fall within threshold
    return Order::getRandomMarketSide();
}

double Pricer::calculatePrice(Equity eq)
{
    // TODO: calculate price
    EquityPriceData data = getPriceData(eq);
}

double Pricer::calculatePrice(Future fut)
{
    // TODO: calculate price
    FuturePriceData data = getPriceData(fut);
}

double Pricer::calculateQnty(Equity eq)
{
    // TODO: calculate qnty
    EquityPriceData data = getPriceData(eq);
}

double Pricer::calculateQnty(Future fut)
{
    // TODO: calculate qnty
    FuturePriceData data = getPriceData(fut);
}

}  // namespace solstice::pricing
