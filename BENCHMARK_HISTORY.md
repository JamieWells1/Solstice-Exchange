# Benchmark History For Solstice Order Book & Matching Engine

## Initial MVP (v0.1.0)

This MVP implementation is single-threaded, with plans for future iterations to
support multithreading and concurrent order processing.

[Full release notes](https://github.com/JamieWells1/Solstice-Exchange/releases/tag/v0.1)

**Config:**

- Orders: 100,000
- Tickers: 10
- Price Range: $8–$10 (to 2 d.p)
- Quantity Range: 1–20

**Result:**

- Matched: _49,788_
- Time Taken: _1,225 ms_
- Throughput: _~81,600 orders/sec_

## Optimise Random Number Generation (v0.1.2)

Switched from creating new mt19937 generators on every call to using
static generators, used for generating random order values.

[Full release notes](https://github.com/JamieWells1/Solstice-Exchange/releases/tag/v0.1.2)

**Config:**

- Orders: 100,000
- Tickers: 10
- Price Range: $8–$10 (to 2 d.p)
- Quantity Range: 1–20

**Result:**

- Matched: _54,888_
- Time Taken: _487 ms_
- Throughput: _~205,300 orders/sec_

## Multi-threaded Order Processing (v0.2.0)

Introduced multi-threading to order processing via a thread pool that's initialised before order processing.

[Full release notes](https://github.com/JamieWells1/Solstice-Exchange/releases/tag/v0.2.0)

**Config:**

- Orders: 100,000
- Tickers: 10
- Price Range: $8–$10 (to 2 d.p)
- Quantity Range: 1–20

**Result:**

- Matched: _54,652_
- Time Taken: _191 ms_
- Throughput: _~523,600 orders/sec_
