#include <matcher.h>
#include <order.h>

#include <deque>
#include <expected>
#include <memory>
#include <string>

#include "order_book.h"

namespace solstice
{

std::expected<void, std::string> Matcher::matchOrder(
    OrderPtr incomingOrder, std::optional<int> price)
{
    // for a given order, get all orders with same ticker, price, and
    // opposing order sides
    auto matchedOrders = d_orderBook->getMatchingOrders(incomingOrder);

    OrderPtr orderToMatch = matchedOrders.at(0);

    // TODO: Problem: currently, we are only looking at orders that have an
    // equal price to our target order. We want to look at existing orders
    // that offer the best deal -- e.g. buy @ 10 , but there's a sell @ 9,
    // so we want to take that

    // Solution: use a separate data structure to store the IDs 

    if (incomingOrder->orderSide() == OrderSide::Buy)
    {
        if (!orderToMatch)
        {
            auto retryWithHigherPrice =
                matchOrder(incomingOrder, incomingOrder->price() + 1);
        }

        if (orderToMatch->qnty() < incomingOrder->qnty())
        {
            d_orderBook->markOrderAsComplete(incomingOrder);
        }
    }

    return {};
}

Matcher::Matcher(std::shared_ptr<OrderBook> orderbook)
    : d_orderBook(orderbook)
{
}

}  // namespace solstice
