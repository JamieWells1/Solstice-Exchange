#include <get_random.h>
#include <order.h>
#include <asset_class.h>
#include <order_side.h>

#include <chrono>
#include <format>
#include <memory>
#include <ostream>

namespace
{

std::expected<void, std::string> validatePrice(const double price)
{
    if (price < 0)
    {
        return std::unexpected(std::format("Invalid price: {}", price, "\n"));
    }

    return {};
}

std::expected<void, std::string> validateQnty(const double qnty)
{
    if (qnty < 0)
    {
        return std::unexpected(std::format("Invalid quantity: {}", qnty, "\n"));
    }

    return {};
}

std::expected<void, std::string> validateTimeOrderPlaced(const TimePoint& timeOrderPlaced)
{
    TimePoint now = std::chrono::system_clock::now();

    if (timeOrderPlaced > now)
    {
        return std::unexpected("Order cannot be placed in the future\n");
    }

    auto delayInSeconds =
        std::chrono::duration_cast<std::chrono::seconds>(now - timeOrderPlaced).count();
    if (delayInSeconds > 10)
    {
        return std::unexpected("Order timed out due to extensive processing time\n");
    }

    return {};
}

std::expected<void, std::string> validateOrderAttributes(double price, double qnty,
                                                         TimePoint& timeOrderPlaced)
{
    auto validPrice = validatePrice(price);
    auto validQnty = validateQnty(qnty);
    auto validTimeOrderPlaced = validateTimeOrderPlaced(timeOrderPlaced);

    if (!validPrice)
    {
        return std::unexpected(validPrice.error());
    }

    if (!validQnty)
    {
        return std::unexpected(validQnty.error());
    }

    if (!validTimeOrderPlaced)
    {
        return std::unexpected(validTimeOrderPlaced.error());
    }

    return {};
}

}  // namespace

// ==================
// Order class
// ==================

namespace solstice
{

Order::Order(int uid, Underlying underlying, double price, double qnty, OrderSide orderSide,
             TimePoint timeOrderPlaced)
    : d_uid(uid),
      d_underlying(underlying),
      d_price(price),
      d_qnty(qnty),
      d_orderSide(orderSide),
      d_timeOrderPlaced(timeOrderPlaced)
{
    d_orderComplete = false;
    d_outstandingQnty = qnty;
}

const int Order::uid() const { return d_uid; }

const Underlying Order::underlying() const { return d_underlying; }

const double Order::price() const { return d_price; }

const double Order::qnty() const { return d_qnty; }

const double Order::outstandingQnty() const { return d_outstandingQnty; }

double Order::outstandingQnty(double newOutstandingQnty)
{
    d_outstandingQnty = newOutstandingQnty;
    return newOutstandingQnty;
};

const OrderSide Order::orderSide() const { return d_orderSide; }

const std::string Order::orderSideString() const
{
    return d_orderSide == solstice::OrderSide::Bid ? "Bid" : "ask";
}

const TimePoint Order::timeOrderPlaced() const { return d_timeOrderPlaced; }

const bool Order::orderComplete() const { return d_orderComplete; }

bool Order::orderComplete(bool isFulfilled)
{
    d_orderComplete = isFulfilled;
    return d_orderComplete;
}

const std::expected<TimePoint, std::string> Order::timeOrderFulfilled() const
{
    // Cannot return time of fulfillment if fulfillment hasn't yet occured
    if (!d_orderComplete)
    {
        return std::unexpected("Order has not been fulfilled yet\n");
    }
    return d_timeOrderFulfilled;
}

std::expected<std::shared_ptr<Order>, std::string> Order::createOrder(int uid,
                                                                      Underlying underlying,
                                                                      double price, double qnty,
                                                                      OrderSide orderSide)
{
    TimePoint timeOrderPlaced = getTimeNow();

    auto isOrderValid = validateOrderAttributes(price, qnty, timeOrderPlaced);
    if (!isOrderValid)
    {
        return std::unexpected(isOrderValid.error());
    }

    auto order = std::shared_ptr<Order>(
        new (std::nothrow) Order{uid, underlying, price, qnty, orderSide, timeOrderPlaced});

    return order;
}

std::ostream& operator<<(std::ostream& os, const Order& order)
{
    os << "Order UID: " << order.uid() << " | Ticker: " << to_string(order.underlying())
       << " | Price: " << order.price() << " | Quantity: " << order.qnty()
       << " | Is bid: " << std::boolalpha << (order.orderSide() == OrderSide::Bid);

    return os;
}
}  // namespace solstice
