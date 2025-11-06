#include <asset_class.h>
#include <config.h>
#include <get_random.h>
#include <market_side.h>
#include <order_type.h>
#include <pricer.h>

#include <unordered_map>

namespace solstice::pricing
{

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

void EquityPriceData::lastPrice(int newLastPrice) { d_lastPrice = newLastPrice; }

void EquityPriceData::highestBid(int newHighestBid) { d_highestBid = newHighestBid; }

void EquityPriceData::lowestAsk(int newLowestAsk) { d_lowestAsk = newLowestAsk; }

void EquityPriceData::demandFactor(int newDemandFactor) { d_demandFactor = newDemandFactor; }

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

void FuturePriceData::lastPrice(int newLastPrice) { d_lastPrice = newLastPrice; }

void FuturePriceData::highestBid(int newHighestBid) { d_highestBid = newHighestBid; }

void FuturePriceData::lowestAsk(int newLowestAsk) { d_lowestAsk = newLowestAsk; }

void FuturePriceData::demandFactor(int newDemandFactor) { d_demandFactor = newDemandFactor; }

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

double Pricer::calculatePrice(Equity eq, MarketSide mktSide)
{
    EquityPriceData data = getPriceData(eq);

    if (data.executions() == 0)
    {
        // return early and set the first order to be initial price set by pricer
        return data.lastPrice();
    }

    // order follows bullish momentum
    if (mktSide == MarketSide::Bid)
    {
    }

    // factors to consider:
    // 1. demandFactor
    // 2. SD of price from MA
    // 3. MarketSide
    // 4. lastPrice
    // 5. bidAsk spread
}

double Pricer::calculatePrice(Future fut, MarketSide mktSide)
{
    // TODO: calculate price
    FuturePriceData data = getPriceData(fut);
}

double Pricer::calculateQnty(Equity eq, MarketSide mktSide, double price)
{
    // TODO: calculate qnty
    EquityPriceData data = getPriceData(eq);
}

double Pricer::calculateQnty(Future fut, MarketSide mktSide, double price)
{
    // TODO: calculate qnty
    FuturePriceData data = getPriceData(fut);
}

// ===================================================================
// POST-PROCESSING
// ===================================================================

void Pricer::update(matching::OrderPtr order)
{
    withPriceData(
        order->underlying(),
        [&order](auto& priceData)
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

            if (priceData.executions() > 0)
            {
                priceData.lastPrice(newPrice);
                priceData.pricesSum(priceData.pricesSum() + newPrice);
                double newPricesSumSquared =
                    (newPrice * newPrice) + (priceData.pricesSum() * priceData.pricesSum());
                priceData.pricesSumSquared(newPricesSumSquared);

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

            // TODO:
            // calculate new demand factor
            double df = priceData.demandFactor();
        });
}

}  // namespace solstice::pricing
