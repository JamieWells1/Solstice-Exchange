#include <matcher.h>
#include <order.h>

#include <expected>
#include <memory>
#include <string>

#include "order_book.h"

namespace solstice
{

std::expected<void, std::string> Matcher::matchOrder(
    OrderPtr incomingOrder, double orderMatchingPrice)
{
    const auto bestPrice = d_orderBook->getBestPrice(incomingOrder);
    if (!bestPrice)
    {
        return std::unexpected(bestPrice.error());
    }

    std::deque<OrderPtr>& ordersAtBestPrice =
        d_orderBook->getOrdersAtPrice(incomingOrder, *bestPrice);

    bool incomingOrderFulfilled = false;

    while (!incomingOrderFulfilled)
    {
        if (ordersAtBestPrice.size() == 0)
        {
            return std::unexpected(std::format(
                "No orders at price = ", *bestPrice, ", aborting"));
        }

        const OrderPtr bestOrder = ordersAtBestPrice.at(0);

        // in the case where the best order's quantity isn't
        // enough to fulfill incoming order
        if (bestOrder->qnty() < incomingOrder->qnty())
        {
            d_orderBook->markOrderAsFulfilled(bestOrder);

            double oldOrderQnty = incomingOrder->qnty();
            double newOutstandingQnty = incomingOrder->outstandingQnty(
                oldOrderQnty - bestOrder->qnty());
        }
        else if (bestOrder->qnty() == incomingOrder->qnty())
        {
            d_orderBook->markOrderAsFulfilled(bestOrder);
            d_orderBook->markOrderAsFulfilled(incomingOrder);

            incomingOrderFulfilled = true;
        }
        else
        // best order has a greater quantity than incoming order
        {
            d_orderBook->markOrderAsFulfilled(incomingOrder);

            double oldOrderQnty = bestOrder->qnty();
            double newOutstandingQnty = bestOrder->outstandingQnty(
                oldOrderQnty - incomingOrder->qnty());

            incomingOrderFulfilled = true;
        }
    }
}

Matcher::Matcher(std::shared_ptr<OrderBook> orderbook)
    : d_orderBook(orderbook)
{
}

}  // namespace solstice
