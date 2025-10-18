#include <market_data.h>
#include <sharp_movements.h>
#include <strategy.h>

namespace solstice::strategy
{

SharpMovements::SharpMovements(Strategy strategy, MarketData marketData)
    : Dispatcher(std::move(strategy), std::move(marketData))
{
}

Report SharpMovements::execute()
{
    return Report(1,2,3,4,5,6,7);
}

}  // namespace solstice::strategy
