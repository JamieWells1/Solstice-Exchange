#include <config.h>
#include <get_random.h>
#include <pricer.h>

#include "asset_class.h"

namespace solstice::pricing
{

// EquityPriceData getters

double EquityPriceData::lastPrice() { return d_lastPrice; }

double EquityPriceData::highestBid() { return d_highestBid; }

double EquityPriceData::lowestAsk() { return d_lowestAsk; }

double EquityPriceData::demandFactor() { return d_demandFactor; }

// EquityPriceData setters

void EquityPriceData::lastPrice(int newLastPrice) { d_lastPrice = newLastPrice; }

void EquityPriceData::highestBid(int newHighestBid) { d_highestBid = newHighestBid; }

void EquityPriceData::lowestAsk(int newLowestAsk) { d_lowestAsk = newLowestAsk; }

void EquityPriceData::demandFactor(int newDemandFactor) { d_demandFactor = newDemandFactor; }

// FuturePriceData getters

double FuturePriceData::lastPrice() { return d_lastPrice; }

double FuturePriceData::highestBid() { return d_highestBid; }

double FuturePriceData::lowestAsk() { return d_lowestAsk; }

double FuturePriceData::demandFactor() { return d_demandFactor; }

// FuturePriceData setters

void FuturePriceData::lastPrice(int newLastPrice) { d_lastPrice = newLastPrice; }

void FuturePriceData::highestBid(int newHighestBid) { d_highestBid = newHighestBid; }

void FuturePriceData::lowestAsk(int newLowestAsk) { d_lowestAsk = newLowestAsk; }

void FuturePriceData::demandFactor(int newDemandFactor) { d_demandFactor = newDemandFactor; }

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

void Pricer::update(matching::OrderPtr order, bool orderMatched)
{
    // TODO: Implement (called after order is processed by Orchestrator::processOrder)
}

PricerDepOrderData Pricer::compute(Underlying underlying)
{
    // TODO: Implement (orchestrator for generating price, marketSide and qnty. Called by
    // Order::createWithPricer)

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

MarketSide& Pricer::calculateMarketSide(Underlying underlying)
{
    // calculate marketside
}

double Pricer::calculatePrice(Underlying underlying)
{
    // calculate price
}

double Pricer::calculateQnty(Underlying underlying)
{
    // calculate qnty
}

}  // namespace solstice::pricing
