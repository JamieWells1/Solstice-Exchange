#include <asset_class.h>
#include <config.h>
#include <get_random.h>
#include <market_side.h>
#include <order_type.h>
#include <pricer.h>

#include <cmath>
#include <unordered_map>

#include "time_point.h"

namespace solstice::pricing
{

constexpr double baseOrderValue = 10000;
// risk-free rate
constexpr double r = 0.05;

const std::unordered_map<OrderType, double> probabilities = {
    {OrderType::CrossSpread, 0.3}, {OrderType::InsideSpread, 0.2}, {OrderType::AtSpread, 0.5}};

// EquityPriceData getters

Equity EquityPriceData::underlying() { return d_equity; }

double EquityPriceData::lastPrice() { return d_lastPrice; }

double EquityPriceData::highestBid() { return d_highestBid; }

double EquityPriceData::lowestAsk() { return d_lowestAsk; }

double EquityPriceData::demandFactor() { return d_demandFactor; }

double EquityPriceData::movingAverage() { return d_movingAverage; }

int EquityPriceData::executions() { return d_executions; }

int EquityPriceData::maRange() { return d_maRange; }

double EquityPriceData::pricesSum() { return d_pricesSum; }

double EquityPriceData::pricesSumSquared() { return d_pricesSumSquared; }

// EquityPriceData setters

void EquityPriceData::underlying(Equity newUnderlying) { d_equity = newUnderlying; }

void EquityPriceData::lastPrice(double newLastPrice) { d_lastPrice = newLastPrice; }

void EquityPriceData::highestBid(double newHighestBid) { d_highestBid = newHighestBid; }

void EquityPriceData::lowestAsk(double newLowestAsk) { d_lowestAsk = newLowestAsk; }

void EquityPriceData::demandFactor(double newDemandFactor) { d_demandFactor = newDemandFactor; }

void EquityPriceData::movingAverage(double newMovingAverage) { d_movingAverage = newMovingAverage; }

void EquityPriceData::incrementExecutions() { d_executions++; }

void EquityPriceData::pricesSum(double newPricesSum) { d_pricesSum = newPricesSum; }

void EquityPriceData::pricesSumSquared(double newPricesSumSquared)
{
    d_pricesSumSquared = newPricesSumSquared;
}

// FuturePriceData getters

Future FuturePriceData::underlying() { return d_future; }

double FuturePriceData::lastPrice() { return d_lastPrice; }

double FuturePriceData::highestBid() { return d_highestBid; }

double FuturePriceData::lowestAsk() { return d_lowestAsk; }

double FuturePriceData::demandFactor() { return d_demandFactor; }

double FuturePriceData::movingAverage() { return d_movingAverage; }

int FuturePriceData::executions() { return d_executions; }

int FuturePriceData::maRange() { return d_maRange; }

double FuturePriceData::pricesSum() { return d_pricesSum; }

double FuturePriceData::pricesSumSquared() { return d_pricesSumSquared; }

// FuturePriceData setters

void FuturePriceData::underlying(Future newUnderlying) { d_future = newUnderlying; }

void FuturePriceData::lastPrice(double newLastPrice) { d_lastPrice = newLastPrice; }

void FuturePriceData::highestBid(double newHighestBid) { d_highestBid = newHighestBid; }

void FuturePriceData::lowestAsk(double newLowestAsk) { d_lowestAsk = newLowestAsk; }

void FuturePriceData::demandFactor(double newDemandFactor) { d_demandFactor = newDemandFactor; }

void FuturePriceData::movingAverage(double newMovingAverage) { d_movingAverage = newMovingAverage; }

void FuturePriceData::incrementExecutions() { d_executions++; }

void FuturePriceData::pricesSum(double newPricesSum) { d_pricesSum = newPricesSum; }

void FuturePriceData::pricesSumSquared(double newPricesSumSquared)
{
    d_pricesSumSquared = newPricesSumSquared;
}

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

void Pricer::initialisePricerEquities()
{
    for (const auto& underlying : underlyingsPool<Equity>())
    {
        d_equityDataMap.emplace(underlying, EquityPriceData(underlying));
    }
}

void Pricer::initialisePricerFutures()
{
    for (const auto& underlying : underlyingsPool<Future>())
    {
        d_futureDataMap.emplace(underlying, FuturePriceData(underlying));
    }
}

// Utils

double standardDeviation(EquityPriceData& data)
{
    double n = data.executions();
    if (n < 2) return 0;

    return std::sqrt((data.pricesSumSquared() / n) - std::pow((data.pricesSum() / n), 2));
}

double standardDeviation(FuturePriceData& data)
{
    double n = data.executions();
    if (n < 2) return 0;

    return std::sqrt((data.pricesSumSquared() / n) - std::pow((data.pricesSum() / n), 2));
}

// ===================================================================
// PRE-PROCESSING
// ===================================================================

double Pricer::generateSeedPrice()
{
    Config cfg = Config::instance().value();
    return Random::getRandomDouble(cfg.minPrice(), cfg.maxPrice());
}

EquityPriceData& Pricer::getPriceData(Equity eq) { return d_equityDataMap.at(eq); }

FuturePriceData& Pricer::getPriceData(Future fut) { return d_futureDataMap.at(fut); }

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

OrderType Pricer::getOrderType()
{
    OrderType type;
    double random = Random::getRandomDouble(0, 1);

    bool inCrossSpreadBand = random < probabilities.at(OrderType::CrossSpread);

    bool inInsideSpreadBand = random >= probabilities.at(OrderType::CrossSpread) &&
                              random < (probabilities.at(OrderType::CrossSpread) +
                                        probabilities.at(OrderType::InsideSpread));

    bool inAtSpreadBand = random >= (probabilities.at(OrderType::CrossSpread) +
                                     probabilities.at(OrderType::InsideSpread));

    if (inCrossSpreadBand)
    {
        type = OrderType::CrossSpread;
    }
    else if (inInsideSpreadBand)
    {
        type = OrderType::InsideSpread;
    }
    else if (inAtSpreadBand)
    {
        type = OrderType::AtSpread;
    }

    return type;
}

double Pricer::calculatePriceImpl(MarketSide mktSide, double lowestAsk, double highestBid,
                                  double demandFactor)
{
    double price = 0.0;
    OrderType type = getOrderType();

    double spread = lowestAsk - highestBid;
    double midSpread = (lowestAsk + highestBid) / 2;
    double halfSpread = (midSpread - highestBid);

    // order follows bullish momentum
    if (mktSide == MarketSide::Bid)
    {
        switch (type)
        {
            case solstice::OrderType::InsideSpread:
            {
                if (spread > 0)
                {
                    // make price within spread influenced by demandFactor rather than completely
                    // random
                    double priceLowerBound = (highestBid + ((halfSpread)*demandFactor));
                    double priceUpperBound = (lowestAsk + ((halfSpread)*demandFactor));

                    price = Random::getRandomDouble(priceLowerBound, priceUpperBound);
                }
                else
                {
                    price = highestBid;
                }
                break;
            }

            case (OrderType::CrossSpread):
            {
                if (spread > 0)
                {
                    double offset = halfSpread * std::abs(demandFactor);

                    double priceLowerBound = lowestAsk + offset;
                    double priceUpperBound = priceLowerBound + offset;

                    price = Random::getRandomDouble(priceLowerBound, priceUpperBound);
                }
                break;
            }

            case (OrderType::AtSpread):
            {
                price = highestBid;
            }
        }
    }

    // order follows bearish momentum
    if (mktSide == MarketSide::Ask)
    {
        switch (type)
        {
            case solstice::OrderType::InsideSpread:
            {
                if (spread > 0)
                {
                    double priceLowerBound = (highestBid - ((halfSpread)*demandFactor));
                    double priceUpperBound = (lowestAsk - ((halfSpread)*demandFactor));

                    price = Random::getRandomDouble(priceLowerBound, priceUpperBound);
                }
                else
                {
                    price = lowestAsk;
                }
                break;  // Prevent fall-through
            }

            case (OrderType::CrossSpread):
            {
                if (spread > 0)
                {
                    double offset = halfSpread * std::abs(demandFactor);

                    double priceUpperBound = highestBid - offset;
                    double priceLowerBound = priceUpperBound - offset;

                    price = Random::getRandomDouble(priceLowerBound, priceUpperBound);
                }
                break;
            }

            case (OrderType::AtSpread):
            {
                price = lowestAsk;
            }
        }
    }

    return std::max(1.0, price);
}

double timeToExpiry(Future fut)
{
    std::string name = to_string(fut);
    CurrentDate dateNow = currentDate();

    int expiryMonth = monthToInt(name.substr(name.length() - 5, 3));
    int expiryYear = std::stoi(name.substr(name.length() - 2, 2));

    // NOTE: expiry year is ignored to avoid expiration in the future if futures are not updated
    if (expiryMonth == dateNow.month)
    {
        return 1;
    }
    return std::abs(expiryMonth - dateNow.month) / 12.0;
}

double Pricer::calculateCarryAdjustment(Future fut)
{
    FuturePriceData data = getPriceData(fut);

    double spot = data.lastPrice();
    double t = timeToExpiry(fut);

    // where r is risk-free rate
    return spot * std::exp(r * t);
}

double Pricer::calculatePrice(Equity eq, MarketSide mktSide)
{
    EquityPriceData data = getPriceData(eq);
    if (data.executions() == 0)
    {
        // return early and set the first order to be initial price set by pricer
        return data.lastPrice();
    }

    return calculatePriceImpl(mktSide, data.lowestAsk(), data.highestBid(), data.demandFactor());
}

double Pricer::calculatePrice(Future fut, MarketSide mktSide)
{
    FuturePriceData data = getPriceData(fut);
    if (data.executions() == 0)
    {
        return data.lastPrice();
    }

    double costOfCarry = calculateCarryAdjustment(fut);
    double adjustedBid = data.highestBid() + costOfCarry;
    double adjustedAsk = data.lowestAsk() + costOfCarry;

    return calculatePriceImpl(mktSide, adjustedAsk, adjustedBid, data.demandFactor());
}

double Pricer::calculateQnty(Equity eq, MarketSide mktSide, double price)
{
    EquityPriceData data = getPriceData(eq);
    double n = data.executions();

    double sigma = n > 1 ? standardDeviation(data) : 0;
    double maxQuantity = baseOrderValue * std::abs(data.demandFactor()) / (price * (1 + sigma));

    if (maxQuantity < 1) return 1;

    return Random::getRandomDouble(1.0, maxQuantity);
}

double Pricer::calculateQnty(Future fut, MarketSide mktSide, double price)
{
    FuturePriceData data = getPriceData(fut);
    double n = data.executions();

    double sigma = n > 1 ? standardDeviation(data) : 0;
    double maxQuantity = baseOrderValue * std::abs(data.demandFactor()) / (price * (1 + sigma));

    if (maxQuantity < 1) return 1;

    return Random::getRandomDouble(1.0, maxQuantity);
}

// ===================================================================
// POST-PROCESSING
// ===================================================================

void Pricer::update(matching::OrderPtr order)
{
    withPriceData(
        order->underlying(),
        [&order, this](auto& priceData)
        {
            bool isBid = order->marketSide() == MarketSide::Bid;
            bool isAsk = order->marketSide() == MarketSide::Ask;

            if (isBid)
            {
                if (!priceData.highestBid() || priceData.highestBid() < order->matchedPrice())
                {
                    priceData.highestBid(order->matchedPrice());
                }
            }

            if (isAsk)
            {
                if (!priceData.lowestAsk() || priceData.lowestAsk() > order->matchedPrice())
                {
                    priceData.lowestAsk(order->matchedPrice());
                }
            }

            double newPrice = order->matchedPrice();

            // Always update lastPrice
            priceData.lastPrice(newPrice);

            if (priceData.executions() > 0)
            {
                priceData.pricesSum(priceData.pricesSum() + newPrice);
                priceData.pricesSumSquared(priceData.pricesSumSquared() + (newPrice * newPrice));

                // calculate new moving average in O(1) time
                int e = priceData.executions();
                int n = std::min(e, priceData.maRange());

                // at this point, neither movingAverage nor executions have been updated
                double totalMinusCurrent = priceData.movingAverage() * n;
                double totalInclCurrent = totalMinusCurrent + newPrice;
                double newMovingAverage = totalInclCurrent / (n + 1);
                priceData.movingAverage(newMovingAverage);
            }
            else
            {
                // movingAverage = lastPrice if it's first order at this underlying
                priceData.movingAverage(newPrice);
            }

            priceData.incrementExecutions();

            // calculate new demand factor
            priceData.demandFactor(updatedDemandFactor(priceData));
        });
}

}  // namespace solstice::pricing
