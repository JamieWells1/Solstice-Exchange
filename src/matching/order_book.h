#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <asset_class.h>
#include <order.h>
#include <order_side.h>
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

using BidPricesAtPriceLevel = std::set<double, std::greater<double>>;
using askPricesAtPriceLevel = std::set<double, std::less<double>>;

struct ActiveOrders
{
    PriceLevelMap bids;
    PriceLevelMap asks;

    BidPricesAtPriceLevel bidPrices;
    askPricesAtPriceLevel askPrices;
};

class OrderBook
{
    friend class Orchestrator;

   public:
    const std::vector<Transaction>& transactions() const;

    std::map<double, std::deque<OrderPtr>>& sameOrderSidePriceLevelMap(OrderPtr order);
    std::map<double, std::deque<OrderPtr>>& oppositeOrderSidePriceLevelMap(OrderPtr order);

    std::optional<std::reference_wrapper<std::deque<OrderPtr>>> getOrdersDequeAtPrice(
        const OrderPtr order);

    std::deque<OrderPtr>& ordersDequeAtPrice(OrderPtr order);
    std::deque<OrderPtr>& getOrdersDequeAtPrice(OrderPtr order, int priceToMatch);

    std::expected<std::reference_wrapper<std::deque<OrderPtr>>, std::string>
    getPriceLevelOppositeOrders(OrderPtr order, double priceToUse);

    const std::expected<double, std::string> getBestPrice(OrderPtr orderToMatch);

    void markOrderAsFulfilled(OrderPtr completedOrder);

    template <typename T>
    void initialiseBookAtUnderlyings()
    {
        for (const auto& underlying : getUnderlyingsPool<T>())
        {
            d_activeOrders[underlying];
        }
    }

    void addOrderToBook(OrderPtr order);

    void removeOrderFromBook(OrderPtr orderToRemove);

   private:
    std::unordered_map<Underlying, ActiveOrders> d_activeOrders;

    std::vector<Transaction> d_transactions;

    std::expected<std::reference_wrapper<BidPricesAtPriceLevel>, std::string>
    getBidPricesAtPriceLevel(OrderPtr order);

    std::expected<std::reference_wrapper<askPricesAtPriceLevel>, std::string>
    getaskPricesAtPriceLevel(OrderPtr order);

    BidPricesAtPriceLevel& bidPricesAtPriceLevel(OrderPtr order);
    askPricesAtPriceLevel& askPricesAtPriceLevel(OrderPtr order);
};
}  // namespace solstice::matching

#endif  // ORDERBOOK_H
