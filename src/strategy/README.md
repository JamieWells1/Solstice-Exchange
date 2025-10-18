# Solstice Algorithmic Trading Strategy Engine

Solstice Strategy Engine is a high-performance backtesting and strategy execution framework built using C++23 with Python bindings via pybind11. The engine enables rapid development and testing of quantitative trading strategies with seamless integration between C++ execution logic and Python data pipelines.

The architecture leverages the Strategy Pattern with a dispatcher-based routing system, allowing multiple strategy implementations to coexist with minimal overhead. Market data is ingested via Python (yfinance), transformed into C++-native structures, and processed through strategy-specific execution logic.

## Key Features

- **Hybrid C++/Python Architecture**: Core strategy logic in C++23 for performance, data ingestion and configuration in Python for flexibility.
- **Strategy Dispatcher Pattern**: Modular, extensible strategy registry with factory-based instantiation.
- **pybind11 Integration**: Seamless Python-C++ interoperability with automatic type conversions and exception handling.
- **Market Data Pipeline**: Configurable data fetching via yfinance with support for multiple timeframes, intervals, and tickers.
- **Report Generation**: Structured performance metrics including PnL, trade counts, win/loss ratios, and candle analysis.
- **Development-Optimized Build System**: Fast iteration with optional rebuild flags and separate CMake/setuptools workflows.

---

## Local Execution

### Initial Setup

Build and install the Python C++ extension:

```bash
pip install -e .
```

### Running Backtests

Fast execution (no rebuild):

```bash
./backtest.sh
```

or directly:

```bash
python src/strategy/entry_point.py
```

With C++ extension rebuild (use after modifying C++ files):

```bash
./backtest.sh --rebuild
```

### Configuration

Backtest parameters are configured via `src/config/backtest/data_config.json`:

- **ticker**: Symbol to fetch (e.g., "AAPL", "TSLA")
- **interval**: Candle interval ("1m", "5m", "1h", "1d", etc.)
- **time_frame**: Either "period" or "start_end"
  - **period**: Relative period ("1mo", "3mo", "1y", etc.)
  - **start_end**: Absolute date range (start_date, end_date)

Example configuration:

```json
{
  "ticker": "AAPL",
  "interval": "1d",
  "time": {
    "time_frame": "period",
    "period": "1y"
  }
}
```

---

## Benchmarks

*Benchmark results will be added as strategies are implemented and tested.*

