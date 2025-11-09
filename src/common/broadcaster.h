#ifndef BROADCASTER_H
#define BROADCASTER_H

#include <asset_class.h>
#include <time_point.h>

#include <json.hpp>
#include <memory>
#include <optional>
#include <set>
#include <thread>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include "websocketpp/common/connection_hdl.hpp"

namespace solstice
{

class Order;

namespace matching
{
struct Transaction;
}

using json = nlohmann::json;

class Broadcaster
{
   public:
    Broadcaster();
    ~Broadcaster();

    void broadcastTrade(const matching::Transaction& transaction);
    void broadcastOrder(const std::shared_ptr<Order>& order);
    void broadcastBookUpdate(const Underlying& underlying, const std::optional<double>& bestBid,
                             const std::optional<double>& bestAsk);

   private:
    void broadcast(const std::string& message);

    using WebSocketServer = websocketpp::server<websocketpp::config::asio>;
    using ConnectionHandle = websocketpp::connection_hdl;

    WebSocketServer server;

    std::set<ConnectionHandle, std::owner_less<ConnectionHandle>> connections;
    std::thread serverThread;
};

}  // namespace solstice

#endif  // BROADCASTER_H
