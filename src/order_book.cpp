#include <matcher.h>
#include <order.h>
#include <order_book.h>
#include <transaction.h>
#include <truncate.h>

#include <deque>
#include <iostream>
#include <memory>

#include "order_side.h"

namespace solstice
{

const std::vector<Transaction>& OrderBook::transactions() const
{
    return d_transactions;
}

const std::deque<OrderPtr>& OrderBook::getMatchingOrders(
    const OrderPtr& incoming)
{
    auto oppositeOrderSide = (incoming->orderSide() == OrderSide::Buy)
                                 ? OrderSide::Sell
                                 : OrderSide::Buy;

    return d_activeOrders[incoming->tkr()]
        .activeOrders[oppositeOrderSide][incoming->price()];
}

void OrderBook::addOrderToOrderBook(OrderPtr order)
{
    d_activeOrders[order->tkr()]
        .activeOrders[order->orderSide()][order->price()]
        .push_back(order);
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

Transaction OrderBook::match(OrderPtr buyOrder, OrderPtr sellOrder)
{
    Transaction transaction(buyOrder, sellOrder, 10, 10);
    return transaction;
}
}  // namespace solstice
