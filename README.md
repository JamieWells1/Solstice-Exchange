# Solstice Exchange

> **_Matching Engine, Pricing Engine, and Algorithmic Trading Strategy Bundled Into One Shared Library: Solstice_**

Solstice is a modular C++23 library for building high-performance electronic trading systems. It integrates core exchange infrastructure — including a limit order book, execution matcher, pricing simulator, and algorithmic strategy executor, all under a unified parent namespace.

While only the matching engine is fully implemented at present, the architecture is designed for clear separation of responsibilities across:

- `solstice::matching` — Order book, matcher, order processor.
- `solstice::pricing` — Market simulation and pricing feed (WIP).
- `solstice::strategy` — Automated strategy execution (WIP).

---

## Performance

### Latest (v0.2.0, Multi-threaded)

> 🚀 Peak Throughput: **523,560 orders/sec** @ v0.2.0 on M4 MacBook Pro (multi-threaded)

| Orders Executed | Execution Time (ms) | Throughput (orders/sec) |
|------------------|---------------------|--------------------------|
| 100,000          | 191                 | 523,560                  |

See [`BENCHMARK_HISTORY.md`](BENCHMARK_HISTORY.md) for all historical runs, and [`src/matching/README.md`](src/matching/README.md) for full implementation details.

---

## Modules

- [`src/matching/README.md`](src/matching/README.md) — Matching engine design and benchmarks.
- [`src/pricing/README.md`](src/pricing/README.md) — Pricing engine spec (placeholder).
- [`src/strategy/README.md`](src/strategy/README.md) — Strategy engine spec (placeholder).
- [`BENCHMARK_HISTORY.md`](BENCHMARK_HISTORY.md) — Historical performance data and trends.

---

## Status

| Component  | Status      |
|------------|-------------|
| Matching   | ✅ Complete |
| Pricing    | 🚧 Planned  |
| Strategy   | 🚧 Planned  |

---

This repo serves as a single, extensible foundation for simulating and testing real-world trading systems in a controlled, performant environment.

MIT License
