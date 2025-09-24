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

std::deque<OrderPtr>& OrderBook::getOrdersDequeAtPrice(OrderPtr order)
{
    auto& ordersDeque =
        d_activeOrders[order->tkr()]
            .activeOrders[order->orderSide()][order->price()];

    return ordersDeque;
}

std::deque<OrderPtr>& OrderBook::getOrdersDequeAtPrice(OrderPtr order,
                                                       int price)
{
    auto& ordersDeque =
        d_activeOrders[order->tkr()]
            .activeOrders[order->orderSide()][order->price()];

    return ordersDeque;
}

const std::deque<OrderPtr>& OrderBook::getMatchingOrders(
    const OrderPtr& incoming)
{
    auto oppositeOrderSide = (incoming->orderSide() == OrderSide::Buy)
                                 ? OrderSide::Sell
                                 : OrderSide::Buy;

    return getOrdersDequeAtPrice(incoming);
}

const std::deque<OrderPtr>& OrderBook::getMatchingOrders(
    const OrderPtr& incoming, int priceToMatch)
{
    auto oppositeOrderSide = (incoming->orderSide() == OrderSide::Buy)
                                 ? OrderSide::Sell
                                 : OrderSide::Buy;

    return getOrdersDequeAtPrice(incoming, priceToMatch);
}

void OrderBook::addOrderToBook(OrderPtr order)
{
    // add to UID lookup map
    d_uidMap[order->uid()] = order;

    // add price to price lookup map
    if (order->orderSide() == OrderSide::Buy)
    {
        d_buyPrices.insert(order->price());
    }
    else
    {
        d_sellPrices.insert(order->price());
    }

    // add order to map of active orders
    getOrdersDequeAtPrice(order).push_back(order);
}

void OrderBook::removeOrderFromBook(OrderPtr orderToRemove)
{
    d_uidMap.erase(orderToRemove->uid());

    // always remove first element - FIFO
    getOrdersDequeAtPrice(orderToRemove).at(0);
}

void OrderBook::markOrderAsComplete(OrderPtr completedOrder)
{
    completedOrder->orderComplete(true);

    removeOrderFromBook(completedOrder);
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
