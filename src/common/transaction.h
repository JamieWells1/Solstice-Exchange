#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <get_random.h>
#include <order.h>
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
    const std::string& uid() const;
    int bidUid() const;
    int askUid() const;
    const Underlying& underlying() const;
    double price() const;
    double qnty() const;
    const TimePoint& timeExecuted() const;

   private:
    Transaction(OrderPtr bid, OrderPtr ask, double price, double qnty);

    std::string d_uid;
    int d_bidUid;
    int d_askUid;
    Underlying d_underlying;
    double d_price;
    double d_qnty;
    TimePoint d_timeExecuted;
};

std::ostream& operator<<(std::ostream os, const Transaction& transaction);

}  // namespace solstice::matching

#endif  // TRANSACTION_H
