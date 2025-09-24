#include <matcher.h>
#include <order.h>

#include <expected>
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

std::expected<void, std::string> Matcher::matchOrder(
    OrderPtr incomingOrder, double orderMatchingPrice) const
{
    double bestPrice = orderMatchingPrice;
    if (orderMatchingPrice == -1)
    {
        auto bestPriceAvailable = d_orderBook->getBestPrice(incomingOrder);
        if (!bestPriceAvailable)
        {
            return std::unexpected(bestPriceAvailable.error());
        }

        bestPrice = *bestPriceAvailable;
    }
    // first get lowest buy or highest sell for ticker

    // second get all orders at that price in FIFO order
    std::deque<OrderPtr>& ordersAtBestPrice =
        d_orderBook->getOrdersAtPrice(incomingOrder, bestPrice);

    if (ordersAtBestPrice.empty())
    {
        std::map<double, std::deque<OrderPtr>>& priceLevelMap =
            d_orderBook->priceLevelMap(incomingOrder);

        auto it = priceLevelMap.find(bestPrice);
        if (it != priceLevelMap.end() &&
            std::next(it) != priceLevelMap.end())
        {
            auto nextIt = std::next(it);
            const double nextBestPrice = nextIt->first;

            if (!withinPriceRange(nextBestPrice, incomingOrder))
            {
                return std::unexpected(
                    "No matches found due to all other orders being "
                    "out "
                    "of "
                    "pricing range");
            }

            ordersAtBestPrice = d_orderBook->getOrdersAtPrice(
                incomingOrder, nextBestPrice);

            while (!ordersAtBestPrice.empty())
            {
                if (ordersAtBestPrice.at(0)->qnty() <
                    incomingOrder->qnty())
                {
                    d_orderBook->markOrderAsFulfilled(
                        ordersAtBestPrice.at(0));

                    double oldOrderQnty = incomingOrder->qnty();
                    double newOutstandingQnty =
                        incomingOrder->outstandingQnty(
                            oldOrderQnty -
                            ordersAtBestPrice.at(0)->qnty());

                    ordersAtBestPrice.pop_front();
                    return matchOrder(incomingOrder, nextBestPrice);
                }
                else if (ordersAtBestPrice.at(0)->qnty() ==
                         incomingOrder->qnty())
                {
                    d_orderBook->markOrderAsFulfilled(
                        ordersAtBestPrice.at(0));
                    d_orderBook->markOrderAsFulfilled(incomingOrder);

                    ordersAtBestPrice.pop_front();
                    return {};
                }
                else
                // best order has a greater quantity than incoming
                // order
                {
                    d_orderBook->markOrderAsFulfilled(incomingOrder);

                    double oldOrderQnty = ordersAtBestPrice.at(0)->qnty();
                    double newOutstandingQnty =
                        ordersAtBestPrice.at(0)->outstandingQnty(
                            oldOrderQnty - incomingOrder->qnty());

                    ordersAtBestPrice.pop_front();
                    return {};
                }
            }
        }
        else
        {
            return std::unexpected(
                "No matches found due to insufficient orders");
        }
    }

    return {};
}

Matcher::Matcher(std::shared_ptr<OrderBook> orderbook)
    : d_orderBook(orderbook)
{
}

}  // namespace solstice
