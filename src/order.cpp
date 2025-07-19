#include <order.h>

// /utils
#include <getRandom.h>

#include <ctime>
#include <format>
#include <stdexcept>

Order::Order(Ticker tkr, double price, double qnty, bool isBuy,
             time_t timeOrderPlaced)
    : d_tkr(tkr),
      d_price(price),
      d_qnty(qnty),
      d_isBuy(isBuy),
      d_timeOrderPlaced(timeOrderPlaced) {
    d_uid = getRandomUid();
    d_orderComplete = false;

    if (!validatePrice(price)) {
        throw std::invalid_argument(std::format(
            "Invalid price for order {} : {}", uid(), price));
    }
};

int Order::uid() const { return d_uid; }

Ticker Order::tkr() const { return d_tkr; }

double Order::price() const { return d_price; }

double Order::qnty() const { return d_qnty; }

bool Order::isBuy() const { return d_isBuy; }

time_t Order::timeOrderPlaced() const { return d_timeOrderPlaced; }

std::expected<time_t, std::string> Order::timeOrderFulfilled() const {
    // Cannot return time of fulfillment if fulfillment hasn't yet occured
    if (!d_orderComplete) {
        return std::unexpected("Order has not been fulfilled yet");
    }
    return d_timeOrderFulfilled;
}

/*
Methods to validate order
*/

    bool Order::validatePrice(const double price) {
        if (d_price < 0) {
            return false;
        }
        return true;
    }
