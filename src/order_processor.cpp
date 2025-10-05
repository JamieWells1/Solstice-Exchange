#include <config.h>
#include <log_level.h>
#include <logging.h>
#include <order.h>
#include <order_book.h>
#include <order_processor.h>
#include <order_side.h>
#include <ticker.h>

#include <iostream>
#include <memory>
#include <utility>

using namespace solstice;

namespace
{

inline Ticker getTkr(int tkrPoolCount)
{
    return getRandomTkr(tkrPoolCount);
}

double getPrice(int minPrice, int maxPrice)
{
    return Random::getRandomDouble(minPrice, maxPrice);
}

double getQnty(int minQnty, int maxQnty)
{
    return Random::getRandomInt(minQnty, maxQnty);
}

OrderSide getOrderSide()
{
    if (Random::getRandomBool())
    {
        return OrderSide::Buy;
    }
    else
    {
        return OrderSide::Sell;
    }
}

}  // namespace

namespace solstice
{

OrderProcessor::OrderProcessor(Config config,
                               std::shared_ptr<OrderBook> orderBook,
                               Matcher matcher)
    : d_config(config), d_orderBook(orderBook), d_matcher(matcher)
{
}

std::expected<OrderPtr, std::string> OrderProcessor::generateOrder(
    int ordersGenerated)
{
    int uid = ordersGenerated;
    Ticker tkr = getTkr(d_config.d_tkrPoolCount);
    double price = getPrice(d_config.d_minPrice, d_config.d_maxPrice);
    double qnty = getQnty(d_config.d_minQnty, d_config.d_maxQnty);
    OrderSide orderSide = getOrderSide();

    auto order = Order::createOrder(uid, tkr, price, qnty, orderSide);

    if (!order)
    {
        return std::unexpected(order.error());
    }

    return order;
}

std::expected<std::pair<int, int>, std::string>
OrderProcessor::produceOrders()
{
    int ordersMatched = 0;
    int ordersExecuted = 0;

    for (size_t i = 0; i < d_config.d_ordersToGenerate; i++)
    {
        std::expected<OrderPtr, std::string> order = generateOrder(i);
        if (!order)
        {
            return std::unexpected(order.error());
        }

        if (processOrder(*order))
        {
            ordersMatched++;
        }
        ordersExecuted++;
    }

    return std::pair{ordersExecuted, ordersMatched};
}

bool OrderProcessor::processOrder(OrderPtr order)
{
    d_orderBook->addOrderToBook(order);

    auto orderMatched = d_matcher.matchOrder(order);
    if (!orderMatched)
    {
        if (d_config.d_logLevel >= LogLevel::DEBUG)
        {
            std::cout << "Order " << order->uid()
                      << " failed to match: " << orderMatched.error();
        }
        return false;
    }
    else
    {
        d_orderBook->markOrderAsFulfilled(order);
        if (d_config.d_logLevel >= LogLevel::DEBUG)
        {
            std::cout << *orderMatched;
        }
        return true;
    }
}

std::expected<void, std::string> OrderProcessor::start()
{
    auto config = Config::initConfig();

    if (!config)
    {
        return std::unexpected(config.error());
    }

    auto orderBook = std::make_shared<OrderBook>();
    Matcher matcher{orderBook};
    OrderProcessor processor{*config, orderBook, matcher};

    auto start = getTimeNow();
    auto result = processor.produceOrders();
    auto end = getTimeNow();

    if (!result)
    {
        return std::unexpected(
            "An error occured when trying to create orders: " +
            result.error());
    }

    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    if (config->d_logLevel >= LogLevel::INFO)
    {
        std::cout << "\nSUMMARY:"
                  << "\nOrders executed: " << result->first
                  << "\nOrders matched: " << result->second
                  << "\nTime taken: " << duration;
    }

    return {};
}
}  // namespace solstice
