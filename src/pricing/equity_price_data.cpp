#include <equity_price_data.h>

namespace solstice::pricing
{

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

void EquityPriceData::underlying(Equity newUnderlying) { d_equity = newUnderlying; }
void EquityPriceData::lastPrice(double newLastPrice) { d_lastPrice = newLastPrice; }
void EquityPriceData::highestBid(double newHighestBid) { d_highestBid = newHighestBid; }
void EquityPriceData::lowestAsk(double newLowestAsk) { d_lowestAsk = newLowestAsk; }
void EquityPriceData::demandFactor(double newDemandFactor) { d_demandFactor = newDemandFactor; }
void EquityPriceData::movingAverage(double newMovingAverage) { d_movingAverage = newMovingAverage; }
void EquityPriceData::incrementExecutions() { d_executions++; }
void EquityPriceData::pricesSum(double newPricesSum) { d_pricesSum = newPricesSum; }
void EquityPriceData::pricesSumSquared(double newPricesSumSq)
{
    d_pricesSumSquared = newPricesSumSq;
}

double EquityPriceData::standardDeviation(EquityPriceData& data)
{
    double n = data.executions();
    if (n < 2) return 0;

    return std::sqrt((data.pricesSumSquared() / n) - std::pow((data.pricesSum() / n), 2));
}

}  // namespace solstice::pricing
