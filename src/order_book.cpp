#include <order.h>
#include <order_book.h>
#include <transaction.h>
#include <truncate.h>

#include <deque>
#include <iostream>
#include <memory>
#include "order_processor.h"
#include "order_side.h"

namespace solstice
{

const std::vector<Transaction>& OrderBook::transactions() const
{
    return d_transactions;
}

std::expected<std::shared_ptr<Order>, std::string> OrderBook::receiveOrder(std::shared_ptr<Order> order)
{
    d_uidMap.emplace(order->uid(), order);

    if (!order)
    {
        return std::unexpected("No order received by order book");
    }

    auto result = onNewOrder(order);

    if (!result)
    {
        return std::unexpected(result.error());
    }

    return std::move(order);
}

void OrderBook::addOrderToOrderBook(std::shared_ptr<Order> order)
{
    auto sideIndex = static_cast<size_t>(order->orderSide());

    d_activeOrders[order->tkr()]
        .activeOrders[order->orderSide()][sideIndex]
        .push_back(order);
}

std::expected<void, std::string> OrderBook::onNewOrder(std::shared_ptr<Order> order)
{
    addOrderToOrderBook(order);

    auto result = OrderProcessor::processOrder(order);
    if (!result)
    {
        return std::unexpected(result.error());
    }

    return {};
}

/*
void OrderBook::printSellOrders()
{
    std::cout << "\nSELL ORDERS: " << std::endl;
    for (const auto& [price, orders] : d_sellOrders)
    {
        std::cout << "$" << price << ": [";
        for (const auto& order : orders)
        {
            std::cout << " id_" << truncate(order->uid(), 5) << ' ';
        }
        std::cout << "]" << std::endl;
    }
}

void OrderBook::printBuyOrders()
{
    std::cout << "\nBUY ORDERS: " << std::endl;
    for (const auto& [price, orders] : d_buyOrders)
    {
        std::cout << "$" << price << ": [";
        for (const auto& order : orders)
        {
            std::cout << " id_" << truncate(order->uid(), 5) << ' ';
        }
        std::cout << "]" << std::endl;
    }
}
*/

Transaction OrderBook::match(std::shared_ptr<Order> buyOrder,
                             std::shared_ptr<Order> sellOrder)
{
    Transaction transaction(buyOrder, sellOrder, 10, 10);
    return transaction;
}
}  // namespace solstice
