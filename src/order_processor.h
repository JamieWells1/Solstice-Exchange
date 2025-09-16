#ifndef ORDER_PROCESSOR_H
#define ORDER_PROCESSOR_H

#include <config.h>
#include <order.h>
#include <order_book.h>
#include <matcher.h>

#include <string.h>
#include <memory>

namespace solstice
{

class OrderProcessor
{
   public:
    static std::expected<void, std::string> start();

    Config d_config;
    OrderBook d_orderBook;
    Matcher d_matcher;

    std::expected<void, std::string> onNewOrder(OrderPtr order);

    std::expected<void, std::string> processOrder(OrderPtr order);

   private:
    OrderProcessor(Config config, OrderBook orderBook, Matcher matcher);

    std::expected<OrderPtr, std::string> generateOrder();

    std::expected<void, std::string> produceOrders();
};
}  // namespace solstice

#endif  // ORDER_H
