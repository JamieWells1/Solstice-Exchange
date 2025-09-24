#include <matcher.h>
#include <order.h>
#include <order_book.h>
#include <transaction.h>
#include <truncate.h>

#include <deque>
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

std::map<double, std::deque<OrderPtr>>& OrderBook::priceLevelMap(OrderPtr order)
{
    auto& book = d_activeOrders[order->tkr()];

    auto& priceLevelMap = (order->orderSide() == OrderSide::Buy)
                            ? book.buyOrders
                            : book.sellOrders;

    return priceLevelMap;
}

std::set<double, std::greater<double>>& OrderBook::buyPricesAtPriceLevel(
    OrderPtr order)
{
    auto& buyOrdersSet = d_activeOrders[order->tkr()].buyPrices;

    return buyOrdersSet;
}

std::set<double, std::less<double>>& OrderBook::sellPricesAtPriceLevel(
    OrderPtr order)
{
    auto& sellOrdersSet = d_activeOrders[order->tkr()].sellPrices;

    return sellOrdersSet;
}

std::deque<OrderPtr>& OrderBook::getOrdersAtPrice(const OrderPtr incoming)
{
    auto oppositeOrderSide = (incoming->orderSide() == OrderSide::Buy)
                                 ? OrderSide::Sell
                                 : OrderSide::Buy;

    return ordersDequeAtPrice(incoming);
}

std::deque<OrderPtr>& OrderBook::getOrdersAtPrice(const OrderPtr incoming,
                                                  int priceToMatch)
{
    auto oppositeOrderSide = (incoming->orderSide() == OrderSide::Buy)
                                 ? OrderSide::Sell
                                 : OrderSide::Buy;

    return ordersDequeAtPrice(incoming, priceToMatch);
}

const std::expected<double, std::string> OrderBook::getBestPrice(
    OrderPtr orderToMatch)
{
    if (orderToMatch->orderSide() == OrderSide::Buy)
    {
        auto& sellPricesSet = sellPricesAtPriceLevel(orderToMatch);

        if (sellPricesSet.size() == 0)
        {
            return std::unexpected("No sell orders found for this ticker");
        }

        // find highest sell price at or below target price
        auto it = sellPricesSet.upper_bound(orderToMatch->price());
        if (it == sellPricesSet.begin())
        {
            return std::unexpected(
                "No matching sell orders lower than or equal to buy "
                "price");
        }
        --it;

        return *sellPricesSet.begin();
    }
    else
    {
        auto& buyPricesSet = buyPricesAtPriceLevel(orderToMatch);

        if (buyPricesSet.size() == 0)
        {
            return std::unexpected("No buy orders found for this ticker");
        }

        // find highest buy price at or below target price
        auto it = buyPricesSet.upper_bound(orderToMatch->price());
        if (it == buyPricesSet.begin())
        {
            return std::unexpected(
                "No matching buy orders lower than or equal to buy "
                "price");
        }
        --it;

        return *buyPricesSet.begin();
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
    auto& ordersAtPrice = ordersDequeAtPrice(orderToRemove);
    ordersAtPrice.pop_front();

    // only remove from prices set if it's the last order left at
    // that price
    if (ordersAtPrice.size() == 0)
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

void OrderBook::markOrderAsFulfilled(OrderPtr completedOrder)
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
