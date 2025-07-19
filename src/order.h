#ifndef ORDER_H
#define ORDER_H

// /enums
#include <ticker.h>

#include <chrono>
#include <ctime>
#include <expected>
#include <string>

using TimePoint = std::chrono::system_clock::time_point;

class Order {
   public:
    Order(Ticker tkr, double price, double qnty, bool isBuy,
          TimePoint timeOrderPlaced);

    Ticker tkr() const;
    double price() const;
    double qnty() const;
    bool isBuy() const;
    TimePoint timeOrderPlaced() const;

    int uid() const;
    std::expected<TimePoint, std::string> timeOrderFulfilled() const;
    bool orderComplete() const;

   private:
    Ticker d_tkr;
    double d_price;
    double d_qnty;
    bool d_isBuy;
    TimePoint d_timeOrderPlaced;

    int d_uid;
    TimePoint d_timeOrderFulfilled;
    bool d_orderComplete;
};

#endif
