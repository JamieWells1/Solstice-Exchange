#include <config.h>
#include <logging.h>
#include <order.h>
#include <order_book.h>
#include <order_processor.h>

#include <memory>
#include <random>

namespace solstice
{

OrderProcessor::OrderProcessor(Config config, OrderBook orderBook)
    : d_config(config), d_orderBook(orderBook)
{
}

inline Ticker OrderProcessor::getTicker() const
{
    static const std::array<Ticker, 6> validTickers = {
        Ticker::AAPL, Ticker::TSLA, Ticker::GOOGL,
        Ticker::AMZN, Ticker::MSFT, Ticker::GOOG};

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, validTickers.size() - 1);
    return validTickers[dist(gen)];
}

double OrderProcessor::getPrice() const
{
    // TODO: return price based on mktdata
    return Random::getRandomNumber(d_config.d_minPrice,
                                   d_config.d_maxPrice);
}

double OrderProcessor::getQnty() const
{
    return Random::getRandomNumber(d_config.d_minQuantity,
                                   d_config.d_maxQuantity);
}

bool OrderProcessor::getIsBuy() const { return Random::getRandomBool(); }

std::expected<std::shared_ptr<Order>, std::string>
OrderProcessor::generateOrder() const
{
    Ticker d_ticker = getTicker();
    double d_price = getPrice();
    double d_qnty = getQnty();
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
    for (unsigned int i = 0; i < d_config.d_ordersToGenerate; i++)
    {
        std::expected<std::shared_ptr<Order>, std::string> order =
            OrderProcessor::generateOrder();

        if (!order)
        {
            return std::unexpected(order.error());
        }

        d_orderBook.receiveOrder(*order);

        LOG("ORDER GENERATED: " << *(*order));
    }

    return {};
}

std::expected<void, std::string> OrderProcessor::start()
{
    Config config = *Config::initConfig();
    OrderBook orderBook;
    OrderProcessor OrderProcessor(config, orderBook);

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
