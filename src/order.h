#ifndef ORDER_H
#define ORDER_H

// /enums
#include <ticker.h>

class Order {
    int d_uid;
    Ticker d_tkr;
    double d_price;
    double d_qnty;
    bool d_isBuy;

   public:
    Order(Ticker tkr, double price, double qnty, bool isBuy);

    int uid() const;
    Ticker tkr() const;
    double price() const;
    double qnty() const;
    bool isBuy() const;
};

#endif
