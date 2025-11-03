#include <asset_class.h>
#include <config.h>
#include <log_level.h>
#include <logging.h>
#include <market_side.h>
#include <orchestrator.h>
#include <order.h>
#include <order_book.h>
#include <pricer.h>

#include <atomic>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>

namespace solstice::matching
{

Orchestrator::Orchestrator(Config config, std::shared_ptr<OrderBook> orderBook,
                           std::shared_ptr<Matcher> matcher,
                           std::shared_ptr<pricing::Pricer> pricer)
    : d_config(config), d_orderBook(orderBook), d_matcher(matcher), d_pricer(pricer)
{
}

const Config& Orchestrator::config() const { return d_config; }
const std::shared_ptr<OrderBook>& Orchestrator::orderBook() const { return d_orderBook; }
const std::shared_ptr<Matcher>& Orchestrator::matcher() const { return d_matcher; }

std::expected<OrderPtr, std::string> Orchestrator::generateOrder(int ordersGenerated)
{
    int uid = ordersGenerated;

    auto underlying = getUnderlying(d_config.assetClass());
    if (!underlying)
    {
        return std::unexpected(underlying.error());
    }

    auto order = Order::createWithPricer(d_pricer, *underlying, uid);
    // TODO: add createWithRandomValues() helper function above

    if (!order)
    {
        return std::unexpected(order.error());
    }

    return order;
}

bool Orchestrator::processOrder(OrderPtr order)
{
    auto mutexIt = d_underlyingMutexes.find(order->underlying());
    if (mutexIt != d_underlyingMutexes.end())
    {
        std::lock_guard<std::mutex> lock(mutexIt->second);
        d_orderBook->addOrderToBook(order);

        auto orderMatched = d_matcher->matchOrder(order);

        if (!orderMatched)
        {
            if (d_config.logLevel() >= LogLevel::DEBUG)
            {
                std::cout << "Order " << order->uid()
                          << " failed to match: " << orderMatched.error();
            }

            d_pricer->update(order, false);
            return false;
        }
        else
        {
            if (d_config.logLevel() >= LogLevel::DEBUG)
            {
                std::cout << *orderMatched;
            }

            d_pricer->update(order, true);
            return true;
        }
    }
    else
    {
        // no mutex for this underlying - process without locking
        d_orderBook->addOrderToBook(order);

        auto orderMatched = d_matcher->matchOrder(order);
        if (!orderMatched)
        {
            if (d_config.logLevel() >= LogLevel::DEBUG)
            {
                std::cout << "Order " << order->uid()
                          << " failed to match: " << orderMatched.error();
            }

            d_pricer->update(order, false);
            return false;
        }
        else
        {
            if (d_config.logLevel() >= LogLevel::DEBUG)
            {
                std::cout << *orderMatched;
            }

            d_pricer->update(order, true);
            return true;
        }
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
            setUnderlyingsPool(d_config.underlyingPoolCount(), ALL_EQUITIES);

            d_orderBook->initialiseBookAtUnderlyings<Equity>();
            d_pricer->initialisePricerEquities<Equity>();

            for (Equity underlying : underlyingsPool<Equity>())
            {
                d_underlyingMutexes[underlying];
            }

            break;
        case AssetClass::Future:
            setUnderlyingsPool(d_config.underlyingPoolCount(), ALL_FUTURES);

            d_orderBook->initialiseBookAtUnderlyings<Future>();
            d_pricer->initialisePricerFutures<Future>();

            for (Future underlying : underlyingsPool<Future>())
            {
                d_underlyingMutexes[underlying];
            }

            break;
        case AssetClass::COUNT:
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

    for (size_t i = 0; i < d_config.ordersToGenerate(); i++)
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
    auto config = Config::instance();

    if (!config)
    {
        return std::unexpected(config.error());
    }

    auto orderBook = std::make_shared<OrderBook>();
    auto matcher = std::make_shared<Matcher>(orderBook);
    auto pricer = std::make_shared<pricing::Pricer>(orderBook);

    Orchestrator orchestrator{*config, orderBook, matcher, pricer};

    orchestrator.initialiseUnderlyings(config->assetClass());

    auto start = getTimeNow();
    auto result = orchestrator.produceOrders();
    auto end = getTimeNow();

    if (!result)
    {
        return std::unexpected("An error occured when trying to create orders: " + result.error());
    }

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    if (config->logLevel() >= LogLevel::INFO)
    {
        std::cout << "\nSUMMARY:"
                  << "\nOrders executed: " << result->first
                  << "\nOrders matched: " << result->second << "\nTime taken: " << duration;
    }

    return {};
}
}  // namespace solstice::matching
