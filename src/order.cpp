#include <order.h>

// /utils
#include <getRandom.h>

#include <chrono>
#include <format>
#include <iostream>
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
        throw std::runtime_error("Order timed out due to long processing time");
    }
}
}  // namespace

/*
Order class
*/

Order::Order(Ticker tkr, double price, double qnty, bool isBuy,
             TimePoint timeOrderPlaced)
    : d_tkr(tkr),
      d_price(price),
      d_qnty(qnty),
      d_isBuy(isBuy),
      d_timeOrderPlaced(timeOrderPlaced) {
    d_uid = getRandomUid();
    d_orderComplete = false;

    try {
        validatePrice(price);
        validateQnty(qnty);
    } catch (const std::invalid_argument& e) {
        std::cerr
            << std::format(
                   "Failed to place order {} due to invalid argument: {}",
                   uid(), e.what())
            << std::endl;
    } catch (const std::runtime_error& e) {
        std::cerr << std::format(
            "Failed to place order {} due to a runtime error: {}", uid(),
            e.what()) << std::endl;
    }
};

int Order::uid() const { return d_uid; }

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
