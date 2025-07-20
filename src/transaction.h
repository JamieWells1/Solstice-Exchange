#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <getrandom.h>
#include <order.h>
#include <ticker.h>
#include <timepoint.h>

#include <memory>
#include <ostream>
#include <string>

class OrderBook;

struct Transaction {
    friend class OrderBook;

   public:
    std::string d_uid;
    std::string d_buyOrderUid;
    std::string d_sellOrderUid;
    Ticker d_tkr;
    double d_price;
    double d_qnty;
    TimePoint d_timeExecuted;

   private:
    // TODO 1: implement constructor where values are moved from two Order
    // objects into a Transaction construction
    Transaction(std::shared_ptr<Order> buyOrder,
                std::shared_ptr<Order> sellOrder, double price,
                double qnty);
};

std::ostream& operator<<(std::ostream os, const Transaction& transaction);

#endif  // TRANSACTION_H
