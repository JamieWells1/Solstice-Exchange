#include <matcher.h>
#include <order.h>
#include <order_book.h>
#include <order_side.h>
#include <ticker.h>

#include <expected>
#include <iostream>
#include <memory>
#include <string>

namespace solstice::matching
{

const bool Matcher::withinPriceRange(double price, OrderPtr order) const
{
    if (order->orderSide() == OrderSide::Bid)
    {
        return price > order->price() ? false : true;
    }
    else
    {
        return price < order->price() ? false : true;
    }
}

const double Matcher::getDealPrice(OrderPtr firstOrder,
                                   OrderPtr secondOrder) const
{
    if (firstOrder->price() == secondOrder->price())
    {
        // doesn't matter which price is returned as they are equal
        return firstOrder->price();
    }

    OrderPtr bid = firstOrder->orderSide() == OrderSide::Bid ? firstOrder
                                                             : secondOrder;
    OrderPtr ask = firstOrder == bid ? secondOrder : firstOrder;

    // always return price of resting order
    if (ask->timeOrderPlaced() > bid->timeOrderPlaced())
    {
        return ask->price();
    }

    if (bid->timeOrderPlaced() > ask->timeOrderPlaced())
    {
        return bid->price();
    }

    // tiebreaker - uid as this is based on position in book
    return bid->uid() > ask->uid() ? ask->price() : bid->price();
}

const std::string Matcher::matchSuccessOutput(OrderPtr incomingOrder,
                                              OrderPtr matchedOrder,
                                              double matchedPrice) const
{
    const double dealPrice = getDealPrice(incomingOrder, matchedOrder);

    std::ostringstream oss;
    oss << incomingOrder->uid() << ", " << matchedOrder->uid() << "]\n"
        << "Incoming: " << incomingOrder->orderSideString() << " "
        << incomingOrder->tkrString() << " @ " << incomingOrder->price()
        << ", total: " << incomingOrder->qnty()
        << ", remaining: " << incomingOrder->outstandingQnty();

    if (incomingOrder->outstandingQnty() == 0)
    {
        oss << " [FULFILLED]";
    }
    oss << "\n";

    oss << "Matched: " << matchedOrder->orderSideString() << " "
        << matchedOrder->tkrString() << " @ " << matchedOrder->price()
        << ", total: " << matchedOrder->qnty()
        << ", remaining: " << matchedOrder->outstandingQnty();

    if (matchedOrder->outstandingQnty() == 0)
    {
        oss << " [FULFILLED]";
    }
    oss << "\n\n";

    return oss.str();
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
        d_orderBook->oppositeOrderSidePriceLevelMap(incomingOrder);

    auto it = priceLevelMap.find(bestPrice);
    if (it == priceLevelMap.end())
    {
        return std::unexpected(
            "Insufficient orders available to fulfill incoming order\n");
    }

    auto ordersResult =
        d_orderBook->getPriceLevelOppositeOrders(incomingOrder, bestPrice);

    if (!ordersResult)
    {
        return std::unexpected(ordersResult.error());
    }

    std::deque<OrderPtr>& ordersAtBestPrice = *ordersResult;
    OrderPtr bestOrder = ordersAtBestPrice.at(0);

    if (ordersAtBestPrice.size() == 1 &&
        bestOrder->uid() == incomingOrder->uid())
    {
        return std::unexpected("Orders cannot match themselves\n");
    }

    if (bestOrder->qnty() < incomingOrder->qnty())
    {
        double transactionQnty = bestOrder->qnty();
        incomingOrder->outstandingQnty(incomingOrder->qnty() -
                                       transactionQnty);
        bestOrder->outstandingQnty(0);

        d_orderBook->markOrderAsFulfilled(bestOrder);

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
    else if (bestOrder->qnty() == incomingOrder->qnty())
    {
        bestOrder->outstandingQnty(0);
        incomingOrder->outstandingQnty(0);

        const std::string& finalMatchResult =
            matchSuccessOutput(incomingOrder, bestOrder, bestPrice);

        d_orderBook->markOrderAsFulfilled(bestOrder);
        d_orderBook->markOrderAsFulfilled(incomingOrder);

        return finalMatchResult;
    }
    else
    // best order has a greater quantity than incoming
    // order
    {
        double transactionQnty = incomingOrder->qnty();

        bestOrder->outstandingQnty(bestOrder->qnty() - transactionQnty);
        incomingOrder->outstandingQnty(0);

        const std::string& finalMatchResult =
            matchSuccessOutput(incomingOrder, bestOrder, bestPrice);

        d_orderBook->markOrderAsFulfilled(incomingOrder);

        return finalMatchResult;
    }
}

Matcher::Matcher(std::shared_ptr<OrderBook> orderBook)
    : d_orderBook(orderBook)
{
}

}  // namespace solstice::matching
