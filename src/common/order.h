#ifndef ORDER_H
#define ORDER_H

#include <asset_class.h>
#include <order_side.h>
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
    static std::expected<std::shared_ptr<Order>, std::string> createOrder(int uid, Underlying underlying,
                                                                          double price, double qnty,
                                                                          OrderSide orderSide);

    const int uid() const;
    const Underlying underlying() const;
    const double price() const;
    const double qnty() const;
    const double outstandingQnty() const;
    double outstandingQnty(double newQnty);
    const OrderSide orderSide() const;
    const std::string orderSideString() const;
    const TimePoint timeOrderPlaced() const;

    const std::expected<TimePoint, std::string> timeOrderFulfilled() const;
    const bool orderComplete() const;
    bool orderComplete(bool isFulfilled);

   private:
    Order(int uid, Underlying underlying, double price, double qnty, OrderSide orderSide,
          TimePoint timeOrderPlaced);

    int d_uid;
    Underlying d_underlying;
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
