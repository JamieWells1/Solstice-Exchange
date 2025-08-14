// -1 to randomise a value
struct OrderConfig {
    // number of orders to generate (1 - 1000000)
    int ordersGenerated = 5;

    // percentage of orders to be BUY (0.1 - 1)
    double buyOrderPercentage = 0.5;

    // market volatility among stocks (0 - 1)
    double priceVolatility = 0.5;
};
