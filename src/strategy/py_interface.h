#ifndef PY_INTERFACE_H
#define PY_INTERFACE_H

#include <config.h>
#include <market_data.h>
#include <strategy.h>

#include <expected>

#include "dispatcher.h"

class PyInterface
{
   public:
    static std::expected<PyInterface, std::string> establish();

    solstice::strategy::Strategy strategy() const;

    template <typename T>
    std::expected<solstice::strategy::Report, std::string> orchestrate(RawMarketData& rawData);
};

#endif  // PY_INTERFACE_H
