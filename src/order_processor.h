#ifndef ORDER_PROCESSOR_H
#define ORDER_PROCESSOR_H

#include <config.h>
#include <order.h>

namespace solstice {

class OrderProcessor {
   public:
    static void start();

    Config config;

   private:
    std::expected<std::shared_ptr<Order>, std::string> generateOrder()
        const;

    void produceOrders();

    Ticker getTicker() const;
    double getPrice() const;
    double getQnty() const;
    bool getIsBuy() const;
};
}  // namespace solstice

#endif  // ORDER_H
