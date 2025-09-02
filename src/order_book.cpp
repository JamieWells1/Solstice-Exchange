#include <order.h>
#include <order_book.h>
#include <transaction.h>
#include <truncate.h>

#include <deque>
#include <iostream>

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

    if (order->orderSide() == OrderSide::Buy)
    {
        addBuyOrder(order);
        onBuySellOrder(order);
        return true;
    }
    else
    {
        addSellOrder(order);
        onNewSellOrder(order);
        return true;
    }

    return false;
}

void OrderBook::addSellOrder(std::shared_ptr<Order> order)
{
    // fix syntax
    d_activeOrders[order->orderSide()].d_activeOrders[order->price()].push_back(order);
}

void OrderBook::addBuyOrder(std::shared_ptr<Order> order)
{
    d_activeOrders[order->tkr()].d_activeOrders[order->][order->price()].push_back(
        order);
}

void OrderBook::onNewSellOrder(std::shared_ptr<Order>)
{
    // TODO: what happens when a sell order is placed?
    return;
}

void OrderBook::onBuySellOrder(std::shared_ptr<Order>)
{
    // TODO: what happens when a buy order is placed?
    return;
}

/*
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
*/

Transaction OrderBook::match(std::shared_ptr<Order> buyOrder,
                             std::shared_ptr<Order> sellOrder)
{
    Transaction transaction(buyOrder, sellOrder, 10, 10);
    return transaction;
}
}  // namespace solstice
