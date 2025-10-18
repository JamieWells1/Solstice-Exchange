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
    // strategy implementation
}

}  // namespace solstice::strategy
