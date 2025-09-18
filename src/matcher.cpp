#include <deque>
#include <memory>
#include <string>
#include <expected>
#include "order_book.h"

#include <matcher.h>
#include <order.h>

namespace solstice {

std::expected<void, std::string> Matcher::processOrder(OrderPtr order) {

    auto matchedOrders = d_orderBook->getMatchingOrders(order);

    // TODO

    return {};
}

Matcher::Matcher(std::shared_ptr<OrderBook> orderbook) 
    : d_orderBook(orderbook) {}

} // namespace solstice
