#include <matcher.h>
#include <order.h>
#include <order_book.h>
#include <transaction.h>
#include <truncate.h>

#include <cstddef>
#include <deque>
#include <memory>
#include <string>

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

std::expected<std::reference_wrapper<std::deque<OrderPtr>>, std::string>
OrderBook::getPriceLevelOppositeOrders(OrderPtr order, double priceToUse)
{
    auto it = d_activeOrders.find(order->tkr());
    if (it == d_activeOrders.end())
    {
        return std::unexpected(
            std::format("No orders at ticker {} on opposite order side\n",
                        order->tkrString()));
    }

    ActiveOrders& book = d_activeOrders.at(order->tkr());

    if (order->orderSide() == OrderSide::Buy)
    {
        auto priceIt = book.sellOrders.find(priceToUse);
        if (priceIt == book.sellOrders.end() || priceIt->second.empty())
        {
            return std::unexpected(std::format(
                "No prices at ticker {} on opposite order side\n",
                order->tkrString()));
        }
        return std::ref(priceIt->second);
    }
    else
    {
        auto priceIt = book.buyOrders.find(priceToUse);
        if (priceIt == book.buyOrders.end() || priceIt->second.empty())
        {
            return std::unexpected(std::format(
                "No prices at ticker {} on opposite order side\n",
                order->tkrString()));
        }
        return std::ref(priceIt->second);
    }
}

std::map<double, std::deque<OrderPtr>>&
OrderBook::sameOrderSidePriceLevelMap(OrderPtr order)
{
    auto& book = d_activeOrders.at(order->tkr());

    return (order->orderSide() == OrderSide::Buy) ? book.buyOrders
                                                  : book.sellOrders;
}

std::map<double, std::deque<OrderPtr>>&
OrderBook::oppositeOrderSidePriceLevelMap(OrderPtr order)
{
    auto& book = d_activeOrders.at(order->tkr());

    return (order->orderSide() == OrderSide::Buy) ? book.sellOrders
                                                  : book.buyOrders;
}

std::expected<std::reference_wrapper<BuyPricesAtPriceLevel>, std::string>
OrderBook::getBuyPricesAtPriceLevel(OrderPtr order)
{
    auto it = d_activeOrders.find(order->tkr());
    if (it == d_activeOrders.end())
    {
        return std::unexpected(
            std::format("No buy prices available for ticker {}\n",
                        order->tkrString()));
    }

    return std::ref(it->second.buyPrices);
}

std::set<double, std::greater<double>>& OrderBook::buyPricesAtPriceLevel(
    OrderPtr order)
{
    auto& buyOrdersSet = d_activeOrders[order->tkr()].buyPrices;

    return buyOrdersSet;
}

std::expected<std::reference_wrapper<SellPricesAtPriceLevel>, std::string>
OrderBook::getSellPricesAtPriceLevel(OrderPtr order)
{
    auto it = d_activeOrders.find(order->tkr());
    if (it == d_activeOrders.end())
    {
        return std::unexpected(
            std::format("No sell prices available for ticker {}\n",
                        order->tkrString()));
    }

    return std::ref(it->second.sellPrices);
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
        auto sellPricesSet = getSellPricesAtPriceLevel(orderToMatch);
        if (!sellPricesSet)
        {
            return std::unexpected(sellPricesSet.error());
        }

        auto& sellPrices = sellPricesSet->get();

        if (sellPrices.size() == 0)
        {
            return std::unexpected(
                std::format("No sell orders found for ticker {}\n",
                            orderToMatch->tkrString()));
        }

        double lowestSellPrice = *sellPrices.begin();
        if (lowestSellPrice > orderToMatch->price())
        {
            return std::unexpected(
                "No matching sell orders lower than or equal to buy "
                "price\n");
        }

        return lowestSellPrice;
    }
    else
    {
        auto buyPricesSet = getBuyPricesAtPriceLevel(orderToMatch);
        if (!buyPricesSet)
        {
            return std::unexpected(buyPricesSet.error());
        }

        auto& buyPrices = buyPricesSet->get();

        if (buyPrices.size() == 0)
        {
            return std::unexpected(
                std::format("No buy orders found for ticker {}\n",
                            orderToMatch->tkrString()));
        }

        // find highest buy price at or below target price
        double highestBuyPrice = *buyPrices.begin();
        if (highestBuyPrice < orderToMatch->price())
        {
            return std::unexpected(
                "No matching buy orders lower than or equal to buy "
                "price\n");
        }

        return highestBuyPrice;
    }
}

void OrderBook::addOrderToBook(OrderPtr order)
{
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
    auto& priceDeque = ordersDequeAtPrice(orderToRemove);
    for (size_t i = 0; i < priceDeque.size(); i++)
    {
        if (orderToRemove->uid() == priceDeque[i]->uid())
        {
            priceDeque.erase(priceDeque.begin() + i);
            break;
        }
    }
}

void OrderBook::markOrderAsFulfilled(OrderPtr completedOrder)
{
    // remove references
    completedOrder->orderComplete(true);
    removeOrderFromBook(completedOrder);

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
