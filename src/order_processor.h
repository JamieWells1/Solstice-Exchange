#ifndef ORDER_PROCESSOR_H
#define ORDER_PROCESSOR_H

#include <config.h>
#include <matcher.h>
#include <order.h>
#include <order_book.h>

#include <memory>

namespace solstice
{

class OrderProcessor
{
   public:
    static std::expected<void, std::string> start();

    Config d_config;
    std::shared_ptr<OrderBook> d_orderBook;
    Matcher d_matcher;

    bool processOrder(OrderPtr order);

   private:
    OrderProcessor(Config config, std::shared_ptr<OrderBook> orderBook,
                   Matcher matcher);

    std::expected<OrderPtr, std::string> generateOrder(int ordersGenerated);

    std::expected<std::pair<int, int>, std::string> produceOrders();
};

std::ostream& operator<<(std::ostream& os, ActiveOrders activeOrders);

}  // namespace solstice

#endif  // ORDER_H
