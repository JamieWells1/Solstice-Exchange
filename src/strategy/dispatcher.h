#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <market_data.h>
#include <strategy.h>

#include <memory>

namespace solstice::strategy
{

class Report
{
    int d_candlesAnalysed;
    int d_tradesCompleted;
    int d_longTrades;
    int d_shortTrades;

    double d_pnl;
    int d_winningTrades;
    int d_losingTrades;

    Report(int candlesAnalysed, int tradesCompleted, int longTrades, int shortTrades, double pnl,
           int winningTrades, int losingTrades);
};

class Dispatcher
{
   public:
    template <typename T>
    static const std::unique_ptr<T> constructStrategy(Strategy strategy, MarketData marketData);

    virtual Report execute() = 0;
    virtual ~Dispatcher() = default;

   protected:
    Dispatcher(Strategy strategy, MarketData marketData);

    Strategy d_strategy;
    MarketData d_marketData;
};

}  // namespace solstice::strategy

#endif  // DISPATCHER_H
