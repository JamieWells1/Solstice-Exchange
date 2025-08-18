#ifndef ORDER_PROCESSOR_H
#define ORDER_PROCESSOR_H

#include <config.h>
#include <order.h>

namespace solstice {

class OrderProcessor {
   public:
    static void start(Config config);

   private:
    std::shared_ptr<Order> generateOrder() const;

    // generates orders and adds them to the orderbook
    void produceOrders();

    Ticker getTicker() const;
    double getPrice() const;
    double getQnty() const;
    bool getIsBuy() const;
};
}  // namespace solstice

#endif  // ORDER_H
