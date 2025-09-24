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
    

    return {};
}

Matcher::Matcher(std::shared_ptr<OrderBook> orderbook)
    : d_orderBook(orderbook)
{
}

}  // namespace solstice
