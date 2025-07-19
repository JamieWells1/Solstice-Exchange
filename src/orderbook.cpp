#include <orderbook.h>

OrderBook::OrderBook() : d_ordersFulfilled{0} {}

void OrderBook::addSellOrder(std::shared_ptr<Order> order) {
    d_sellOrders[order->price()].push_back(order);
}

void OrderBook::addBuyOrder(std::shared_ptr<Order> order) {
    d_buyOrders[order->price()].push_back(order);
}

int OrderBook::ordersFulfilled() {
    return d_ordersFulfilled;
}
