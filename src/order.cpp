#include <get_random.h>
#include <order.h>

#include <chrono>
#include <format>
#include <memory>

namespace
{

std::expected<void, std::string> validatePrice(const double price)
{
    if (price < 0)
    {
        return std::unexpected(std::format("Invalid price: {}", price));
    }

    return {};
}

std::expected<void, std::string> validateQnty(const double qnty)
{
    if (qnty < 0)
    {
        return std::unexpected(std::format("Invalid quantity: {}", qnty));
    }

    return {};
}

std::expected<void, std::string> validateTimeOrderPlaced(
    const TimePoint& timeOrderPlaced)
{
    TimePoint now = std::chrono::system_clock::now();

    if (timeOrderPlaced > now)
    {
        return std::unexpected(
            std::format("Order cannot be placed in the future"));
    }

    auto delayInSeconds = std::chrono::duration_cast<std::chrono::seconds>(
                              now - timeOrderPlaced)
                              .count();
    if (delayInSeconds > 10)
    {
        return std::unexpected(std::format(
            "Order timed out due to extensive processing time"));
    }

    return {};
}

std::expected<void, std::string> validateOrderAttributes(
    double price, double qnty, TimePoint& timeOrderPlaced)
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

Order::Order(std::string uid, Ticker tkr, double price, double qnty,
             OrderSide orderSide, TimePoint timeOrderPlaced)
    : d_uid(uid),
      d_tkr(tkr),
      d_price(price),
      d_qnty(qnty),
      d_orderSide(orderSide),
      d_timeOrderPlaced(timeOrderPlaced)
{
    d_orderComplete = false;
    d_outstandingQnty = qnty;
}

std::string Order::uid() const { return d_uid; }

Ticker Order::tkr() const { return d_tkr; }

double Order::price() const { return d_price; }

double Order::qnty() const { return d_qnty; }

double Order::outstandingQnty() const { return d_outstandingQnty; }

double Order::outstandingQnty(double newOutstandingQnty)
{
    d_outstandingQnty = newOutstandingQnty;
    return newOutstandingQnty;
};

OrderSide Order::orderSide() const { return d_orderSide; }

TimePoint Order::timeOrderPlaced() const { return d_timeOrderPlaced; }

bool Order::orderComplete() const { return d_orderComplete; }

bool Order::orderComplete(bool isFulfilled)
{
    d_orderComplete = isFulfilled;
}

std::expected<TimePoint, std::string> Order::timeOrderFulfilled() const
{
    // Cannot return time of fulfillment if fulfillment hasn't yet occured
    if (!d_orderComplete)
    {
        return std::unexpected("Order has not been fulfilled yet");
    }
    return d_timeOrderFulfilled;
}

std::expected<std::shared_ptr<Order>, std::string> Order::createOrder(
    Ticker tkr, double price, double qnty, OrderSide orderSide)
{
    std::string uid = Random::getRandomUid();
    TimePoint timeOrderPlaced = getTimeNow();

    auto isOrderValid =
        validateOrderAttributes(price, qnty, timeOrderPlaced);
    if (!isOrderValid)
    {
        return std::unexpected(isOrderValid.error());
    }

    auto order = std::shared_ptr<Order>(new (std::nothrow) Order{
        uid, tkr, price, qnty, orderSide, timeOrderPlaced});

    return order;
}

std::ostream& operator<<(std::ostream& os, const Order& order)
{
    os << "Order UID: " << order.uid() << " | Ticker: " << order.tkr()
       << " | Price: " << order.price() << " | Quantity: " << order.qnty()
       << " | Is buy: " << std::boolalpha
       << (order.orderSide() == OrderSide::Buy);

    return os;
}
}  // namespace solstice
