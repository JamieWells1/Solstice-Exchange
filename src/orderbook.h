#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <order.h>

#include <functional>
#include <list>
#include <map>
#include <memory>
#include <unordered_map>

class OrderBook {
   public:
    OrderBook();

    void addSellOrder(std::shared_ptr<Order> order);
    void addBuyOrder(std::shared_ptr<Order> order);

    int ordersFulfilled();

   private:
    int d_ordersFulfilled;

    // unordered map of order pointers for fast UID lookup
    std::unordered_map<std::string, std::shared_ptr<Order>> d_uidMap;

    // groups orders by price (orders with same price go in same group)
    std::map<double, std::list<std::shared_ptr<Order>>> d_sellOrders;
    std::map<double, std::list<std::shared_ptr<Order>>,
             std::greater<double>>
        d_buyOrders;
};

#endif  // ORDERBOOK_H
