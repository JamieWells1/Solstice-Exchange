#include <future_price_data.h>

namespace solstice::pricing
{

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

void FuturePriceData::underlying(Future newUnderlying) { d_future = newUnderlying; }
void FuturePriceData::lastPrice(double newLastPrice) { d_lastPrice = newLastPrice; }
void FuturePriceData::highestBid(double newHighestBid) { d_highestBid = newHighestBid; }
void FuturePriceData::lowestAsk(double newLowestAsk) { d_lowestAsk = newLowestAsk; }
void FuturePriceData::demandFactor(double newDemandFactor) { d_demandFactor = newDemandFactor; }
void FuturePriceData::movingAverage(double newMovingAverage) { d_movingAverage = newMovingAverage; }
void FuturePriceData::incrementExecutions() { d_executions++; }
void FuturePriceData::pricesSum(double newPricesSum) { d_pricesSum = newPricesSum; }
void FuturePriceData::pricesSumSquared(double newPricesSumSq)
{
    d_pricesSumSquared = newPricesSumSq;
}

double FuturePriceData::standardDeviation(FuturePriceData& data)
{
    double n = data.executions();
    if (n < 2) return 0;

    return std::sqrt((data.pricesSumSquared() / n) - std::pow((data.pricesSum() / n), 2));
}

}  // namespace solstice::pricing
