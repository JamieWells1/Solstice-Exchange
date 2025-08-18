#ifndef CONFIG_H
#define CONFIG_H

#include <ticker.h>

namespace solstice {

struct Config {
    int ordersToGenerate = 100;

    int minQuantity = 1;
    int maxQuantity = 100;

    double minPrice = 10.0;
    double maxPrice = 1000.0;

    double priceVolatility = 0.5;
};
}  // namespace solstice

#endif  // CONFIG_H
