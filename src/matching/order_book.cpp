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

namespace solstice::matching
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

    return (order->orderSide() == OrderSide::Bid)
               ? book.bids.at(order->price())
               : book.asks.at(order->price());
}

std::deque<OrderPtr>& OrderBook::getOrdersDequeAtPrice(OrderPtr order,
                                                       int priceToMatch)
{
    auto& book = d_activeOrders.at(order->tkr());

    return (order->orderSide() == OrderSide::Bid)
               ? book.bids.at(priceToMatch)
               : book.asks.at(priceToMatch);
}

std::deque<OrderPtr>& OrderBook::ordersDequeAtPrice(OrderPtr order)
{
    auto& book = d_activeOrders[order->tkr()];

    return (order->orderSide() == OrderSide::Bid)
               ? book.bids[order->price()]
               : book.asks[order->price()];
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

    if (order->orderSide() == OrderSide::Bid)
    {
        auto priceIt = book.asks.find(priceToUse);
        if (priceIt == book.asks.end() || priceIt->second.empty())
        {
            return std::unexpected(std::format(
                "No prices at ticker {} on opposite order side\n",
                order->tkrString()));
        }
        return std::ref(priceIt->second);
    }
    else
    {
        auto priceIt = book.bids.find(priceToUse);
        if (priceIt == book.bids.end() || priceIt->second.empty())
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

    return (order->orderSide() == OrderSide::Bid) ? book.bids : book.asks;
}

std::map<double, std::deque<OrderPtr>>&
OrderBook::oppositeOrderSidePriceLevelMap(OrderPtr order)
{
    auto& book = d_activeOrders.at(order->tkr());

    return (order->orderSide() == OrderSide::Bid) ? book.asks : book.bids;
}

std::expected<std::reference_wrapper<BidPricesAtPriceLevel>, std::string>
OrderBook::getBidPricesAtPriceLevel(OrderPtr order)
{
    auto it = d_activeOrders.find(order->tkr());
    if (it == d_activeOrders.end())
    {
        return std::unexpected(
            std::format("No bid prices available for ticker {}\n",
                        order->tkrString()));
    }

    return std::ref(it->second.bidPrices);
}

std::set<double, std::greater<double>>& OrderBook::bidPricesAtPriceLevel(
    OrderPtr order)
{
    auto& bidsSet = d_activeOrders[order->tkr()].bidPrices;

    return bidsSet;
}

std::expected<std::reference_wrapper<askPricesAtPriceLevel>, std::string>
OrderBook::getaskPricesAtPriceLevel(OrderPtr order)
{
    auto it = d_activeOrders.find(order->tkr());
    if (it == d_activeOrders.end())
    {
        return std::unexpected(
            std::format("No ask prices available for ticker {}\n",
                        order->tkrString()));
    }

    return std::ref(it->second.askPrices);
}

std::set<double, std::less<double>>& OrderBook::askPricesAtPriceLevel(
    OrderPtr order)
{
    auto& asksSet = d_activeOrders[order->tkr()].askPrices;

    return asksSet;
}

const std::expected<double, std::string> OrderBook::getBestPrice(
    OrderPtr orderToMatch)
{
    if (orderToMatch->orderSide() == OrderSide::Bid)
    {
        auto askPricesSet = getaskPricesAtPriceLevel(orderToMatch);
        if (!askPricesSet)
        {
            return std::unexpected(askPricesSet.error());
        }

        auto& askPrices = askPricesSet->get();

        if (askPrices.size() == 0)
        {
            return std::unexpected(
                std::format("No ask orders found for ticker {}\n",
                            orderToMatch->tkrString()));
        }

        double lowestaskPrice = *askPrices.begin();
        if (lowestaskPrice > orderToMatch->price())
        {
            return std::unexpected(
                "No matching ask orders lower than or equal to bid "
                "price\n");
        }

        return lowestaskPrice;
    }
    else
    {
        auto bidPricesSet = getBidPricesAtPriceLevel(orderToMatch);
        if (!bidPricesSet)
        {
            return std::unexpected(bidPricesSet.error());
        }

        auto& bidPrices = bidPricesSet->get();

        if (bidPrices.size() == 0)
        {
            return std::unexpected(
                std::format("No bid orders found for ticker {}\n",
                            orderToMatch->tkrString()));
        }

        // find highest bid price at or below target price
        double highestBidPrice = *bidPrices.begin();
        if (highestBidPrice < orderToMatch->price())
        {
            return std::unexpected(
                "No matching bid orders lower than or equal to bid "
                "price\n");
        }

        return highestBidPrice;
    }
}

void OrderBook::addOrderToBook(OrderPtr order)
{
    // add price to price lookup map
    if (order->orderSide() == OrderSide::Bid)
    {
        bidPricesAtPriceLevel(order).insert(order->price());
    }
    else
    {
        askPricesAtPriceLevel(order).insert(order->price());
    }

    // add order to map of active orders safely
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
        if (completedOrder->orderSide() == OrderSide::Bid)
        {
            bidPricesAtPriceLevel(completedOrder)
                .erase(completedOrder->price());
        }
        else
        {
            askPricesAtPriceLevel(completedOrder)
                .erase(completedOrder->price());
        }
    }
}

void OrderBook::initialiseActiveOrders()
{
    for (Ticker tkr : ALL_TICKERS)
    {
        d_activeOrders[tkr];
    }
}

}  // namespace solstice::matching
