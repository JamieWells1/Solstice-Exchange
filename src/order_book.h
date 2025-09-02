#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <order.h>
#include <transaction.h>
#include <ticker.h>
#include <order_side.h>

#include <functional>
#include <memory>
#include <unordered_map>

namespace solstice
{

using OrderPtr = std::shared_ptr<Order>;
using PriceLevelMap = std::unordered_map<double, std::deque<OrderPtr>>;

struct ActiveOrders
{
    std::unordered_map<OrderSide, PriceLevelMap> d_activeOrders;
};

class OrderBook
{
   public:
    bool receiveOrder(OrderPtr order);

    const std::vector<Transaction>& transactions() const;

   private:
    // unordered map of order pointers for fast UID lookup
    std::unordered_map<std::string, OrderPtr> d_uidMap;

    std::unordered_map<Ticker, ActiveOrders> d_activeOrders;

    void addSellOrder(OrderPtr order);
    void addBuyOrder(OrderPtr order);

    void onNewSellOrder(OrderPtr order);
    void onBuySellOrder(OrderPtr order);

    std::vector<Transaction> d_transactions;

    Transaction match(OrderPtr buyOrder,
                      OrderPtr sellOrder);
};
}  // namespace solstice

#endif  // ORDERBOOK_H
