#include <matcher.h>

namespace solstice
{

std::expected<void, std::string> Matcher::processOrder(OrderPtr order)
{
    auto matchedOrders = d_orderBook.getMatchingOrders(order);

    return {};
}

Matcher::Matcher(OrderBook orderbook) : d_orderBook(orderbook) {}
}  // namespace solstice
