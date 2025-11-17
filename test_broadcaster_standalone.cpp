#include <asset_class.h>
#include <broadcaster.h>
#include <order.h>
#include <transaction.h>

#include <chrono>
#include <iostream>
#include <thread>

using namespace solstice;
using namespace solstice::matching;
using namespace solstice::broadcaster;

int main()
{
    Broadcaster broadcaster_instance(8080);

    for (int i = 0; i < 10; i++)
    {
        std::this_thread::sleep_for(std::chrono::seconds(2));

        auto order = Order::create(i, Equity::AAPL, 150.0 + i, 100, MarketSide::Bid);
        if (order)
        {
            broadcaster_instance.broadcastOrder(*order);
        }

        broadcaster_instance.broadcastBookUpdate(Equity::AAPL, std::optional<double>(150.0 + i),
                                                 std::optional<double>(151.0 + i));
    }

    std::cin.get();

    return 0;
}
