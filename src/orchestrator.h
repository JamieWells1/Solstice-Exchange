#ifndef ORCHESTRATOR_H
#define ORCHESTRATOR_H

#include <config.h>
#include <matcher.h>
#include <order.h>
#include <order_book.h>

#include <memory>
#include <mutex>
#include <queue>

namespace solstice::matching
{

class Orchestrator
{
   public:
    static std::expected<void, std::string> start();

    Config d_config;
    std::shared_ptr<OrderBook> d_orderBook;
    Matcher d_matcher;

    bool processOrder(OrderPtr order);

   private:
    Orchestrator(Config config, std::shared_ptr<OrderBook> orderBook, Matcher matcher);

    std::map<Underlying, std::mutex> d_underlyingMutexes;

    std::queue<OrderPtr> d_orderProcessQueue;
    std::mutex d_queueMutex;
    std::condition_variable d_queueConditionVar;
    std::atomic<bool> d_done{false};

    void initialiseUnderlyings(AssetClass assetClass);

    template <typename T>
    void initialiseMutexes(T underlying);

    void pushToQueue(OrderPtr order);
    OrderPtr popFromQueue();

    void workerThread(std::atomic<int>& matched, std::atomic<int>& executed);

    std::expected<OrderPtr, std::string> generateOrder(int ordersGenerated);

    std::expected<std::pair<int, int>, std::string> produceOrders();
};

std::ostream& operator<<(std::ostream& os, ActiveOrders activeOrders);

}  // namespace solstice::matching

#endif  // ORCHESTRATOR_H
