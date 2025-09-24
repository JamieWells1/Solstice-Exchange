#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <order.h>
#include <order_side.h>
#include <ticker.h>
#include <transaction.h>

#include <deque>
#include <functional>
#include <memory>
#include <unordered_map>
#include <set>

namespace solstice
{

using OrderPtr = std::shared_ptr<Order>;
using PriceLevelMap = std::unordered_map<double, std::deque<OrderPtr>>;

using BuyPrices = std::set<int, std::greater<int>>;
using SellPrices = std::set<int, std::less<int>>;

struct ActiveOrders
{
    std::unordered_map<OrderSide, PriceLevelMap> activeOrders;
};

class OrderBook
{
    friend class OrderProcessor;

   public:
    const std::vector<Transaction>& transactions() const;

    const std::deque<OrderPtr>& getMatchingOrders(const OrderPtr& order);

    const std::deque<OrderPtr>& getMatchingOrders(const OrderPtr& order,
                                                  int priceToMatch);

    void markOrderAsComplete(OrderPtr completedOrder);

   private:
    // unordered map of order pointers for fast UID lookup
    std::unordered_map<std::string, OrderPtr> d_uidMap;
    std::unordered_map<Ticker, ActiveOrders> d_activeOrders;

    BuyPrices d_buyPrices;
    SellPrices d_sellPrices;

    std::vector<Transaction> d_transactions;

    Transaction match(OrderPtr buyOrder, OrderPtr sellOrder);

    void addOrderToBook(OrderPtr order);

    void removeOrderFromBook(OrderPtr orderToRemove);

    std::deque<OrderPtr>& getOrdersDequeAtPrice(OrderPtr order);

    std::deque<OrderPtr>& getOrdersDequeAtPrice(OrderPtr order, int price);
};
}  // namespace solstice

#endif  // ORDERBOOK_H
