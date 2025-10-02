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

std::optional<std::reference_wrapper<std::deque<OrderPtr>>>
OrderBook::getOrdersDequeAtPrice(OrderPtr order)
{
    auto it = d_activeOrders.find(order->tkr());
    if (it == d_activeOrders.end())
    {
        return std::nullopt;
    }

    ActiveOrders& book = d_activeOrders.at(order->tkr());

    return (order->orderSide() == OrderSide::Buy)
               ? book.buyOrders.at(order->price())
               : book.sellOrders.at(order->price());
}

std::deque<OrderPtr>& OrderBook::getOrdersDequeAtPrice(OrderPtr order,
                                                       int priceToMatch)
{
    auto& book = d_activeOrders.at(order->tkr());

    return (order->orderSide() == OrderSide::Buy)
               ? book.buyOrders.at(priceToMatch)
               : book.sellOrders.at(priceToMatch);
}

std::deque<OrderPtr>& OrderBook::ordersDequeAtPrice(OrderPtr order)
{
    auto& book = d_activeOrders[order->tkr()];

    return (order->orderSide() == OrderSide::Buy)
               ? book.buyOrders[order->price()]
               : book.sellOrders[order->price()];
}

std::map<double, std::deque<OrderPtr>>& OrderBook::priceLevelMap(
    OrderPtr order)
{
    auto& book = d_activeOrders.at(order->tkr());

    return (order->orderSide() == OrderSide::Buy) ? book.buyOrders
                                                  : book.sellOrders;
}

std::set<double, std::greater<double>>&
OrderBook::getBuyPricesAtPriceLevel(OrderPtr order)
{
    auto& buyOrdersSet = d_activeOrders.at(order->tkr()).buyPrices;

    return buyOrdersSet;
}

std::set<double, std::greater<double>>& OrderBook::buyPricesAtPriceLevel(
    OrderPtr order)
{
    auto& buyOrdersSet = d_activeOrders[order->tkr()].buyPrices;

    return buyOrdersSet;
}

std::set<double, std::less<double>>& OrderBook::getSellPricesAtPriceLevel(
    OrderPtr order)
{
    auto& sellOrdersSet = d_activeOrders.at(order->tkr()).sellPrices;

    return sellOrdersSet;
}

std::set<double, std::less<double>>& OrderBook::sellPricesAtPriceLevel(
    OrderPtr order)
{
    auto& sellOrdersSet = d_activeOrders[order->tkr()].sellPrices;

    return sellOrdersSet;
}

const std::expected<double, std::string> OrderBook::getBestPrice(
    OrderPtr orderToMatch)
{
    if (orderToMatch->orderSide() == OrderSide::Buy)
    {
        auto& sellPricesSet = getSellPricesAtPriceLevel(orderToMatch);

        if (sellPricesSet.size() == 0)
        {
            return std::unexpected(
                std::format("No sell orders found for ticker {}\n",
                            orderToMatch->tkrString()));
        }

        // find highest sell price at or below target price
        auto it = sellPricesSet.upper_bound(orderToMatch->price());
        if (it == sellPricesSet.begin())
        {
            return std::unexpected(
                "No matching sell orders lower than or equal to buy "
                "price\n");
        }
        --it;

        return *it;
    }
    else
    {
        auto& buyPricesSet = getBuyPricesAtPriceLevel(orderToMatch);

        if (buyPricesSet.size() == 0)
        {
            return std::unexpected(
                std::format("No buy orders found for ticker {}\n",
                            orderToMatch->tkrString()));
        }

        // find highest buy price at or below target price
        auto it = buyPricesSet.upper_bound(orderToMatch->price());
        if (it == buyPricesSet.begin())
        {
            return std::unexpected(
                "No matching buy orders lower than or equal to buy "
                "price\n");
        }
        --it;

        return *it;
    }
}

void OrderBook::addOrderToBook(OrderPtr order)
{
    std::cout << "|| OrderBook::addOrderToBook\n";

    std::cout << "=> old order book: [";
    for (OrderPtr order : ordersDequeAtPrice(order))
    {
        std::cout << order << ", ";
    }
    std::cout << "]\n";

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

    std::cout << "=> new order book: [";
    for (OrderPtr order : ordersDequeAtPrice(order))
    {
        std::cout << order << ", ";
    }
    std::cout << "]\n";
}

void OrderBook::removeOrderFromBook(OrderPtr orderToRemove)
{
    ordersDequeAtPrice(orderToRemove).pop_front();
}

void OrderBook::markOrderAsFulfilled(OrderPtr completedOrder)
{
    completedOrder->orderComplete(true);

    // remove references
    removeOrderFromBook(completedOrder);
    d_uidMap.erase(completedOrder->uid());

    // only remove from prices set if it's the last order left at
    // that price
    auto dequeOptional = getOrdersDequeAtPrice(completedOrder);
    if (dequeOptional && dequeOptional->get().empty())
    {
        if (completedOrder->orderSide() == OrderSide::Buy)
        {
            buyPricesAtPriceLevel(completedOrder)
                .erase(completedOrder->price());
        }
        else
        {
            sellPricesAtPriceLevel(completedOrder)
                .erase(completedOrder->price());
        }
    }
}

}  // namespace solstice
