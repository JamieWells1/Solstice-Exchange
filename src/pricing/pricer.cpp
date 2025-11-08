#include <asset_class.h>
#include <config.h>
#include <get_random.h>
#include <market_side.h>
#include <order_type.h>
#include <pricer.h>
#include <time_point.h>

#include <cmath>
#include <unordered_map>

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
                    double shift = halfSpread * demandFactor * 0.5;  // use 50% of half-spread
                    double targetPrice = midSpread + shift;
                    double priceRange = halfSpread * 0.3;  // 30% of half-spread as range

                    double priceLowerBound = std::max(highestBid, targetPrice - priceRange);
                    double priceUpperBound = std::min(lowestAsk, targetPrice + priceRange);

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
                    double offset = halfSpread * std::abs(demandFactor) * 0.5;

                    double priceLowerBound = lowestAsk;
                    double priceUpperBound = lowestAsk + offset;

                    price = Random::getRandomDouble(priceLowerBound, priceUpperBound);
                }
                else
                {
                    price = lowestAsk;
                }
                break;
            }

            case (OrderType::AtSpread):
            {
                price = highestBid;
                break;
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
                    double shift = halfSpread * demandFactor * 0.5;  // use 50% of half-spread
                    double targetPrice = midSpread + shift;
                    double priceRange = halfSpread * 0.3;  // 30% of half-spread as range

                    double priceLowerBound = std::max(highestBid, targetPrice - priceRange);
                    double priceUpperBound = std::min(lowestAsk, targetPrice + priceRange);

                    price = Random::getRandomDouble(priceLowerBound, priceUpperBound);
                }
                else
                {
                    price = lowestAsk;
                }
                break;
            }

            case (OrderType::CrossSpread):
            {
                if (spread > 0)
                {
                    double offset = halfSpread * std::abs(demandFactor) * 0.5;

                    double priceUpperBound = highestBid;
                    double priceLowerBound = std::max(1.0, highestBid - offset);

                    price = Random::getRandomDouble(priceLowerBound, priceUpperBound);
                }
                else
                {
                    price = highestBid;
                }
                break;
            }

            case (OrderType::AtSpread):
            {
                price = lowestAsk;
                break;
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
    return spot * std::exp(r * t) - spot;
}

double Pricer::calculatePrice(Equity eq, MarketSide mktSide)
{
    EquityPriceData& data = getPriceData(eq);

    if (data.highestBid() == 0.0 && data.lowestAsk() == 0.0)
    {
        double initialPrice = data.lastPrice();
        double spreadWidth = initialPrice * 0.002;  // 0.2% initial spread (tighter)

        data.highestBid(initialPrice - spreadWidth / 2);
        data.lowestAsk(initialPrice + spreadWidth / 2);
    }
    else if (data.executions() >= 10)
    {
        double basePrice = data.movingAverage();
        double sigma = standardDeviation(data);

        double spreadWidth = basePrice * (0.002 + sigma * 0.0015);  // Reduced volatility component

        double targetBid = basePrice - spreadWidth / 2;
        double targetAsk = basePrice + spreadWidth / 2;

        data.highestBid(data.highestBid() * 0.95 +
                        targetBid * 0.05);  // Adjust spread more gradually
        data.lowestAsk(data.lowestAsk() * 0.95 + targetAsk * 0.05);
    }

    return calculatePriceImpl(mktSide, data.lowestAsk(), data.highestBid(), data.demandFactor());
}

double Pricer::calculatePrice(Future fut, MarketSide mktSide)
{
    FuturePriceData& data = getPriceData(fut);
    double basePrice = data.lastPrice();

    if (data.executions() > 0)
    {
        basePrice = data.movingAverage();
    }

    double spreadWidth;
    if (data.executions() > 1)
    {
        double sigma = standardDeviation(data);
        spreadWidth = basePrice * (0.005 + sigma * 0.01);  // 0.5% base + volatility component
    }
    else
    {
        spreadWidth = basePrice * 0.01;  // 1% initial spread
    }

    data.highestBid(basePrice - spreadWidth / 2);
    data.lowestAsk(basePrice + spreadWidth / 2);

    double costOfCarry = calculateCarryAdjustment(fut);
    double adjustedBid = data.highestBid() + costOfCarry;
    double adjustedAsk = data.lowestAsk() + costOfCarry;

    return calculatePriceImpl(mktSide, adjustedAsk, adjustedBid, data.demandFactor());
}

double Pricer::calculateQnty(Equity eq, MarketSide mktSide, double price)
{
    EquityPriceData data = getPriceData(eq);
    double n = data.executions();

    double demandScale = 0.3 + (0.7 * std::abs(data.demandFactor()));

    double sigma = n > 1 ? standardDeviation(data) : 0;
    double volAdjustment = std::min(sigma, 0.5);

    double maxQuantity = baseOrderValue * demandScale / (price * (1 + volAdjustment));
    if (maxQuantity < 10) return Random::getRandomDouble(1.0, 10.0);

    return Random::getRandomDouble(1.0, maxQuantity);
}

double Pricer::calculateQnty(Future fut, MarketSide mktSide, double price)
{
    FuturePriceData data = getPriceData(fut);
    double n = data.executions();
    double demandScale = 0.3 + (0.7 * std::abs(data.demandFactor()));

    double sigma = n > 1 ? standardDeviation(data) : 0;
    double volAdjustment = std::min(sigma, 0.5);

    double maxQuantity = baseOrderValue * demandScale / (price * (1 + volAdjustment));

    if (maxQuantity < 10) return Random::getRandomDouble(1.0, 10.0);

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

            if (order->matched())
            {
                double matchedPrice = order->matchedPrice();

                if (isBid && (!priceData.highestBid() || priceData.highestBid() < matchedPrice))
                {
                    priceData.highestBid(matchedPrice);
                }

                if (isAsk && (!priceData.lowestAsk() || priceData.lowestAsk() > matchedPrice))
                {
                    priceData.lowestAsk(matchedPrice);
                }

                priceData.lastPrice(matchedPrice);

                if (priceData.executions() >= 10)  // Only update movingAverage after 10 executions
                {
                    priceData.pricesSum(priceData.pricesSum() + matchedPrice);
                    priceData.pricesSumSquared(priceData.pricesSumSquared() +
                                               (matchedPrice * matchedPrice));

                    // Calculate new moving average in O(1) time
                    int e = priceData.executions();
                    int n = std::min(e, priceData.maRange());

                    double totalMinusCurrent = priceData.movingAverage() * n;
                    double totalInclCurrent = totalMinusCurrent + matchedPrice;
                    double newMovingAverage = totalInclCurrent / (n + 1);
                    priceData.movingAverage(newMovingAverage);
                }
                else if (priceData.executions() == 0)
                {
                    priceData.movingAverage(matchedPrice);
                }

                priceData.incrementExecutions();
                priceData.demandFactor(updatedDemandFactor(priceData));
            }
            else
            {
                double orderPrice = order->price();

                if (isBid && (!priceData.highestBid() || priceData.highestBid() < orderPrice))
                {
                    priceData.highestBid(orderPrice);
                }

                if (isAsk && (!priceData.lowestAsk() || priceData.lowestAsk() > orderPrice))
                {
                    priceData.lowestAsk(orderPrice);
                }
            }
        });
}

}  // namespace solstice::pricing
