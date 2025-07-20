#include <orderbook.h>

OrderBook::OrderBook() : d_ordersFulfilled{0} {}

bool OrderBook::receiveOrder(std::shared_ptr<Order> order) {
    if (order->isBuy()) {
        addBuyOrder(order);
        return true;
    } else {
        addSellOrder(order);
        return true;
    }
    return false;
}

void OrderBook::addSellOrder(std::shared_ptr<Order> order) {
    d_sellOrders[order->price()].push_back(order);
}

void OrderBook::addBuyOrder(std::shared_ptr<Order> order) {
    d_buyOrders[order->price()].push_back(order);
}

int OrderBook::ordersFulfilled() { return d_ordersFulfilled; }
