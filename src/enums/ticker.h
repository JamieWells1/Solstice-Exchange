#ifndef TICKER_H
#define TICKER_H

#include <string>
#include <unordered_map>
#include <vector>

namespace solstice::matching
{

#define TICKER_LIST(X) \
    X(AAPL)            \
    X(MSFT)            \
    X(GOOGL)           \
    X(GOOG)            \
    X(AMZN)            \
    X(META)            \
    X(NVDA)            \
    X(AMD)             \
    X(INTC)            \
    X(QCOM)            \
    X(AVGO)            \
    X(ADBE)            \
    X(CRM)             \
    X(SNOW)            \
    X(NOW)             \
    X(ORCL)            \
    X(IBM)             \
    X(TSM)             \
    X(UBER)            \
    X(LYFT)            \
    X(JPM)             \
    X(BAC)             \
    X(WFC)             \
    X(C)               \
    X(GS)              \
    X(MS)              \
    X(BK)              \
    X(SCHW)            \
    X(BLKB)            \
    X(PYPL)            \
    X(SQ)              \
    X(V)               \
    X(MA)              \
    X(AXP)             \
    X(COIN)            \
    X(RBLX)            \
    X(XOM)             \
    X(CVX)             \
    X(SHEL)            \
    X(BP)              \
    X(TTE)             \
    X(SLB)             \
    X(HAL)             \
    X(PSX)             \
    X(VLO)             \
    X(KMI)             \
    X(TSLA)            \
    X(F)               \
    X(GM)              \
    X(TM)              \
    X(HMC)             \
    X(NKE)             \
    X(HD)              \
    X(LOW)             \
    X(MCD)             \
    X(SBUX)            \
    X(CMCSA)           \
    X(DIS)             \
    X(NFLX)            \
    X(ROKU)            \
    X(TGT)             \
    X(COST)            \
    X(WMT)             \
    X(BABA)            \
    X(PDD)             \
    X(SE)              \
    X(JNJ)             \
    X(PFE)             \
    X(MRK)             \
    X(ABBV)            \
    X(AMGN)            \
    X(GILD)            \
    X(LLY)             \
    X(BMY)             \
    X(UNH)             \
    X(CVS)             \
    X(MDT)             \
    X(TMO)             \
    X(BA)              \
    X(CAT)             \
    X(GE)              \
    X(UPS)             \
    X(FDX)             \
    X(DE)              \
    X(NOC)             \
    X(LMT)             \
    X(ETN)             \
    X(PEP)             \
    X(KO)              \
    X(PG)              \
    X(UL)              \
    X(CL)              \
    X(WBA)             \
    X(CLRN)            \
    X(MO)              \
    X(BUD)             \
    X(NEE)             \
    X(DUK)             \
    X(SO)              \
    X(D)               \
    X(T)               \
    X(VZ)              \
    X(TMUS)            \
    X(PLD)             \
    X(O)               \
    X(AMT)             \
    X(SPOT)            \
    X(ABNB)            \
    X(Z)               \
    X(DASH)            \
    X(SHOP)

enum class Ticker
{
#define X(name) name,
    TICKER_LIST(X)
#undef X
        INVALID
};

constexpr Ticker ALL_TICKERS[] = {
#define X(name) Ticker::name,
    TICKER_LIST(X)
#undef X
};

inline const std::unordered_map<std::string, Ticker> kTickerMap = {
#define X(name) {#name, Ticker::name},
    TICKER_LIST(X)
#undef X
};

inline const std::unordered_map<Ticker, std::string> kTickerNameMap = {
#define X(name) {Ticker::name, #name},
    TICKER_LIST(X)
#undef X
};

inline const std::vector<Ticker> kValidTickers = {
#define X(name) Ticker::name,
    TICKER_LIST(X)
#undef X
};

const std::vector<Ticker>& getTickerPool(int tkrPoolCount);

Ticker getRandomTkr(int tkrPoolCount);

Ticker validateTkr(const std::string& tkr);

const std::string tkrToString(Ticker tkr);

std::ostream& operator<<(std::ostream& os, Ticker tkr);

}  // namespace solstice::matching

#endif  // TICKER_H
