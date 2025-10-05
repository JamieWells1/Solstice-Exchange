# Solstice Matching Engine

A high-performance limit order book written in C++23, designed for experimentation with order flow, benchmarking, and execution algorithms.

## Benchmark (v0.1)

This MVP implementation is single-threaded, with plans for future iterations to
support multithreading and concurrent order processing.

**Config:**

- Orders: 100,000
- Tickers: 10
- Price Range: $8–$10 (to 2 d.p)
- Quantity Range: 1–20

**Result:**

- Matched: 49,788
- Time Taken: 1,225 ms
- Throughput: ~81,600 orders/sec

## Optimise Random Number Generation (v0.1.2)

After switching from creating new mt19937 generators on every call to using
static generators (similar to the existing getRandomUid implementation),
benchmark performance improved by ~60%, reducing execution time from 1225ms to
487ms for 100,000 orders.

Furthermore, a bug was discovered in which the matcher was attempting to find
opposite-side prices in the same-side price level map, causing match failures
when using high-resolution prices (e.g., doubles with 0.01 precision) where exact
price collisions between buy and sell orders were rare.

Since patching and using `double`s instead of integer prices, the engine has seen ~10% increase in orders matched.

**Config:**

- Orders: 100,000
- Tickers: 10
- Price Range: $8–$10 (to 2 d.p)
- Quantity Range: 1–20

**Result:**

- Matched: 54888
- Time Taken: 487 ms
- Throughput: ~205,300 orders/sec

## Multi-threaded Order Processing (v0.2.0)

After completing the first working version of the matching engine and eradicating all bugs that affected functionality, I decided to introduce multi-threading to order processing. Order generation is still done with a single thread, as this is trivial to do in parallel. The number of threads created is determined by `std::thread::hardware_concurrency()`, and are stored in thread pool, `threadPool`.

**Config:**

- Orders: 100,000
- Tickers: 10
- Price Range: $8–$10 (to 2 d.p)
- Quantity Range: 1–20

**Result:**

- Matched: 54652
- Time Taken: 191 ms
- Throughput: ~523,600 orders/sec

### Execution Time Plotted Against Throughput


