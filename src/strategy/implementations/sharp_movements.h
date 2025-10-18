#ifndef SHARP_MOVEMENTS_H
#define SHARP_MOVEMENTS_H

#include <dispatcher.h>
#include <market_data.h>
#include <strategy.h>

namespace solstice::strategy
{

class SharpMovements : public Dispatcher
{
   public:
    SharpMovements(Strategy strategy, MarketData marketData);

    Report execute() override;

   private:
};

}  // namespace solstice::strategy

#endif  // SHARP_MOVEMENTS_H
