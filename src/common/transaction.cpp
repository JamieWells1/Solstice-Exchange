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

const std::string& Transaction::uid() const { return d_uid; }
int Transaction::bidUid() const { return d_bidUid; }
int Transaction::askUid() const { return d_askUid; }
const Underlying& Transaction::underlying() const { return d_underlying; }
double Transaction::price() const { return d_price; }
double Transaction::qnty() const { return d_qnty; }
const TimePoint& Transaction::timeExecuted() const { return d_timeExecuted; }

std::ostream& operator<<(std::ostream& os, const Transaction& transaction)
{
    os << "Transaction UID: " << transaction.uid() << " | Bid order UID: " << transaction.bidUid()
       << " | Ask order UID: " << transaction.askUid()
       << " | Ticker: " << to_string(transaction.underlying())
       << " | Price: " << transaction.price() << " | Quantity: " << transaction.qnty()
       << " | Time executed: " << transaction.timeExecuted();

    return os;
}
}  // namespace solstice::matching
