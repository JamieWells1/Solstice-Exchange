# Solstice Matching Engine

A high-performance limit order book written in modern C++ (C++23), designed for experimentation with order flow, benchmarking, and execution algorithms.

## Benchmark (v0.1)

**Config:**
- Orders: 1,000,000
- Tickers: 3
- Price Range: 10–20
- Quantity Range: 1–10

**Result:**
- Matched: 486,730
- Time Taken: 14,041 ms
- Throughput: ~71,200 orders/sec

## ⚙️ Build

```bash
cmake -S . -B build
cmake --build build
./build/bin/solstice
