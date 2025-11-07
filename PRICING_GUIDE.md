# SOLSTICE PRICING GUIDE

## Custom Implementation Guide for Your Codebase

---

## TABLE OF CONTENTS

1. [Your Current Architecture](#your-current-architecture)
2. [The Fundamental Problem](#the-fundamental-problem)
3. [What to Build in Your Pricer Class](#what-to-build-in-your-pricer-class)
4. [Integrating with Your Order Generation](#integrating-with-your-order-generation)
5. [Initializing Your Futures Contracts](#initializing-your-futures-contracts)
6. [Realistic Spreads and Market Makers](#realistic-spreads-and-market-makers)
7. [Practical Parameter Tuning](#practical-parameter-tuning)
8. [Connecting to Your Backtesting System](#connecting-to-your-backtesting-system)
9. [Thread Safety Considerations](#thread-safety-considerations)
10. [Step-by-Step Implementation Plan](#step-by-step-implementation-plan)
11. [Testing Strategy](#testing-strategy)
12. [Future Enhancements (Beyond v0.3)](#future-enhancements-beyond-v03)
13. [Common Pitfalls to Avoid](#common-pitfalls-to-avoid)
14. [Verification Checklist](#verification-checklist)
15. [Quick Reference: Key Formulas](#quick-reference-key-formulas)
16. [Your Codebase-Specific Notes](#your-codebase-specific-notes)
17. [Final Thoughts](#final-thoughts)

---

## YOUR CURRENT ARCHITECTURE

### What You Already Have

**Order Flow System:**

- `Order` class stores: uid, underlying (variant of Equity or Future), price, quantity, outstanding quantity, market side (Bid/Ask), timestamps
- `OrderBook` manages active orders in price-level maps (bids sorted descending, asks sorted ascending)
- `Matcher` handles price-time priority matching between incoming and resting orders
- `Transaction` records completed trades with both order uids, price, quantity, and execution time
- `Orchestrator` generates random orders and coordinates multi-threaded order processing

**Asset Classes:**

- Two main types: `AssetClass::Equity` and `AssetClass::Future`
- Equities: AAPL, MSFT, GOOGL, AMZN, META, BLK, NVDA, AMD, INTC, QCOM, JPM, BAC, CRM, GS, MS, ORCL, IBM, TSM, UBER, LYFT (20 total)
- Futures: Quarterly contracts (MAR25, JUN25, SEP25, DEC25) for AAPL, MSFT, TSLA (12 total)
- Uses `std::variant<Equity, Future>` as the `Underlying` type

**Configuration System:**

- `Config` singleton controls simulation parameters
- Currently generates random prices between `minPrice` and `maxPrice` (default 8.0 to 10.0)
- Random quantities between `minQnty` and `maxQnty` (default 1 to 20)
- Random selection of underlying from active pool
- 50/50 random choice of Bid vs Ask side

**Empty Pricing Component:**

- `pricing::Pricer` class exists but is completely empty
- This is where your futures pricing logic should live

**Strategy/Backtesting:**

- `MarketData` struct holds OHLCV (open, high, low, close, volume) time series data
- `Account` tracks balance and positions
- `Position` tracks entry/exit prices and position type (long/short)
- SharpMovements strategy exists but pricing isn't connected yet

---

## THE FUNDAMENTAL PROBLEM

### Current State: Pure Random Chaos

Your `Orchestrator::generateOrder()` function (lines 79-101 in orchestrator.cpp) currently does this:

1. Pick random underlying from pool
2. Generate random price uniformly between 8.0 and 10.0
3. Generate random quantity between 1 and 20
4. Randomly choose Bid or Ask

**Why this is problematic:**

If you're trading AAPL futures expiring in March 2025, one order might come in at $8.23 and the next at $9.87, with no relationship between them or to any theoretical value. There's no anchor. Prices bounce around randomly with no gravity pulling them toward a fair value.

For equities this is somewhat acceptable (price is whatever the market says), but for futures it's unrealistic because futures have a calculable theoretical value based on the underlying spot price, interest rates, and dividends.

### What You Need: Anchored Price Discovery

**For equities:**
Price should evolve from trade to trade. If AAPL last traded at $175.50, the next orders should cluster around $175.50 (maybe $175.45 to $175.55 most of the time), not jump to $8.00 or $200.00.

**For futures:**
Calculate a theoretical fair value using the cost-of-carry formula, then generate orders that cluster around this fair value. Market price (from actual trades) will naturally oscillate around fair value.

---

## WHAT TO BUILD IN YOUR PRICER CLASS

### Architecture Design

The `pricing::Pricer` class should become a pricing engine that tracks and calculates prices for all your underlyings. Here's what it needs to store and do:

### Data Storage Needed

**For All Underlyings (Both Equity and Future):**

- Last traded price (the most recent execution price from your order book)
- Best bid price (highest buy order currently in book)
- Best ask price (lowest sell order currently in book)
- Last update timestamp
- Thread-safe access (use `std::shared_mutex` - multiple readers, single writer)

**Additionally For Futures Only:**

- The underlying equity it's based on (AAPL futures → AAPL equity)
- Expiry timestamp (when the contract expires)
- Risk-free interest rate (annualized, stored as decimal like 0.05 for 5%)
- Dividend yield (annualized, stored as decimal like 0.02 for 2%)
- Spot price of underlying equity (read from equity pricing data)

### Core Functions You Need

**Equity Functions:**

`void updateEquityTrade(Equity symbol, double tradePrice, TimePoint timestamp)`

- Called whenever a trade executes for this equity
- Updates the last traded price and timestamp
- Thread-safe write operation

`void updateEquityBookPrices(Equity symbol, double bestBid, double bestAsk)`

- Called whenever the best bid or ask changes in the order book
- Updates the stored bid/ask prices
- Thread-safe write operation

`std::expected<double, std::string> getEquityPrice(Equity symbol)`

- Returns the last traded price for this equity
- If no trades yet, returns midpoint of bid/ask if available
- If no orders in book yet, returns error or a seed price
- Thread-safe read operation

**Futures Functions:**

`void initializeFuture(Future symbol, Equity underlying, TimePoint expiry, double riskFreeRate, double dividendYield)`

- Sets up a futures contract with all necessary parameters
- Called once during initialization for each future you're trading
- Store the linkage between the future and its underlying equity

`std::expected<double, std::string> calculateFairValue(Future symbol, TimePoint currentTime)`

- The core pricing calculation
- Retrieves spot price of underlying equity from equity pricing data
- Calculates time to expiry in years
- Applies cost-of-carry formula: F = S × e^((r - q) × T)
- Returns the theoretical fair value
- Thread-safe read operation

`void updateFutureTrade(Future symbol, double tradePrice, TimePoint timestamp)`

- Called when a trade executes for this future
- Updates last traded price (the actual market price)
- Thread-safe write operation

`std::expected<double, std::string> getFuturePrice(Future symbol, TimePoint currentTime)`

- Can return either fair value or last traded price depending on use case
- For order generation, probably return fair value
- For market data recording, return last traded price if available

**Helper Functions:**

`double timeToExpiryInYears(TimePoint current, TimePoint expiry)`

- Calculates difference between timestamps
- Converts to years as a double
- Returns 0 if current >= expiry (contract expired)

`bool isExpired(Future symbol, TimePoint currentTime)`

- Checks if current time is past expiry
- Used to avoid trading expired contracts

### The Cost-of-Carry Formula Implementation

Here's the logic in plain English for `calculateFairValue()`:

1. Get current time and expiry time for this contract
2. If current time >= expiry time, return spot price (futures converge to spot at expiry)
3. Calculate time difference in nanoseconds
4. Convert to years by dividing by (365.25 × 24 × 3600 × 1,000,000,000)
5. Look up the spot price of the underlying equity using `getEquityPrice()`
6. If spot price not available, return error
7. Calculate the net carry rate: (riskFreeRate - dividendYield)
8. Calculate the exponent: netCarryRate × timeToExpiryInYears
9. Calculate e to the power of exponent using `std::exp(exponent)`
10. Multiply spot price by this exponential factor
11. Return the result

**Example calculation:**

- AAPL spot price: $175.00
- Risk-free rate: 5% (0.05)
- Dividend yield: 2% (0.02)
- Time to expiry: 90 days = 0.2464 years
- Net carry: 0.05 - 0.02 = 0.03
- Exponent: 0.03 × 0.2464 = 0.007392
- e^0.007392 = 1.007417
- Fair value: $175.00 × 1.007417 = $175.65

So AAPL MAR25 futures should trade around $175.65 if spot is $175.00. This is contango (futures above spot) because interest costs exceed dividend income.

---

## INTEGRATING WITH YOUR ORDER GENERATION

### Current Order Generation Flow

In `Orchestrator::generateOrder()` (orchestrator.cpp:79-101):

```
1. Generate uid
2. Get random underlying (equity or future)
3. Generate random price between config min/max
4. Generate random quantity between config min/max
5. Randomly choose Bid or Ask
6. Create Order object
7. Return order
```

### Modified Flow With Pricing

**New approach:**

```
1. Generate uid
2. Get random underlying (equity or future)
3. Get anchor price from Pricer:
   - If equity: get last traded price (or seed price if none)
   - If future: calculate fair value
4. Generate price around anchor using normal distribution:
   - Mean = anchor price
   - Standard deviation = 0.5% to 1.0% of anchor price
5. Generate random quantity (keep current logic)
6. Randomly choose Bid or Ask (keep current logic)
7. Create Order object
8. Return order
```

**What this achieves:**

Orders now cluster around realistic prices. If AAPL last traded at $175.50, orders will mostly be between $174.50 and $176.50 (roughly). Occasionally you'll get outliers at $170 or $180 due to the normal distribution tail, which is realistic.

For futures, orders cluster around the theoretical fair value, creating realistic price discovery where the actual market price (from executed trades) oscillates around fair value.

### Implementation Details

You'll need to modify `Orchestrator::generateOrder()`:

**Step 1: Get the anchor price**

```
If underlying is Equity:
    Try to get last traded price from Pricer
    If no price yet, use a seed price (like $175 for AAPL, $350 for MSFT, etc.)

If underlying is Future:
    Calculate fair value using Pricer.calculateFairValue()
    This automatically pulls spot price of underlying equity
```

**Step 2: Generate distributed price**

Use `std::normal_distribution` with:

- Mean = anchor price from step 1
- StdDev = anchor price × 0.005 (for 0.5% volatility) or × 0.01 (for 1% volatility)

This creates realistic clustering. About 68% of orders will be within 1 standard deviation, 95% within 2 standard deviations, with occasional outliers.

**Step 3: Handle edge cases**

- Ensure generated price is positive (if normal distribution gives negative, regenerate or use absolute value)
- Consider rounding to realistic tick sizes (equities trade in cents, futures might trade in different increments)
- For bids, you might bias slightly lower than anchor; for asks slightly higher (creates natural spread)

### Updating Prices After Trades

You need to hook into your matching engine to update the Pricer when trades occur.

**In Matcher::matchOrder()** (matcher.cpp:81-191):

Currently when a trade executes, you:

1. Update outstanding quantities on orders
2. Call `orderBook->markOrderAsFulfilled()`
3. Return success message

**Add this:**

After a successful match, extract the trade price and call:

```
pricer.updateEquityTrade() or pricer.updateFutureTrade()
```

This keeps the Pricer in sync with actual market prices from your order book.

**Where to get trade price:**

The `getDealPrice()` function (matcher.cpp:23-47) already calculates the actual execution price based on price-time priority. This is the price that should update your Pricer.

### Updating Best Bid/Ask

Your `OrderBook` already tracks best prices through the sorted price level maps:

- `BidPricesAtPriceLevel` (sorted descending with `std::greater<double>`)
- `askPricesAtPriceLevel` (sorted ascending with `std::less<double>`)

**When to update Pricer:**

After these operations in OrderBook:

- `addOrderToBook()` - might create new best bid/ask
- `markOrderAsFulfilled()` - might remove best bid/ask

**How to update:**

Get the first element from the appropriate sorted set:

- Best bid = first element of bidPrices set
- Best ask = first element of askPrices set

Call `pricer.updateEquityBookPrices()` or similar for futures.

---

## INITIALIZING YOUR FUTURES CONTRACTS

### What Parameters You Need

For each of your 12 futures contracts, you need to decide:

**Expiry Dates:**

Your contracts are named MAR25, JUN25, SEP25, DEC25. Standard futures expiry is typically third Friday of the month. For 2025:

- MAR25: March 21, 2025
- JUN25: June 20, 2025
- SEP25: September 19, 2025
- DEC25: December 19, 2025

Convert these to `TimePoint` by constructing a date and time (typically expires at market close, maybe 16:00 EST).

**Risk-Free Rate:**

Use a realistic current rate. As of late 2024/early 2025, US risk-free rate is around 4-5%. You could use:

- 0.045 (4.5%) as a reasonable middle ground
- Make it configurable in your Config class

**Dividend Yields:**

These vary by stock:

- Tech stocks (AAPL, MSFT, TSLA): typically low yields, 0.5% to 1.5%
- TSLA specifically: near zero or zero (doesn't pay much dividend)
- You could use:
  - AAPL: 0.005 (0.5%)
  - MSFT: 0.01 (1%)
  - TSLA: 0.0 (0%)

**Underlying Linkages:**

- AAPL_MAR25 → Equity::AAPL
- AAPL_JUN25 → Equity::AAPL
- AAPL_SEP25 → Equity::AAPL
- AAPL_DEC25 → Equity::AAPL
- MSFT_MAR25 → Equity::MSFT
- (and so on)

### Initialization Sequence

When your simulation starts (in `Orchestrator::initialiseUnderlyings()`):

**Step 1: Initialize equities with seed prices**

Set starting spot prices for each equity:

```
AAPL: $175.00
MSFT: $350.00
TSLA: $180.00
(Or pull from real market data if available)
```

**Step 2: Initialize futures with parameters**

For each future contract:

```
Call pricer.initializeFuture(
    Future::AAPL_MAR25,
    Equity::AAPL,
    expiryTimePoint for March 21 2025,
    0.045,  // risk-free rate
    0.005   // dividend yield
)
```

**Step 3: Calculate initial fair values**

Before generating any orders, calculate fair values for all futures. This gives you the anchor prices for order generation.

---

## REALISTIC SPREADS AND MARKET MAKERS

### Current Problem

With pure random orders, your spread (difference between best bid and best ask) is chaotic. Sometimes huge, sometimes non-existent, purely based on random luck.

### Market Maker Approach (Optional Enhancement)

A market maker is an entity that continuously provides both buy and sell quotes to ensure liquidity. This is optional for v0.3 but makes your simulation much more realistic.

**How it works:**

Create a separate thread or periodic task that:

1. For each active underlying, get current fair value (for futures) or last price (for equities)
2. Calculate a spread (typically 0.1% of price, or 10 basis points)
3. Calculate bid as: price - (spread / 2)
4. Calculate ask as: price + (spread / 2)
5. Submit bid order at this price with decent size (like 100 units)
6. Submit ask order at this price with same size
7. Sleep for a short interval (100ms)
8. Cancel old orders and repeat with updated prices

**Why this helps:**

- Ensures there's always liquidity (orders to trade against)
- Creates realistic bid-ask spreads
- Provides a baseline around fair value
- Noise traders (your random orders) then interact with these quotes

**Implementation in your codebase:**

Add a `marketMakerThread()` method to Orchestrator similar to `workerThread()`. Have it run in parallel with your order generation, continuously updating bid/ask quotes around current prices.

---

## PRACTICAL PARAMETER TUNING

### For Config Class

**Price range parameters:**

Currently you have `minPrice` and `maxPrice` (8.0 to 10.0). With the new system:

- These become less relevant because price is determined by Pricer
- Could repurpose them as:
  - `priceVolatility`: controls standard deviation multiplier (0.005 to 0.02)
  - `spreadWidth`: controls market maker spread (0.001 to 0.01)

**Quantity parameters:**

Keep as-is. Random quantities between 1 and 20 is fine.

**Order flow rate:**

Your `ordersToGenerate` (default 10000) and threading determine how fast orders arrive. For realistic simulation:

- 10,000 orders over a few seconds is very high frequency
- Consider adding a sleep/delay in order generation for slower, more realistic flow
- Or keep it fast for stress testing your matching engine

### For Futures Pricing

**Time period to simulate:**

If you're simulating a trading day or week, decide:

- Starting time (e.g., market open at 9:30 AM EST on a specific date)
- Ending time (e.g., market close at 4:00 PM EST)
- Whether spot prices of equities should drift during simulation (adds realism)

**Spot price evolution:**

Currently spot prices would be static (set at initialization). For more realism:

- Periodically update equity prices based on recent trades
- Use a random walk or mean-reverting process
- This causes futures fair values to change dynamically

**Interest rate and dividend yield:**

These can be static for a single simulation run. In reality they're constant in the short term (days/weeks).

---

## CONNECTING TO YOUR BACKTESTING SYSTEM

### MarketData Integration

Your `MarketData` struct holds OHLCV time series. This is typically used for backtesting strategies against historical data.

**How Pricer connects:**

When backtesting:

1. Feed historical price data into MarketData
2. Use this data to set spot prices in your Pricer
3. Calculate futures fair values based on historical spot prices
4. Your strategy (SharpMovements) analyzes this data and makes trading decisions

**Implementation approach:**

The `mapRawInput()` function takes `RawMarketData` (a map of string to vector of doubles). This is historical data from external source.

Add a method to Pricer:

```
void loadHistoricalSpotPrices(Equity symbol, const std::vector<double>& closes, const std::vector<double>& timestamps)
```

For each timestamp in the historical data:

- Set the equity spot price
- Recalculate all futures fair values that depend on this equity
- Record these for use in strategy backtesting

### Strategy Execution

Your `Dispatcher` executes strategies and generates a `Report` with PnL, winning/losing trades, etc.

**How pricing helps:**

When your strategy decides to enter a long or short position:

1. It needs to know at what price it can execute
2. Use Pricer to get current best ask (for buying) or best bid (for selling)
3. Or use fair value to estimate realistic execution price
4. Record this in the Position object (entryPrice)

When exiting: 5. Use updated Pricer values to get exit price 6. Calculate PnL as (exitPrice - entryPrice) × positionSize × direction

**Example flow:**

SharpMovements strategy detects a sharp price movement in AAPL. It decides to buy AAPL_JUN25 futures.

1. Call `pricer.getFuturePrice(Future::AAPL_JUN25, currentTime)` to get fair value
2. Assume execution at or near this price
3. Create Position with this entry price
4. Later when exiting, get updated fair value
5. Calculate PnL

---

## THREAD SAFETY CONSIDERATIONS

### Your Current Threading Model

`Orchestrator` uses:

- Multiple worker threads pulling from order queue (`workerThread()`)
- Mutex per underlying to prevent race conditions when accessing same order book
- Queue mutex for order queue access

### Pricer Threading Needs

The Pricer will be accessed by:

- Order generation (reading prices to generate order prices)
- Matching engine (writing prices after trades execute)
- Market maker thread if you add one (reading and writing)

**Solution: shared_mutex**

Use `std::shared_mutex` for each underlying's price data:

- Multiple readers can read simultaneously (shared lock)
- Writers get exclusive access (unique lock)
- Order generation mostly reads (shared lock) - very fast
- Trade updates write (unique lock) - infrequent but requires exclusivity

**Implementation pattern:**

```
In Pricer class:
    std::unordered_map<Underlying, std::shared_mutex> pricingMutexes

When reading price:
    std::shared_lock lock(pricingMutexes[underlying])
    // read price data

When writing price:
    std::unique_lock lock(pricingMutexes[underlying])
    // update price data
```

This is similar to your existing `d_underlyingMutexes` in Orchestrator but uses shared_mutex for better read performance.

---

## STEP-BY-STEP IMPLEMENTATION PLAN

### Phase 1: Basic Pricer Structure (30-45 minutes)

1. Define data structures in pricer.h:

   - Struct for equity price data (last trade, bid, ask, timestamp)
   - Struct for futures data (underlying link, expiry, rates, last trade)
   - Maps to store these keyed by Equity/Future enum
   - shared_mutex map for thread safety

2. Implement basic getters/setters:

   - Initialize equity with seed price
   - Update equity price after trade
   - Get equity price for order generation

3. Test with equity-only simulation:
   - Modify Orchestrator to use Pricer for equity prices
   - Verify prices evolve realistically from trade to trade

### Phase 2: Futures Pricing (45-60 minutes)

1. Implement futures initialization:

   - Function to set up each future with expiry, rates, underlying link
   - Create helper to calculate time to expiry in years

2. Implement fair value calculation:

   - Core cost-of-carry formula
   - Handle edge cases (expired contracts, missing spot price)

3. Add futures parameters to Config or hardcode initial values

4. Test calculation manually with known values to verify correctness

### Phase 3: Order Generation Integration (30 minutes)

1. Modify `Orchestrator::generateOrder()`:

   - Get anchor price from Pricer instead of random in range
   - Use normal distribution around anchor
   - Handle both equity and future cases

2. Test that orders now cluster around fair value

3. Verify market price (from trades) oscillates around fair value

### Phase 4: Price Updates (30 minutes)

1. Hook into `Matcher::matchOrder()`:

   - After successful trade, extract trade price
   - Call Pricer update method with trade price

2. Hook into `OrderBook` operations:

   - After adding/removing orders, check if best bid/ask changed
   - Update Pricer with new best prices

3. Verify that Pricer prices stay in sync with order book state

### Phase 5: Testing and Validation (30 minutes)

1. Run simulation with futures
2. Log fair values and market prices over time
3. Verify:

   - Futures price above spot when interest > dividend (contango)
   - Market price oscillates around fair value
   - Prices converge as expiry approaches
   - No crashes or deadlocks from threading

4. Sanity checks:
   - Futures within 10% of spot
   - Spreads are reasonable (not huge gaps)
   - Order flow generates realistic price discovery

### Phase 6: Market Maker (Optional, 45 minutes)

1. Implement market maker logic in separate method
2. Continuously quote bid/ask around fair value
3. Run in parallel with order generation
4. Verify tighter spreads and better liquidity

**Total time: 3-4 hours for basic implementation, 4-5 hours with market maker**

---

## TESTING STRATEGY

### Unit Tests

**Test equity pricing:**

- Initialize with seed price, verify retrieval
- Update with trade, verify new price stored
- Concurrent reads from multiple threads

**Test futures calculation:**

- Known inputs → verify correct output
- Example: spot=100, r=0.05, q=0.02, T=0.25 years → verify F ≈ 100.75
- Test expiry case (T=0) → should return spot
- Test extreme rates → should log warnings

### Integration Tests

**Test order generation:**

- Generate 1000 orders for AAPL futures
- Verify orders cluster around calculated fair value
- Plot histogram of order prices - should be bell curve centered on fair value

**Test price evolution:**

- Start simulation, let it run 1000 trades
- Track market price over time
- Verify it oscillates around fair value without wild jumps

### Sanity Checks During Development

After each phase, print:

- Current fair value for each future
- Last traded market price
- Difference between fair and market (should be small)
- Best bid and best ask (spread should be reasonable)

**Red flags to watch for:**

- Futures price way above or below spot (>10% difference) - bug in calculation
- Market price drifting away from fair value continuously - order generation issue
- Huge bid-ask spreads (>5% of price) - not enough orders or market maker needed
- Negative prices - distribution issue in order generation
- Crashes or deadlocks - threading/mutex issue

---

## FUTURE ENHANCEMENTS (BEYOND V0.3)

### Dynamic Spot Price Evolution

Instead of static spot prices, implement a price process:

- Geometric Brownian Motion (realistic stock price movement)
- Mean reversion (prices drift back toward long-term average)
- Jump diffusion (occasional large moves)

This makes futures pricing dynamic as spot evolves.

### Multiple Interest Rates

Use different rates for different maturities:

- Short-term rate for near expiry (MAR25)
- Long-term rate for far expiry (DEC25)
- Models the yield curve

### Implied Dividend Yields

Instead of hardcoded yields, calculate implied yield from market prices:

- Rearrange formula to solve for q given observed futures price
- This is what traders do in real markets

### Correlation Between Assets

Currently AAPL, MSFT, TSLA move independently. Add correlation:

- When AAPL spot moves up, MSFT tends to move up too
- Tech sector correlation
- Makes simulation more realistic

### Order Types

Currently only limit orders. Add:

- Market orders (execute immediately at best price)
- Stop orders (trigger at certain price)
- Iceberg orders (hidden quantity)

### Market Impact

Large orders should move the price more than small orders:

- Model the price impact of order size
- Affects execution price realistically

---

## COMMON PITFALLS TO AVOID

**Time Units:**

Your `TimePoint` is `std::chrono::system_clock::time_point`. When calculating time to expiry:

- Don't mix nanoseconds, seconds, days carelessly
- Use `std::chrono::duration` to handle conversions
- Test with a known time difference to verify calculation

**Rate Units:**

Interest rate and dividend yield MUST be decimals (0.05 for 5%), not percentages (5). If you use percentages, your exponential will explode.

**Mutex Deadlocks:**

If Pricer and OrderBook both have mutexes:

- Always lock in consistent order
- Don't call OrderBook method while holding Pricer lock if OrderBook might call back to Pricer
- Use RAII lock guards to ensure unlocking

**Variant Handling:**

Your `Underlying` is a `std::variant<Equity, Future>`. When passing to Pricer:

- Use `std::visit()` to determine which type
- Or use separate maps for Equity and Future in Pricer
- Don't assume the variant holds a specific type without checking

**Expired Contracts:**

Don't generate orders for expired futures:

- Check expiry before selecting a future
- Or filter expired futures from the pool

**Division By Zero:**

When calculating time to expiry, if current time equals expiry:

- Don't divide by zero
- Return spot price immediately

**Race Conditions:**

Multiple threads updating last traded price simultaneously:

- Without mutex, updates can be lost
- With wrong mutex, deadlock can occur
- Test with high concurrency to expose issues

---

## VERIFICATION CHECKLIST

Before considering your pricing implementation complete:

**Calculation Correctness:**

- [ ] Manually calculate fair value for one future with known parameters
- [ ] Verify your code produces same result
- [ ] Test edge case: expired contract returns spot
- [ ] Test edge case: zero dividend yield
- [ ] Test edge case: dividend yield exceeds interest rate (backwardation)

**Integration Correctness:**

- [ ] Orders generate with prices near fair value, not random range
- [ ] After trades execute, Pricer updates with trade price
- [ ] Market price visible in logs/output
- [ ] Market price oscillates around fair value over time
- [ ] Best bid is below fair value, best ask is above (spread straddles fair value)

**Thread Safety:**

- [ ] Run with maximum threads (hardware_concurrency)
- [ ] No crashes or deadlocks after 100,000 orders
- [ ] No data races detected by ThreadSanitizer (if available)

**Performance:**

- [ ] Pricing calculations don't slow down order generation
- [ ] Shared locks allow concurrent reads
- [ ] No excessive lock contention

**Realism:**

- [ ] Spreads are reasonable (0.1% to 1% of price)
- [ ] Price discovery looks realistic (not random walks)
- [ ] Futures converge toward spot as expiry approaches
- [ ] Contango/backwardation relationship correct based on rates

---

## QUICK REFERENCE: KEY FORMULAS

**Futures Fair Value:**

```
F = S × e^((r - q) × T)

Where:
F = futures price we're calculating
S = spot price of underlying (from equity pricer)
r = risk-free rate (decimal, e.g., 0.05)
q = dividend yield (decimal, e.g., 0.02)
T = time to expiry in years

If r > q: futures trades above spot (contango)
If r < q: futures trades below spot (backwardation)
At expiry (T=0): F = S
```

**Time to Expiry in Years:**

```
Get current TimePoint
Get expiry TimePoint
Subtract to get duration
Convert to seconds: std::chrono::duration<double> seconds = expiry - current
Divide by seconds in a year: T = seconds.count() / (365.25 * 24 * 3600)
```

**Normal Distribution for Order Prices:**

```
std::normal_distribution<double> dist(anchorPrice, anchorPrice * volatility)
double orderPrice = dist(randomGenerator)

Where:
anchorPrice = fair value (futures) or last trade (equity)
volatility = 0.005 to 0.01 typically (0.5% to 1%)
```

**Bid-Ask Spread:**

```
spreadWidth = anchorPrice * 0.001  (0.1% spread)
bid = anchorPrice - (spreadWidth / 2)
ask = anchorPrice + (spreadWidth / 2)
```

---

## YOUR CODEBASE-SPECIFIC NOTES

**Namespaces:**

- Your code uses `solstice` namespace with sub-namespaces (`matching`, `strategy`, `pricing`)
- Pricer should be in `solstice::pricing`
- Access other components as `solstice::matching::OrderBook`, etc.

**Variant Usage:**

- `Underlying` is `std::variant<Equity, Future>`
- Use as key in maps requires std::hash specialization (already done)
- Use `std::holds_alternative<Equity>(underlying)` to check type
- Use `std::get<Equity>(underlying)` to extract value

**TimePoint:**

- Defined as `std::chrono::system_clock::time_point` in time_point.h
- `timeNow()` utility available
- Supports standard chrono operations

**Config Singleton:**

- Access via `Config::instance()` which returns `std::expected<Config, std::string>`
- Always check if the expected contains value before using
- Modify Config to add pricing-related parameters

**Order Book Structure:**

- Stores `ActiveOrders` per underlying in `d_activeOrders` map
- Each ActiveOrders has PriceLevelMap for bids and asks
- Already has thread safety per underlying via `d_underlyingMutexes` in Orchestrator

**Logging:**

- Uses custom logging with LogLevel enum (DEBUG, INFO, etc.)
- Access via `config->logLevel()` to check if should log
- Use `std::cout` for logging (no special logger object)

**Testing:**

- Tests appear to use ctest (CMake)
- Add unit tests for Pricer in tests directory
- Follow existing test patterns

---

## FINAL THOUGHTS

You're building a sophisticated order book simulation. The pricing component is the anchor that turns random chaos into realistic price discovery.

**Start simple:** Get equity pricing working first (just tracking last trade). This is straightforward and lets you test the integration points.

**Then add futures:** Implement the cost-of-carry calculation. This is mathematically elegant and satisfying when it works.

**Verify constantly:** Print intermediate values. Check that futures > spot when r > q. Verify trades happen near fair value. Plot price over time.

**Thread safety matters:** With multiple worker threads, race conditions will crash your simulation or corrupt data. Use shared_mutex and RAII locks religiously.

**Make it configurable:** Hard-code parameters initially to get it working, then move them to Config so you can experiment with different market conditions.

The goal is realistic price behavior that emerges naturally from your matching engine, guided by theoretical fair values. When you see market prices oscillating around fair value with occasional deviations due to order flow imbalances, you'll know it's working.

Good luck building this on your flight. You have all the pieces - now assemble them into a working pricing engine.
