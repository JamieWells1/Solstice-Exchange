#include <dispatcher.h>
#include <sharp_movements.h>

#include <memory>

namespace solstice::strategy
{

Report::Report(int candlesAnalysed, int tradesCompleted, int longTrades, int shortTrades,
               double pnl, int winningTrades, int losingTrades)
    : d_candlesAnalysed(candlesAnalysed),
      d_tradesCompleted(tradesCompleted),
      d_longTrades(longTrades),
      d_shortTrades(shortTrades),
      d_pnl(pnl),
      d_winningTrades(winningTrades),
      d_losingTrades(losingTrades) {};

template <typename T>
const std::unique_ptr<T> Dispatcher::constructStrategy(Strategy strategy, MarketData marketData)
{
    switch (strategy)
    {
        case Strategy::SharpMovements:
            return std::make_unique<SharpMovements>(std::move(strategy), std::move(marketData));
        default:
            return nullptr;
    }
}

}  // namespace solstice::strategy
