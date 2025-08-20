#include <order.h>
#include <order_book.h>
#include <transaction.h>
#include <truncate.h>

#include <iostream>
#include <deque>

namespace solstice
{

const std::vector<Transaction>& OrderBook::transactions() const
{
    return d_transactions;
}

bool OrderBook::receiveOrder(std::shared_ptr<Order> order)
{
    d_uidMap.emplace(order->uid(), order);

    // TODO: add map/deque of timestamps for fast time lookup

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

#ifdef ENABLE_LOGGING
void OrderBook::printSellOrders()
{
    std::cout << "\nSELL ORDERS: " << std::endl;
    for (const auto& [price, orders] : d_sellOrders)
    {
        std::cout << "$" << price << ": [";
        for (const auto& order : orders)
        {
            std::cout << " id_" << truncate(order->uid(), 5) << ' ';
        }
        std::cout << "]" << std::endl;
    }
}

void OrderBook::printBuyOrders()
{
    std::cout << "\nBUY ORDERS: " << std::endl;
    for (const auto& [price, orders] : d_buyOrders)
    {
        std::cout << "$" << price << ": [";
        for (const auto& order : orders)
        {
            std::cout << " id_" << truncate(order->uid(), 5) << ' ';
        }
        std::cout << "]" << std::endl;
    }
}
#endif

Transaction OrderBook::match(std::shared_ptr<Order> buyOrder,
                             std::shared_ptr<Order> sellOrder)
{
    Transaction transaction(buyOrder, sellOrder, 10, 10);
    return transaction;
}
}  // namespace solstice
