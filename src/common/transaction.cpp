#include <time_point.h>
#include <transaction.h>

namespace solstice::matching
{

Transaction::Transaction(OrderPtr bid, OrderPtr ask, double price, double qnty)
    : d_uid(Random::getRandomUid()),
      d_timeExecuted(getTimeNow()),
      d_bidUid(bid->uid()),
      d_askUid(ask->uid()),
      d_price(price),
      d_qnty(qnty),
      d_underlying(bid->underlying())
{
}

std::ostream& operator<<(std::ostream& os, const Transaction& transaction)
{
    os << "Transaction UID: " << transaction.d_uid << " | Bid order UID: " << transaction.d_bidUid
       << " | Ask order UID: " << transaction.d_askUid
       << " | Ticker: " << to_string(transaction.d_underlying)
       << " | Price: " << transaction.d_price << " | Quantity: " << transaction.d_qnty
       << " | Time executed: " << transaction.d_timeExecuted;

    return os;
}
}  // namespace solstice::matching
