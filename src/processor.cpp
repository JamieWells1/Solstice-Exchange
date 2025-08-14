#include <order_book.h>
#include <processor.h>
#include <order_config.h>

using namespace solstice;

const std::string Processor::start() {
    OrderBook orderbook;
}

std::shared_ptr<Order> Processor::generateOrder() {
    Ticker ticker = getTicker();
    double price = getPrice();
    double qnty = getQnty();
    bool isBuy = getIsBuy();

    auto order = Order::createOrder(ticker, price, qnty, isBuy);

    // carry on
}
