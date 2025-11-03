#ifndef ORDER_H
#define ORDER_H

#include <asset_class.h>
#include <market_side.h>
#include <time_point.h>

#include <ctime>
#include <expected>
#include <memory>
#include <string>

namespace solstice::pricing
{
class Pricer;
}

namespace solstice
{

class Order
{
   public:
    static std::expected<std::shared_ptr<Order>, std::string> create(int uid, Underlying underlying,
                                                                     double price, double qnty,
                                                                     MarketSide marketSide);

    static std::expected<std::shared_ptr<Order>, std::string> createWithPricer(
        std::shared_ptr<pricing::Pricer> pricer, Underlying underlying, int uid);

    int uid() const;
    Underlying underlying() const;
    double price() const;
    double qnty() const;
    double outstandingQnty() const;
    MarketSide marketSide() const;
    std::string marketSideString() const;
    TimePoint timeOrderPlaced() const;
    std::expected<TimePoint, std::string> timeOrderFulfilled() const;
    bool orderComplete() const;
    double outstandingQnty(double newQnty);
    bool orderComplete(bool isFulfilled);

    static double getRandomPrice(int minPrice, int maxPrice);
    static double getRandomQnty(int minQnty, int maxQnty);
    static MarketSide getRandomMarketSide();

   private:
    Order(int uid, Underlying underlying, double price, double qnty, MarketSide marketSide,
          TimePoint timeOrderPlaced);

    static std::expected<void, std::string> validatePrice(const double price);
    static std::expected<void, std::string> validateQnty(const double qnty);
    static std::expected<void, std::string> validateOrderAttributes(double price, double qnty,
                                                         TimePoint& timeOrderPlaced);

    int d_uid;
    Underlying d_underlying;
    double d_price;
    double d_qnty;
    double d_outstandingQnty;
    MarketSide d_marketSide;
    TimePoint d_timeOrderPlaced;
    TimePoint d_timeOrderFulfilled;
    bool d_orderComplete;
};

std::ostream& operator<<(std::ostream& os, const Order& order);

}  // namespace solstice

#endif
