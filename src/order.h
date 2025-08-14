#ifndef ORDER_H
#define ORDER_H

// /enums
#include <ticker.h>
#include <timepoint.h>

#include <ctime>
#include <expected>
#include <memory>
#include <string>

namespace solstice {

class Order {
   public:
    static std::expected<std::shared_ptr<Order>, std::string> createOrder(
        Ticker tkr, double price, double qnty, bool isBuy);

    std::string uid() const;
    Ticker tkr() const;
    double price() const;
    double qnty() const;
    bool isBuy() const;
    TimePoint timeOrderPlaced() const;

    std::expected<TimePoint, std::string> timeOrderFulfilled() const;
    bool orderComplete() const;

   private:
    Order(std::string uid, Ticker tkr, double price, double qnty,
          bool isBuy, TimePoint timeOrderPlaced);

    std::string d_uid;
    Ticker d_tkr;
    double d_price;
    double d_qnty;
    bool d_isBuy;
    TimePoint d_timeOrderPlaced;

    TimePoint d_timeOrderFulfilled;
    bool d_orderComplete;
};
}  // namespace solstice

#endif
