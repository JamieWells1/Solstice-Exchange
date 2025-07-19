#ifndef ORDER_H
#define ORDER_H

// /enums
#include "ticker.h"

// /utils
#include "getRandom.h"

struct Order {
    int d_uid;
    Ticker d_tkr;
    double d_price;
    double d_qnty;
    bool d_isBuy;

    Order(Ticker tkr, double price, double qnty, bool isBuy)
        : d_tkr(tkr), d_price(price), d_qnty(qnty), d_isBuy(isBuy) {
            d_uid = getRandomUid();
        }
};

#endif
