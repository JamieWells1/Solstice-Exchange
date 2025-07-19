#include <order.h>

// /utils
#include <getRandom.h>

Order::Order(Ticker tkr, double price, double qnty, bool isBuy)
        : d_tkr(tkr), d_price(price), d_qnty(qnty), d_isBuy(isBuy) {
            d_uid = getRandomUid();
        };

int Order::uid() const {
    return d_uid;
}

Ticker Order::tkr() const {
    return d_tkr;
}

double Order::price() const {
    return d_price;
}

double Order::qnty() const {
    return d_qnty;
}

bool Order::isBuy() const {
    return d_isBuy;
}
