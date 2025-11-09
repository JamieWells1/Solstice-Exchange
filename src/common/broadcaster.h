#ifndef BROADCASTER_H
#define BROADCASTER_H

#include <asset_class.h>
#include <time_point.h>

#include <json.hpp>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <thread>
#include <vector>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

namespace solstice
{

class Order;

namespace matching
{
struct Transaction;
}

using json = nlohmann::json;

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

// Forward declaration
class WebSocketSession;

class Broadcaster
{
   public:
    Broadcaster(unsigned short port = 8080);
    ~Broadcaster();

    // Disable copy/move
    Broadcaster(const Broadcaster&) = delete;
    Broadcaster& operator=(const Broadcaster&) = delete;

    void broadcastTrade(const matching::Transaction& transaction);
    void broadcastOrder(const std::shared_ptr<Order>& order);
    void broadcastBookUpdate(const Underlying& underlying, const std::optional<double>& bestBid,
                             const std::optional<double>& bestAsk);

    // Session management (called by sessions)
    void addSession(std::shared_ptr<WebSocketSession> session);
    void removeSession(std::shared_ptr<WebSocketSession> session);

   private:
    void broadcast(const std::string& message);
    void run(unsigned short port);

    net::io_context ioc_;
    std::thread ioThread_;
    std::mutex sessionsMutex_;
    std::vector<std::weak_ptr<WebSocketSession>> sessions_;
};

// WebSocket session - handles individual connections
class WebSocketSession : public std::enable_shared_from_this<WebSocketSession>
{
   public:
    explicit WebSocketSession(tcp::socket&& socket, Broadcaster& broadcaster);
    ~WebSocketSession();

    void run();
    void send(std::shared_ptr<std::string const> const& message);

   private:
    void onAccept(beast::error_code ec);
    void onRead(beast::error_code ec, std::size_t bytes_transferred);
    void onWrite(beast::error_code ec, std::size_t bytes_transferred);

    websocket::stream<beast::tcp_stream> ws_;
    Broadcaster& broadcaster_;
    beast::flat_buffer buffer_;
    std::vector<std::shared_ptr<std::string const>> writeQueue_;
};

// Listener - accepts incoming connections
class Listener : public std::enable_shared_from_this<Listener>
{
   public:
    Listener(net::io_context& ioc, tcp::endpoint endpoint, Broadcaster& broadcaster);
    void run();

   private:
    void doAccept();
    void onAccept(beast::error_code ec, tcp::socket socket);

    net::io_context& ioc_;
    tcp::acceptor acceptor_;
    Broadcaster& broadcaster_;
};

}  // namespace solstice

#endif  // BROADCASTER_H
