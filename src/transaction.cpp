#include <transaction.h>

#include "time_point.h"

using namespace solstice;

Transaction::Transaction(std::shared_ptr<Order> buyOrder,
                         std::shared_ptr<Order> sellOrder, double price,
                         double qnty)
    : d_uid(Random::getRandomUid()),
      d_timeExecuted(getTimeNow()),
      d_buyOrderUid(buyOrder->uid()),
      d_sellOrderUid(sellOrder->uid()),
      d_price(price),
      d_qnty(qnty),
      d_tkr(buyOrder->tkr())
{
}

std::ostream& operator<<(std::ostream& os, const Transaction& transaction)
{
    os << "Transaction UID: " << transaction.d_uid
       << " | Buyer UID: " << transaction.d_buyOrderUid
       << " | Seller UID: " << transaction.d_sellOrderUid
       << " | Ticker: " << transaction.d_tkr
       << " | Price: " << transaction.d_price
       << " | Quantity: " << transaction.d_qnty
       << " | Time executed: " << transaction.d_timeExecuted;

    return os;
}
