#include <dispatcher.h>

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
      d_losingTrades(losingTrades)
{
}

Dispatcher::Dispatcher(Strategy strategy, MarketData marketData)
    : d_strategy(std::move(strategy)), d_marketData(std::move(marketData))
{
}

}  // namespace solstice::strategy
