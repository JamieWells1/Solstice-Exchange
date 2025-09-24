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

std::deque<OrderPtr>& OrderBook::ordersDequeAtPrice(OrderPtr order)
{
    auto& book = d_activeOrders[order->tkr()];

    auto& ordersDeque = (order->orderSide() == OrderSide::Buy)
                            ? book.buyOrders[order->price()]
                            : book.sellOrders[order->price()];

    return ordersDeque;
}

std::deque<OrderPtr>& OrderBook::ordersDequeAtPrice(OrderPtr order,
                                                    int price)
{
    auto& book = d_activeOrders[order->tkr()];

    auto& ordersDeque = (order->orderSide() == OrderSide::Buy)
                            ? book.buyOrders[order->price()]
                            : book.sellOrders[order->price()];

    return ordersDeque;
}

std::set<double, std::greater<double>>& OrderBook::buyPricesAtPriceLevel(OrderPtr order)
{
    auto& buyOrdersSet = d_activeOrders[order->tkr()].buyPrices;

    return buyOrdersSet;
}

std::set<double, std::less<double>>& OrderBook::sellPricesAtPriceLevel(OrderPtr order)
{
    auto& sellOrdersSet = d_activeOrders[order->tkr()].sellPrices;

    return sellOrdersSet;
}

const std::deque<OrderPtr>& OrderBook::getMatchingOrders(
    const OrderPtr incoming)
{
    auto oppositeOrderSide = (incoming->orderSide() == OrderSide::Buy)
                                 ? OrderSide::Sell
                                 : OrderSide::Buy;

    return ordersDequeAtPrice(incoming);
}

const std::deque<OrderPtr>& OrderBook::getMatchingOrders(
    const OrderPtr incoming, int priceToMatch)
{
    auto oppositeOrderSide = (incoming->orderSide() == OrderSide::Buy)
                                 ? OrderSide::Sell
                                 : OrderSide::Buy;

    return ordersDequeAtPrice(incoming, priceToMatch);
}

const int OrderBook::getBestPrice(OrderPtr orderToMatch)
{
    if (orderToMatch->orderSide() == OrderSide::Buy)
    {
        auto& buyPricesSet = buyPricesAtPriceLevel(orderToMatch);

        // return by value
        auto CHANGE_ME = buyPricesSet.begin();
    }
}

void OrderBook::addOrderToBook(OrderPtr order)
{
    // add to UID lookup map
    d_uidMap[order->uid()] = order;

    // add price to price lookup map
    if (order->orderSide() == OrderSide::Buy)
    {
        buyPricesAtPriceLevel(order).insert(order->price());
    }
    else
    {
        sellPricesAtPriceLevel(order).insert(order->price());
    }

    // add order to map of active orders
    ordersDequeAtPrice(order).push_back(order);
}

void OrderBook::removeOrderFromBook(OrderPtr orderToRemove)
{
    d_uidMap.erase(orderToRemove->uid());

    // always remove first element - FIFO
    ordersDequeAtPrice(orderToRemove).at(0);

    // only remove from prices set if it's the last order left at
    // that price
    if (ordersDequeAtPrice(orderToRemove).size() == 0)
    {
        if (orderToRemove->orderSide() == OrderSide::Buy)
        {
            buyPricesAtPriceLevel(orderToRemove)
                .erase(orderToRemove->price());
        }
        else
        {
            sellPricesAtPriceLevel(orderToRemove)
                .erase(orderToRemove->price());
        }
    }
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
            std::cout << " id_" << truncate(order->uid(), 5) << '
';
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
            std::cout << " id_" << truncate(order->uid(), 5) << '
';
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
