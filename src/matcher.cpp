#include <matcher.h>
#include <order.h>

#include <expected>
#include <iostream>
#include <memory>
#include <string>

#include "order_book.h"

namespace solstice
{

const bool Matcher::withinPriceRange(double price, OrderPtr order) const
{
    if (order->orderSide() == OrderSide::Buy)
    {
        return price > order->price() ? false : true;
    }
    else
    {
        return price < order->price() ? false : true;
    }
}

bool Matcher::matchOrder(OrderPtr incomingOrder,
                         double orderMatchingPrice) const
{
    double bestPrice = orderMatchingPrice;

    if (orderMatchingPrice == -1)
    {
        auto bestPriceAvailable = d_orderBook->getBestPrice(incomingOrder);
        if (!bestPriceAvailable)
        {
            std::cout << bestPriceAvailable.error() << std::endl;
            return false;
        }

        bestPrice = *bestPriceAvailable;
    }

    std::map<double, std::deque<OrderPtr>>& priceLevelMap =
        d_orderBook->priceLevelMap(incomingOrder);

    auto it = priceLevelMap.find(bestPrice);
    if (it == priceLevelMap.end() || std::next(it) == priceLevelMap.end())
    {
        std::cout
            << "Insufficient orders available to fulfill incoming order"
            << std::endl;
        return false;
    }

    std::deque<OrderPtr>& ordersAtBestPrice =
        d_orderBook->getOrdersAtPrice(incomingOrder, bestPrice);

    if (ordersAtBestPrice.empty())
    {
        std::cout << "No orders available to match" << std::endl;
        return false;
    }

    if (ordersAtBestPrice.at(0)->qnty() < incomingOrder->qnty())
    {
        double oldOrderQnty = incomingOrder->qnty();
        double newOutstandingQnty = incomingOrder->outstandingQnty(
            oldOrderQnty - ordersAtBestPrice.at(0)->qnty());

        ordersAtBestPrice.pop_front();

        if (!ordersAtBestPrice.empty())
        {
            return matchOrder(incomingOrder, bestPrice);
        }
        else
        {
            auto nextIt = std::next(it);
            const double nextBestPrice = nextIt->first;

            if (!withinPriceRange(nextBestPrice, incomingOrder))
            {
                std::cout << "All other orders out of price range"
                          << std::endl;
                return false;
            }

            return matchOrder(incomingOrder, nextBestPrice);
        }
    }
    else if (ordersAtBestPrice.at(0)->qnty() == incomingOrder->qnty())
    {
        d_orderBook->markOrderAsFulfilled(ordersAtBestPrice.at(0));

        ordersAtBestPrice.pop_front();
        return true;
    }
    else
    // best order has a greater quantity than incoming
    // order
    {
        double oldOrderQnty = ordersAtBestPrice.at(0)->qnty();
        double newOutstandingQnty =
            ordersAtBestPrice.at(0)->outstandingQnty(
                oldOrderQnty - incomingOrder->qnty());

        ordersAtBestPrice.pop_front();
        return true;
    }
}

Matcher::Matcher(std::shared_ptr<OrderBook> orderbook)
    : d_orderBook(orderbook)
{
}

}  // namespace solstice
