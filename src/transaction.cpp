#include <transaction.h>

std::ostream& operator<<(std::ostream& os,
                         const Transaction& transaction) {
    os << "Transaction UID: " << transaction.uid
       << " | Buyer UID: " << transaction.buyerUid
       << " | Seller UID: " << transaction.sellerUid
       << " | Ticker: " << transaction.tkr
       << " | Price: " << transaction.price
       << " | Quantity: " << transaction.qnty
       << " | Time executed: " << transaction.timeExecuted;
    return os;
}
