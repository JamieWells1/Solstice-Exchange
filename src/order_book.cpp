#include <order_book.h>

#include "transaction.h"

using namespace solstice;

const std::vector<Transaction>& OrderBook::transactions() const
{
    return d_transactions;
}

bool OrderBook::receiveOrder(std::shared_ptr<Order> order)
{
    d_uidMap.emplace(order->uid(), order);

    if (order->isBuy())
    {
        addBuyOrder(order);
        return true;
    }
    else
    {
        addSellOrder(order);
        return true;
    }
    return false;
}

void OrderBook::addSellOrder(std::shared_ptr<Order> order)
{
    d_sellOrders[order->price()].push_back(order);
}

void OrderBook::addBuyOrder(std::shared_ptr<Order> order)
{
    d_buyOrders[order->price()].push_back(order);
}

Transaction OrderBook::match(std::shared_ptr<Order> buyOrder,
                             std::shared_ptr<Order> sellOrder,
                             double price, double qnty)
{
    Transaction transaction(buyOrder, sellOrder, price, qnty);
    return transaction;
}
