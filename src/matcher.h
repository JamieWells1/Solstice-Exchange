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
    OrderBook d_orderBook;

    Matcher(OrderBook orderbook);

    std::expected<void, std::string> processOrder(OrderPtr order);
};
}  // namespace solstice

#endif  // MATCH_H
