# Solstice Matching Engine

A high-performance limit order book written in C++23, designed for experimentation with order flow, benchmarking, and execution algorithms.

## Benchmark (v0.1)

**Config:**
- Orders: 100,000
- Tickers: 10
- Price Range: $8–$10 (to 2 d.p)
- Quantity Range: 1–20

**Result:**
- Matched: 49,788
- Time Taken: 1,225 ms
- Throughput: ~81,600 orders/sec
