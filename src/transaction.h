#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <ticker.h>
#include <timepoint.h>

#include <ostream>
#include <string>

struct Transaction {
   public:
    std::string uid;
    std::string buyerUid;
    std::string sellerUid;
    Ticker tkr;
    double price;
    double qnty;
    TimePoint timeExecuted;
};

std::ostream& operator<<(std::ostream os, const Transaction& transaction);

#endif  // TRANSACTION_H
