#include <orderbook.h>

#include "transaction.h"

using namespace solstice;

const std::vector<Transaction>& OrderBook::transactions() const {
    return d_transactions;
}

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

Transaction OrderBook::match(std::shared_ptr<Order> buyOrder,
                             std::shared_ptr<Order> sellOrder) {
    // TODO 2: needs implementation
    Transaction transaction(buyOrder, sellOrder, 10, 10);
    return transaction;
}
