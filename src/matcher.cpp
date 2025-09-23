#include <deque>
#include <memory>
#include <string>
#include <expected>
#include "order_book.h"

#include <matcher.h>
#include <order.h>

namespace solstice {

std::expected<void, std::string> Matcher::matchOrder(OrderPtr order, std::optional<int> price) {
    
    // for a given order, get all orders with same ticker, price, and opposing order sides
    auto matchedOrders = d_orderBook->getMatchingOrders(order);

    OrderPtr orderToMatch = matchedOrders.at(0);
    if (!orderToMatch)
    {
        auto result = matchOrder(order, order->price()-1);
    }

    if (orderToMatch->qnty() < order->qnty())
    {
        d_orderBook->markOrderAsComplete(order);
    }

    return {};
}

Matcher::Matcher(std::shared_ptr<OrderBook> orderbook) 
    : d_orderBook(orderbook) {}

} // namespace solstice
