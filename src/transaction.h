#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <get_random.h>
#include <order.h>
#include <ticker.h>
#include <time_point.h>

#include <memory>
#include <ostream>
#include <string>

namespace solstice::matching
{
using OrderPtr = std::shared_ptr<Order>;

class OrderBook;

struct Transaction
{
    friend class OrderBook;

   public:
    std::string d_uid;
    int d_buyOrderUid;
    int d_sellOrderUid;
    Ticker d_tkr;
    double d_price;
    double d_qnty;
    TimePoint d_timeExecuted;

   private:
    Transaction(OrderPtr buyOrder, OrderPtr sellOrder, double price,
                double qnty);
};

std::ostream& operator<<(std::ostream os, const Transaction& transaction);

}  // namespace solstice::matching

#endif  // TRANSACTION_H
