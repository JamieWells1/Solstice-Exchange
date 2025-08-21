#include <config.h>
#include <logging.h>
#include <order.h>
#include <order_book.h>
#include <order_processor.h>

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

bool getIsBuy() { return Random::getRandomBool(); }

}  // namespace

namespace solstice
{

OrderProcessor::OrderProcessor(Config config, OrderBook orderBook)
    : d_config(config), d_orderBook(orderBook)
{
}

std::expected<std::shared_ptr<Order>, std::string>
OrderProcessor::generateOrder()
{
    Ticker d_ticker = getTicker();
    double d_price = getPrice(d_config.d_minPrice, d_config.d_maxPrice);
    double d_qnty = getQnty(d_config.d_minQnty, d_config.d_maxQnty);
    bool d_isBuy = getIsBuy();

    auto order = Order::createOrder(d_ticker, d_price, d_qnty, d_isBuy);

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
        std::expected<std::shared_ptr<Order>, std::string> order =
            OrderProcessor::generateOrder();

        if (!order)
        {
            return std::unexpected(order.error());
        }

        d_orderBook.receiveOrder(*order);
    }

#ifdef ENABLE_LOGGING
    d_orderBook.printSellOrders();
    d_orderBook.printBuyOrders();
#endif

    return {};
}

std::expected<void, std::string> OrderProcessor::start()
{
    auto config = Config::initConfig();

    if (!config)
    {
        return std::unexpected(config.error());
    }

    OrderBook orderBook;
    OrderProcessor OrderProcessor(*config, orderBook);

    auto response = OrderProcessor.produceOrders();

    if (!response)
    {
        return std::unexpected(
            "An error occured when trying to create orders: " +
            response.error());
    }

    return {};
}
}  // namespace solstice
