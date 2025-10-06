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

namespace solstice::matching
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

    std::map<double, std::deque<OrderPtr>>& sameOrderSidePriceLevelMap(OrderPtr order);
    std::map<double, std::deque<OrderPtr>>& oppositeOrderSidePriceLevelMap(
    OrderPtr order);

    std::optional<std::reference_wrapper<std::deque<OrderPtr>>>
    getOrdersDequeAtPrice(const OrderPtr order);

    std::deque<OrderPtr>& ordersDequeAtPrice(OrderPtr order);
    std::deque<OrderPtr>& getOrdersDequeAtPrice(OrderPtr order,
                                                int priceToMatch);

    std::expected<std::reference_wrapper<std::deque<OrderPtr>>, std::string>
    getPriceLevelOppositeOrders(OrderPtr order, double priceToUse);

    const std::expected<double, std::string> getBestPrice(
        OrderPtr orderToMatch);

    void markOrderAsFulfilled(OrderPtr completedOrder);

    void initialiseActiveOrders();

   private:
    std::unordered_map<Ticker, ActiveOrders> d_activeOrders;

    std::vector<Transaction> d_transactions;

    void addOrderToBook(OrderPtr order);

    void removeOrderFromBook(OrderPtr orderToRemove);

    std::expected<std::reference_wrapper<BuyPricesAtPriceLevel>, std::string>
    getBuyPricesAtPriceLevel(OrderPtr order);

    std::expected<std::reference_wrapper<SellPricesAtPriceLevel>, std::string>
    getSellPricesAtPriceLevel(OrderPtr order);

    BuyPricesAtPriceLevel& buyPricesAtPriceLevel(OrderPtr order);
    SellPricesAtPriceLevel& sellPricesAtPriceLevel(OrderPtr order);
};
}  // namespace solstice::matching

#endif  // ORDERBOOK_H
