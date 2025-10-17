#ifndef MATCH_H
#define MATCH_H

#include <order.h>
#include <order_book.h>

#include <expected>
#include <memory>
#include <string>

namespace solstice::matching
{

using OrderPtr = std::shared_ptr<Order>;

class Orchestrator;

class Matcher
{
    friend class Orchestrator;

   public:
    Matcher(std::shared_ptr<OrderBook> orderBook);

    std::shared_ptr<OrderBook> d_orderBook;

    std::expected<std::string, std::string> matchOrder(OrderPtr order,
                                                       double orderMatchingPrice = -1) const;

   private:
    const bool withinPriceRange(double price, OrderPtr order) const;

    const double getDealPrice(OrderPtr firstOrder, OrderPtr secondOrder) const;

    const std::string matchSuccessOutput(OrderPtr incomingOrder, OrderPtr matchedOrder,
                                         double matchedPrice) const;
};
}  // namespace solstice::matching

#endif  // MATCH_H
