#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <order.h>
#include <order_side.h>
#include <ticker.h>
#include <transaction.h>

#include <functional>
#include <memory>
#include <unordered_map>

namespace solstice
{

using OrderPtr = std::shared_ptr<Order>;
using PriceLevelMap = std::unordered_map<double, std::deque<OrderPtr>>;

struct ActiveOrders
{
    std::unordered_map<OrderSide, PriceLevelMap> activeOrders;
};

class OrderBook
{
   public:
    std::expected<OrderPtr, std::string> receiveOrder(OrderPtr order);

    const std::vector<Transaction>& transactions() const;

    const std::deque<OrderPtr> getMatchingOrders(OrderPtr order);

    void addOrderToOrderBook(OrderPtr order);
    std::expected<void, std::string> onNewOrder(OrderPtr order);

   private:
    // unordered map of order pointers for fast UID lookup
    std::unordered_map<std::string, OrderPtr> d_uidMap;
    std::unordered_map<Ticker, ActiveOrders> d_activeOrders;

    std::vector<Transaction> d_transactions;

    Transaction match(OrderPtr buyOrder, OrderPtr sellOrder);
};
}  // namespace solstice

#endif  // ORDERBOOK_H
