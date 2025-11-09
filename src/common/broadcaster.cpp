#include <asset_class.h>
#include <broadcaster.h>
#include <order.h>
#include <transaction.h>

#include <algorithm>
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

// ===================================================================
// WebSocketSession Implementation
// ===================================================================

WebSocketSession::WebSocketSession(tcp::socket&& socket, Broadcaster& broadcaster)
    : ws_(std::move(socket)), broadcaster_(broadcaster)
{
}

WebSocketSession::~WebSocketSession()
{
    broadcaster_.removeSession(shared_from_this());
}

void WebSocketSession::run()
{
    // Set suggested timeout settings for the websocket
    ws_.set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));

    // Set a decorator to change the Server of the handshake
    ws_.set_option(websocket::stream_base::decorator([](websocket::response_type& res) {
        res.set(beast::http::field::server, "Solstice-LOB-Broadcaster");
    }));

    // Accept the websocket handshake
    ws_.async_accept(beast::bind_front_handler(&WebSocketSession::onAccept, shared_from_this()));
}

void WebSocketSession::onAccept(beast::error_code ec)
{
    if (ec)
    {
        std::cerr << "WebSocket accept error: " << ec.message() << std::endl;
        return;
    }

    // Add this session to the broadcaster
    broadcaster_.addSession(shared_from_this());

    std::cout << "Client connected" << std::endl;

    // Read messages (we'll ignore incoming messages for now, but need to keep reading)
    ws_.async_read(buffer_,
                   beast::bind_front_handler(&WebSocketSession::onRead, shared_from_this()));
}

void WebSocketSession::onRead(beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);

    // Connection closed or error
    if (ec == websocket::error::closed)
    {
        std::cout << "Client disconnected" << std::endl;
        return;
    }

    if (ec)
    {
        std::cerr << "WebSocket read error: " << ec.message() << std::endl;
        return;
    }

    // Clear the buffer and continue reading
    buffer_.consume(buffer_.size());
    ws_.async_read(buffer_,
                   beast::bind_front_handler(&WebSocketSession::onRead, shared_from_this()));
}

void WebSocketSession::send(std::shared_ptr<std::string const> const& message)
{
    // Post to the websocket's strand to ensure thread safety
    net::post(ws_.get_executor(),
              beast::bind_front_handler(
                  [self = shared_from_this(), message]()
                  {
                      // Add message to queue
                      self->writeQueue_.push_back(message);

                      // If we're already writing, return (onWrite will handle the queue)
                      if (self->writeQueue_.size() > 1) return;

                      // Start async write
                      self->ws_.async_write(
                          net::buffer(*self->writeQueue_.front()),
                          beast::bind_front_handler(&WebSocketSession::onWrite, self));
                  }));
}

void WebSocketSession::onWrite(beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);

    if (ec)
    {
        std::cerr << "WebSocket write error: " << ec.message() << std::endl;
        return;
    }

    // Remove the written message from queue
    writeQueue_.erase(writeQueue_.begin());

    // Send next message if any
    if (!writeQueue_.empty())
    {
        ws_.async_write(net::buffer(*writeQueue_.front()),
                        beast::bind_front_handler(&WebSocketSession::onWrite, shared_from_this()));
    }
}

// ===================================================================
// Listener Implementation
// ===================================================================

Listener::Listener(net::io_context& ioc, tcp::endpoint endpoint, Broadcaster& broadcaster)
    : ioc_(ioc), acceptor_(net::make_strand(ioc)), broadcaster_(broadcaster)
{
    beast::error_code ec;

    // Open the acceptor
    acceptor_.open(endpoint.protocol(), ec);
    if (ec)
    {
        std::cerr << "Listener open error: " << ec.message() << std::endl;
        return;
    }

    // Allow address reuse
    acceptor_.set_option(net::socket_base::reuse_address(true), ec);
    if (ec)
    {
        std::cerr << "Listener set_option error: " << ec.message() << std::endl;
        return;
    }

    // Bind to the server address
    acceptor_.bind(endpoint, ec);
    if (ec)
    {
        std::cerr << "Listener bind error: " << ec.message() << std::endl;
        return;
    }

    // Start listening for connections
    acceptor_.listen(net::socket_base::max_listen_connections, ec);
    if (ec)
    {
        std::cerr << "Listener listen error: " << ec.message() << std::endl;
        return;
    }
}

void Listener::run()
{
    doAccept();
}

void Listener::doAccept()
{
    acceptor_.async_accept(net::make_strand(ioc_),
                           beast::bind_front_handler(&Listener::onAccept, shared_from_this()));
}

void Listener::onAccept(beast::error_code ec, tcp::socket socket)
{
    if (ec)
    {
        std::cerr << "Listener accept error: " << ec.message() << std::endl;
    }
    else
    {
        // Create session and run it
        std::make_shared<WebSocketSession>(std::move(socket), broadcaster_)->run();
    }

    // Accept next connection
    doAccept();
}

// ===================================================================
// Broadcaster Implementation
// ===================================================================

Broadcaster::Broadcaster(unsigned short port) : ioc_(1)
{
    // Start the server in a background thread
    ioThread_ = std::thread([this, port]() { this->run(port); });

    std::cout << "WebSocket server started on port " << port << std::endl;
}

Broadcaster::~Broadcaster()
{
    // Stop the io_context
    ioc_.stop();

    // Wait for the thread to finish
    if (ioThread_.joinable())
    {
        ioThread_.join();
    }
}

void Broadcaster::run(unsigned short port)
{
    auto const address = net::ip::make_address("0.0.0.0");
    auto const endpoint = tcp::endpoint{address, port};

    // Create and launch the listener
    std::make_shared<Listener>(ioc_, endpoint, *this)->run();

    // Run the io_context
    ioc_.run();
}

void Broadcaster::addSession(std::shared_ptr<WebSocketSession> session)
{
    std::lock_guard<std::mutex> lock(sessionsMutex_);
    sessions_.push_back(session);
}

void Broadcaster::removeSession(std::shared_ptr<WebSocketSession> session)
{
    std::lock_guard<std::mutex> lock(sessionsMutex_);
    sessions_.erase(std::remove_if(sessions_.begin(), sessions_.end(),
                                   [&](const std::weak_ptr<WebSocketSession>& weak) {
                                       return weak.expired() || weak.lock() == session;
                                   }),
                    sessions_.end());
}

void Broadcaster::broadcast(const std::string& message)
{
    // Make a shared pointer to the message for efficiency
    auto sharedMessage = std::make_shared<std::string>(message);

    std::lock_guard<std::mutex> lock(sessionsMutex_);

    // Send to all active sessions
    for (auto it = sessions_.begin(); it != sessions_.end();)
    {
        if (auto session = it->lock())
        {
            session->send(sharedMessage);
            ++it;
        }
        else
        {
            // Remove expired session
            it = sessions_.erase(it);
        }
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

}  // namespace solstice
