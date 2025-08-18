#include <config.h>
#include <order_book.h>
#include <order_processor.h>

#include <random>
#include <memory>

using namespace solstice;

inline Ticker OrderProcessor::getTicker() const {
    static const std::array<Ticker, 6> validTickers = {
        Ticker::AAPL, Ticker::TSLA, Ticker::GOOGL,
        Ticker::AMZN, Ticker::MSFT, Ticker::GOOG};

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, validTickers.size() - 1);
    return validTickers[dist(gen)];
}

double OrderProcessor::getPrice() const {
    return Random::getRandomNumber(config.minPrice, config.maxPrice);
}

double OrderProcessor::getQnty() const {
    return Random::getRandomNumber(config.minQuantity, config.maxQuantity);
}

bool OrderProcessor::getIsBuy() const {
    // TODO
    return Random::getRandomBool();
}

std::expected<std::shared_ptr<Order>, std::string>
OrderProcessor::generateOrder() const {
    Ticker ticker = getTicker();
    double price = getPrice();
    double qnty = getQnty();
    bool isBuy = getIsBuy();

    auto order = Order::createOrder(ticker, price, qnty, isBuy);

    // TODO: carry on
    return order;
}

void OrderProcessor::produceOrders() {
    // TODO
    return;
}

void OrderProcessor::start() {
    Config config;
    OrderBook orderbook;
    
    // produceOrders();
    // TODO
}
