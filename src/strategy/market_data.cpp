#include <market_data.h>

namespace solstice::strategy
{

MarketData::MarketData(std::vector<double> opens, std::vector<double> highs,
                       std::vector<double> lows, std::vector<double> closes,
                       std::vector<double> volume, std::vector<double> timestamps)
    : d_opens(opens),
      d_highs(highs),
      d_lows(lows),
      d_closes(closes),
      d_volume(volume),
      d_timestamps(timestamps)
{
}

const std::vector<double>& MarketData::opens() const { return d_opens; }
const std::vector<double>& MarketData::highs() const { return d_highs; }
const std::vector<double>& MarketData::lows() const { return d_lows; }
const std::vector<double>& MarketData::closes() const { return d_closes; }
const std::vector<double>& MarketData::volume() const { return d_volume; }
const std::vector<double>& MarketData::timestamps() const { return d_timestamps; }

MarketData MarketData::mapRawInput(RawMarketData& inputData)
{
    std::vector<double> opens = inputData["opens"];
    std::vector<double> highs = inputData["highs"];
    std::vector<double> lows = inputData["lows"];
    std::vector<double> closes = inputData["closes"];
    std::vector<double> volume = inputData["volume"];
    std::vector<double> timestamps = inputData["timestamps"];

    return MarketData(opens, highs, lows, closes, volume, timestamps);
}

}  // namespace solstice::strategy
