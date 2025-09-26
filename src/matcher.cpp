#include <matcher.h>
#include <order.h>

#include <expected>
#include <iostream>
#include <memory>
#include <string>

#include <order_book.h>
#include <order_side.h>
#include <ticker.h>

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

std::string orderSideToString(solstice::OrderSide side) {
    return side == solstice::OrderSide::Buy ? "Buy" : "Sell";
}
const std::string Matcher::matchSuccessOutput(OrderPtr matchedOrder) const
{
    std::string side = orderSideToString(matchedOrder->orderSide());

    std::cout << matchedOrder->orderSide();
    return "Order fulfilled successfully: " + side + matchedOrder->tkr();

    // print info about both orders depending on match status
}

std::expected<std::string, std::string> Matcher::matchOrder(
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

    std::map<double, std::deque<OrderPtr>>& priceLevelMap =
        d_orderBook->priceLevelMap(incomingOrder);

    auto it = priceLevelMap.find(bestPrice);
    if (it == priceLevelMap.end() || std::next(it) == priceLevelMap.end())
    {
        return std::unexpected(
            "Insufficient orders available to fulfill incoming order\n");
    }

    std::deque<OrderPtr>& ordersAtBestPrice =
        d_orderBook->getOrdersAtPrice(incomingOrder, bestPrice);

    if (ordersAtBestPrice.empty())
    {
        return std::unexpected("No orders available to match\n");
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
                return std::unexpected(
                    "All other orders out of price range\n");
            }

            return matchOrder(incomingOrder, nextBestPrice);
        }
    }
    else if (ordersAtBestPrice.at(0)->qnty() == incomingOrder->qnty())
    {
        auto matchedOrder = ordersAtBestPrice.at(0);
        d_orderBook->markOrderAsFulfilled(matchedOrder);

        ordersAtBestPrice.pop_front();
        return matchSuccessOutput(matchedOrder);
    }
    else
    // best order has a greater quantity than incoming
    // order
    {
        auto matchedOrder = ordersAtBestPrice.at(0);

        double oldOrderQnty = matchedOrder->qnty();
        double newOutstandingQnty = matchedOrder->outstandingQnty(
            oldOrderQnty - incomingOrder->qnty());

        ordersAtBestPrice.pop_front();
        return matchSuccessOutput(matchedOrder);
    }
}

Matcher::Matcher(std::shared_ptr<OrderBook> orderbook)
    : d_orderBook(orderbook)
{
}

}  // namespace solstice
