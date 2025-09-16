#ifndef MATCH_H
#define MATCH_H

#include <order.h>

#include <expected>
#include <memory>
#include <string>

namespace solstice
{

using OrderPtr = std::shared_ptr<Order>;

class Matcher
{
   public:
   private:
    std::expected<void, std::string> processOrder(OrderPtr order);
};
}  // namespace solstice

#endif  // MATCH_H
