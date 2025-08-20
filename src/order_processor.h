#ifndef ORDER_PROCESSOR_H
#define ORDER_PROCESSOR_H

#include <config.h>
#include <order.h>
#include <order_book.h>

namespace solstice
{

class OrderProcessor
{
   public:
    static std::expected<void, std::string> start();

    Config d_config;
    OrderBook d_orderBook;

   private:
    OrderProcessor(Config config, OrderBook orderBook);

    std::expected<std::shared_ptr<Order>, std::string> generateOrder();

    std::expected<void, std::string> produceOrders();
};
}  // namespace solstice

#endif  // ORDER_H
