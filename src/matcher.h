#ifndef MATCH_H
#define MATCH_H

#include <order.h>
#include <order_book.h>

#include <expected>
#include <memory>
#include <string>

namespace solstice
{

using OrderPtr = std::shared_ptr<Order>;

class OrderProcessor;

class Matcher
{
    friend class OrderProcessor;

   public:
    std::shared_ptr<OrderBook> d_orderBook;

    Matcher(std::shared_ptr<OrderBook> orderbook);

    std::expected<void, std::string> matchOrder(OrderPtr order, double orderMatchingPrice = -1);
};
}  // namespace solstice

#endif  // MATCH_H
