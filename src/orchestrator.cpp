#include <asset_class.h>
#include <config.h>
#include <log_level.h>
#include <logging.h>
#include <orchestrator.h>
#include <order.h>
#include <order_book.h>
#include <order_side.h>

#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>

using namespace solstice;

namespace
{

std::expected<Underlying, std::string> getUnderlying(AssetClass assetClass)
{
    switch (assetClass)
    {
        case AssetClass::Equity:
            return getRandomUnderlying<Equity>();
        case AssetClass::Future:
            return getRandomUnderlying<Future>();
        default:
            return std::unexpected("Invalid asset class\n");
    }
}

double getPrice(int minPrice, int maxPrice) { return Random::getRandomDouble(minPrice, maxPrice); }

double getQnty(int minQnty, int maxQnty) { return Random::getRandomInt(minQnty, maxQnty); }

OrderSide getOrderSide()
{
    if (Random::getRandomBool())
    {
        return OrderSide::Bid;
    }
    else
    {
        return OrderSide::Ask;
    }
}

}  // namespace

namespace solstice::matching
{

Orchestrator::Orchestrator(Config config, std::shared_ptr<OrderBook> orderBook, Matcher matcher)
    : d_config(config), d_orderBook(orderBook), d_matcher(matcher)
{
}

std::expected<OrderPtr, std::string> Orchestrator::generateOrder(int ordersGenerated)
{
    int uid = ordersGenerated;

    auto underlying = getUnderlying(d_config.d_assetClass);
    if (!underlying)
    {
        return std::unexpected(underlying.error());
    }

    double price = getPrice(d_config.d_minPrice, d_config.d_maxPrice);
    double qnty = getQnty(d_config.d_minQnty, d_config.d_maxQnty);
    OrderSide orderSide = getOrderSide();

    auto order = Order::createOrder(uid, *underlying, price, qnty, orderSide);

    if (!order)
    {
        return std::unexpected(order.error());
    }

    return order;
}

bool Orchestrator::processOrder(OrderPtr order)
{
    std::lock_guard<std::mutex> lock(d_underlyingMutexes.at(order->underlying()));

    d_orderBook->addOrderToBook(order);

    auto orderMatched = d_matcher.matchOrder(order);
    if (!orderMatched)
    {
        if (d_config.d_logLevel >= LogLevel::DEBUG)
        {
            std::cout << "Order " << order->uid() << " failed to match: " << orderMatched.error();
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

void Orchestrator::pushToQueue(OrderPtr order)
{
    {
        std::lock_guard<std::mutex> lock(d_queueMutex);
        d_orderProcessQueue.push(order);
    }
    d_queueConditionVar.notify_one();
}

OrderPtr Orchestrator::popFromQueue()
{
    std::unique_lock<std::mutex> lock(d_queueMutex);

    d_queueConditionVar.wait(lock,
                             [this] { return !d_orderProcessQueue.empty() || d_done.load(); });

    if (d_orderProcessQueue.empty())
    {
        return nullptr;
    }

    OrderPtr order = d_orderProcessQueue.front();
    d_orderProcessQueue.pop();
    return order;
}

void Orchestrator::workerThread(std::atomic<int>& matched, std::atomic<int>& executed)
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

void Orchestrator::initialiseUnderlyings(AssetClass assetClass)
{
    switch (assetClass)
    {
        case AssetClass::Equity:
            d_orderBook->initialiseBookAtUnderlyings<Equity>();

            // add mutexes here rather than creating another switch case statement
            for (Equity underlying : getUnderlyingsPool<Equity>())
            {
                d_underlyingMutexes[underlying];
            }

            break;
        case AssetClass::Future:
            d_orderBook->initialiseBookAtUnderlyings<Future>();

            for (Future underlying : getUnderlyingsPool<Future>())
            {
                d_underlyingMutexes[underlying];
            }

            break;
        case AssetClass::COUNT:
            // Handle error
            break;
    }
}

std::expected<std::pair<int, int>, std::string> Orchestrator::produceOrders()
{
    d_done.store(false);

    std::atomic<int> ordersMatched{0};
    std::atomic<int> ordersExecuted{0};

    const int numThreads = std::thread::hardware_concurrency();
    std::vector<std::thread> threadPool;

    for (int i = 0; i < numThreads; i++)
    {
        threadPool.emplace_back(&Orchestrator::workerThread, this, std::ref(ordersMatched),
                                std::ref(ordersExecuted));
    }

    for (size_t i = 0; i < d_config.d_ordersToGenerate; i++)
    {
        auto order = generateOrder(i);
        if (!order)
        {
            d_done.store(true);
            d_queueConditionVar.notify_all();
            for (auto& thread : threadPool) thread.join();
            return std::unexpected(order.error());
        }
        pushToQueue(*order);
    }

    d_done.store(true);
    d_queueConditionVar.notify_all();

    // 5. Wait for workers
    for (auto& worker : threadPool)
    {
        worker.join();
    }

    return std::pair{ordersExecuted.load(), ordersMatched.load()};
}

std::expected<void, std::string> Orchestrator::start()
{
    auto config = Config::initConfig();

    if (!config)
    {
        return std::unexpected(config.error());
    }

    auto orderBook = std::make_shared<OrderBook>();
    Matcher matcher{orderBook};
    Orchestrator orchestrator{*config, orderBook, matcher};

    orchestrator.initialiseUnderlyings(config->d_assetClass);

    auto start = getTimeNow();
    auto result = orchestrator.produceOrders();
    auto end = getTimeNow();

    if (!result)
    {
        return std::unexpected("An error occured when trying to create orders: " + result.error());
    }

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    if (config->d_logLevel >= LogLevel::INFO)
    {
        std::cout << "\nSUMMARY:"
                  << "\nOrders executed: " << result->first
                  << "\nOrders matched: " << result->second << "\nTime taken: " << duration;
    }

    return {};
}
}  // namespace solstice::matching
