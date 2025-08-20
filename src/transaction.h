#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <get_random.h>
#include <order.h>
#include <ticker.h>
#include <time_point.h>

#include <memory>
#include <ostream>
#include <string>

namespace solstice
{

class OrderBook;

struct Transaction
{
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
    Transaction(std::shared_ptr<Order> buyOrder,
                std::shared_ptr<Order> sellOrder, double price,
                double qnty);
};

std::ostream& operator<<(std::ostream os, const Transaction& transaction);

}  // namespace solstice

#endif  // TRANSACTION_H
