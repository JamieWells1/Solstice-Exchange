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
#include <mutex>
#include <thread>
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

bool OrderProcessor::processOrder(OrderPtr order)
{
    std::lock_guard<std::mutex> lock(d_tickerMutexes.at(order->tkr()));

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

void OrderProcessor::pushToQueue(OrderPtr order)
{
    {
        std::lock_guard<std::mutex> lock(d_queueMutex);
        d_orderProcessQueue.push(order);
    }
    d_queueConditionVar.notify_one();
}

OrderPtr OrderProcessor::popFromQueue()
{
    std::unique_lock<std::mutex> lock(d_queueMutex);

    d_queueConditionVar.wait(
        lock,
        [this] { return !d_orderProcessQueue.empty() || d_done.load(); });

    if (d_orderProcessQueue.empty())
    {
        return nullptr;
    }

    OrderPtr order = d_orderProcessQueue.front();
    d_orderProcessQueue.pop();
    return order;
}

void OrderProcessor::workerThread(std::atomic<int>& matched,
                                  std::atomic<int>& executed)
{
    while (true)
    {
        OrderPtr order = popFromQueue();

        if (!order)
        {
            break;
        }

        if (processOrder(order))
        {
            matched++;
        }
        executed++;
    }
}

void OrderProcessor::initialiseMutexes()
{
    // create a new mutex for each ticker
    for (Ticker tkr : ALL_TICKERS)
    {
        d_tickerMutexes[tkr];
    }
}

std::expected<std::pair<int, int>, std::string>
OrderProcessor::produceOrders()
{
    d_done.store(false);

    std::atomic<int> ordersMatched{0};
    std::atomic<int> ordersExecuted{0};

    const int numThreads = std::thread::hardware_concurrency();
    std::vector<std::thread> workers;

    for (int i = 0; i < numThreads; i++)
    {
        workers.emplace_back(&OrderProcessor::workerThread, this,
                             std::ref(ordersMatched),
                             std::ref(ordersExecuted));
    }

    for (size_t i = 0; i < d_config.d_ordersToGenerate; i++)
    {
        auto order = generateOrder(i);
        if (!order)
        {
            d_done.store(true);
            d_queueConditionVar.notify_all();
            for (auto& w : workers) w.join();
            return std::unexpected(order.error());
        }
        pushToQueue(*order);
    }

    d_done.store(true);
    d_queueConditionVar.notify_all();

    // 5. Wait for workers
    for (auto& worker : workers)
    {
        worker.join();
    }

    return std::pair{ordersExecuted.load(), ordersMatched.load()};
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

    processor.initialiseMutexes();

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
