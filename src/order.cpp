#include <order.h>

// /utils
#include <getrandom.h>

#include <chrono>
#include <format>
#include <memory>
#include <stdexcept>

namespace {
void validatePrice(const double price) {
    if (price < 0) {
        throw std::invalid_argument(
            std::format("Invalid price: {}", price));
    }
}

void validateQnty(const double qnty) {
    if (qnty < 0) {
        throw std::invalid_argument(
            std::format("Invalid quantity: {}", qnty));
    }
}

void validateTimeOrderPlaced(const TimePoint timeOrderPlaced) {
    TimePoint now = std::chrono::system_clock::now();

    if (timeOrderPlaced > now) {
        throw std::invalid_argument(
            "Order cannot be placed in the future");
    }

    auto delayInSeconds = std::chrono::duration_cast<std::chrono::seconds>(
                              now - timeOrderPlaced)
                              .count();
    if (delayInSeconds > 10) {
        throw std::runtime_error(
            "Order timed out due to long processing time");
    }
}
}  // namespace

/*
Order class
*/

Order::Order(std::string uid, Ticker tkr, double price, double qnty,
             bool isBuy, TimePoint timeOrderPlaced)
    : d_uid(uid),
      d_tkr(tkr),
      d_price(price),
      d_qnty(qnty),
      d_isBuy(isBuy),
      d_timeOrderPlaced(timeOrderPlaced) {
    d_orderComplete = false;

    // these functions throw
    validatePrice(price);
    validateQnty(qnty);
    validateTimeOrderPlaced(timeOrderPlaced);
}

std::string Order::uid() const { return d_uid; }

Ticker Order::tkr() const { return d_tkr; }

double Order::price() const { return d_price; }

double Order::qnty() const { return d_qnty; }

bool Order::isBuy() const { return d_isBuy; }

TimePoint Order::timeOrderPlaced() const { return d_timeOrderPlaced; }

std::expected<TimePoint, std::string> Order::timeOrderFulfilled() const {
    // Cannot return time of fulfillment if fulfillment hasn't yet occured
    if (!d_orderComplete) {
        return std::unexpected("Order has not been fulfilled yet");
    }
    return d_timeOrderFulfilled;
}

std::expected<std::shared_ptr<Order>, std::string> Order::createOrder(
    Ticker tkr, double price, double qnty, bool isBuy,
    TimePoint timeOrderPlaced) {
    std::string uid = getRandomUid();

    try {
        auto order = std::shared_ptr<Order>(
            new Order{uid, tkr, price, qnty, isBuy, timeOrderPlaced});
        return order;

    } catch (const std::invalid_argument& e) {
        return std::unexpected(std::format(
            "Failed to push order {} due to invalid argument: {}", uid,
            e.what()));

    } catch (const std::runtime_error& e) {
        return std::unexpected(
            std::format("Failed to push order {} due to runtime error: {}",
                        uid, e.what()));
    }
}
