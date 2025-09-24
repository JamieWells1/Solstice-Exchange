#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <order.h>
#include <order_side.h>
#include <ticker.h>
#include <transaction.h>

#include <deque>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <unordered_map>

namespace solstice
{

using OrderPtr = std::shared_ptr<Order>;
using PriceLevelMap = std::map<double, std::deque<OrderPtr>>;

using BuyPricesAtPriceLevel = std::set<double, std::greater<double>>;
using SellPricesAtPriceLevel = std::set<double, std::less<double>>;

struct ActiveOrders
{
    PriceLevelMap buyOrders;
    PriceLevelMap sellOrders;

    BuyPricesAtPriceLevel buyPrices;
    SellPricesAtPriceLevel sellPrices;
};

class OrderBook
{
    friend class OrderProcessor;

   public:
    const std::vector<Transaction>& transactions() const;

    std::deque<OrderPtr>& getOrdersAtPrice(const OrderPtr order);

    std::deque<OrderPtr>& getOrdersAtPrice(const OrderPtr order,
                                           int priceToMatch);

    std::map<double, std::deque<OrderPtr>>& priceLevelMap(OrderPtr order);

        std::deque<OrderPtr>& ordersDequeAtPrice(OrderPtr order);
    std::deque<OrderPtr>& ordersDequeAtPrice(OrderPtr order, int price);

    const std::expected<double, std::string> getBestPrice(
        OrderPtr orderToMatch);

    void markOrderAsFulfilled(OrderPtr completedOrder);

   private:
    // unordered map of order pointers for fast UID lookup
    std::unordered_map<std::string, OrderPtr> d_uidMap;
    std::unordered_map<Ticker, ActiveOrders> d_activeOrders;

    std::vector<Transaction> d_transactions;

    Transaction match(OrderPtr buyOrder, OrderPtr sellOrder);

    void addOrderToBook(OrderPtr order);

    void removeOrderFromBook(OrderPtr orderToRemove);

    BuyPricesAtPriceLevel& buyPricesAtPriceLevel(OrderPtr order);
    SellPricesAtPriceLevel& sellPricesAtPriceLevel(OrderPtr order);
};
}  // namespace solstice

#endif  // ORDERBOOK_H
