#include <config.h>
#include <logging.h>
#include <order.h>
#include <order_book.h>
#include <order_processor.h>
#include <order_side.h>

#include <deque>
#include <memory>
#include <random>

using namespace solstice;

namespace
{

inline Ticker getTicker()
{
    static const std::array<Ticker, 6> validTickers = {
        Ticker::AAPL, Ticker::TSLA, Ticker::GOOGL,
        Ticker::AMZN, Ticker::MSFT, Ticker::GOOG};

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, validTickers.size() - 1);
    return validTickers[dist(gen)];
}

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
    Ticker d_ticker = getTicker();
    double d_price = getPrice(d_config.d_minPrice, d_config.d_maxPrice);
    double d_qnty = getQnty(d_config.d_minQnty, d_config.d_maxQnty);
    OrderSide d_orderSide = getOrderSide();

    auto order =
        Order::createOrder(d_ticker, d_price, d_qnty, d_orderSide);

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

    bool orderMatched = d_matcher.matchOrder(order);
    if (orderMatched)
    {
        d_orderBook->markOrderAsFulfilled(order);
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
