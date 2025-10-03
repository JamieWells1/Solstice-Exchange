#include <matcher.h>
#include <order.h>
#include <order_book.h>
#include <order_side.h>
#include <ticker.h>

#include <expected>
#include <iostream>
#include <memory>
#include <string>

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

const double Matcher::getDealPrice(OrderPtr firstOrder,
                                   OrderPtr secondOrder) const
{
    if (firstOrder->price() == secondOrder->price())
    {
        // doesn't matter which price is returned as they are equal
        return firstOrder->price();
    }

    OrderPtr buyOrder = firstOrder->orderSide() == OrderSide::Buy
                            ? firstOrder
                            : secondOrder;
    OrderPtr sellOrder = firstOrder == buyOrder ? secondOrder : firstOrder;

    // always return price of resting order
    if (sellOrder->timeOrderPlaced() > buyOrder->timeOrderPlaced())
    {
        return sellOrder->price();
    }

    if (buyOrder->timeOrderPlaced() > sellOrder->timeOrderPlaced())
    {
        return buyOrder->price();
    }

    // tiebreaker - uid as this is based on position in book
    return buyOrder->uid() > sellOrder->uid() ? sellOrder->price()
                                              : buyOrder->price();
}

const std::string Matcher::matchSuccessOutput(OrderPtr incomingOrder,
                                              OrderPtr matchedOrder) const
{
    const double dealPrice = getDealPrice(incomingOrder, matchedOrder);

    std::ostringstream oss;
    oss << "MATCHED TRADE @ " << dealPrice << "\n"
        << "  Incoming: " << incomingOrder->orderSideString() << " "
        << incomingOrder->tkrString()
        << " | Remaining: " << incomingOrder->outstandingQnty() << "\n"
        << "  Matched : " << matchedOrder->orderSideString() << " "
        << matchedOrder->tkrString()
        << " | Remaining: " << matchedOrder->outstandingQnty()
        << std::endl;

    return oss.str();
}

std::expected<std::string, std::string> Matcher::matchOrder(
    OrderPtr incomingOrder, double orderMatchingPrice) const
{
    std::cout << "|| Matcher::matchOrder\n";

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
        d_orderBook->getOrdersDequeAtPrice(incomingOrder, bestPrice);

    if (ordersAtBestPrice.empty())
    {
        return std::unexpected("No orders available to match\n");
    }

    for (size_t i = 0; i < ordersAtBestPrice.size(); ++i)
    {
        if (!ordersAtBestPrice[i])
        {
            std::cout << "Null order at index " << i << "\n";
        }
        else
        {
            std::cout << "Order at index " << i
                      << " qnty=" << ordersAtBestPrice[i]->qnty() << "\n";
        }
    }

    OrderPtr bestOrder = ordersAtBestPrice.at(0);

    if (bestOrder->qnty() < incomingOrder->qnty())
    {
        double oldOrderQnty = incomingOrder->qnty();
        double newOutstandingQnty = incomingOrder->outstandingQnty(
            oldOrderQnty - bestOrder->qnty());

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
        d_orderBook->markOrderAsFulfilled(bestOrder);
        d_orderBook->markOrderAsFulfilled(incomingOrder);

        const std::string& finalMatchResult =
            matchSuccessOutput(incomingOrder, bestOrder);

        return finalMatchResult;
    }
    else
    // best order has a greater quantity than incoming
    // order
    {
        double oldOrderQnty = bestOrder->qnty();
        double newOutstandingQnty = bestOrder->outstandingQnty(
            oldOrderQnty - incomingOrder->qnty());

        d_orderBook->markOrderAsFulfilled(incomingOrder);

        const std::string& finalMatchResult =
            matchSuccessOutput(incomingOrder, bestOrder);

        return finalMatchResult;
    }
}

Matcher::Matcher(std::shared_ptr<OrderBook> orderbook)
    : d_orderBook(orderbook)
{
}

}  // namespace solstice
