#include <asset_class.h>
#include <get_random.h>
#include <market_side.h>
#include <order.h>

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

std::expected<void, std::string> validateOrderAttributes(double price, double qnty,
                                                         TimePoint& timeOrderPlaced)
{
    auto validPrice = validatePrice(price);
    auto validQnty = validateQnty(qnty);

    if (!validPrice)
    {
        return std::unexpected(validPrice.error());
    }

    if (!validQnty)
    {
        return std::unexpected(validQnty.error());
    }

    return {};
}

}  // namespace

// ==================
// Order class
// ==================

namespace solstice
{

Order::Order(int uid, Underlying underlying, double price, double qnty, MarketSide marketSide,
             TimePoint timeOrderPlaced)
    : d_uid(uid),
      d_underlying(underlying),
      d_price(price),
      d_qnty(qnty),
      d_marketSide(marketSide),
      d_timeOrderPlaced(timeOrderPlaced)
{
    d_orderComplete = false;
    d_outstandingQnty = qnty;
}

int Order::uid() const { return d_uid; }

Underlying Order::underlying() const { return d_underlying; }

double Order::price() const { return d_price; }

double Order::qnty() const { return d_qnty; }

double Order::outstandingQnty() const { return d_outstandingQnty; }

double Order::outstandingQnty(double newOutstandingQnty)
{
    d_outstandingQnty = newOutstandingQnty;
    return newOutstandingQnty;
};

MarketSide Order::marketSide() const { return d_marketSide; }

std::string Order::marketSideString() const
{
    return d_marketSide == solstice::MarketSide::Bid ? "Bid" : "ask";
}

TimePoint Order::timeOrderPlaced() const { return d_timeOrderPlaced; }

bool Order::orderComplete() const { return d_orderComplete; }

bool Order::orderComplete(bool isFulfilled)
{
    d_orderComplete = isFulfilled;
    return d_orderComplete;
}

std::expected<TimePoint, std::string> Order::timeOrderFulfilled() const
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
                                                                      MarketSide marketSide)
{
    TimePoint timeOrderPlaced = getTimeNow();

    auto isOrderValid = validateOrderAttributes(price, qnty, timeOrderPlaced);
    if (!isOrderValid)
    {
        return std::unexpected(isOrderValid.error());
    }

    auto order = std::shared_ptr<Order>(
        new (std::nothrow) Order{uid, underlying, price, qnty, marketSide, timeOrderPlaced});

    return order;
}

std::ostream& operator<<(std::ostream& os, const Order& order)
{
    os << "Order UID: " << order.uid() << " | Ticker: " << to_string(order.underlying())
       << " | Price: " << order.price() << " | Quantity: " << order.qnty()
       << " | Is bid: " << std::boolalpha << (order.marketSide() == MarketSide::Bid);

    return os;
}
}  // namespace solstice
