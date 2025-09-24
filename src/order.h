#ifndef ORDER_H
#define ORDER_H

#include <order_side.h>
#include <ticker.h>
#include <time_point.h>

#include <ctime>
#include <expected>
#include <memory>
#include <string>

namespace solstice
{

class Order
{
   public:
    static std::expected<std::shared_ptr<Order>, std::string> createOrder(
        Ticker tkr, double price, double qnty, OrderSide orderSide);

    std::string uid() const;
    Ticker tkr() const;
    double price() const;
    double qnty() const;
    double outstandingQnty() const;
    double outstandingQnty(double newQnty);
    OrderSide orderSide() const;
    TimePoint timeOrderPlaced() const;

    std::expected<TimePoint, std::string> timeOrderFulfilled() const;
    bool orderComplete() const;
    bool orderComplete(bool isFulfilled);

   private:
    Order(std::string uid, Ticker tkr, double price, double qnty,
          OrderSide orderSide, TimePoint timeOrderPlaced);

    std::string d_uid;
    Ticker d_tkr;
    double d_price;
    double d_qnty;
    double d_outstandingQnty;
    OrderSide d_orderSide;
    TimePoint d_timeOrderPlaced;

    TimePoint d_timeOrderFulfilled;
    bool d_orderComplete;
};

std::ostream& operator<<(std::ostream& os, const Order& order);

}  // namespace solstice

#endif
