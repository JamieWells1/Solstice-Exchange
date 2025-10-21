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
    static std::expected<std::shared_ptr<Order>, std::string> createOrder(int uid,
                                                                          Underlying underlying,
                                                                          double price, double qnty,
                                                                          MarketSide marketSide);

    int uid() const;
    Underlying underlying() const;
    double price() const;
    double qnty() const;
    double outstandingQnty() const;
    MarketSide marketSide() const;
    std::string marketSideString() const;
    TimePoint timeOrderPlaced() const;
    std::expected<TimePoint, std::string> timeOrderFulfilled() const;
    bool orderComplete() const;

    double outstandingQnty(double newQnty);
    bool orderComplete(bool isFulfilled);

   private:
    Order(int uid, Underlying underlying, double price, double qnty, MarketSide marketSide,
          TimePoint timeOrderPlaced);

    int d_uid;
    Underlying d_underlying;
    double d_price;
    double d_qnty;
    double d_outstandingQnty;
    MarketSide d_marketSide;
    TimePoint d_timeOrderPlaced;
    TimePoint d_timeOrderFulfilled;
    bool d_orderComplete;
};

std::ostream& operator<<(std::ostream& os, const Order& order);

}  // namespace solstice

#endif
