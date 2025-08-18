#include <order_book.h>
#include <config.h>
#include <order_processor.h>
#include <random>

using namespace solstice;

void OrderProcessor::start(Config config) { OrderBook orderbook; }

inline Ticker getTicker() {
    static const std::array<Ticker, 6> validTickers = {
        Ticker::AAPL, Ticker::TSLA, Ticker::GOOGL, Ticker::AMZN, Ticker::MSFT, Ticker::GOOG};

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, validTickers.size() - 1);
    return validTickers[dist(gen)];
}


std::shared_ptr<Order> OrderProcessor::generateOrder() const {
    Ticker ticker = getTicker();
    double price = getPrice();
    double qnty = getQnty();
    bool isBuy = getIsBuy();

    auto order = Order::createOrder(ticker, price, qnty, isBuy);

    // TODO: carry on
}
