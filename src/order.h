#ifndef ORDER_H
#define ORDER_H

// /enums
#include <ticker.h>

#include <ctime>
#include <expected>
#include <string>

class Order {
   public:
    Order(Ticker tkr, double price, double qnty, bool isBuy,
          time_t timeOrderPlaced);

    Ticker tkr() const;
    double price() const;
    double qnty() const;
    bool isBuy() const;
    time_t timeOrderPlaced() const;

    int uid() const;
    std::expected<time_t, std::string> timeOrderFulfilled() const;
    bool orderComplete() const;

   private:
    Ticker d_tkr;
    double d_price;
    double d_qnty;
    bool d_isBuy;
    time_t d_timeOrderPlaced;

    int d_uid;
    time_t d_timeOrderFulfilled;
    bool d_orderComplete;

    bool validatePrice(const double price);
};

#endif
