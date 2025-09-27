#include <config.h>
#include <logging.h>
#include <order.h>
#include <order_book.h>
#include <order_processor.h>
#include <order_side.h>
#include <ticker.h>

#include <memory>

using namespace solstice;

namespace
{

inline Ticker getTkr() { return getRandomTkr(); }

double getPrice(int minPrice, int maxPrice)
{
    return Random::getRandomNumber(minPrice, maxPrice);
}

double getQnty(int minQnty, int maxQnty)
{
    return Random::getRandomNumber(minQnty, maxQnty);
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

std::expected<OrderPtr, std::string> OrderProcessor::generateOrder()
{
    int uid = d_orderBook->d_uidMap.size();
    Ticker tkr = getTkr();
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

std::expected<void, std::string> OrderProcessor::produceOrders()
{
    for (size_t i = 0; i < d_config.d_ordersToGenerate; i++)
    {
        std::expected<OrderPtr, std::string> order = generateOrder();
        if (!order)
        {
            return std::unexpected(order.error());
        }

        processOrder(*order);
    }

    return {};
}

void OrderProcessor::processOrder(OrderPtr order)
{
    d_orderBook->d_uidMap.emplace(order->uid(), order);
    d_orderBook->addOrderToBook(order);

    auto orderMatched = d_matcher.matchOrder(order);
    if (!orderMatched)
    {
        std::cout << orderMatched.error();
    }

    d_orderBook->markOrderAsFulfilled(order);
    std::cout << *orderMatched;
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

    std::expected<void, std::string> result = processor.produceOrders();

    if (!result)
    {
        return std::unexpected(
            "An error occured when trying to create orders: " +
            result.error());
    }

    return {};
}
}  // namespace solstice
