#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <order.h>

#include <string>

namespace solstice {

class Processor {
   public:
    static const std::string start();

   private:
    std::shared_ptr<Order> generateOrder() const;

    Ticker getTicker() const;
    double getPrice() const;
    double getQnty() const;
    bool getIsBuy() const;
};
}  // namespace solstice

#endif  // ORDERBOOK_H
