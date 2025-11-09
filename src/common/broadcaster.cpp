#include <asset_class.h>
#include <broadcaster.h>
#include <order.h>
#include <transaction.h>

#include <chrono>
#include <cstdint>
#include <iostream>

namespace solstice
{

namespace
{

int64_t timePointToNanos(const TimePoint& tp)
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(tp.time_since_epoch()).count();
}
}  // namespace

Broadcaster::Broadcaster()
{
    server.init_asio();

    server.set_access_channels(websocketpp::log::alevel::none);
    server.set_error_channels(websocketpp::log::elevel::none);

    server.set_open_handler(
        [this](ConnectionHandle hdl)
        {
            connections.insert(hdl);
            std::cout << "Client connected. Total connections: " << connections.size() << std::endl;
        });

    server.set_close_handler(
        [this](ConnectionHandle hdl)
        {
            connections.erase(hdl);
            std::cout << "Client disconnected. Total connections: " << connections.size()
                      << std::endl;
        });

    server.listen(8080);
    server.start_accept();

    serverThread = std::thread([this]() { server.run(); });

    std::cout << "WebSocket server started on port 8080" << std::endl;
}

Broadcaster::~Broadcaster()
{
    server.stop();
    if (serverThread.joinable())
    {
        serverThread.join();
    }
}

void Broadcaster::broadcastTrade(const matching::Transaction& transaction)
{
    json msg = {{"type", "trade"},
                {"transaction_id", transaction.uid()},
                {"symbol", to_string(transaction.underlying())},
                {"price", transaction.price()},
                {"quantity", transaction.qnty()},
                {"bid_uid", transaction.bidUid()},
                {"ask_uid", transaction.askUid()},
                {"timestamp", timePointToNanos(transaction.timeExecuted())}};

    broadcast(msg.dump());
}

void Broadcaster::broadcastOrder(const std::shared_ptr<Order>& order)
{
    json msg = {{"type", "order"},
                {"order_id", order->uid()},
                {"symbol", to_string(order->underlying())},
                {"price", order->price()},
                {"quantity", order->qnty()},
                {"outstanding_quantity", order->outstandingQnty()},
                {"side", order->marketSideString()},
                {"matched", order->matched()},
                {"timestamp", timePointToNanos(order->timeOrderPlaced())}};

    broadcast(msg.dump());
}

void Broadcaster::broadcastBookUpdate(const Underlying& underlying,
                                      const std::optional<double>& bestBid,
                                      const std::optional<double>& bestAsk)
{
    json msg = {{"type", "book_update"}, {"symbol", to_string(underlying)}};

    if (bestBid.has_value())
    {
        msg["best_bid"] = bestBid.value();
    }
    else
    {
        msg["best_bid"] = nullptr;
    }

    if (bestAsk.has_value())
    {
        msg["best_ask"] = bestAsk.value();
    }
    else
    {
        msg["best_ask"] = nullptr;
    }

    broadcast(msg.dump());
}

void Broadcaster::broadcast(const std::string& message)
{
    for (auto hdl : connections)
    {
        try
        {
            server.send(hdl, message, websocketpp::frame::opcode::text);
        }
        catch (const std::exception& e)
        {
            std::cerr << "Broadcast error: " << e.what() << std::endl;
        }
    }
}

}  // namespace solstice
